#-------------------------------------------------
#
# Project created by QtCreator 2017-10-31T13:53:29
#
#-------------------------------------------------

QT       += widgets testlib network xml

INCLUDEPATH += ../app/src

TARGET = tst_fabooltest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QT_MESSAGELOGCONTEXT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += tst_fabooltest.cpp \
    ../app/src/common/sysParam.cpp \
    ../app/src/tracer/bigQuery.cpp \
    ../app/src/tracer/pythonApp.cpp \
    ../app/src/transfer/svgColor.cpp \
    ../app/src/transfer/svgParser.cpp \
    ../app/src/transfer/svgTransform.cpp \
    ../app/src/transfer/unitTransfer.cpp \
    ../app/src/transfer/svgPath.cpp \
    ../app/src/transfer/svgPathEle.cpp \
    ../app/src/transfer/imgParser.cpp \
    ../app/src/transfer/kdTree.cpp \
    ../app/src/transfer/kdTreeNode.cpp \
    ../app/src/graphics/paramData.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"

HEADERS += \
    ../app/src/common/common.h \
    ../app/src/common/sysParam.h \
    ../app/src/tracer/bigQuery.h \
    ../app/src/tracer/pythonApp.h \
    ../app/src/transfer/svgColor.h \
    ../app/src/transfer/svgParser.h \
    ../app/src/transfer/svgTransform.h \
    ../app/src/transfer/unitTransfer.h \
    ../app/src/transfer/svgPath.h \
    ../app/src/transfer/svgPathEle.h \
    ../app/src/transfer/svgCommon.h \
    ../app/src/transfer/imgParser.h \
    ../app/src/transfer/kdTree.h \
    ../app/src/transfer/kdTreeNode.h \
    ../app/src/graphics/paramData.h
