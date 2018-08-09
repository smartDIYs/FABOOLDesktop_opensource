#-------------------------------------------------
#
# Project created by QtCreator 2017-09-26T14:59:33
#
#-------------------------------------------------

QT       += core gui    \
            svg         \
            serialport  \
            network     \
            xml         \
            concurrent

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = FABOOLDesktop
TEMPLATE = app

win32 {
    SUFFIX = .exe
} macx {
    SUFFIX = .app
} unix:!macx {
    SUFFIX =
}

# App version info
#win*:RC_FILE = $${TARGET}.rc
#win*:build_pass:touch($$RC_FILE, version.h)

VERSION_MAJOR = 1
VERSION_MINOR = 4
VERSION_BUILD = 4

DEFINES += APP_VERSION_MAJOR=$$VERSION_MAJOR \
       APP_VERSION_MINOR=$$VERSION_MINOR \
       APP_VERSION_BUILD=$$VERSION_BUILD

win32 {
DEFINES += APP_TARGET=\"$$TARGET\" \
           APP_SUFFIX=\"$$SUFFIXX\"
} unix {
DEFINES += APP_TARGET=\\\"$$TARGET\\\" \
           APP_SUFFIX=\\\"$$SUFFIX\\\"
}

VERSION = $${VERSION_MAJOR}.$${VERSION_MINOR}.$${VERSION_BUILD}
QMAKE_TARGET_COMPANY = smartDIYs Co.,Ltd.
QMAKE_TARGET_PRODUCT = FABOOL Desktop
QMAKE_TARGET_DESCRIPTION = FABOOL Laser series.
QMAKE_TARGET_COPYRIGHT = (C)2018


# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS QT_MESSAGELOGCONTEXT
TRANSLATIONS += translate/fabool_desktop_ja.ts \
                translate/fabool_desktop_en.ts

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.

# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

win32 {
    debug:LIBS   += -L$$PWD/'3rd/quazip/lib_win/' -lquazipd
    release:LIBS += -L$$PWD/'3rd/quazip/lib_win/' -lquazip
} macx {
    QUAZIP_DIR = /Users/user/Qt/quazipForMac
    QMAKE_INCDIR += $$QUAZIP_DIR/include
    QMAKE_LIBDIR += $$QUAZIP_DIR/lib
    QMAKE_RPATHDIR += $$QUAZIP_DIR/lib
    QMAKE_LIBS += -lquazip
} unix:!macx {
    QUAZIP_DIR = ../3rd/quazip
    QMAKE_INCDIR += $$QUAZIP_DIR/include
    QMAKE_LIBDIR += $$QUAZIP_DIR/lib_lnx
    QMAKE_RPATHDIR += $$QUAZIP_DIR/lib_lnx
    QMAKE_LIBS += -lquazip
}

INCLUDEPATH += src

SOURCES +=                                  \
    src/main.cpp                            \
    src/common/dataManager.cpp              \
    src/common/sysParam.cpp                 \
    src/command/commandManager.cpp          \
    src/command/deleteCommand.cpp           \
    src/command/importCommand.cpp           \
    src/command/pasteCommand.cpp            \
    src/command/shapeChangeCommand.cpp      \
    src/command/groupCommand.cpp            \
    src/command/ungroupCommand.cpp          \
    src/graphics/graphicsFlexibleFrame.cpp  \
    src/graphics/itemInfo.cpp               \
    src/graphics/paramData.cpp              \
    src/graphics/sizeGrip.cpp               \
    src/graphics/vectorImageItem.cpp        \
    src/graphics/groupInfo.cpp              \
    src/transfer/svgColor.cpp               \
    src/transfer/svgParser.cpp              \
    src/transfer/svgPath.cpp                \
    src/transfer/svgPathEle.cpp             \
    src/transfer/svgTransform.cpp           \
    src/transfer/unitTransfer.cpp           \
    src/ui/graphicArea/mainScene.cpp        \
    src/ui/graphicArea/mainView.cpp         \
    src/ui/graphicArea/maintenanceView.cpp  \
    src/ui/mainDialog.cpp                   \
    src/ui/widgets/paramWidget.cpp          \
    src/ui/widgets/paramRow.cpp             \
    src/ui/widgets/itemShapeWidget.cpp      \
    src/ui/widgets/customPushButton.cpp     \
    src/ui/dialogs/machineSettingDialog.cpp \
    src/ui/dialogs/firstRegistrationDialog.cpp \
    src/ui/widgets/maintenanceWidget.cpp    \
    src/transfer/imgParser.cpp              \
    src/ui/dialogs/progressDialog.cpp       \
    src/ui/listView/selectListDialog.cpp    \
    src/ui/listView/materialListModel.cpp   \
    src/ui/listView/listModelCore.cpp       \
    src/ui/listView/selectListView.cpp      \
    src/ui/listView/listedDataCore.cpp      \
    src/ui/listView/libraryListedData.cpp   \
    src/ui/listView/libraryListModel.cpp    \
    src/ui/listView/materialListedData.cpp  \
    src/ui/dialogs/errMesDialog.cpp         \
    src/ui/widgets/customTableWidget.cpp    \
    src/ui/widgets/verticalOrderedWidget.cpp \
    src/common/singleApplication.cpp \
    src/transfer/dxfParser.cpp \
    src/transfer/dxfDefinedColor.cpp \
    src/common/faboolApi.cpp \
    src/common/versionInfo.cpp \
    src/ui/dialogs/aboutDialog.cpp \
    src/transfer/kdTree.cpp \
    src/machine/faboolMachine.cpp \
    src/machine/serialManager.cpp \
    src/machine/device.cpp \
    src/machine/util/debug.cpp \
    src/machine/util/debugPanel.cpp \
    src/transfer/processTime.cpp \
    src/common/sortablePointF.cpp

HEADERS  +=                                 \
    src/common/common.h                     \
    src/common/dataManager.h                \
    src/common/sysParam.h                   \
    src/command/commandManager.h            \
    src/command/deleteCommand.h             \
    src/command/importCommand.h             \
    src/command/pasteCommand.h              \
    src/command/shapeChangeCommand.h        \
    src/command/groupCommand.h              \
    src/command/ungroupCommand.h            \
    src/graphics/graphicsFlexibleFrame.h    \
    src/graphics/itemInfo.h                 \
    src/graphics/paramData.h                \
    src/graphics/sizeGrip.h                 \
    src/graphics/vectorImageItem.h          \
    src/graphics/groupInfo.h                \
    src/transfer/svgColor.h                 \
    src/transfer/svgCommon.h                \
    src/transfer/svgParser.h                \
    src/transfer/svgPath.h                  \
    src/transfer/svgPathEle.h               \
    src/transfer/svgTransform.h             \
    src/transfer/unitTransfer.h             \
    src/ui/graphicArea/maintenanceView.h    \
    src/ui/graphicArea/mainScene.h          \
    src/ui/graphicArea/mainView.h           \
    src/ui/mainDialog.h                     \
    src/ui/widgets/paramWidget.h            \
    src/ui/widgets/paramRow.h               \
    src/ui/widgets/itemShapeWidget.h        \
    src/ui/widgets/customPushButton.h       \
    src/ui/dialogs/machineSettingDialog.h   \
    src/ui/dialogs/firstRegistrationDialog.h \
    src/ui/widgets/maintenanceWidget.h      \
    src/transfer/parserCore.h               \
    src/transfer/imgParser.h                \
    src/ui/dialogs/progressDialog.h         \
    src/ui/listView/selectListDialog.h      \
    src/ui/listView/materialListModel.h     \
    src/ui/listView/listModelCore.h         \
    src/ui/listView/selectListView.h        \
    src/ui/listView/listedDataCore.h        \
    src/ui/listView/libraryListedData.h     \
    src/ui/listView/libraryListModel.h      \
    src/ui/listView/materialListedData.h    \
    src/ui/dialogs/errMesDialog.h           \
    src/ui/widgets/customTableWidget.h      \
    src/ui/widgets/verticalOrderedWidget.h \
    src/common/singleApplication.h \
    src/transfer/dxfParser.h \
    src/transfer/dxfDefinedColor.h \
    src/common/faboolApi.h \
    src/common/versionInfo.h \
    src/ui/dialogs/aboutDialog.h \
    src/transfer/kdTree.h \
    src/machine/faboolMachine.h \
    src/machine/serialManager.h \
    src/machine/device.h \
    src/machine/util/debug.h \
    src/machine/util/debugPanel.h \
    src/transfer/processTime.h \
    src/common/sortablePointF.h


FORMS    +=                                 \
    src/ui/dialogs/machineSettingDialog.ui  \
    src/ui/widgets/paramWidget.ui           \
    src/ui/mainDialog.ui                    \
    src/ui/widgets/itemShapeWidget.ui       \
    src/ui/dialogs/firstRegistrationDialog.ui \
    src/ui/widgets/maintenanceWidget.ui     \
    src/ui/dialogs/progressDialog.ui        \
    src/ui/listView/selectListDialog.ui     \
    src/ui/dialogs/errMesDialog.ui \
    src/ui/dialogs/aboutDialog.ui \
    src/machine/util/debugPanel.ui


RESOURCES += \
    rc.qrc

win32 {
    RC_ICONS = app.ico
} unix:!macx {

} macx {
    ICON = app.icns
}

DISTFILES += fabool_fomat.astylerc
