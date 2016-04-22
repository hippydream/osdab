/**************************************************************************
** Filename: qtsvgz_plugin.cpp
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

#include "qtsvgz_plugin.h"
#include "qtsvgz_engine.h"

QtSvgzIconEnginePlugin::QtSvgzIconEnginePlugin(QObject* parent)
: QIconEnginePluginV2(parent)
{

}

QtSvgzIconEnginePlugin::~QtSvgzIconEnginePlugin()
{
}

QIconEngineV2* QtSvgzIconEnginePlugin::create(const QString& filename)
{
	QtSvgzIconEngine* engine = new QtSvgzIconEngine;
	if (!filename.isNull())
		engine->addFile(filename, QSize(), QIcon::Normal, QIcon::On);
	return engine;
}

QStringList QtSvgzIconEnginePlugin::keys() const
{
	return QStringList() << QLatin1String("svgz");
}

Q_EXPORT_PLUGIN2(QtSvgzIconEnginePlugin, QtSvgzIconEnginePlugin)