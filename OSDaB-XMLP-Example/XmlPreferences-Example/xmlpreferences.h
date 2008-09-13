/****************************************************************************
** Filename: xmlpreferences.h
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

#ifndef OSDAB_XMLPREFERENCES__H
#define OSDAB_XMLPREFERENCES__H

#include <QtGlobal>
#include <QString>

class QIODevice;
class QByteArray;
class QBitArray;
class QStringList;
class QRect;
class QPoint;
class QSize;

#ifndef QT_CORE_ONLY
class QColor;
#endif

class XmlPreferencesPrivate;

class XmlPreferences
{
public:
	enum BinEncodingType { CommaSeparatedValues, Base64 };

	enum ErrorCode
	{
		NoError = 0,
		FileNotFound,
		FileOpenFailed,
		IncompatibleFileVersion,
		IncompatibleProductVersion
	};

	static XmlPreferences& instance();
	virtual ~XmlPreferences();


	/************************************************************************
	 Misc methods
	*************************************************************************/

	bool isEmpty();

	bool load(const QString& file);
	bool save(const QString& file);

	void clear();
	void clearSection(const QString& section = QString());

	void setProductInfo(const QString& company, const QString& product);
	void setProductVersion(quint8 version);

	void setIgnoreMissingProductInfo(bool ignore);
	void setIgnoreMissingProductVersion(bool ignore);

	void setDefaultSectionName(const QString& name);

	bool autoAddSections() const;
	void setAutoAddSections(bool enable);

	void setBinaryEncodingType(BinEncodingType encoding);
	BinEncodingType binaryEncodingType() const;

	ErrorCode lastErrorCode() const;
	QString lastErrorString() const;


	/************************************************************************
	 Getters
	*************************************************************************/

	QString getString(const QString& name, const QString& section = QString());
	QStringList getStringList(const QString& name, const QString& section = QString());
	bool getBool(const QString& name, const QString& section = QString(), bool* ok = 0);
	int getInt(const QString& name, const QString& section = QString(), bool* ok = 0);
	qint64 getInt64(const QString& name, const QString& section = QString(), bool* ok = 0);
	QBitArray getBitArray(const QString& name, const QString& section = QString());
	QByteArray getByteArray(const QString& name, const QString& section = QString());
	QRect getRect(const QString& name, const QString& section = QString());
	QPoint getPoint(const QString& name, const QString& section = QString());
	QSize getSize(const QString& name, const QString& section = QString());

#ifndef QT_CORE_ONLY
	QColor getColor(const QString& name, const QString& section = QString());
#endif


	/************************************************************************
	 Setters
	*************************************************************************/

	void setString(const QString& name, const QString& value, const QString& section = QString());
	void setStringList(const QString& name, const QStringList& value, const QString& section = QString());
	void setBool(const QString& name, bool value, const QString& section = QString());
	void setInt(const QString& name, int value, const QString& section = QString());
	void setInt64(const QString& name, qint64 value, const QString& section = QString());
	void setBitArray(const QString& name, const QBitArray& value, const QString& section = QString());
	void setByteArray(const QString& name, const QByteArray& value, const QString& section = QString());
	void setRect(const QString& name, const QRect& value, const QString& section = QString());
	void setSize(const QString& name, const QSize& value, const QString& section = QString());
	void setPoint(const QString& name, const QPoint& value, const QString& section = QString());

#ifndef QT_CORE_ONLY
	void setColor(const QString& name, const QColor& value, const QString& section = QString());
#endif

private:
	static XmlPreferences* mInstance;
	XmlPreferences();
	XmlPreferencesPrivate* d;
};

XmlPreferences& settings();

#endif // OSDAB_XMLPREFERENCES__H
