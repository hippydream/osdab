/****************************************************************************
** Filename: ratingform.h
** Last updated [dd/mm/yyyy]: 01/03/2007
**
** Test routine for the RatingLabel class.
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

#ifndef OSDAB_RATINGFORM_H
#define OSDAB_RATINGFORM_H

#include "ui_ratingform.h"

class RatingForm : public QWidget, protected Ui::RatingForm
{
	Q_OBJECT

public:
	RatingForm(QWidget* parent = 0) : QWidget(parent)
	{
		setupUi(this);
		setWindowTitle("OSDaB RatingLabel");

		rating->setMaximum(5);
		rating->setPixmap(RatingLabel::RatedRole, QPixmap(":/img/rated.png"));
		rating->setPixmap(RatingLabel::UnratedRole, QPixmap(":/img/unrated.png"));
		rating->setPixmap(RatingLabel::HoveredRole, QPixmap(":/img/hovered.png"));

		// Initialize the QLabel status text.
		ratingHovered(-1);

		connect (status, SIGNAL(linkActivated(const QString&)), this, SLOT(linkActivated(const QString&)) );
		connect (rating, SIGNAL(hovered(int)), this, SLOT(ratingHovered(int)) );
	}

private slots:
	void linkActivated(const QString& url)
	{
		// We only have one label and one url here ;)
		Q_UNUSED(url);

		rating->setValue(0);
	}

	void ratingHovered(int rating)
	{
		switch (rating)
		{
		case 1: status->setText(tr("1 - Awful movie.")); break;
		case 2: status->setText(tr("2 - Disappointing movie.")); break;
		case 3: status->setText(tr("3 - Mediocre movie.")); break;
		case 4: status->setText(tr("4 - Good movie.")); break;
		case 5: status->setText(tr("5 - Outstanding movie.")); break;
		default: status->setText(tr("Click on a star to set the desired rating or <a href='movida://clear/rating'>click here</a> to clear it."));
		}
	}
};

#endif // OSDAB_RATINGFORM_H
