#-------------------------------------------------
#
# Project created by QtCreator 2017-11-19T18:42:39
#
#-------------------------------------------------

QT      += core gui \
    network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = Secret_file_sharing_service
TEMPLATE = app
CONFIG  += c++11

SOURCES += main.cpp\
    mainwindow.cpp \
    login.cpp \
    server.cpp

HEADERS += mainwindow.h \
    login.h \
    server.h

FORMS   += mainwindow.ui \
    login.ui \
    server.ui