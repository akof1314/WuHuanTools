#-------------------------------------------------
#
# Project created by QtCreator 2014-04-09T16:20:39
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = CompareStr
TEMPLATE = app
CONFIG   += c++11

SOURCES += main.cpp\
        mainwindow.cpp \
    formdialog.cpp

HEADERS  += mainwindow.h \
    formdialog.h

FORMS    += mainwindow.ui \
    formdialog.ui

RESOURCES += \
    resall.qrc
