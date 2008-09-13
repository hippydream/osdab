TEMPLATE = app
INCLUDEPATH += . ../
QT -= qtgui
CONFIG += release console

INCLUDEPATH += . ../

# Input
HEADERS += ../sha1hash.h
SOURCES += main.cpp ../sha1hash.cpp
DESTDIR = bin
MOC_DIR = tmp
OBJECTS_DIR = tmp