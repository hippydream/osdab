/****************************************************************************
** Filename: TestBase64.h
** Last updated [dd/mm/yyyy]: 09/06/2006
**
** Simple dialog for testing of the Base64 class.
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

#ifndef TESTBASE64_H
#define TESTBASE64_H

#include <QtGui/QDialog>

class QVBoxLayout;
class QHBoxLayout;
class QGridLayout;
class QPushButton;
class QTextEdit;
class QLabel;
class QCheckBox;

class TestBase64 : public QDialog
{
	Q_OBJECT

public:
	TestBase64(QWidget* parent = 0);
	~TestBase64();

private:
	QPushButton* mB_Dec;
	QPushButton* mB_Enc;
	QTextEdit* mTE_Dec;
	QLabel* mL_Title;
	QCheckBox* mCB_Crop;
	QGridLayout* Base64Layout;

private slots:
	virtual void decode();
	virtual void encode();
};

#endif // TESTBASE64_H
