/****************************************************************************
** Filename: md5hash.h
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

#ifndef OSDAB_MD5HASH_H
#define OSDAB_MD5HASH_H

class QByteArray;
class QString;
class QFile;

class MD5Hash
{
public:
	static QString hashData(const QByteArray& bytes);
	static QString hashData(const char* bytes, int size);
	static QString hashFile(QFile& file);
	static QString hashFile(const QString& filename);
};

#endif // OSDAB_MD5HASH_H
