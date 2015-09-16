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
    dialognewgame.cpp \
    templatewizard.cpp \
    templateentry.cpp \
    aboutdialog.cpp \
    preferencesdialog.cpp

HEADERS  += mainwindow.h \
    dialognewgame.h \
    templatewizard.h \
    templateentry.h \
    aboutdialog.h \
    preferencesdialog.h

FORMS    += mainwindow.ui \
    dialognewgame.ui \
    aboutdialog.ui \
    preferencesdialog.ui

RESOURCES += \
    resources.qrc
