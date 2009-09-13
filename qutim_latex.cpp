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

#include "qutim_latex.h"
#include <QProcess>
#include <QTemporaryFile>
#include <QFile>
#include <QDir>

bool LatexPlugin::init(PluginSystemInterface *plugin_system)
{
	PluginInterface::init(plugin_system);
	m_plugin_system = plugin_system;

	m_str = "<img src='file://%1' alt='%2' /> ";

	m_icon = new QIcon();
//	m_count = 0;

	m_send_message = emit m_plugin_system->registerEventHandler("Core/ChatWindow/SendLevel1.5", this);
	m_recieve_message = emit m_plugin_system->registerEventHandler("Core/ChatWindow/ReceiveLevel3", this);

	m_convScript = "kopete_latexconvert.sh";

	return true;
}

// Cleanup temp images on release.
void LatexPlugin::release()
{
	QStringList files = QDir::temp().entryList(QStringList("qutimlatex*.png"),
								 QDir::Files | QDir::NoSymLinks);

	foreach( QString file, files )
	{
		QFile::remove( QDir::tempPath()+"/"+file );
	}
}

QString LatexPlugin::handleLatex(const QString &latexFormula)
{
	QTemporaryFile *tempFile=new QTemporaryFile();
	tempFile->setFileTemplate(QDir::tempPath()+"/qutimlatex-XXXXXXXXXXXXXXXX.png");	// replace TempFile on qHash( latexFormula )
	tempFile->open();
	m_tempFiles.append(tempFile);
	QString fileName = tempFile->fileName();

	QProcess p;

	QString argumentRes = "-r %1x%2";
	QString argumentOut = "-o %1";
	//QString argumentFormat = "-fgif";  //we uses gif format because MSN only handle gif
	int hDPI, vDPI;
	hDPI = 150;	//	hDPI = LatexConfig::self()->horizontalDPI();
	vDPI = 150;	//	vDPI = LatexConfig::self()->verticalDPI();


	QStringList args;
	args << argumentRes.arg(QString::number(hDPI), QString::number(vDPI)) << argumentOut.arg(fileName) /*<< argumentFormat*/ << latexFormula ;

//	qDebug() << " Rendering " << m_convScript << " " <<  argumentRes.arg(QString::number(hDPI), QString::number(vDPI)) << " " << argumentOut.arg(fileName);

	p.execute(m_convScript, args);
	return fileName;
}

void LatexPlugin::processEvent(Event &event)
{
	if(event.args.size()<1)
		return;

	if( event.id == m_send_message || event.id == m_recieve_message )
	{
		QString *msg = (QString *)event.args.at(1);

		QRegExp rx("(\\$\\$.+\\$\\$)");
		rx.setMinimal(true);
		int pos = 0;
		while (pos >= 0)
		{
			pos = rx.indexIn(*msg, pos);
			if (pos >= 0)
			{
				pos++;

				QString quotedFormula = rx.cap(1);
				QString pureFormula = QString(quotedFormula).replace("$$", "");
				QString mesg = m_str.arg( handleLatex( quotedFormula )).arg( pureFormula );
				msg->replace(quotedFormula, mesg);

//				m_count++;
			}
		}

//		qDebug() << "LaTeX result=" << *msg;
	}
}

void LatexPlugin::setProfileName(const QString &profile_name)
{
	m_profile_name = profile_name;
}

QString LatexPlugin::name()
{
	return "LaTeX";
}

QString LatexPlugin::description()
{
	return "Views LaTeX formules";
}

QIcon *LatexPlugin::icon()
{
	return m_icon;
}

QString LatexPlugin::type()
{
	return "view";
}

void LatexPlugin::saveSettings()
{
//	settingswidget->saveSettings();
}

QWidget *LatexPlugin::settingsWidget()
{
	QWidget *settingswidget = new QWidget;
	return settingswidget;
}

void LatexPlugin::removeSettingsWidget()
{
//	delete settingsWidget();
}

Q_EXPORT_PLUGIN2(LatexPlugin, LatexPlugin);
