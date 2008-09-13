/****************************************************************************
** Filename: sha1hash.cpp
** Last updated [dd/mm/yyyy]: 09/06/2006
**
** Computes SHA1 hash of a file or buffer.
**
** SHA (Secure Hash Algorithm) is a cryptographically safe function developed
** by NIST and NSA in 1992. It fulfils all the requirements imposed for
** one-way hashing functions and it generates very different images for similar
** sequences.
** This code is based on the the 100% free public domain implementation of
** the SHA-1 algorithm by Dominik Reichl <dominik.reichl@t-online.de>
**
** NO WARRANTY is given for this code.
** Use it at your own risk.
** It should produce correct SHA1 digests, but no security-related test has been made.
**
** Copyright (C) 2007 Angius Fabrizio. All rights reserved.
**
** This file is part of the OSDaB project (http://osdab.sourceforge.net/).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 2 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See the file LICENSE.GPL that came with this software distribution or
** visit http://www.gnu.org/copyleft/gpl.html for GPL licensing information.
**
**********************************************************************/

#include "sha1hash.h"

#include <QString>
#include <QFile>

// 512K buffer
#define SHA1_READ_BUFFER 524288

/************************************************************************
Core functions
*************************************************************************/

// Rotate x bits to the left
#ifndef ROL32
#define ROL32(_val32, _nBits) (((_val32)<<(_nBits))|((_val32)>>(32-(_nBits))))
#endif

#ifdef SHA1_LITTLE_ENDIAN
#define SHABLK0(i) (SHA1HashPrivate::block->l[i] = (ROL32(SHA1HashPrivate::block->l[i],24) & 0xFF00FF00) | (ROL32(SHA1HashPrivate::block->l[i],8) & 0x00FF00FF))
#else
#define SHABLK0(i) (SHA1HashPrivate::block->l[i])
#endif

#define SHABLK(i) (SHA1HashPrivate::block->l[i&15] = ROL32(SHA1HashPrivate::block->l[(i+13)&15] ^ SHA1HashPrivate::block->l[(i+8)&15] ^ SHA1HashPrivate::block->l[(i+2)&15] ^ SHA1HashPrivate::block->l[i&15],1))

// SHA-1 rounds
#define _R0(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK0(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
#define _R1(v,w,x,y,z,i) { z+=((w&(x^y))^y)+SHABLK(i)+0x5A827999+ROL32(v,5); w=ROL32(w,30); }
#define _R2(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0x6ED9EBA1+ROL32(v,5); w=ROL32(w,30); }
#define _R3(v,w,x,y,z,i) { z+=(((w|x)&y)|(w&x))+SHABLK(i)+0x8F1BBCDC+ROL32(v,5); w=ROL32(w,30); }
#define _R4(v,w,x,y,z,i) { z+=(w^x^y)+SHABLK(i)+0xCA62C1D6+ROL32(v,5); w=ROL32(w,30); }


/*!
	\class SHA1Hash sha1hash.h
	\ingroup OSDaB

	\brief Computes SHA1 hash of a file or buffer.
*/


/************************************************************************
Private interface
*************************************************************************/

//! \internal
class SHA1HashPrivate
{
public:
	typedef union
	{
		unsigned char c[64];
		unsigned long l[16];
	} SHA1_WORKSPACE_BLOCK;

	static unsigned char workspace[64];
	static SHA1_WORKSPACE_BLOCK *block;

	static unsigned long state[5];
	static unsigned long count[2];
	static unsigned char buffer[64];

	static void sha1Transform(unsigned long state[5], const unsigned char buffer[64]);
	static void updateHash(const unsigned char *data, unsigned int len);
	static QString finalize();
};

//! \internal
QString SHA1HashPrivate::finalize()
{
	// finalize hash
	unsigned long i = 0;
	unsigned char finalcount[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };

	for (i = 0; i < 8; i++)
		finalcount[i] = (unsigned char)((count[(i >= 4 ? 0 : 1)]
			>> ((3 - (i & 3)) * 8) ) & 255); // Endian independent

	SHA1HashPrivate::updateHash((unsigned char *)"\200", 1);

	while ((count[0] & 504) != 448)
		SHA1HashPrivate::updateHash((unsigned char *)"\0", 1);

	SHA1HashPrivate::updateHash(finalcount, 8); // Causes a sha1Transform()

	unsigned char digestBytes[20];
	QString digest;

	for (i = 0; i < 20; i++)
		digestBytes[i] = (unsigned char)((state[i >> 2] >> ((3 - (i & 3)) * 8) ) & 255);

	digest.sprintf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		digestBytes[0], digestBytes[1], digestBytes[2], digestBytes[3], digestBytes[4],
		digestBytes[5], digestBytes[6], digestBytes[7], digestBytes[8], digestBytes[9],
		digestBytes[10], digestBytes[11], digestBytes[12], digestBytes[13], digestBytes[14],
		digestBytes[15], digestBytes[16], digestBytes[17], digestBytes[18], digestBytes[19]);


	// You may need to wipe variables for security reasons before returning!!!

	return digest;
}

// init static variables

unsigned char SHA1HashPrivate::workspace[64] = {0};
SHA1HashPrivate::SHA1_WORKSPACE_BLOCK* SHA1HashPrivate::block = 0;
unsigned long SHA1HashPrivate::state[5] = {0};
unsigned long SHA1HashPrivate::count[2] = {0};
unsigned char SHA1HashPrivate::buffer[64] = {0};

/*!
	\internal Hash a single 512-bit block. This is the core of the algorithm.
*/
void SHA1HashPrivate::sha1Transform(unsigned long state[5], const unsigned char buffer[64])
{
	unsigned long a = 0, b = 0, c = 0, d = 0, e = 0;

	memcpy(block, buffer, 64);

	// Copy state[] to working vars
	a = state[0];
	b = state[1];
	c = state[2];
	d = state[3];
	e = state[4];

	// 4 rounds of 20 operations each. Loop unrolled.
	_R0(a,b,c,d,e, 0); _R0(e,a,b,c,d, 1); _R0(d,e,a,b,c, 2); _R0(c,d,e,a,b, 3);
	_R0(b,c,d,e,a, 4); _R0(a,b,c,d,e, 5); _R0(e,a,b,c,d, 6); _R0(d,e,a,b,c, 7);
	_R0(c,d,e,a,b, 8); _R0(b,c,d,e,a, 9); _R0(a,b,c,d,e,10); _R0(e,a,b,c,d,11);
	_R0(d,e,a,b,c,12); _R0(c,d,e,a,b,13); _R0(b,c,d,e,a,14); _R0(a,b,c,d,e,15);
	_R1(e,a,b,c,d,16); _R1(d,e,a,b,c,17); _R1(c,d,e,a,b,18); _R1(b,c,d,e,a,19);
	_R2(a,b,c,d,e,20); _R2(e,a,b,c,d,21); _R2(d,e,a,b,c,22); _R2(c,d,e,a,b,23);
	_R2(b,c,d,e,a,24); _R2(a,b,c,d,e,25); _R2(e,a,b,c,d,26); _R2(d,e,a,b,c,27);
	_R2(c,d,e,a,b,28); _R2(b,c,d,e,a,29); _R2(a,b,c,d,e,30); _R2(e,a,b,c,d,31);
	_R2(d,e,a,b,c,32); _R2(c,d,e,a,b,33); _R2(b,c,d,e,a,34); _R2(a,b,c,d,e,35);
	_R2(e,a,b,c,d,36); _R2(d,e,a,b,c,37); _R2(c,d,e,a,b,38); _R2(b,c,d,e,a,39);
	_R3(a,b,c,d,e,40); _R3(e,a,b,c,d,41); _R3(d,e,a,b,c,42); _R3(c,d,e,a,b,43);
	_R3(b,c,d,e,a,44); _R3(a,b,c,d,e,45); _R3(e,a,b,c,d,46); _R3(d,e,a,b,c,47);
	_R3(c,d,e,a,b,48); _R3(b,c,d,e,a,49); _R3(a,b,c,d,e,50); _R3(e,a,b,c,d,51);
	_R3(d,e,a,b,c,52); _R3(c,d,e,a,b,53); _R3(b,c,d,e,a,54); _R3(a,b,c,d,e,55);
	_R3(e,a,b,c,d,56); _R3(d,e,a,b,c,57); _R3(c,d,e,a,b,58); _R3(b,c,d,e,a,59);
	_R4(a,b,c,d,e,60); _R4(e,a,b,c,d,61); _R4(d,e,a,b,c,62); _R4(c,d,e,a,b,63);
	_R4(b,c,d,e,a,64); _R4(a,b,c,d,e,65); _R4(e,a,b,c,d,66); _R4(d,e,a,b,c,67);
	_R4(c,d,e,a,b,68); _R4(b,c,d,e,a,69); _R4(a,b,c,d,e,70); _R4(e,a,b,c,d,71);
	_R4(d,e,a,b,c,72); _R4(c,d,e,a,b,73); _R4(b,c,d,e,a,74); _R4(a,b,c,d,e,75);
	_R4(e,a,b,c,d,76); _R4(d,e,a,b,c,77); _R4(c,d,e,a,b,78); _R4(b,c,d,e,a,79);

	// Add the working vars back into state[]
	state[0] += a;
	state[1] += b;
	state[2] += c;
	state[3] += d;
	state[4] += e;

	// Wipe variables
	a = b = c = d = e = 0;
}


/*!
	\internal Use this to hash your data.
*/
void SHA1HashPrivate::updateHash(const unsigned char *data, unsigned int len)
{
	unsigned long i = 0, j;

	j = (count[0] >> 3) & 63;

	if((count[0] += len << 3) < (len << 3)) count[1]++;

	count[1] += (len >> 29);

	if((j + len) > 63)
	{
		memcpy(&buffer[j], data, (i = 64 - j));
		SHA1HashPrivate::sha1Transform(SHA1HashPrivate::state, buffer);

		for (; i+63 < len; i += 64)
			SHA1HashPrivate::sha1Transform(SHA1HashPrivate::state, &data[i]);

		j = 0;
	}
	else i = 0;

	memcpy(&buffer[j], &data[i], len - i);
}


/************************************************************************
Public interface
*************************************************************************/

/*!
	Computes the MD5 checksum of a byte array.
*/
QString SHA1Hash::hashData(const char* data, int size)
{
	SHA1HashPrivate::block = (SHA1HashPrivate::SHA1_WORKSPACE_BLOCK *) SHA1HashPrivate::workspace;

	// Initialize SHA1 constants.
	SHA1HashPrivate::state[0] = 0x67452301;
	SHA1HashPrivate::state[1] = 0xEFCDAB89;
	SHA1HashPrivate::state[2] = 0x98BADCFE;
	SHA1HashPrivate::state[3] = 0x10325476;
	SHA1HashPrivate::state[4] = 0xC3D2E1F0;
	SHA1HashPrivate::count[0] = 0;
	SHA1HashPrivate::count[1] = 0;

	unsigned char* udata = (unsigned char*) data;
	SHA1HashPrivate::updateHash(udata, size);

	return SHA1HashPrivate::finalize();
}

/*!
	Convenience method.
	Computes the MD5 checksum of a byte array.
*/
QString SHA1Hash::hashData(const QByteArray& bytes)
{
	return hashData(bytes.data(), bytes.size());
}

/*!
	Computes the MD5 checksum of a whole file.
*/
QString SHA1Hash::hashFile(QFile& file)
{
	if (!file.open(QIODevice::ReadOnly))
		return QString();

	SHA1HashPrivate::block = (SHA1HashPrivate::SHA1_WORKSPACE_BLOCK *) SHA1HashPrivate::workspace;

	// Initialize SHA1 constants.
	SHA1HashPrivate::state[0] = 0x67452301;
	SHA1HashPrivate::state[1] = 0xEFCDAB89;
	SHA1HashPrivate::state[2] = 0x98BADCFE;
	SHA1HashPrivate::state[3] = 0x10325476;
	SHA1HashPrivate::state[4] = 0xC3D2E1F0;
	SHA1HashPrivate::count[0] = 0;
	SHA1HashPrivate::count[1] = 0;

	qint64 read = 0;
	qint64 total = 0;
	QByteArray buffer(SHA1_READ_BUFFER, 0);
	unsigned char* udata = (unsigned char*) buffer.data();
	char* data = buffer.data();

	while ( (read = file.read(data, SHA1_READ_BUFFER)) > 0 )
	{
		total += read;
		SHA1HashPrivate::updateHash(udata, read);
	}

	if (total < 0)
		return QString();

	return SHA1HashPrivate::finalize();
}

/*!
	Convenience method.
	Computes the MD5 checksum of a whole file.
*/
QString SHA1Hash::hashFile(const QString& filename)
{
	QFile file(filename);
	return hashFile(file);
}
