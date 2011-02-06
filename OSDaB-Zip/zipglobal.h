/****************************************************************************
** Filename: zipglobal.h
** Last updated [dd/mm/yyyy]: 06/02/2011
**
** pkzip 2.0 file compression.
**
** Some of the code has been inspired by other open source projects,
** (mainly Info-Zip and Gilles Vollant's minizip).
** Compression and decompression actually uses the zlib library.
**
** Copyright (C) 2007-2011 Angius Fabrizio. All rights reserved.
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

#ifndef OSDAB_ZIPGLOBAL__H
#define OSDAB_ZIPGLOBAL__H

#include <QtCore/QtGlobal>

/* If you want to build the OSDaB Zip code as
   a library, define OSDAB_ZIP_LIB in the library's .pro file and
   in the libraries using it OR remove the #ifndef OSDAB_ZIP_LIB
   define below and leave the #else body. Also remember to define
   OSDAB_ZIP_BUILD_LIB in the library's project).
*/

#ifndef OSDAB_ZIP_LIB
# define OSDAB_ZIP_EXPORT
#else
# if defined(OSDAB_ZIP_BUILD_LIB)
#   define OSDAB_ZIP_EXPORT Q_DECL_EXPORT
# else
#   define OSDAB_ZIP_EXPORT Q_DECL_IMPORT
# endif
#endif

#endif // OSDAB_ZIPGLOBAL__H
