/****************************************************************************
** Filename: xmlpreferences.cpp
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

#include "xmlpreferences.h"
#include "xmlpreferences_p.h"
#include "base64.h"

#include <QFile>
#include <QDir>
#include <QCoreApplication>

/*!
	\class XmlPreferences xmlpreferences.h

	\brief The XmlPreferences class allows to easily store and retrieve
	application preferences from XML files.

	The default section is just a convenience section. It beheaves exactly like any
	other section but you can set its name once and never need to call it again (the default
	name for this section is "default").

	Adding support for new data types involves the following steps:

	<ul>
	<li>add the get and set methods to the header file interface
	<li>implement a getTYPE method; this usually involves
		<ul>
		<li>changing the type returned in case of error
		<li>changing QVariant::TYPE to your type
		<li>changing return v.data.toTYPE() to your type
		</ul>
	 <li>implement a setTYPE method (usually tou only need to change the method parameters)
		<ul>
		<li>add a case entry to the writeSection() method to serialize the data type to a string
		<li>add an else if entry to the parseSection() method to deserialize the data type from a string
		</ul>
	 </ul>
*/

/*!
	\enum XmlPreferences::BinEncodingType Identifies how binary data should be encoded.

	\value CommaSeparatedValues Separates values using a simple comma
	\value Base64 Encodes binary data using base 64 encoding
*/


/************************************************************************
Public interface
*************************************************************************/

/*!
	Convenience method to access the XmlPreferences singleton.
*/
XmlPreferences& settings()
{
	return XmlPreferences::instance();
}

/*!
	Returns the application unique preferences instance.
*/
XmlPreferences& XmlPreferences::instance()
{
	if (mInstance == 0)
		mInstance = new XmlPreferences();
	return *mInstance;
}

/*!
	Frees any allocated resources.
*/
XmlPreferences::~XmlPreferences()
{
	if (this == mInstance)
		delete d;
	else delete mInstance;
}

/*!
	Returns true if there are no preferences set.
*/
bool XmlPreferences::isEmpty()
{
	return d->data.isEmpty();
}

/*!
	Sets the encoding type to be used with binary data.
	This only affects new items that are added (or modified)
	after calling this method.
	Older items will use the previously set encoding type!
*/
void XmlPreferences::setBinaryEncodingType(BinEncodingType e)
{
	d->encoding = e;
}

//! Returns the encoding type used to encode binary data.
XmlPreferences::BinEncodingType XmlPreferences::binaryEncodingType() const
{
	return d->encoding;
}

//! Returns the last occurred error as an enum value.
XmlPreferences::ErrorCode XmlPreferences::lastErrorCode() const
{
	return d->lastErrorCode;
}

//! Returns the last occurred error as a human readable string.
QString XmlPreferences::lastErrorString() const
{
	switch (d->lastErrorCode)
	{
	case NoError: return QCoreApplication::translate("XmlPreferences", "No error occurred.");
	case FileNotFound: return QCoreApplication::translate("XmlPreferences", "File not found.");
	case FileOpenFailed: return QCoreApplication::translate("XmlPreferences", "Unable to open preferences file.");
	case IncompatibleFileVersion: return QCoreApplication::translate("XmlPreferences", "File was created with an incompatible XmlPreferences version.");
	case IncompatibleProductVersion: return QCoreApplication::translate("XmlPreferences", "Preferences file created with an incompatible version of this application.");
	default: ;
	}

	return QCoreApplication::translate("XmlPreferences", "Unknown error.");
}

/*!
	Attempts to load settings from a file. Old data is not cleared! You will need to explicitly
	call clear() for this purpose.
	Returns false and sets the lastErrorCode property on error.
*/
bool XmlPreferences::load(const QString& filename)
{
	d->lastErrorCode = NoError;

	xmlDocPtr doc = xmlParseFile(filename.toAscii().data());
	if (doc == 0)
		return false;

	xmlNodePtr cur = xmlDocGetRootElement(doc);
	if (cur == 0)
	{
		xmlFreeDoc(doc);
		return false;
	}

	if (xmlStrcmp((cur)->name, (const xmlChar*) "xml-preferences"))
	{
		xmlFreeDoc(doc);
		return false;
	}

	xmlChar* attr = 0;
	QString version;

	attr = xmlGetProp(cur, (const xmlChar*) "xmlp-version");
	if (attr != 0)
	{
		bool ok;
		quint8 xmlPrefV = QString((const char*) attr).toInt(&ok);
		xmlFree(attr);

		if (ok)
		{
			if (xmlPrefV != d->xmlPrefVersion)
			{
				d->lastErrorCode = IncompatibleFileVersion;
				return false;
			}
		}
	}

	// Check product version
	if (d->version != 0)
	{
		quint8 version = 0;
		xmlNodePtr childNode = cur->xmlChildrenNode;

		while (childNode != 0)
		{
			if (xmlStrcmp(childNode->name, (const xmlChar*) "product-info") == 0)
			{
				attr = xmlGetProp(childNode, (const xmlChar*) "version");
				if (attr != 0)
				{
					bool ok;
					version = QString((const char*) attr).toUShort(&ok);
					xmlFree(attr);
					if (!ok)
						version = 0;
					break;
				}
			}

			childNode = childNode->next;
		}

		if (!d->checkApplicationVersion(version))
		{
			xmlFreeDoc(doc);
			d->lastErrorCode = IncompatibleProductVersion;
			return false;
		}
	}

	xmlNodePtr childNode = cur->xmlChildrenNode;
	while (childNode != 0)
	{
		if (xmlStrcmp(childNode->name, (const xmlChar*) "preferences") != 0)
		{
			childNode = childNode->next;
			continue;
		}

		xmlNodePtr prefNode = childNode->xmlChildrenNode;
		while (prefNode != 0)
		{
			if (xmlStrcmp(prefNode->name, (const xmlChar*) "section") != 0)
			{
				prefNode = prefNode->next;
				continue;
			}

			attr = xmlGetProp(prefNode, (const xmlChar*) "name");

			if (attr == 0)
			{
				prefNode = prefNode->next;
				continue;
			}

			// Find or create the right section
			QString sectionName((const char*) attr);
			XmlPreferencesPrivate::SectionTable::Iterator sectionIterator
				= d->data.find(sectionName);

			if (sectionIterator == d->data.end())
			{
				// Create a new section
				XmlPreferencesPrivate::Section section;
				d->parseSection(doc, prefNode, section);
				d->data.insert(sectionName, section);
			}
			else d->parseSection(doc, prefNode, sectionIterator.value());

			xmlFree(attr);

			prefNode = prefNode->next;
		}

		childNode = childNode->next;
	}

	xmlFreeDoc(doc);
	return true;
}

/*!
	Attempts to write the settings to a file.
	Returns false and sets the lastErrorCode property on error.
*/
bool XmlPreferences::save(const QString& filename)
{
	QFile file(filename);
	if (!file.open(QIODevice::WriteOnly))
	{
		d->lastErrorCode = file.exists() ? FileOpenFailed : FileNotFound;
		return false;
	}

	XmlWriter out(&file);
	QHash<QString,QString> attrs;

	attrs.insert("xmlp-version", QString::number(d->xmlPrefVersion));
	out.writeOpenTag("xml-preferences", attrs);
	attrs.clear();

	if (!d->company.isEmpty())
		attrs.insert("company", d->company);
	if (!d->product.isEmpty())
		attrs.insert("product", d->product);
	if (d->version != 0)
		attrs.insert("version", QString::number(d->version));

	if (!attrs.isEmpty())
	{
		out.writeAtomTag("product-info", attrs);
		attrs.clear();
	}

	out.writeOpenTag("preferences", attrs);

	for (XmlPreferencesPrivate::SectionTable::Iterator sectionIterator = d->data.begin();
		sectionIterator != d->data.end(); ++sectionIterator)
	{
		d->writeSection(out, sectionIterator.key(), sectionIterator.value());
	}

	out.writeCloseTag("preferences");
	out.writeCloseTag("xml-preferences");

	return true;
}

/*!
	Returns the string value associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Returns QString() if no such value could be found.
*/
QString XmlPreferences::getString(const QString& name, const QString& section)
{
	return d->getData(name, section).toString();
}

/*!
	Returns the string list associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Returns an empty list if no such value could be found.
*/
QStringList XmlPreferences::getStringList(const QString& name, const QString& section)
{
	return d->getData(name, section).toStringList();
}

/*!
	Returns the boolean value associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Sets \a ok to false if no such value could be found and \a ok is not null.
*/
bool XmlPreferences::getBool(const QString& name, const QString& section, bool* ok)
{
	QVariant v = d->getData(name, section);
	if (ok != 0)
		*ok = v.isValid() && v.type() == QVariant::Bool;
	return v.toBool();
}

/*!
	Returns the integer value associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Sets \a ok to false if no such value could be found and \a ok is not null.
*/
int XmlPreferences::getInt(const QString& name, const QString& section, bool* ok)
{
	QVariant v = d->getData(name, section);
	if (ok != 0)
		*ok = v.isValid() && v.type() == QVariant::Int;
	return v.toInt();
}


/*!
	Returns the 64bit integer value associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Sets \a ok to false if no such value could be found and \a ok is not null.
*/
qint64 XmlPreferences::getInt64(const QString& name, const QString& section, bool* ok)
{
	QVariant v = d->getData(name, section);
	if (ok != 0)
		*ok = v.isValid() && v.type() == QVariant::LongLong;
	return v.toLongLong();
}

/*!
	Returns the bit array associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Returns an empty array if no such value could be found
*/
QBitArray XmlPreferences::getBitArray(const QString& name, const QString& section)
{
	return d->getData(name, section).toBitArray();
}

/*!
	Returns the byte array associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Returns an empty array if no such value could be found
*/
QByteArray XmlPreferences::getByteArray(const QString& name, const QString& section)
{
	return d->getData(name, section).toByteArray();
}

/*!
	Returns the rectangle associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Returns an empty rectangle if no such value could be found
*/
QRect XmlPreferences::getRect(const QString& name, const QString& section)
{
	return d->getData(name, section).toRect();
}

/*!
	Returns the point associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Returns an invalid point if no such value could be found
*/
QPoint XmlPreferences::getPoint(const QString& name, const QString& section)
{
	return d->getData(name, section).toPoint();
}

/*!
	Returns the size associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Returns an invalid size if no such value could be found
*/
QSize XmlPreferences::getSize(const QString& name, const QString& section)
{
	return d->getData(name, section).toSize();
}

#ifndef QT_CORE_ONLY

/*!
	Returns the color associated to \a name in \a section or in the
	default section if the \a section name is empty.
	Returns a null color if no such value could be found
*/
QColor XmlPreferences::getColor(const QString& name, const QString& section)
{
	return d->getData(name, section).value<QColor>();
}

#endif // QT_CORE_ONLY

/*!
	Removes a section and all its data.
	Removes the default section if \p section is empty.
 */
void XmlPreferences::clearSection(const QString& section)
{
	if (d->data.isEmpty())
		return;

	XmlPreferencesPrivate::SectionTable::Iterator sectionIterator
		= d->data.find(section.isEmpty() ? d->defaultSectionName : section);

	if (sectionIterator != d->data.end())
		d->data.erase(sectionIterator);
}

/*!
	Adds a string type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
 */
void XmlPreferences::setString(const QString& name, const QString& value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	d->setData(name, section, v);
}

/*!
	Adds a string list type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
*/
void XmlPreferences::setStringList(const QString& name, const QStringList& value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	d->setData(name, section, v);
}

/*!
	Adds a boolean type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
*/
void XmlPreferences::setBool(const QString& name, bool value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	d->setData(name, section, v);
}

/*!
	Adds an integer type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
*/
void XmlPreferences::setInt(const QString& name, int value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	d->setData(name, section, v);
}

/*!
	Adds a 64bit integer type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
*/
void XmlPreferences::setInt64(const QString& name, qint64 value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	d->setData(name, section, v);
}

/*!
	Adds a bit array integer type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
*/
void XmlPreferences::setBitArray(const QString& name, const QBitArray& value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	v.encoding = d->encoding;
	d->setData(name, section, v);
}

/*!
	Adds a byte array integer type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
*/
void XmlPreferences::setByteArray(const QString& name, const QByteArray& value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	v.encoding = d->encoding;
	d->setData(name, section, v);
}

/*!
	Adds a rectangle type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
*/
void XmlPreferences::setRect(const QString& name, const QRect& value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	d->setData(name, section, v);
}

/*!
	Adds a size type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
*/
void XmlPreferences::setSize(const QString& name, const QSize& value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	d->setData(name, section, v);
}

/*!
	Adds a point type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
*/
void XmlPreferences::setPoint(const QString& name, const QPoint& value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	d->setData(name, section, v);
}

#ifndef QT_CORE_ONLY

/*!
	Adds a color type value to \a section or to the default section if \a section is empty
	and binds it to \a name.
*/
void XmlPreferences::setColor(const QString& name, const QColor& value, const QString& section)
{
	XmlPreferencesPrivate::EncVariant v;
	v.data = QVariant(value);
	d->setData(name, section, v);
}

#endif // QT_CORE_ONLY

/*!
	Clears all the data. This won't remove the name of the default section!
 */
void XmlPreferences::clear()
{
	d->data.clear();
}

/*!
	Sets the product info.
	This is of no real use but if set it will be added at the top of the XML file.
 */
void XmlPreferences::setProductInfo(const QString& company, const QString& product)
{
	d->company = company;
	d->product = product;
}

/*!
	Sets the product version.
 */
void XmlPreferences::setProductVersion(quint8 version)
{
	d->version = version;
}

/*!
	Sets the name of the default section if \a name is not empty.
 */
void XmlPreferences::setDefaultSectionName(const QString& name)
{
	if (!name.isEmpty())
	{
		d->defaultSectionName = name;
	}
}


/************************************************************************
Private interface
*************************************************************************/

//! \internal
XmlPreferences* XmlPreferences::mInstance = 0;

//! \internal Private constructor.
XmlPreferences::XmlPreferences()
{
	d = new XmlPreferencesPrivate;
}

//! \internal
XmlPreferencesPrivate::XmlPreferencesPrivate()
{
	defaultSectionName = "default";
	version = 0;
	ignoreMissingProductVersion = true;
	lastErrorCode = XmlPreferences::NoError;
	encoding = XmlPreferences::Base64;
}

//! \internal
void XmlPreferencesPrivate::writeSection(XmlWriter& out, const QString& name, const Section& section)
{
	QHash<QString,QString> attrs;

	attrs.insert("name", name);
	out.writeOpenTag("section", attrs);
	attrs.clear();

	for (Section::ConstIterator sectionIterator = section.constBegin();
		sectionIterator != section.constEnd(); ++sectionIterator)
	{
		const EncVariant& v = sectionIterator.value();

		attrs.insert("name", sectionIterator.key());

		switch (v.data.type())
		{
		case QVariant::String :
			attrs.insert("type", "string");
			out.writeTaggedString("setting", v.data.toString(), attrs);
			break;
		case QVariant::StringList :
			{
				attrs.insert("type", "stringlist");
				out.writeOpenTag("setting", attrs);
				attrs.clear();
				QStringList list = v.data.toStringList();
				for (int i = 0; i < list.size(); ++i)
					out.writeTaggedString("value", list.at(i), attrs);
				out.writeCloseTag("setting");
			}
			break;
		case QVariant::Bool :
			attrs.insert("type", "bool");
			out.writeTaggedString("setting", v.data.toBool() ? "true" : "false", attrs);
			break;
		case QVariant::Int :
			attrs.insert("type", "int");
			out.writeTaggedString("setting", QString::number(v.data.toInt()), attrs);
			break;
		case QVariant::LongLong :
			attrs.insert("type", "int64");
			out.writeTaggedString("setting", QString::number(v.data.toLongLong()), attrs);
			break;
		case QVariant::Rect :
			{
				attrs.insert("type", "rect");
				QRect rect = v.data.toRect();
				QString s = QString("%1;%2;%3;%4").arg(rect.x()).arg(rect.y()).arg(rect.width()).arg(rect.height());
				out.writeTaggedString("setting", s, attrs);
			}
			break;
		case QVariant::Point :
			{
				attrs.insert("type", "point");
				QPoint point = v.data.toPoint();
				QString s = QString("%1;%2").arg(point.x()).arg(point.y());
				out.writeTaggedString("setting", s, attrs);
			}
			break;
		case QVariant::Size :
			{
				attrs.insert("type", "size");
				QSize size = v.data.toSize();
				QString s = QString("%1;%2").arg(size.width()).arg(size.height());
				out.writeTaggedString("setting", s, attrs);
			}
			break;
		case QVariant::ByteArray :
			{
				attrs.insert("type", "bytearray");
				const QByteArray ba = v.data.toByteArray();
				switch (v.encoding)
				{
				case XmlPreferences::Base64:
					attrs.insert("encoding", "base64");
					out.writeTaggedString("setting", Base64::encode(ba), attrs);
					break;
				default:
					attrs.insert("encoding", "csv");
					QString s;
					for (uint i = 0; i < (uint) ba.size(); ++i)
						(i != 0) ? s += "," + QString::number((uint)ba.at(i), 16) : s += QString::number((uint)ba.at(i), 16);
					out.writeTaggedString("setting", s, attrs);
				}
				attrs.clear();
			}
			break;
		case QVariant::BitArray :
			{
				attrs.insert("type", "bitarray");
				const QBitArray ba = v.data.toBitArray();
				attrs.insert("size", QString::number(ba.size()));
				switch (v.encoding)
				{
				case XmlPreferences::Base64:
					attrs.insert("encoding", "base64");
					out.writeTaggedString("setting", Base64::encode(ba), attrs);
					break;
				default:
					attrs.insert("encoding", "csv");
					QString s;
					for (uint i = 0; i < (uint) ba.size(); ++i)
						(i != 0) ? s += ba.testBit(i) ? ",1" : ",0" : s += ba.testBit(i) ? "1" : "0";
					out.writeTaggedString("setting", s, attrs);
				}
				attrs.clear();
			}
			break;
#ifndef QT_CORE_ONLY
		case QVariant::Color :
			attrs.insert("type", "color");
			out.writeTaggedString("setting", v.data.value<QColor>().name(), attrs);
			break;
#endif // QT_CORE_ONLY
		default:
			;
		}
	}

	out.writeCloseTag("section");
}

//! \internal
bool XmlPreferencesPrivate::checkApplicationVersion(quint8 v)
{
	if (v == 0)
		return ignoreMissingProductVersion;

	return version == v;
}

//! \internal
void XmlPreferencesPrivate::parseSection(xmlDocPtr doc, xmlNodePtr rootNode, Section& section)
{
	xmlNodePtr cur = rootNode->xmlChildrenNode;
	xmlChar* attr = 0;

	while (cur != 0)
	{
		if (xmlStrcmp(cur->name, (const xmlChar*) "setting") != 0)
		{
			cur = cur->next;
			continue;
		}

		attr = xmlGetProp(cur, (const xmlChar*) "name");
		if (attr == 0)
		{
			cur = cur->next;
			continue;
		}

		QString name((const char*) attr);
		xmlFree(attr);

		attr = xmlGetProp(cur, (const xmlChar*) "type");
		if (attr == 0)
		{
			cur = cur->next;
			continue;
		}

		QString type((const char*) attr);
		xmlFree(attr);

		if (type == "string")
		{
			xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (txt != 0)
			{
				EncVariant v;
				v.data = QVariant( QString( (const char*)txt ) );
				section.insert(name, v);
				xmlFree(txt);
			}
		}
		else if (type == "stringlist")
		{
			xmlNodePtr listNode = cur->xmlChildrenNode;

			QString listText;
			QStringList list;

			while (listNode != 0)
			{
				xmlChar* txt = xmlNodeListGetString(doc, listNode->xmlChildrenNode, 1);
				if (txt != 0)
				{
					list.append( QString((const char*) txt) );
					xmlFree(txt);
				}

				listNode = listNode->next;
			}

			EncVariant v;
			v.data = QVariant(list);
			section.insert(name, v);
		}
		else if (type == "bool")
		{
			xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (txt != 0)
			{
				EncVariant v;
				v.data = QVariant(xmlStrcmp(txt, (const xmlChar*)"true") == 0);
				section.insert(name, v);
				xmlFree(txt);
			}
		}
		else if (type == "int")
		{
			xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (txt != 0)
			{
				QString n( (const char*) txt );
				xmlFree(txt);

				bool ok;
				int i = n.toInt(&ok);

				if (ok)
				{
					EncVariant v;
					v.data = QVariant(i);
					section.insert(name, v);
				}
			}
		}
		else if (type == "int64")
		{
			xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (txt != 0)
			{
				QString n( (const char*) txt );
				xmlFree(txt);

				bool ok;
				qlonglong i = n.toLongLong(&ok);

				if (ok)
				{
					EncVariant v;
					v.data = QVariant(i);
					section.insert(name, v);
				}
			}
		}
		else if (type == "rect")
		{
			xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (txt != 0)
			{
				QString n( (const char*) txt );
				xmlFree(txt);

				QStringList lst = n.split(';', QString::KeepEmptyParts);

				// a qrect has 4 values: xpos, ypos, width and height
				if (lst.size() == 4)
				{
					int x, y, w, h;
					int count = 0;
					bool ok;

					x = lst[0].toInt(&ok);
					if (ok) ++count;
					y = lst[1].toInt(&ok);
					if (ok) ++count;
					w = lst[2].toInt(&ok);
					if (ok) ++count;
					h = lst[3].toInt(&ok);
					if (ok) ++count;

					if (count == 4)
					{
						EncVariant v;
						v.data = QVariant(QRect(x,y,w,h));
						section.insert(name, v);
					}
				}
			}
		}
		else if (type == "point")
		{
			xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (txt != 0)
			{
				QString n( (const char*) txt );
				xmlFree(txt);

				QStringList lst = n.split(';', QString::KeepEmptyParts);

				// a qrect has 2 values, the x and y coordinates
				if (lst.size() == 2)
				{
					int x, y;
					int count = 0;
					bool ok;

					x = lst[0].toInt(&ok);
					if (ok) ++count;
					y = lst[1].toInt(&ok);
					if (ok) ++count;

					if (count == 2)
					{
						EncVariant v;
						v.data = QVariant(QPoint(x,y));
						section.insert(name, v);
					}
				}
			}
		}
		else if (type == "size")
		{
			xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (txt != 0)
			{
				QString n( (const char*) txt );
				xmlFree(txt);

				QStringList lst = n.split(';', QString::KeepEmptyParts);

				// a qrect has 2 values: width and height
				if (lst.size() == 2)
				{
					int w, h;
					int count = 0;
					bool ok;

					w = lst[0].toInt(&ok);
					if (ok) ++count;
					h = lst[1].toInt(&ok);
					if (ok) ++count;

					if (count == 2)
					{
						EncVariant v;
						v.data = QVariant(QSize(w,h));
						section.insert(name, v);
					}
				}
			}
		}
		else if (type == "bytearray")
		{
			xmlChar* attr = xmlGetProp(cur, (const xmlChar*)"encoding");
			if (attr != 0)
			{
				if (xmlStrcmp(attr, (const xmlChar*)"base64") == 0)
				{
					xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
					if (txt != 0)
					{
						QString n( (const char*) txt );
						xmlFree(txt);

						QByteArray ba = Base64::decode(n);
						EncVariant v;
						v.data = QVariant(ba);
						v.encoding = XmlPreferences::Base64;
						section.insert(name, v);
					}
				}
				else if (xmlStrcmp(attr, (const xmlChar*)"csv") == 0)
				{
					xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
					if (txt != 0)
					{
						QString n( (const char*) txt );
						xmlFree(txt);

						QStringList lst = n.split(',', QString::SkipEmptyParts);
						uint sz = lst.size();
						QByteArray ba(sz, '\0');
						bool ok;
						uint count = 0;
						for (QStringList::ConstIterator itr=lst.constBegin(); itr!=lst.constEnd(); ++itr)
						{
							uint n = (*itr).toUInt(&ok,16);
							if (ok) ba.data()[count++] = (uchar)n;
						}
						if (count < sz) ba.resize(count);
						EncVariant v;
						v.data = QVariant(ba);
						v.encoding = XmlPreferences::CommaSeparatedValues;
						section.insert(name, v);
					}
				}

				xmlFree(attr);
			}
		}
		else if (type == "bitarray")
		{
			QString n;

			xmlChar* attr = xmlGetProp(cur, (const xmlChar*)"size");
			if (attr != 0)
			{
				QString n((const char*)attr);
				xmlFree(attr);

				bool ok;
				uint arr_sz = n.toUInt(&ok);

				if (!ok)
				{
					cur = cur->next;
					continue;
				}

				attr = xmlGetProp(cur, (const xmlChar*)"encoding");
				if (attr == 0)
				{
					cur = cur->next;
					continue;
				}

				if (xmlStrcmp(attr, (const xmlChar*)"base64") == 0)
				{
					xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
					if (txt != 0)
					{
						QString n( (const char*) txt );
						xmlFree(txt);

						QBitArray ba = Base64::decode(n, arr_sz);
						EncVariant v;
						v.data = QVariant(ba);
						v.encoding = XmlPreferences::Base64;
						section.insert(name, v);
					}
				}
				else if (xmlStrcmp(attr, (const xmlChar*)"csv") == 0)
				{
					xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
					if (txt != 0)
					{
						QString n( (const char*) txt );
						xmlFree(txt);

						QStringList lst = n.split(',', QString::SkipEmptyParts);
						uint sz = lst.size();
						if (arr_sz < sz) sz = arr_sz;
						QBitArray ba(sz);
						uint count = 0;
						for (QStringList::ConstIterator itr=lst.constBegin(); itr!=lst.constEnd(); ++itr)
						{
							if (*itr == "1") ba.setBit(count++,true);
							else if (*itr == "0") ba.setBit(count++,false);
						}
						if (count < sz)
							ba.resize(count);
						EncVariant v;
						v.data = QVariant(ba);
						v.encoding = XmlPreferences::CommaSeparatedValues;
						section.insert(name, v);
					}
				}

				xmlFree(attr);
			}
		}
#ifndef QT_CORE_ONLY
		else if (type == "color")
		{
			xmlChar* txt = xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);
			if (txt != 0)
			{
				QString n( (const char*) txt );
				xmlFree(txt);

				QColor col(n);
				if (col.isValid())
				{
					EncVariant v;
					v.data = QVariant(col);
					section.insert(name, v);
				}
			}
		}
#endif // QT_CORE_ONLY

		cur = cur->next;

	} // while
}

//! \internal Returns a preferences value as a QVariant.
QVariant XmlPreferencesPrivate::getData(const QString& name, const QString& section)
{
	if (name.isEmpty())
		return QVariant();

	SectionTable::Iterator sectionIterator = data.find(section.isEmpty() ? defaultSectionName : section);

	if (sectionIterator == data.end())
		return QVariant();

	Section& sec = sectionIterator.value();

	Section::Iterator itemIterator = sec.find(name);
	if (itemIterator == sec.end())
		return QVariant();

	return itemIterator.value().data;
}

//! \internal
void XmlPreferencesPrivate::setData(const QString& name, const QString& section, const EncVariant& ev)
{
	if (name.isEmpty())
		return;

	QString actualSectionName = section.isEmpty() ? defaultSectionName : section;

	SectionTable::Iterator sectionIterator = data.find(actualSectionName);
	if (sectionIterator == data.end())
	{
		Section newSection;
		sectionIterator = data.insert(actualSectionName, newSection);
	}

	sectionIterator.value().insert(name, ev);
}
