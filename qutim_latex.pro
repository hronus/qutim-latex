TARGET = latex
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
RCC_DIR = build

HEADERS += qutim_latex.h
SOURCES += qutim_latex.cpp
INCLUDEPATH += /usr/include ../../include
CONFIG += qt plugin
QT += core gui network
TEMPLATE = lib
CONFIG += silent