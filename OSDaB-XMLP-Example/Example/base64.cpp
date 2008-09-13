/****************************************************************************
** Filename: base64.cpp
** Last updated [dd/mm/yyyy]: 09/06/2006
**
** Class for encoding/decoding of text using base 64 coding.
**
** See RFC 2045 for details on base 64 coding.
** Code is based on the original GPLed work by Dawit Alemayehu and Rik Hemsley
** (KDE development team). Many changes have been made and some optimization
** should have been reached. Original version did not ignore padding-chars
** while decoding - see RFC 2045 ;)
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

#include "base64.h"

#include <QByteArray>
#include <QBitArray>
#include <QString>


/*!
	\class Base64 base64.h
	\ingroup OSDaB

	\brief Class for encoding/decoding of text using base 64 coding.
*/


/************************************************************************
Private interface
*************************************************************************/

//! \internal
class Base64Private
{
public:
	static const char encodeTable[64];
	static const char decodeTable[128];
};

//! \internal
const char Base64Private::encodeTable[64] =
{
	0x41, 0x42, 0x43, 0x44, 0x45, 0x46, 0x47, 0x48,
	0x49, 0x4A, 0x4B, 0x4C, 0x4D, 0x4E, 0x4F, 0x50,
	0x51, 0x52, 0x53, 0x54, 0x55, 0x56, 0x57, 0x58,
	0x59, 0x5A, 0x61, 0x62, 0x63, 0x64, 0x65, 0x66,
	0x67, 0x68, 0x69, 0x6A, 0x6B, 0x6C, 0x6D, 0x6E,
	0x6F, 0x70, 0x71, 0x72, 0x73, 0x74, 0x75, 0x76,
	0x77, 0x78, 0x79, 0x7A, 0x30, 0x31, 0x32, 0x33,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x2B, 0x2F
};

//! \internal
const char Base64Private::decodeTable[128] =
{
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x3F,
	0x34, 0x35, 0x36, 0x37, 0x38, 0x39, 0x3A, 0x3B,
	0x3C, 0x3D, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06,
	0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E,
	0x0F, 0x10, 0x11, 0x12, 0x13, 0x14, 0x15, 0x16,
	0x17, 0x18, 0x19, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x20,
	0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28,
	0x29, 0x2A, 0x2B, 0x2C, 0x2D, 0x2E, 0x2F, 0x30,
	0x31, 0x32, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00
};

/************************************************************************
Public interface
*************************************************************************/

/*!
	Decodes a base64 encoded string.
 */
QByteArray Base64::decode(const QString& encoded)
{
	if (encoded.isEmpty()) return QByteArray();

	unsigned int len = encoded.length(); // num. of chars we need to decode

	// for faster access to chars we need to decode
	QByteArray in = encoded.toAscii();

	const char* data = in.data();

	unsigned int final = 0; // final length does not count invalid chars in encoded string (RFC 2045)

    for (unsigned int idx=0; idx<len; ++idx)
    {
        uchar ch = data[idx];
        if ( (ch < 123) && ((ch == 65) || (Base64Private::decodeTable[ch] != 0)) )
            in.data()[final++] = Base64Private::decodeTable[ch];
    }

	if ( final < 1 ) return QByteArray();

	// base64 uses 4-byte blocks, but input may have exceeding bytes!
	unsigned int rest = final%4;
	final -= rest;

	len = final/4;

	switch (rest)
	{
	case 2: final = len*3 + 1; break; // we can get another byte out of 2 6-bit chars
	case 3: final = len*3 + 2; break; // we can get 2 bytes out of 3 6-bit chars
	default: final = len*3;
	}

	QByteArray out(final, '\0');

	unsigned int outIdx = 0; // index for the next decoded byte
	unsigned int inIdx = 0; // index for the next encoded byte

	// 4-byte to 3-byte conversion
	for (unsigned int idx=0; idx<len; ++idx)
	{
		out.data()[outIdx++] = (((in.at(inIdx) << 2) & 252) | ((in.at(inIdx+1) >> 4) & 003));
		out.data()[outIdx++] = (((in.at(inIdx+1) << 4) & 240) | ((in.at(inIdx+2) >> 2) & 017));
		out.data()[outIdx++] = (((in.at(inIdx+2) << 6) & 192) | (in.at(inIdx+3) & 077));
		inIdx += 4;
	}

	switch (rest)
	{
	case 2:
		//out.data()[outIdx] = (((in.at(inIdx) << 2) & 255) | ((in.at(inIdx+1) >> 4) & 003));
		out.data()[outIdx] = ((in.at(inIdx) << 2) & 252) | ((in.at(inIdx+1) >> 4) & 03); // get one byte out of two 6-bit chars
		break;
	case 3:
		//out.data()[outIdx++] = (((in.at(inIdx) << 2) & 255) | ((in.at(inIdx+1) >> 4) & 003));
		out.data()[outIdx++] = ((in.at(inIdx) << 2) & 252) | ((in.at(inIdx+1) >> 4) & 03); // get one byte out of two 6-bit chars
		out.data()[outIdx] = ((in.at(inIdx+1) << 4) & 240) | ((in.at(inIdx+2) >> 2) & 017) ; // get next byte
		//out.data()[outIdx] = (((in.at(inIdx+1) << 4) & 255) | ((in.at(inIdx+2) >> 2) & 017));
	}

	return out;
}

/*!
	Convenience function, decodes a base64 encoded string to a bit array of given size.
	If size is bigger than the decoded array's size, only decoded bytes are returned.
 */
QBitArray Base64::decode(const QString& encoded, unsigned int size)
{
	if (size == 0) return QBitArray();
	QByteArray ba = decode(encoded);

	unsigned int sz = ba.size()*8;
	if (sz == 0) return QBitArray();
	if (size < sz) sz = size;

	QBitArray bits(sz);

	unsigned int idx = 0, currentByte = 0;

	for (unsigned int i=0; i<sz; ++i)
	{
		if (idx == 8)
		{
			idx = 0;
			currentByte++;
		}

		bits.setBit(i, ba.at(currentByte) & 1<<(7-idx));
		idx++;
	}

	return bits;
}

/*!
	Encodes a byte array. Adds a CRLF pair at column 76 if \a limitLines is true.
 */
QString Base64::encode(const QByteArray& decoded, bool limitLines)
{
	unsigned int len = decoded.size();

	if (len == 0) return QString();

	unsigned int idxIn = 0; // index of next decoded byte
    unsigned int idxOut = 0; // index for next encoded byte
	const char* data = decoded.data(); // convenience pointer to decoded data

	const unsigned int rest = len%3;
	unsigned int final = (len < 3) ? 4 : (rest == 0) ? (len/3)*4 : (len/3)*4 + 4; // final output size

	if (limitLines)
		final += (len%76 == 0) ? (final/76 - 1)*2 : (final/76)*2;

	QByteArray out(final, '\0');

	len = (len-rest)/3;
	unsigned int charsOnLine = 0;

	// decoding + 3-byte to 4-byte conversion
	for (unsigned int idx=0; idx<len; ++idx)
	{
		if (limitLines && (charsOnLine > 0) && (charsOnLine%76 == 0))
		{ out.data()[idxOut++] = '\r'; out.data()[idxOut++] = '\n'; charsOnLine = 0; }

		out.data()[idxOut++] = Base64Private::encodeTable[(data[idxIn] >> 2) & 077];
		out.data()[idxOut++] = Base64Private::encodeTable[(data[idxIn+1] >> 4) & 017 | (data[idxIn] << 4) & 077];
		out.data()[idxOut++] = Base64Private::encodeTable[(data[idxIn+2] >> 6) & 003 | (data[idxIn+1] << 2) & 077];
		out.data()[idxOut++] = Base64Private::encodeTable[data[idxIn+2] & 077];
		idxIn += 3;
		charsOnLine += 4;
	}

	switch (rest)
	{
	case 1:
		// need to encode one last byte
		// add 4 0-bits to the right, encode two chars and append two '=' padding chars to output
		if (limitLines && (charsOnLine%76 == 0))
		{ out.data()[idxOut++] = '\r'; out.data()[idxOut++] = '\n'; }

		out.data()[idxOut++] = Base64Private::encodeTable[(data[idxIn] >> 2) & 077]; // encode first 6 bits
		out.data()[idxOut++] = Base64Private::encodeTable[(data[idxIn] << 4) & 077]; // encode next 6 bits (2 significant + 4 0-bits!)
		// padding
		out.data()[idxOut++] = '=';
		out.data()[idxOut] = '=';
		break;
	case 2:
		// need to encode two last bytes
		// add 2 0-bits to the right, encode three chars and append one '=' padding char to output
		if (limitLines && (charsOnLine%76 == 0))
		{ out.data()[idxOut++] = '\r'; out.data()[idxOut++] = '\n'; }

		out.data()[idxOut++] = Base64Private::encodeTable[(data[idxIn] >> 2) & 077]; // encode first 6 bits
		out.data()[idxOut++] = Base64Private::encodeTable[(data[idxIn+1] >> 4) & 017 | (data[idxIn] << 4) & 077]; // encode next 6 bits
		out.data()[idxOut++] = Base64Private::encodeTable[(data[idxIn+1] << 2) & 077]; // last 6 bits (4 significant + 2 0-bits)
		// padding
		out.data()[idxOut] = '=';
	}

	return QString(out);
}

/*!
	Encodes a bit array. Adds a CRLF pair at column 76 if \a limitLines is true.
 */
QString Base64::encode(const QBitArray& decoded, bool limitLines)
{
	unsigned int sz = decoded.size();
	unsigned int rest = sz%8;
	unsigned int bsz = (rest == 0) ? sz/8 : (sz/8)+1;
	unsigned int currByte = 0, idx = 0;

	QByteArray ba(bsz, '\0');

	for (unsigned int i=0; i<sz; i+=8)
	{
		ba.data()[currByte] = 0;

		for (idx=0; idx<8; ++idx)
		{
			if (i+idx == sz)
				break;

			if (decoded.at(i+idx))
				ba.data()[currByte] |= 1<<(7-idx);
		}

		currByte++;
	}

	return encode(ba,limitLines);
}
