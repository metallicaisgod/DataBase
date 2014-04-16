#-------------------------------------------------
#
# Project created by QtCreator 2014-04-14T20:35:41
#
#-------------------------------------------------

QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DataBase
TEMPLATE = app
DESTDIR = "../debug"
LIBS += -L"$${DESTDIR}" \
    -lIADataBase
PRECOMPILED_HEADER += stdafx.h

SOURCES += main.cpp\
        mainwindow.cpp \
    splitterhandle.cpp \
    gridsplitter.cpp \
    crosssplitterhandle.cpp

HEADERS  += mainwindow.h \
    stdafx.h \
    splitterhandle.h \
    gridsplitter.h \
    crosssplitterhandle.h

FORMS    += mainwindow.ui
