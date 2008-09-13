TEMPLATE = app
INCLUDEPATH += . ../
CONFIG += console debug
TARGET = xmlp

INCLUDEPATH += . ../

# Input
HEADERS += ../xmlpreferences.h ../xmlpreferences_p.h xmlwriter_p.h xmlwriter.h base64.h
SOURCES += ../xmlpreferences.cpp main.cpp xmlwriter.cpp base64.cpp
DESTDIR = bin
MOC_DIR = tmp
OBJECTS_DIR = tmp

win32 {
	LIBS += lib/win32/libxml2.lib
}
else {
	LIBS += -lxml2
}
