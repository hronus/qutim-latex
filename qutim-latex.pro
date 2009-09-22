TARGET = latex
TEMPLATE = lib
CONFIG += silent

OBJECTS_DIR	= build
MOC_DIR		= build
UI_DIR		= build
RCC_DIR		= build

HEADERS += src/qutim_latex.h \
	src/settings.h

SOURCES += src/qutim_latex.cpp \
	src/settings.cpp

FORMS += src/settings.ui

CONFIG += qt \
	plugin

QT += core \
	gui


unix:INCLUDEPATH += /usr/include
INSTALLPATH = /usr
target.path = $$INSTALLPATH/lib/qutim
INSTALLS = target
