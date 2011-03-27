/****************************************************************************
** Filename: zipglobal.cpp
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
** This file is part of the OSDaB project (http://osdab.42cows.org/).
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

#include "zipglobal.h"

#if defined(Q_OS_WIN) || defined(Q_OS_WINCE) || defined(Q_OS_LINUX) || defined (Q_OS_MACX)
#define OSDAB_ZIP_HAS_UTC
#include <ctime>
#else
#undef OSDAB_ZIP_HAS_UTC
#endif

OSDAB_BEGIN_NAMESPACE(Zip)

/*! Returns the current UTC offset in seconds unless OSDAB_ZIP_NO_UTC is defined
    and method is implemented for the current platform and 0 otherwise.
*/
int OSDAB_ZIP_MANGLE(currentUtcOffset)()
{
#if !(!defined OSDAB_ZIP_NO_UTC && defined OSDAB_ZIP_HAS_UTC)
    return 0;
#else
    time_t curr_time_t;
    time(&curr_time_t);

#if defined Q_OS_WIN
    struct tm _tm_struct;
    struct tm* tm_struct = &_tm_struct;
#else
    struct tm* tm_struct = 0;
#endif

#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // use the reentrant version of localtime() where available
    tzset();
    tm res;
    tm_struct = gmtime_r(&curr_time_t, &res);
#elif defined Q_OS_WIN
    if (gmtime_s(tm_struct, &curr_time_t))
        return 0;
#else
    tm_struct = gmtime(&curr_time_t);
#endif

    if (!tm_struct)
        return 0;

    const time_t global_time_t = mktime(tm_struct);

#if !defined(QT_NO_THREAD) && defined(_POSIX_THREAD_SAFE_FUNCTIONS)
    // use the reentrant version of localtime() where available
    tm_struct = localtime_r(&curr_time_t, &res);
#elif defined Q_OS_WIN
    if (localtime_s(tm_struct, &curr_time_t))
        return 0;
#else
    tm_struct = localtime(&curr_time_t);
#endif
    
    if (!tm_struct)
        return 0;

    const time_t local_time_t = mktime(tm_struct);

    const int utcOffset = - qRound(difftime(global_time_t, local_time_t));
    return tm_struct->tm_isdst > 0 ? utcOffset + 3600 : utcOffset;
#endif // No UTC

    return 0;
}

QDateTime OSDAB_ZIP_MANGLE(fromFileTimestamp)(const QDateTime& dateTime)
{
#if !defined OSDAB_ZIP_NO_UTC && defined OSDAB_ZIP_HAS_UTC
    const int utc = OSDAB_ZIP_MANGLE(currentUtcOffset)();
    return dateTime.toUTC().addSecs(utc);
#else
    return dateTime;
#endif // OSDAB_ZIP_NO_UTC
}

QDateTime OSDAB_ZIP_MANGLE(toFileTimestamp)(const QDateTime& dateTime)
{
#if !defined OSDAB_ZIP_NO_UTC && defined OSDAB_ZIP_HAS_UTC
    return dateTime;
#else
    return dateTime;
#endif // OSDAB_ZIP_NO_UTC
}

OSDAB_END_NAMESPACE
