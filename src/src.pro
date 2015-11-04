#-------------------------------------------------
#
# Project created by QtCreator 2015-08-19T16:01:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cocos2d-console-gui
target.path = $${PREFIX}/bin
INSTALLS += target
win32 {
    DESTDIR = ..
} else {
    DESTDIR = ../bin
}
TEMPLATE = app
VERSION = 0.0.4
GIT_VERSION = $$system(git describe --abbrev=4 --dirty --always --tags)
DEFINES += GIT_VERSION=\\\"$$GIT_VERSION\\\"

CONFIG += c++11
CONFIG += debug_and_release


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
    cocos2dguiapplication.cpp \
    librariesdialog.cpp \
    systemstate.cpp \
    runmgr.cpp \
    highlighter.cpp \
    fileutils.cpp

HEADERS  += mainwindow.h \
    templatewizard.h \
    templateentry.h \
    aboutdialog.h \
    preferencesdialog.h \
    progressdialog.h \
    welcomedialog.h \
    newgamedialog.h \
    gamestate.h \
    cocos2dguiapplication.h \
    librariesdialog.h \
    systemstate.h \
    runmgr.h \
    highlighter.h \
    fileutils.h

FORMS    += mainwindow.ui \
    aboutdialog.ui \
    preferencesdialog.ui \
    progressdialog.ui \
    welcomedialog.ui \
    newgamedialog.ui \
    librariesdialog.ui

RESOURCES += \
    resources.qrc

QMAKE_CXXFLAGS += -Werror

macx {
    TARGET = Cocos2d-Console-GUI
    ICON = res/cocosproj-icon-mac.icns
    QMAKE_INFO_PLIST = Info.plist
}
