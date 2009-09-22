/*
	qutIM LaTeX Plugin

	Copyright (c) 2009 by Skorobogatov Michail <hronus-qt@yandex.ru>

 ***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************
*/

#include "settings.h"

#include <QFileDialog>

Settings::Settings(QWidget *parent) :
	QWidget(parent)
{
	m_ui.setupUi(this);

	connect( m_ui.latexButton, SIGNAL(clicked()), this, SLOT(pathChanged()));
	connect( m_ui.dvipsButton, SIGNAL(clicked()), this, SLOT(pathChanged()));
	connect( m_ui.convertButton, SIGNAL(clicked()), this, SLOT(pathChanged()));
}

void Settings::pathChanged()
{
	QString objectName = sender()->objectName().remove("Button");

	QString s = QFileDialog::getOpenFileName(this,	tr("Open file"), "/usr/bin", QString("All files (*)"));
	QLineEdit* le = qobject_cast<QLineEdit*>( findChild<QLineEdit*>(objectName + "Edit") );
	if( le )
		le->setText( s );
}
