#-------------------------------------------------
#
# Project created by QtCreator 2013-09-17T17:36:39
#
#-------------------------------------------------

QT       += core gui kde

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = MyShapeTest
TEMPLATE = app

HEADERS += `pkg-config --cflags opencv`
LIBS += `pkg-config --libs opencv`

SOURCES += main.cpp\
        mainwindow.cpp

HEADERS  += mainwindow.h

FORMS    += mainwindow.ui


