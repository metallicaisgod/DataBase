#-------------------------------------------------
#
# Project created by QtCreator 2014-04-14T20:35:41
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DataBase
TEMPLATE = app

PRECOMPILED_HEADER += stdafx.h

SOURCES += main.cpp\
        mainwindow.cpp \
    customsplitter.cpp

HEADERS  += mainwindow.h \
    stdafx.h \
    customsplitter.h

FORMS    += mainwindow.ui
