/**************************************************************************
** Filename: main.cpp
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
#include "mainwindow.h"
#include <QApplication>

// Remember to add this tho your startup file and to link both QtSvg and QtSvgzPlugin.
Q_IMPORT_PLUGIN(QtSvgzIconEnginePlugin)

int main(int argc, char** argv)
{
	QApplication app(argc, argv);
	MainWindow mainWin;
	mainWin.show();
	return app.exec();
}