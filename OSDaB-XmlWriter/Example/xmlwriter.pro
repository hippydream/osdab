TEMPLATE = app
INCLUDEPATH += . ../
QT -= qtgui
CONFIG += release console

INCLUDEPATH += . ../

# Input
HEADERS += ../xmlwriter.h
SOURCES += main.cpp ../xmlwriter.cpp
DESTDIR = bin
MOC_DIR = tmp
OBJECTS_DIR = tmp