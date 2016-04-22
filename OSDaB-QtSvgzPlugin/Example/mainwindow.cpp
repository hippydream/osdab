/**************************************************************************
** Filename: mainwindow.cpp
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

#include <QtGui>

#include "iconpreviewarea.h"
#include "mainwindow.h"
#include <QStyle>

MainWindow::MainWindow()
{
    centralWidget = new QWidget;
    setCentralWidget(centralWidget);

    createPreviewGroupBox();

    createActions();
    createMenus();

    QGridLayout *mainLayout = new QGridLayout;
    mainLayout->addWidget(previewGroupBox, 0, 0, 1, 2);
    centralWidget->setLayout(mainLayout);
    int sz = QApplication::style()->pixelMetric(QStyle::PM_LargeIconSize);
    previewArea->setSize(QSize(sz, sz));

    setWindowTitle(tr("QtSvgzPlugin Example Application"));
	setWindowIcon(QIcon(":/images/icon.svgz"));
    checkCurrentStyle();

    resize(minimumSizeHint());
}

void MainWindow::about()
{
    QMessageBox::about(this, tr("About QtSvgzPlugin"),
            tr("This applications illustrates how the <b>QtSvgzPlugin</b> plugin "
               "renders compressed SVG icons thanks to Qt4's QtSvg module and "
               "the zlib library (included in the Qt libraries)."));
}


void MainWindow::changeSize()
{
    QAction *action = qobject_cast<QAction *>(sender());
    int sz = action->data().toInt();
    sz = QApplication::style()->pixelMetric((QStyle::PixelMetric)sz);
    previewArea->setSize(QSize(sz, sz));
}

void MainWindow::changeStyle(bool checked)
{
    if (!checked)
        return;

    QAction *action = qobject_cast<QAction *>(sender());
    QStyle *style = QStyleFactory::create(action->data().toString());
    Q_ASSERT(style);
    QApplication::setStyle(style);
}

void MainWindow::loadImage()
{
    QString fileName = QFileDialog::getOpenFileName(this,
                                    tr("Open Image"), "",
                                    tr("Compressed SVG (*.svgz);;"
                                       "All Files (*)"));
    if (!fileName.isEmpty()) {
        previewArea->setIcon(QIcon(fileName));
    }
}

void MainWindow::createPreviewGroupBox()
{
    previewGroupBox = new QGroupBox(tr("Preview"));

    previewArea = new IconPreviewArea;

    QVBoxLayout *layout = new QVBoxLayout;
    layout->addWidget(previewArea);
    previewGroupBox->setLayout(layout);
}

void MainWindow::createActions()
{
	loadImageAct = new QAction(QIcon(":/images/document-open.svgz"), tr("&Open Image..."), this);
    loadImageAct->setShortcut(tr("Ctrl+O"));
    connect(loadImageAct, SIGNAL(triggered()), this, SLOT(loadImage()));

	exitAct = new QAction(QIcon(":/images/application-exit.svgz"), tr("&Quit"), this);
    exitAct->setShortcut(tr("Ctrl+Q"));
    connect(exitAct, SIGNAL(triggered()), this, SLOT(close()));

    styleActionGroup = new QActionGroup(this);
    foreach (QString styleName, QStyleFactory::keys()) {
        QAction *action = new QAction(styleActionGroup);
        action->setText(tr("%1 Style").arg(styleName));
        action->setData(styleName);
        action->setCheckable(true);
        connect(action, SIGNAL(triggered(bool)), this, SLOT(changeStyle(bool)));
    }

    /*
        PM_SmallIconSize
        PM_LargeIconSize
        PM_ToolBarIconSize
        PM_ListViewIconSize
        PM_IconViewIconSize
        PM_TabBarIconSize
    */
    sizeActionGroup = new QActionGroup(this);
    QAction* a = new QAction(sizeActionGroup);
    a->setText(tr("Small icon size"));
    a->setData((int)QStyle::PM_SmallIconSize);
    a->setCheckable(true);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(changeSize()));

    a = new QAction(sizeActionGroup);
    a->setText(tr("Large icon size"));
    a->setData((int)QStyle::PM_LargeIconSize);
    a->setCheckable(true);
    a->setChecked(true); // DEFAULT
    connect(a, SIGNAL(triggered(bool)), this, SLOT(changeSize()));

    a = new QAction(sizeActionGroup);
    a->setText(tr("Tool bar icon size"));
    a->setData((int)QStyle::PM_ToolBarIconSize);
    a->setCheckable(true);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(changeSize()));

    a = new QAction(sizeActionGroup);
    a->setText(tr("List view icon size"));
    a->setData((int)QStyle::PM_ListViewIconSize);
    a->setCheckable(true);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(changeSize()));

    a = new QAction(sizeActionGroup);
    a->setText(tr("Icon view icon size"));
    a->setData((int)QStyle::PM_IconViewIconSize);
    a->setCheckable(true);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(changeSize()));

    a = new QAction(sizeActionGroup);
    a->setText(tr("Tab bar icon size"));
    a->setData((int)QStyle::PM_TabBarIconSize);
    a->setCheckable(true);
    connect(a, SIGNAL(triggered(bool)), this, SLOT(changeSize()));

    aboutAct = new QAction(tr("&About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(about()));

    aboutQtAct = new QAction(tr("About &Qt"), this);
    connect(aboutQtAct, SIGNAL(triggered()), qApp, SLOT(aboutQt()));
}

void MainWindow::createMenus()
{
    fileMenu = menuBar()->addMenu(tr("&File"));
    fileMenu->addAction(loadImageAct);
    fileMenu->addSeparator();
    fileMenu->addAction(exitAct);

    viewMenu = menuBar()->addMenu(tr("&View"));
    foreach (QAction *action, styleActionGroup->actions())
        viewMenu->addAction(action);

    sizeMenu = menuBar()->addMenu(tr("&Size"));
    foreach (QAction *action, sizeActionGroup->actions())
        sizeMenu->addAction(action);

    menuBar()->addSeparator();

    helpMenu = menuBar()->addMenu(tr("&Help"));
    helpMenu->addAction(aboutAct);
    helpMenu->addAction(aboutQtAct);
}

void MainWindow::checkCurrentStyle()
{
    foreach (QAction *action, styleActionGroup->actions()) {
        QString styleName = action->data().toString();
        QStyle *candidate = QStyleFactory::create(styleName);
        Q_ASSERT(candidate);
        if (candidate->metaObject()->className()
                == QApplication::style()->metaObject()->className()) {
            action->trigger();
            return;
        }
        delete candidate;
    }
}
