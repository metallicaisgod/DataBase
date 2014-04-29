#-------------------------------------------------
#
# Project created by QtCreator 2014-04-16T20:36:42
#
#-------------------------------------------------

TARGET = IADataBase
TEMPLATE = lib
CONFIG += staticlib
QT += core xml
DESTDIR = "../"
DEFINES += _CRT_SECURE_NO_WARNINGS
INCLUDEPATH += ./headers


SOURCES += \
    sources/GuideDataBase.cpp \
    sources/GuideHelper.cpp \
    sources/IADataBase.cpp \
    sources/ParserHelper.cpp \
    sources/stdafx.cpp

HEADERS += \
    headers/common.h \
    headers/DbObjects.h \
    headers/Enumerator.h \
    headers/ext_resources.h \
    headers/GuideDataBase.h \
    headers/GuideHelper.h \
    headers/IADataBase.h \
    headers/ParserHelper.h \
    headers/stdafx.h \
    headers/structs.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
