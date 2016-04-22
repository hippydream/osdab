/**************************************************************************
** Filename: iconpreviewarea.cpp
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

IconPreviewArea::IconPreviewArea(QWidget *parent)
    : QWidget(parent)
{
    QGridLayout *mainLayout = new QGridLayout;
    setLayout(mainLayout);

    stateLabels[0] = createHeaderLabel(tr("Off"));
    stateLabels[1] = createHeaderLabel(tr("On"));
    Q_ASSERT(NumStates == 2);

    modeLabels[0] = createHeaderLabel(tr("Normal"));
    modeLabels[1] = createHeaderLabel(tr("Active"));
    modeLabels[2] = createHeaderLabel(tr("Disabled"));
    modeLabels[3] = createHeaderLabel(tr("Selected"));
    Q_ASSERT(NumModes == 4);

    for (int j = 0; j < NumStates; ++j)
        mainLayout->addWidget(stateLabels[j], j + 1, 0);

    for (int i = 0; i < NumModes; ++i) {
        mainLayout->addWidget(modeLabels[i], 0, i + 1);

        for (int j = 0; j < NumStates; ++j) {
            pixmapLabels[i][j] = createPixmapLabel();
            mainLayout->addWidget(pixmapLabels[i][j], j + 1, i + 1);
        }
    }
}

void IconPreviewArea::setIcon(const QIcon &icon)
{
    this->icon = icon;
    updatePixmapLabels();
}

void IconPreviewArea::setSize(const QSize &size)
{
    if (size != this->size) {
        this->size = size;
        updatePixmapLabels();
    }
}

QLabel *IconPreviewArea::createHeaderLabel(const QString &text)
{
    QLabel *label = new QLabel(tr("<b>%1</b>").arg(text));
    label->setAlignment(Qt::AlignCenter);
    return label;
}

QLabel *IconPreviewArea::createPixmapLabel()
{
    QLabel *label = new QLabel;
    label->setEnabled(false);
    label->setAlignment(Qt::AlignCenter);
    label->setFrameShape(QFrame::Box);
    label->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    label->setBackgroundRole(QPalette::Base);
    label->setAutoFillBackground(true);
    label->setMinimumSize(132, 132);
    return label;
}

void IconPreviewArea::updatePixmapLabels()
{
    for (int i = 0; i < NumModes; ++i) {
        QIcon::Mode mode;
        if (i == 0) {
            mode = QIcon::Normal;
        } else if (i == 1) {
            mode = QIcon::Active;
        } else if (i == 2) {
            mode = QIcon::Disabled;
        } else {
            mode = QIcon::Selected;
        }

        for (int j = 0; j < NumStates; ++j) {
            QIcon::State state = (j == 0) ? QIcon::Off : QIcon::On;
            QPixmap pixmap = icon.pixmap(size, mode, state);
            pixmapLabels[i][j]->setPixmap(pixmap);
            pixmapLabels[i][j]->setEnabled(!pixmap.isNull());
        }
    }
}
