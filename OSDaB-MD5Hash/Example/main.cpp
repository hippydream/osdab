/****************************************************************************
** Filename: main.cpp
** Last updated [dd/mm/yyyy]: 09/06/2006
**
** Test routine for the MD5Hash class [Requires Qt4 - QtCore module].
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

#include "md5hash.h"

#include <QFile>
#include <QFileInfo>

#include <iostream>

using namespace std;

int main(int argc, char** args)
{
	if (argc != 2)
	{
		cout << "Compute MD5 hash of a file" << endl << endl;
		cout << "Usage: MD5Hash FILE" << endl << endl;
		cout << "(C) 2005 Angius Fabrizio\nLicensed under the terms of the GNU GPL Version 2 or later" << endl;
		return -1;
	}

	QFile file(args[1]);
	if (!file.exists())
	{
		cout << "File does not exist!" << endl;
		return -1;
	}

	QString hash = MD5Hash::hashFile(file);
	if (hash.isEmpty())
	{
		cout << "Unable to compute MD5 hash!" << endl;
		return -1;
	}

	QFileInfo fi(file);

	cout << hash.toAscii().data() << "  " << fi.absoluteFilePath().toAscii().data() << endl;

	return 0;
}
