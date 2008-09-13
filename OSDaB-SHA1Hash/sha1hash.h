/****************************************************************************
** Filename: sha1hash.h
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

#ifndef OSDAB_SHA1HASH_H
#define OSDAB_SHA1HASH_H

// If you're compiling big endian, just comment out the following line
#define SHA1_LITTLE_ENDIAN

class QString;
class QFile;
class QByteArray;

class SHA1Hash
{
public:
	static QString hashData(const QByteArray& bytes);
	static QString hashData(const char* bytes, int size);
	static QString hashFile(QFile& file);
	static QString hashFile(const QString& filename);
};

#endif // OSDAB_SHA1HASH_H
