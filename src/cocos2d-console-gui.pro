#-------------------------------------------------
#
# Project created by QtCreator 2015-08-19T16:01:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cocos2d-console-gui
TEMPLATE = app
VERSION = 0.0.1
DEFINES += APP_VERSION=\\\"$$VERSION\\\"

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    templatewizard.cpp \
    templateentry.cpp \
    aboutdialog.cpp \
    preferencesdialog.cpp \
    progressdialog.cpp \
    welcomedialog.cpp \
    newgamedialog.cpp \
    gamestate.cpp \
    cocos2dguiapplication.cpp

HEADERS  += mainwindow.h \
    templatewizard.h \
    templateentry.h \
    aboutdialog.h \
    preferencesdialog.h \
    progressdialog.h \
    welcomedialog.h \
    newgamedialog.h \
    gamestate.h \
    cocos2dguiapplication.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    preferencesdialog.ui \
    progressdialog.ui \
    welcomedialog.ui \
    newgamedialog.ui

RESOURCES += \
    resources.qrc

QMAKE_CXXFLAGS += -Werror

macx {
    TARGET = Cocos2d-Console-GUI
    ICON = res/cocos2dproj-icon-mac.icns
    QMAKE_INFO_PLIST = Info.plist
}
