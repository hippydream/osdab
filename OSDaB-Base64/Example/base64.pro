CONFIG += release
TEMPLATE = app

INCLUDEPATH += . ../

HEADERS += ../base64.h testbase64.h
SOURCES += ../base64.cpp testbase64.cpp main.cpp
RESOURCES += images.qrc

RC_FILE = base64.rc
DESTDIR = bin
MOC_DIR = tmp
OBJECTS_DIR = tmp
RCC_DIR = tmp
