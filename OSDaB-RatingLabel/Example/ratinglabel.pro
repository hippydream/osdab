TEMPLATE = app
INCLUDEPATH += . ../

HEADERS += ratingform.h ../ratinglabel.h
SOURCES += main.cpp ../ratinglabel.cpp
RESOURCES += images.qrc
FORMS += ratingform.ui

DESTDIR = bin
MOC_DIR = tmp
OBJECTS_DIR = tmp
RCC_DIR = tmp