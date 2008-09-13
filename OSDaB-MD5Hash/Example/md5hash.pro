TEMPLATE = app
INCLUDEPATH += . ../
QT -= qtgui
CONFIG += release console

INCLUDEPATH += . ../

# Input
HEADERS += ../md5hash.h
SOURCES += main.cpp ../md5hash.cpp
DESTDIR = bin
MOC_DIR = tmp
OBJECTS_DIR = tmp