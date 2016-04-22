/****************************************************************************
** Filename: main.cpp
** Last updated [dd/mm/yyyy]: 28/07/2007
**
** Test routine for the XmlWriter class.
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

#include "xmlwriter.h"

#include <QFile>
#include <QHash>

#include <iostream>

using namespace std;

int main(int, char**)
{
	cout << "Writing some XML to myFile.xml" << endl;

	QFile file("myFile.xml");
	if (!file.open(QIODevice::WriteOnly))
	{
		cout << "Failed to open myFile.xml" << endl;
		return -1;
	}

	// Default text encoding: UTF-8
	XmlWriter xout(&file);

	// A hash is used to add attributes
	QHash<QString,QString> attrs;

	attrs.insert("version", "1.0");
	xout.writeOpenTag("sample_document", attrs);
	attrs.clear();
	xout.writeOpenTag("info");
	attrs.insert("hidden", "true");
	xout.writeAtomTag("visibility", attrs);
	attrs.clear();
	attrs.insert("id", "540");
	attrs.insert("type", "default");
	xout.writeAtomTag("user", attrs);
	attrs.clear();
	xout.writeTaggedString("data", "Here comes some interesting text");
	xout.writeCloseTag("info");
	xout.writeOpenTag("info");
	attrs.insert("hidden", "false");
	xout.writeAtomTag("visibility", attrs);
	attrs.clear();
	attrs.insert("id", "550");
	attrs.insert("type", "default");
	xout.writeAtomTag("user", attrs);
	attrs.clear();
	xout.writeTaggedString("data", "Some more text");
	xout.writeCloseTag("info");
	xout.writeCloseTag("sample_document");

	return 0;
}
