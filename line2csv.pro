#-------------------------------------------------
#
# Project created by QtCreator 2015-06-09T09:20:06
#
#-------------------------------------------------

QT       += core

QT       -= gui

TARGET = line2csv
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app


SOURCES += main.cpp

win32: LIBS += -L$$PWD/../../../../../MinGW/msys/1.0/local/lib/ -llibgdal

INCLUDEPATH += $$PWD/../../../../../MinGW/msys/1.0/local/include
DEPENDPATH += $$PWD/../../../../../MinGW/msys/1.0/local/include
