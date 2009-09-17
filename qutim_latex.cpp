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

	m_send_message = emit m_plugin_system->registerEventHandler("Core/ChatWindow/SendLevel1.5", this);
	m_recieve_message = emit m_plugin_system->registerEventHandler("Core/ChatWindow/ReceiveLevel3", this);

	m_convScript = "tex2im.sh";

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

void runProcess( QProcess& proc, const QString& exe, const QStringList args )
{
	const QString S = "LaTex plugin: ";
	proc.start( exe, args);

	if (!proc.waitForStarted())
	{
		qDebug() << S << exe << " not started!";
	}

	if (!proc.waitForFinished())
	{
		qDebug() << S << exe << " not finished!";
	}

	if( proc.exitCode() )
		qDebug() << S << exe << " failed!";
}

QString LatexPlugin::handleLatex(const QString &latexFormula)
{
	QTemporaryFile *tempFile=new QTemporaryFile();
	tempFile->setFileTemplate(QDir::tempPath()+"/qutimlatex-XXXXXXXXX.png");
	tempFile->open();
	m_tempFiles.append(tempFile);
	QString fileName = tempFile->fileName();

	QString argumentRes = "-r %1x%2";
	QString argumentOut = "-o %1";

	int hDPI, vDPI;
	hDPI = 150;	//	hDPI = LatexConfig::self()->horizontalDPI();
	vDPI = 150;	//	vDPI = LatexConfig::self()->verticalDPI();

	QDir dir;
	const QString tempDir = QDir::tempPath() + "/qutim-latex-temp";
	dir.mkdir( tempDir );
	if( !dir.exists( tempDir ) )
	{
		qDebug() << "Could not create temp" << tempDir;
		return QString();
	}

	QString tex =  QString(
	"\\documentclass[12pt]{article}\n"
	"\\usepackage{color}\n "
	"\\usepackage[dvips]{graphicx}\n "
	"\\pagestyle{empty}\n "
	"\\pagecolor{white}\n "
	"\\begin{document}\n "
	"{\\color{black}\n "
	"\\begin{eqnarray*}\n "
	"%1 \n"
	"\\end{eqnarray*}}\n "
	"\\end{document}\n").arg(latexFormula);

	const QString TeXFile = "out.tex";
	const QString DviFile = "out.dvi";
	const QString EpsFile = "out.eps";

	QFile file(tempDir+"/"+TeXFile);
	file.open( QIODevice::WriteOnly | QIODevice::Truncate );
	file.write( tex.toLocal8Bit() );
	file.close();

	QProcess proc;
	proc.setWorkingDirectory( tempDir );
	runProcess( proc, "latex", QStringList() << "-interaction=batchmode" << TeXFile );
	runProcess( proc, "dvips", QStringList() << "-o" << EpsFile << "-E" << DviFile );

	runProcess( proc, "convert", QStringList() << "+adjoin" << "-antialias" << "-transparent" << "white"
				<< "-density" << QString("%1x%2").arg(hDPI).arg(vDPI) << EpsFile << fileName );

	QFileInfoList list = QDir(tempDir).entryInfoList();
	foreach( QFileInfo f, list)
		QFile::remove( f.absoluteFilePath());

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
				QString mesg = m_str.arg( handleLatex( pureFormula )).arg( pureFormula );
				msg->replace(quotedFormula, mesg);

			}
		}
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

