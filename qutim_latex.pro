TARGET = latex
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build

HEADERS += qutim_latex.h
SOURCES += qutim_latex.cpp
CONFIG += qt plugin
QT += core gui network
TEMPLATE = lib
CONFIG += silent

INSTALLPATH = /usr/local
target.path = $$INSTALLPATH/lib/qutim

data.path += $$INSTALLPATH/bin
data.files += tex2im.sh

INSTALLS = target data
