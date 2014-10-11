#-------------------------------------------------
#
# Project created by QtCreator 2014-10-05T22:38:05
#
#-------------------------------------------------

QT       += core gui dbus
CONFIG += link_pkgconfig
QMAKE_CXXFLAGS += -std=c++11
PKGCONFIG += libbuffy

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = buffy
TEMPLATE = app

SOURCES += main.cpp\
        buffy.cpp \
    foldermodel.cpp \
    folders.cpp \
    sorterfilter.cpp \
    folderview.cpp \
    preferences.cpp \
    dbus.cpp

HEADERS  += buffy.h \
    foldermodel.h \
    folders.h \
    sorterfilter.h \
    folderview.h \
    preferences.h \
    dbus.h

FORMS    += buffy.ui \
    preferences.ui

RESOURCES += \
    resources.qrc
