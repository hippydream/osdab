/**************************************************************************
** Filename: qtsvgz_plugin.h
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
**************************************************************************/

#ifndef QTSVGZ_PLUGIN_H
#define QTSVGZ_PLUGIN_H

#include <QIconEnginePluginV2>

class QtSvgzIconEnginePlugin : public QIconEnginePluginV2
{
	Q_OBJECT

public:
	QtSvgzIconEnginePlugin(QObject* parent = 0);
	virtual ~QtSvgzIconEnginePlugin();

	virtual QIconEngineV2* create(const QString& filename = QString());
	virtual QStringList keys() const;
};

#endif // QTSVGZ_PLUGIN_H
