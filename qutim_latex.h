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


#ifndef LATEXPLUGIN_H
#define LATEXPLUGIN_H
#include "qutim/plugininterface.h"

#include <QDebug>

class QTemporaryFile;

using namespace qutim_sdk_0_2;

/*!
  TODO: Cleanup temp images on destruct.
  TODO: Settings( on/off plugin usage ).
  */
class LatexPlugin : public QObject, SimplePluginInterface, EventHandler
{
	Q_OBJECT
	Q_INTERFACES(qutim_sdk_0_2::PluginInterface)

public:
	virtual bool init(PluginSystemInterface *plugin_system);
	virtual void release() {}
	virtual void processEvent(Event &event);
	virtual void setProfileName(const QString &profile_name);
	virtual QString name();
	virtual QString description();
	virtual QString type();
	virtual QIcon *icon();
	virtual QWidget *settingsWidget();
	virtual void saveSettings();

private slots:
	virtual void removeSettingsWidget();

private:
	QString handleLatex(const QString &latexFormula);

	PluginSystemInterface *m_plugin_system;
	QString m_profile_name;
	QString m_account_name;
	QString m_convScript;
	QIcon *m_icon;
	uint m_count;

	quint16 m_send_message;
//	quint16 m_send_message_2;
	quint16 m_recieve_message;

	QString m_str;

	QList<QTemporaryFile*> m_tempFiles;
};
#endif
