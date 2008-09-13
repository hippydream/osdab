INCLUDEPATH += . ../Plugin
TEMP = tmp
MOC_DIR = tmp/moc
OBJECTS_DIR = tmp/obj
DESTDIR = bin
TARGET = QtSvgzExample
TEMPLATE = app
QT += svg

LIBS += -L../Plugin/lib -lQtSvgzPlugin

QMAKE_TARGET_COMPANY = Angius Fabrizio
QMAKE_TARGET_PRODUCT = QtSvgzExample
QMAKE_TARGET_COPYRIGHT = Copyright (C) 2007 Angius Fabrizio - GNU GPL v2 or later
QMAKE_TARGET_DESCRIPTION = SVGZ icon engine example application.

HEADERS = \
	iconpreviewarea.h \
	mainwindow.h
	
SOURCES = \
	iconpreviewarea.cpp \
	main.cpp \
	mainwindow.cpp
	
RESOURCES += images.qrc