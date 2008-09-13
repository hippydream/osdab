/****************************************************************************
** Filename: xmlpreferences_p.h
** Last updated [dd/mm/yyyy]: 28/01/2007
**
** Class for reading and writing of XML configuration files.
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

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Movida API.  It exists purely as an
// implementation detail. This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#ifndef OSDAB_XMLPREFERENCES_P__H
#define OSDAB_XMLPREFERENCES_P__H

#include "xmlwriter.h"

#include <QByteArray>
#include <QBitArray>
#include <QVariant>
#include <QHash>
#include <QStringList>
#include <QRect>

#ifndef QT_CORE_ONLY
#include <QColor>
#endif

#include <libxml/xmlmemory.h>
#include <libxml/parser.h>


class XmlPreferencesPrivate
{
public:
	static const quint8 xmlPrefVersion = 1;

	XmlPreferencesPrivate();

	typedef struct EncVariant
	{
		QVariant data;
		XmlPreferences::BinEncodingType encoding;
	};

	typedef QHash<QString, EncVariant> Section;
	typedef QHash<QString, Section> SectionTable;

	SectionTable data;

	QString defaultSectionName;

	QString product;
	QString company;
	quint8 version;

	bool ignoreMissingProductVersion;

	XmlPreferences::ErrorCode lastErrorCode;
	XmlPreferences::BinEncodingType encoding;

	void writeSection(XmlWriter& out, const QString& name, const Section& section);
	void parseSection(xmlDocPtr doc, xmlNodePtr rootNode, Section& section);
	bool checkApplicationVersion(quint8 version);

	QVariant getData(const QString& name, const QString& section);
	void setData(const QString& name, const QString& section, const EncVariant& data);
};

#endif // OSDAB_XMLPREFERENCES_P__H
