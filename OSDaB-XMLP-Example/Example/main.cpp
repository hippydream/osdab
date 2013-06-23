/****************************************************************************
** Filename: main.cpp
** Last updated [dd/mm/yyyy]: 28/01/2007
**
** Test routine for the XmlWriter class.
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

#include "xmlpreferences.h"

#include <QString>
#include <QStringList>
#include <QByteArray>

#include <iostream>

using namespace std;

int main(int, char**)
{
	cout << "Loading the AmazingPreferencesGenerator routine..." << endl;

	XmlPreferences& s = settings();

	s.setProductInfo("The OSDaB Company", "OSDaB XMLP");
	s.setProductVersion(2);

	// Add some values to the default section, but first rename it to "global"
	s.setDefaultSectionName("global");

	s.setBool("myBool", true);
	s.setStringList("myList", QString("a,b,c,d,e,f,g,h").split(","));

	// Default Base64 encoding
	s.setByteArray("b64", QByteArray("\1\2\3\4\5\6", 6), "binary");

	// CSV encoding
	s.setBinaryEncodingType(XmlPreferences::CommaSeparatedValues);
	s.setByteArray("csv", QByteArray("\1\2\3\4\5\6", 6), "binary");

	cout << "Attempting to save preferences to config.xml" << endl;

	if (!s.save("config.xml"))
	{
		cout << "Failed! Reason: " << s.lastErrorString().toLatin1().data();
		return -1;
	}
	
	s.clear();
	cout << "Application preferences cleared" << endl;

	cout << "Attempting to load preferences from config.xml" << endl;

	if (!s.load("config.xml"))
	{
		cout << "Failed! Reason: " << s.lastErrorString().toLatin1().data();
		return -1;
	}

	bool ok;
	bool success = true;

	bool b = s.getBool("myBool", QString(), &ok);
	success &= (ok && b);

	QString str = s.getStringList("myList").join(",");
	success &= str == "a,b,c,d,e,f,g,h";
	
	QByteArray ba = s.getByteArray("b64", "binary");
	if (ba.size() != 6)
		success = false;
	else 
	{
		for (int i=0; i<6 && success; ++i)
			if (ba.at(i) != i+1)
				success = false;
	}

	ba = s.getByteArray("csv", "binary");
	if (ba.size() != 6)
		success = false;
	else 
	{
		for (int i=0; i<6 && success; ++i)
			if (ba.at(i) != i+1)
				success = false;
	}

	if (success)
		cout << "SUCCESS: No corrupt preferences ;)" << endl;
	else cout << "FAILED: Corrupt preferences found, please notify the author :(" << endl;

	return 0;
}
