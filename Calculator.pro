#-------------------------------------------------
#
# Project created by QtCreator 2015-08-25T15:44:58
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = calculator
TEMPLATE = app

#win32:RC_ICONS += calculator.ico

SOURCES += main.cpp\
        eventwatcher.cpp \
        functions.c \
        mainwindow.cpp

HEADERS  += mainwindow.h \
    eventwatcher.h \
    functions.h

FORMS    += mainwindow.ui 

