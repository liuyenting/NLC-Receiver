#-------------------------------------------------
#
# Project created by QtCreator 2015-12-18T08:25:29
#
#-------------------------------------------------

QT       += core gui
QT_CONFIG       -= no-pkg-config

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET      = nlc-receiver
TEMPLATE    = app


SOURCES     +=  main.cpp \
                mainwindow.cpp \
                camera.cpp \
                utility.cpp \
    opencvviewer.cpp

HEADERS     +=  camera.hpp \
                mainwindow.hpp \
                utility.hpp \
    opencvviewer.hpp

FORMS       +=  mainwindow.ui

macx: LIBS  +=  -L $$PWD/../../../../../opt/local/lib \
                -ldc1394.22 \
                -lopencv_core \
                -lopencv_imgproc

INCLUDEPATH += $$PWD/../../../../../opt/local/include
DEPENDPATH  += $$PWD/../../../../../opt/local/include
