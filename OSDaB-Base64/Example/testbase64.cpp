/****************************************************************************
** Filename: TestBase64.cpp
** Last updated [dd/mm/yyyy]: 29/09/2005
**
** Simple dialog for testing of the TestBase64 class.
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

#include "testbase64.h"
#include "base64.h"

#include <QFile>
#include <QDataStream>
#include <QByteArray>
#include <QString>

#include <QLabel>
#include <QFileDialog>
#include <QCheckBox>
#include <QPushButton>
#include <QTextEdit>
#include <QLayout>
#include <QToolTip>
#include <QWhatsThis>
#include <QImage>
#include <QPixmap>
#include <QMessageBox>


TestBase64::TestBase64(QWidget* parent)
: QDialog(parent, 0)
{
	setWindowTitle( "Base64 test dialog" );
	setWindowIcon(QIcon(":/base64.png"));

	Base64Layout = new QGridLayout(this);
    Base64Layout->setSizeConstraint(QLayout::SetFixedSize);

    mL_Title = new QLabel(this);
    Base64Layout->addWidget(mL_Title, 0, 0);

	mCB_Crop = new QCheckBox(this);
	Base64Layout->addWidget(mCB_Crop, 1, 0);

    mB_Dec = new QPushButton(this);
    Base64Layout->addWidget(mB_Dec, 2, 0);

    mB_Enc = new QPushButton(this);
    Base64Layout->addWidget(mB_Enc, 3, 0);


    resize( QSize(424, 112).expandedTo(minimumSizeHint()) );

	mL_Title->setText("Hit ENCODE or DECODE and choose an input and an output file");
	mB_Dec->setText("DECODE");
	mB_Enc->setText("ENCODE");
	mCB_Crop->setText("max 76 chars per line (as for RFC 2045)");

    // signals and slots connections
    connect( mB_Dec, SIGNAL( clicked() ), this, SLOT( decode() ) );
    connect( mB_Enc, SIGNAL( clicked() ), this, SLOT( encode() ) );
}

TestBase64::~TestBase64()
{
}

void TestBase64::decode()
{
	QString inName = QFileDialog::getOpenFileName();
	if (inName.isEmpty()) return;

	QFile inFile(inName);
	if (!inFile.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this, "Base64 test dialog", "Failed to open input file!");
		return;
	}

	QByteArray in = inFile.readAll();
	QString txt(in);

	if (txt.isEmpty())
	{
		QMessageBox::warning(this, "Base64 test dialog", "Empty input file!");
		return;
	}

	QString fname = QFileDialog::getSaveFileName();
	if (fname.isEmpty()) return;

	QFile file(fname);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(this, "Base64 test dialog", "Failed to open output file!");
		return;
	}

	QDataStream out(&file);
	QByteArray ba = Base64::decode(txt);
	out.writeRawData(ba, ba.size());

	inFile.close();
	file.close();
}

void TestBase64::encode()
{
	QString inName = QFileDialog::getOpenFileName();
	if (inName.isEmpty()) return;

	QFile inFile(inName);
	if (!inFile.open(QIODevice::ReadOnly))
	{
		QMessageBox::warning(this, "Base64 test dialog", "Failed to open input file!");
		return;
	}

	QString fname = QFileDialog::getSaveFileName();
	if (fname.isEmpty()) return;

	QFile file(fname);
	if (!file.open(QIODevice::WriteOnly))
	{
		QMessageBox::warning(this, "Base64 test dialog", "Failed to open output file!");
		return;
	}

	QByteArray in = inFile.readAll();
	QString txt = Base64::encode(in,mCB_Crop->isChecked());

	QDataStream out(&file);
	QByteArray bo = txt.toAscii().data();
	out.writeRawData(bo, bo.size());
	out.unsetDevice();
	file.close();
	inFile.close();
}
