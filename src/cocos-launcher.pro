#-------------------------------------------------
#
# Project created by QtCreator 2015-08-19T16:01:38
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = cocos-launcher
TEMPLATE = app

CONFIG += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    dialognewgame.cpp \
    templatewizard.cpp

HEADERS  += mainwindow.h \
    dialognewgame.h \
    templatewizard.h

FORMS    += mainwindow.ui \
    dialognewgame.ui

RESOURCES += \
    resources.qrc
