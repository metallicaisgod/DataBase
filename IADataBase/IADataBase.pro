#-------------------------------------------------
#
# Project created by QtCreator 2014-04-16T20:36:42
#
#-------------------------------------------------

QT       -= gui

TARGET = IADataBase
TEMPLATE = lib
CONFIG += staticlib
DESTDIR = "../"
DEFINES += TIXML_USE_TICPP _CRT_SECURE_NO_WARNINGS
INCLUDEPATH += ./headers


SOURCES += \
    sources/GuideDataBase.cpp \
    sources/GuideHelper.cpp \
    sources/IADataBase.cpp \
    sources/ParserHelper.cpp \
    sources/stdafx.cpp \
    sources/ticpp.cpp \
    sources/tinystr.cpp \
    sources/tinyxml.cpp \
    sources/tinyxmlerror.cpp \
    sources/tinyxmlparser.cpp

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
    headers/structs.h \
    headers/ticpp.h \
    headers/ticpprc.h \
    headers/tinystr.h \
    headers/tinyxml.h
unix {
    target.path = /usr/lib
    INSTALLS += target
}
