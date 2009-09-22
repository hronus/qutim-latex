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
#include "settings.h"
#include <QProcess>
#include <QTemporaryFile>
#include <QTextDocument>
#include <QFile>
#include <QDir>

bool LatexPlugin::init(PluginSystemInterface *plugin_system)
{
	PluginInterface::init(plugin_system);
	m_plugin_system = plugin_system;

	m_settingsWidget = 0;
	m_str = "<img src='file://%1' alt='%2' /> ";

	m_icon = new QIcon();

	m_send_message = emit m_plugin_system->registerEventHandler("Core/ChatWindow/SendLevel1.5", this);
	m_recieve_message = emit m_plugin_system->registerEventHandler("Core/ChatWindow/ReceiveLevel3", this);

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
	runProcess( proc, latexCommand, QStringList() << "-interaction=batchmode" << TeXFile );
	runProcess( proc, dvipsCommand, QStringList() << "-o" << EpsFile << "-E" << DviFile );

	runProcess( proc, convertCommand, QStringList() << "+adjoin" << "-antialias" << "-transparent" << "white"
				<< "-density" << QString("%1x%2").arg(dpi.width()).arg(dpi.height()) << EpsFile << fileName );

	QFileInfoList list = QDir(tempDir).entryInfoList();
	foreach( QFileInfo f, list)
		QFile::remove( f.absoluteFilePath());

	return fileName;
}

void LatexPlugin::processEvent(Event &event)
{
	if(event.args.size()<1)
		return;

	if( !m_usable )
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

				QTextDocument td;
				td.setHtml( quotedFormula );
				QString quotedFormulaPlain = td.toPlainText();
				QString pureFormula = QString(quotedFormulaPlain).replace("$$", "");
				QString mesg = m_str.arg( handleLatex( pureFormula )).arg( pureFormula );

				msg->replace(quotedFormula, mesg);
			}
		}
	}
}

/*!
  @note setProfile must call before processing
  */
void LatexPlugin::setProfileName(const QString &profile_name)
{
	m_profile_name = profile_name;

	qDebug() << "LaTex profile" << profile_name;
	QSettings settings(QSettings::defaultFormat(),QSettings::UserScope, "qutim/qutim."+m_profile_name, "qutim-latex");
	latexCommand = settings.value("latex", "latex").toString();
	dvipsCommand = settings.value("dvips", "dvips").toString();
	convertCommand = settings.value("convert", "convert").toString();
	dpi = settings.value("dpi", QSize(150,150)).toSize();
	m_usable = settings.value( "usable", true ).toBool();
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

QWidget *LatexPlugin::settingsWidget()
{
	m_settingsWidget = new Settings;

	m_settingsWidget->setLatexCommand( latexCommand );
	m_settingsWidget->setDvipsCommand( dvipsCommand );
	m_settingsWidget->setConvertCommand( convertCommand );
	m_settingsWidget->setDpi( dpi );
	m_settingsWidget->setUsable( m_usable );

	return m_settingsWidget;
}

void LatexPlugin::saveSettings()
{
	if( !m_settingsWidget )
		return ;

	QSettings settings(QSettings::defaultFormat(),QSettings::UserScope, "qutim/qutim."+m_profile_name, "qutim-latex");

	latexCommand = m_settingsWidget->latexCommand();
	settings.setValue("latex", latexCommand);

	dvipsCommand = m_settingsWidget->dvipsCommand();
	settings.setValue("dvips", dvipsCommand);

	convertCommand = m_settingsWidget->convertCommand();
	settings.setValue("convert", convertCommand);

	m_usable = m_settingsWidget->usable();
	settings.setValue( "usable", m_usable );

	dpi = m_settingsWidget->dpi();
	settings.setValue("dpi", dpi);
}

void LatexPlugin::removeSettingsWidget()
{
	m_settingsWidget->deleteLater();
}

Q_EXPORT_PLUGIN2(LatexPlugin, LatexPlugin);

