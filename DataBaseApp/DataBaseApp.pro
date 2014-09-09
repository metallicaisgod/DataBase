#-------------------------------------------------
#
# Project created by QtCreator 2014-04-14T20:35:41
#
#-------------------------------------------------

QT       += core gui opengl xml

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = DataBase
TEMPLATE = app
DESTDIR = "../"
DEFINES += _CRT_SECURE_NO_WARNINGS
LIBS += -L"$${DESTDIR}" \
    -lIADataBase
INCLUDEPATH += . \
    $${_PRO_FILE_PWD_}

PRECOMPILED_HEADER += stdafx.h

SOURCES += main.cpp\
        mainwindow.cpp \
    splitterhandle.cpp \
    gridsplitter.cpp \
    crosssplitterhandle.cpp \
    treemodel.cpp \
    treeitem.cpp \
    tablemodel.cpp \
    implantdialog.cpp \
    abutmentdialog.cpp \
    glmodel.cpp

HEADERS  += mainwindow.h \
    stdafx.h \
    splitterhandle.h \
    gridsplitter.h \
    crosssplitterhandle.h \
    treemodel.h \
    treeitem.h \
    tablemodel.h \
    types.h \
    implantdialog.h \
    abutmentdialog.h \
    glmodel.h

FORMS    += mainwindow.ui \
    implantdialog.ui \
    abutmentdialog.ui

RESOURCES += \
    resources.qrc

OTHER_FILES += \
    icon_rec.rc

win32:RC_FILE = icon_rec.rc
