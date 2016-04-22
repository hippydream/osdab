/****************************************************************************
** Filename: md5hash.cpp
** Last updated [dd/mm/yyyy]: 09/06/2006
**
** Computes MD5 hash of a file or buffer.
**
** The MD5 algorithm was written by Ron Rivest.
** This class is based on the code written by Colin Plumb in 1993,
** our understanding is that no copyright is claimed and that this code is in
** the public domain.
**
** NO WARRANTY is given for this code.
** Use it at your own risk.
** It should produce correct MD5 digests, but no security-related test has been made.
**
** Copyright (C) 2007-2016 Angius Fabrizio. All rights reserved.
**
** This file is part of the OSDaB project (http://osdab.42cows.org/).
**
** This file may be distributed and/or modified under the terms of the
** GNU General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.
**
** This file is provided AS IS with NO WARRANTY OF ANY KIND, INCLUDING THE
** WARRANTY OF DESIGN, MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE.
**
** See the file LICENSE.GPL that came with this software distribution or
** visit http://www.gnu.org/licenses/gpl-3.0.en.html for GPL licensing information.
**
**********************************************************************/

#include "md5hash.h"

#include <QFile>
#include <QByteArray>
#include <QBitArray>

// 512K buffer
#define MD5_READ_BUFFER 524288


/************************************************************************
Core functions
*************************************************************************/

// #define F1(x, y, z) (x & y | ~x & z)
#define F1(x, y, z) (z ^ (x & (y ^ z)))
#define F2(x, y, z) F1(z, x, y)
#define F3(x, y, z) (x ^ y ^ z)
#define F4(x, y, z) (y ^ (x | ~z))

// This is the central step in the MD5 algorithm.
#define MD5STEP(f, w, x, y, z, data, s) \
	( w += f(x, y, z) + data,  w = w<<s | w>>(32-s),  w += x )


/*!
	\class MD5Hash md5hash.h
	\ingroup OSDaB

	\brief Computes MD5 hash of a file or buffer.
*/


/************************************************************************
Private interface
*************************************************************************/

//! \internal
class MD5HashPrivate
{
public:
	struct md5Context
	{
		unsigned int buffer[4];
		unsigned int bits[2];
		unsigned char in[64];
	};

	static void updateHash(md5Context* context, const char* data, long int length);
	static void md5Transform(unsigned int buf[4], unsigned int const in[16]);
	static QString finalize(md5Context& ctx);
};

//! \internal
void MD5HashPrivate::updateHash(md5Context* ctx, const char* data, long int length)
{
	unsigned int t;

	// Update bitcount
	t = ctx->bits[0];
	if ((ctx->bits[0] = t + ((unsigned int) length << 3)) < t)
		ctx->bits[1]++; // Carry from low to high
	ctx->bits[1] += length >> 29;

	t = (t >> 3) & 0x3f; // buffer index

	// Handle any leading odd-sized chunks
	if (t)
	{
		unsigned char *p = (unsigned char *) ctx->in + t;

		t = 64 - t;

		if ((unsigned long int)length < t)
		{
			memcpy(p, data, length);
			return;
		}

		memcpy(p, data, t);
		md5Transform(ctx->buffer, (unsigned int *) ctx->in);
		data += t;
		length -= t;
	}

	// Process data in 64-byte chunks
	while (length >= 64)
	{
		memcpy(ctx->in, data, 64);
		md5Transform(ctx->buffer, (unsigned int *) ctx->in);
		data += 64;
		length -= 64;
	}

	// Handle any remaining bytes of data.
	memcpy(ctx->in, data, length);
}

//! \internal
QString MD5HashPrivate::finalize(md5Context& ctx)
{
	unsigned count;
	unsigned char *p;

	// Compute number of bytes mod 64
	count = (ctx.bits[0] >> 3) & 0x3F;

	// Set the first char of padding to 0x80.
	// This is safe since there is always at least one byte free
	p = ctx.in + count;
	*p++ = 0x80;

	// Bytes of padding needed to make 64 bytes
	count = 64 - 1 - count;

	// Pad out to 56 mod 64
	if (count < 8)
	{
		// Two lots of padding:  Pad the first block to 64 bytes
		memset(p, 0, count);
		md5Transform(ctx.buffer, (unsigned int *) ctx.in);

		// Now fill the next block with 56 bytes
		memset(ctx.in, 0, 56);
	}
	else
	{
		// Pad block to 56 bytes
		memset(p, 0, count - 8);
	}

	// Append length in bits and transform
	((unsigned int *) ctx.in)[14] = ctx.bits[0];
	((unsigned int *) ctx.in)[15] = ctx.bits[1];

	md5Transform(ctx.buffer, (unsigned int *) ctx.in);

	// You may need to wipe variables for security reasons before returning!!!

	QString digest;
	digest.sprintf("%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x%02x",
		(ctx.buffer[0])&0xFF, (ctx.buffer[0]>>8)&0xFF, (ctx.buffer[0]>>16)&0xFF, (ctx.buffer[0]>>24)&0xFF,
		(ctx.buffer[1])&0xFF, (ctx.buffer[1]>>8)&0xFF, (ctx.buffer[1]>>16)&0xFF, (ctx.buffer[1]>>24)&0xFF,
		(ctx.buffer[2])&0xFF, (ctx.buffer[2]>>8)&0xFF, (ctx.buffer[2]>>16)&0xFF, (ctx.buffer[2]>>24)&0xFF,
		(ctx.buffer[3])&0xFF, (ctx.buffer[3]>>8)&0xFF, (ctx.buffer[3]>>16)&0xFF, (ctx.buffer[3]>>24)&0xFF
		);

	return digest;
}

/*! \internal
	The core of the MD5 algorithm, this alters an existing MD5 hash to
	reflect the addition of 16 longwords of new data. MD5Update blocks
	the data and converts bytes into longwords for this routine.
 */
void MD5HashPrivate::md5Transform(unsigned int buf[4], unsigned int const in[16])
{
	register unsigned int a, b, c, d;

	a = buf[0];
	b = buf[1];
	c = buf[2];
	d = buf[3];

	MD5STEP(F1, a, b, c, d, in[0] + 0xd76aa478, 7);
	MD5STEP(F1, d, a, b, c, in[1] + 0xe8c7b756, 12);
	MD5STEP(F1, c, d, a, b, in[2] + 0x242070db, 17);
	MD5STEP(F1, b, c, d, a, in[3] + 0xc1bdceee, 22);
	MD5STEP(F1, a, b, c, d, in[4] + 0xf57c0faf, 7);
	MD5STEP(F1, d, a, b, c, in[5] + 0x4787c62a, 12);
	MD5STEP(F1, c, d, a, b, in[6] + 0xa8304613, 17);
	MD5STEP(F1, b, c, d, a, in[7] + 0xfd469501, 22);
	MD5STEP(F1, a, b, c, d, in[8] + 0x698098d8, 7);
	MD5STEP(F1, d, a, b, c, in[9] + 0x8b44f7af, 12);
	MD5STEP(F1, c, d, a, b, in[10] + 0xffff5bb1, 17);
	MD5STEP(F1, b, c, d, a, in[11] + 0x895cd7be, 22);
	MD5STEP(F1, a, b, c, d, in[12] + 0x6b901122, 7);
	MD5STEP(F1, d, a, b, c, in[13] + 0xfd987193, 12);
	MD5STEP(F1, c, d, a, b, in[14] + 0xa679438e, 17);
	MD5STEP(F1, b, c, d, a, in[15] + 0x49b40821, 22);

	MD5STEP(F2, a, b, c, d, in[1] + 0xf61e2562, 5);
	MD5STEP(F2, d, a, b, c, in[6] + 0xc040b340, 9);
	MD5STEP(F2, c, d, a, b, in[11] + 0x265e5a51, 14);
	MD5STEP(F2, b, c, d, a, in[0] + 0xe9b6c7aa, 20);
	MD5STEP(F2, a, b, c, d, in[5] + 0xd62f105d, 5);
	MD5STEP(F2, d, a, b, c, in[10] + 0x02441453, 9);
	MD5STEP(F2, c, d, a, b, in[15] + 0xd8a1e681, 14);
	MD5STEP(F2, b, c, d, a, in[4] + 0xe7d3fbc8, 20);
	MD5STEP(F2, a, b, c, d, in[9] + 0x21e1cde6, 5);
	MD5STEP(F2, d, a, b, c, in[14] + 0xc33707d6, 9);
	MD5STEP(F2, c, d, a, b, in[3] + 0xf4d50d87, 14);
	MD5STEP(F2, b, c, d, a, in[8] + 0x455a14ed, 20);
	MD5STEP(F2, a, b, c, d, in[13] + 0xa9e3e905, 5);
	MD5STEP(F2, d, a, b, c, in[2] + 0xfcefa3f8, 9);
	MD5STEP(F2, c, d, a, b, in[7] + 0x676f02d9, 14);
	MD5STEP(F2, b, c, d, a, in[12] + 0x8d2a4c8a, 20);

	MD5STEP(F3, a, b, c, d, in[5] + 0xfffa3942, 4);
	MD5STEP(F3, d, a, b, c, in[8] + 0x8771f681, 11);
	MD5STEP(F3, c, d, a, b, in[11] + 0x6d9d6122, 16);
	MD5STEP(F3, b, c, d, a, in[14] + 0xfde5380c, 23);
	MD5STEP(F3, a, b, c, d, in[1] + 0xa4beea44, 4);
	MD5STEP(F3, d, a, b, c, in[4] + 0x4bdecfa9, 11);
	MD5STEP(F3, c, d, a, b, in[7] + 0xf6bb4b60, 16);
	MD5STEP(F3, b, c, d, a, in[10] + 0xbebfbc70, 23);
	MD5STEP(F3, a, b, c, d, in[13] + 0x289b7ec6, 4);
	MD5STEP(F3, d, a, b, c, in[0] + 0xeaa127fa, 11);
	MD5STEP(F3, c, d, a, b, in[3] + 0xd4ef3085, 16);
	MD5STEP(F3, b, c, d, a, in[6] + 0x04881d05, 23);
	MD5STEP(F3, a, b, c, d, in[9] + 0xd9d4d039, 4);
	MD5STEP(F3, d, a, b, c, in[12] + 0xe6db99e5, 11);
	MD5STEP(F3, c, d, a, b, in[15] + 0x1fa27cf8, 16);
	MD5STEP(F3, b, c, d, a, in[2] + 0xc4ac5665, 23);

	MD5STEP(F4, a, b, c, d, in[0] + 0xf4292244, 6);
	MD5STEP(F4, d, a, b, c, in[7] + 0x432aff97, 10);
	MD5STEP(F4, c, d, a, b, in[14] + 0xab9423a7, 15);
	MD5STEP(F4, b, c, d, a, in[5] + 0xfc93a039, 21);
	MD5STEP(F4, a, b, c, d, in[12] + 0x655b59c3, 6);
	MD5STEP(F4, d, a, b, c, in[3] + 0x8f0ccc92, 10);
	MD5STEP(F4, c, d, a, b, in[10] + 0xffeff47d, 15);
	MD5STEP(F4, b, c, d, a, in[1] + 0x85845dd1, 21);
	MD5STEP(F4, a, b, c, d, in[8] + 0x6fa87e4f, 6);
	MD5STEP(F4, d, a, b, c, in[15] + 0xfe2ce6e0, 10);
	MD5STEP(F4, c, d, a, b, in[6] + 0xa3014314, 15);
	MD5STEP(F4, b, c, d, a, in[13] + 0x4e0811a1, 21);
	MD5STEP(F4, a, b, c, d, in[4] + 0xf7537e82, 6);
	MD5STEP(F4, d, a, b, c, in[11] + 0xbd3af235, 10);
	MD5STEP(F4, c, d, a, b, in[2] + 0x2ad7d2bb, 15);
	MD5STEP(F4, b, c, d, a, in[9] + 0xeb86d391, 21);

	buf[0] += a;
	buf[1] += b;
	buf[2] += c;
	buf[3] += d;
}


/************************************************************************
Public interface
*************************************************************************/

/*!
	Computes the MD5 checksum of a byte array.
*/
QString MD5Hash::hashData(const char* bytes, int size)
{
	/*
	 * Start MD5 accumulation. Set bit count to 0 and buffer to mysterious
	 * initialization constants.
	 */

	MD5HashPrivate::md5Context ctx;

	ctx.buffer[0] = 0x67452301;
	ctx.buffer[1] = 0xefcdab89;
	ctx.buffer[2] = 0x98badcfe;
	ctx.buffer[3] = 0x10325476;

	ctx.bits[0] = 0;
	ctx.bits[1] = 0;

	MD5HashPrivate::updateHash(&ctx, bytes, size);

	return MD5HashPrivate::finalize(ctx);
}

/*!
	Convenience method.
	Computes the MD5 checksum of a byte array.
*/
QString MD5Hash::hashData(const QByteArray& bytes)
{
	return hashData(bytes.data(), bytes.size());
}

/*!
	Computes the MD5 checksum of a whole file.
*/
QString MD5Hash::hashFile(QFile& file)
{
	/* To compute the message digest of a chunk of bytes, declare an
	 * MD5Context structure, pass it to MD5Init, call MD5Update as
	 * needed on buffers full of bytes, and then call MD5Final, which
	 * will fill a supplied 16-byte array with the digest.
	 */

	if (!file.open(QIODevice::ReadOnly))
		return QString();

	/*
	 * Start MD5 accumulation. Set bit count to 0 and buffer to mysterious
	 * initialization constants.
	 */

	MD5HashPrivate::md5Context ctx;

	ctx.buffer[0] = 0x67452301;
	ctx.buffer[1] = 0xefcdab89;
	ctx.buffer[2] = 0x98badcfe;
	ctx.buffer[3] = 0x10325476;

	ctx.bits[0] = 0;
	ctx.bits[1] = 0;

	long int read;
	QByteArray buffer(MD5_READ_BUFFER, 0);
	char* data = buffer.data();

	while ( (read = file.read(data, MD5_READ_BUFFER)) > 0 )
		MD5HashPrivate::updateHash(&ctx, data, read);

	if (read < 0)
		return QString();

	return MD5HashPrivate::finalize(ctx);
}

/*!
	Convenience method.
	Computes the MD5 checksum of a whole file.
*/
QString MD5Hash::hashFile(const QString& filename)
{
	QFile file(filename);
	return hashFile(file);
}
