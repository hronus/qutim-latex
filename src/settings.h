/*	qutIM LaTeX Plugin

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

#ifndef SETTINGS_H
#define SETTINGS_H

#include "ui_settings.h"

namespace Ui
{
	class Settings;
}


class Settings : public QWidget
{
	Q_OBJECT
public:
	Settings(QWidget *parent = 0);

	void setUsable( bool b )
	{	m_ui.usePlugin->setChecked( b ); }

	bool usable() const
	{ return m_ui.usePlugin->isChecked(); }

	void setDpi( const QSize& size )
	{
		m_ui.horizontalDPI->setValue( size.width() );
		m_ui.verticalDPI->setValue( size.height() );
	}

	QSize dpi() const
	{	return QSize(m_ui.horizontalDPI->value(), m_ui.verticalDPI->value()); }

	void setLatexCommand( const QString& text )
	{	return m_ui.latexEdit->setText( text ); }

	QString latexCommand() const
	{	return m_ui.latexEdit->text(); }

	void setDvipsCommand( const QString& text )
	{	m_ui.dvipsEdit->setText( text ); }

	QString dvipsCommand() const
	{	return m_ui.dvipsEdit->text(); }

	QString convertCommand() const
	{	return m_ui.convertEdit->text(); }

	void setConvertCommand( const QString& text )
	{	m_ui.convertEdit->setText( text ); }

private slots:
	void pathChanged();

private:
	Ui::Settings m_ui;
};

#endif // SETTINGS_H
