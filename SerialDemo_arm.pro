#-------------------------------------------------
#
# Project created by QtCreator 2015-08-28T10:39:08
#
#-------------------------------------------------

QT       += core gui

TARGET = SerialDemo_arm
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    ztool.cpp \
    serialac.cpp \
    serialacmanager.cpp

HEADERS  += mainwindow.h \
    ztool.h \
    serialac.h \
    serialacmanager.h

FORMS    += mainwindow.ui
include(qextserialport/src/qextserialport.pri)
