/****************************************************************************
** Filename: ratinglabel.h
** Last updated [dd/mm/yyyy]: 01/03/2007
**
** QWidget subclass, used to draw a 'rating'-like image sequence.
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
**********************************************************************/

#ifndef OSDAB_RATINGLABEL_H
#define OSDAB_RATINGLABEL_H

#include <QWidget>
#include <QPixmap>

class RatingLabel_P;
class QPaintEvent;
class QMouseEvent;

class RatingLabel : public QWidget
{
	Q_OBJECT

public:
	enum PixmapRole { RatedRole, UnratedRole, HoveredRole };

	RatingLabel(QWidget* parent = 0);
	virtual ~RatingLabel();

	int value() const;
	int minimum() const;
	int maximum() const;

	QPixmap pixmap(PixmapRole role) const;
	void setPixmap(PixmapRole role, const QPixmap& pm);

	QSize sizeHint() const;
	QSize minimumSizeHint() const;

protected:
	virtual void paintEvent(QPaintEvent*);
	virtual void mouseMoveEvent(QMouseEvent*);
	virtual void leaveEvent(QEvent*);
	virtual void mousePressEvent(QMouseEvent*);
	virtual void mouseReleaseEvent(QMouseEvent*);

public slots:
	void setValue(int value);
	void setMinimum(int value);
	void setMaximum(int value);

signals:
	void valueChanged(int value);
	//! Emitted when a pixmap is hovered or un-hovered (-1).
	void hovered(int value);

private:
	RatingLabel_P* d;
};

#endif // OSDAB_RATINGLABEL_H
