/**************************************************************************
** Filename: mainwindow.h
**
** Copyright (C) 2007-2016 Angius Fabrizio. All rights reserved.
** Based on the "icons" example bundled with Qt 4
**   Copyright (C) 2005-2007 Trolltech ASA. All rights reserved.
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

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QIcon>
#include <QList>
#include <QMainWindow>
#include <QPixmap>

class QAction;
class QActionGroup;
class QGroupBox;
class QMenu;
class QRadioButton;
class QTableWidget;
class IconPreviewArea;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow();

private slots:
    void about();
    void changeStyle(bool checked);
    void changeSize();
    void loadImage();

private:
    void createPreviewGroupBox();
    void createActions();
    void createMenus();
    void checkCurrentStyle();

    QWidget *centralWidget;

    QGroupBox *previewGroupBox;
    IconPreviewArea *previewArea;

    QMenu *fileMenu;
    QMenu *viewMenu;
    QMenu *sizeMenu;
    QMenu *helpMenu;
    QAction *loadImageAct;
    QAction *exitAct;
    QActionGroup *styleActionGroup;
    QActionGroup *sizeActionGroup;
    QAction *aboutAct;
    QAction *aboutQtAct;
};

#endif
