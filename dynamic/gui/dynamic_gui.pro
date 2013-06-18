
TEMPLATE = app
DEPENDPATH += .
INCLUDEPATH += .

HEADERS += mainwindow.h

SOURCES += main.cpp \
           mainwindow.cpp

INCLUDEPATH += /usr/include/libxml2

LIBS += ../libdynamic.a -lxml2

QMAKE_CXXFLAGS += -Wno-unused-function