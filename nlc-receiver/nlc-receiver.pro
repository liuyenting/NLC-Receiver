#-------------------------------------------------
#
# Project created by QtCreator 2015-12-18T08:25:29
#
#-------------------------------------------------

QT          +=  core gui
QT_CONFIG   -=  no-pkg-config

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

QMAKE_CXXFLAGS      +=  -std=c++11

macx {
    QMAKE_CXXFLAGS +=  -stdlib=libc++
    QMAKE_LFLAGS   +=  -stdlib=libc++
}

TARGET      = nlc-receiver
TEMPLATE    = app


SOURCES     +=  main.cpp \
                mainwindow.cpp \
                camera.cpp \
                utility.cpp \
    opencvviewer.cpp \
    analysis.cpp

HEADERS     +=  camera.hpp \
                mainwindow.hpp \
                utility.hpp \
    opencvviewer.hpp \
    analysis.hpp

FORMS       +=  mainwindow.ui

macx: LIBS  +=  -L $$PWD/../../../../../opt/local/lib \
                -ldc1394.22 \
                -lopencv_core \
                -lopencv_imgproc \
                -lopencv_highgui \
                -lopencv_imgcodecs

INCLUDEPATH += $$PWD/../../../../../opt/local/include
DEPENDPATH  += $$PWD/../../../../../opt/local/include
