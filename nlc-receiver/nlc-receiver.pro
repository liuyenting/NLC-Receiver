#-------------------------------------------------
#
# Project created by QtCreator 2015-12-18T08:25:29
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = nlc-receiver
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    camera.cpp \
    utility.cpp

HEADERS  += \
    camera.hpp \
    mainwindow.hpp \
    utility.hpp

FORMS += \
    mainwindow.ui

macx: LIBS += -L$$PWD/../../../../../opt/local/lib/ -ldc1394.22

INCLUDEPATH += $$PWD/../../../../../opt/local/include
DEPENDPATH += $$PWD/../../../../../opt/local/include
