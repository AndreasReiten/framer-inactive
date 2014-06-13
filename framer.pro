#-------------------------------------------------
#
# Project created by QtCreator 2014-06-13T14:47:09
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = framer
TEMPLATE = app


SOURCES += main.cpp\
        mainwindow.cpp \
    lib/qxfilelib/qxfilelib.cpp

HEADERS  += mainwindow.h \
    lib/qxfilelib/qxfilelib.h \
    lib/qxfilelib/qxfilelib_global.h
