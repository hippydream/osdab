INCLUDEPATH += .
DEPENDPATH += .
TEMP = tmp
MOC_DIR = tmp/moc
OBJECTS_DIR = tmp/obj
DESTDIR = lib
TARGET = QtSvgzPlugin
TEMPLATE = lib
CONFIG += plugin static
QT += svg

QMAKE_TARGET_COMPANY = Angius Fabrizio
QMAKE_TARGET_PRODUCT = QtSvgzPlugin
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2007 Angius Fabrizio - GNU GPL v2 or later
QMAKE_TARGET_DESCRIPTION = SVGZ icon engine plugin for Qt4.

HEADERS += qtsvgz_plugin.h qtsvgz_engine.h
SOURCES += qtsvgz_plugin.cpp qtsvgz_engine.cpp