
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .


# Input
HEADERS += mainwindow.h

SOURCES += main.cpp \
           mainwindow.cpp

INCLUDEPATH += /usr/include/libxml2

LIBS += ../libdynamic.a -lxml2
