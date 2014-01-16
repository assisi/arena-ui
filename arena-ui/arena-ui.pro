#-------------------------------------------------
#
# Project created by QtCreator 2014-01-14T15:53:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = arena-ui
TEMPLATE = app


INCLUDEPATH += ../msg/cpp

SOURCES += main.cpp\
        arenaui.cpp\
        ../msg/cpp/base_msgs.pb.cc\
        ../msg/cpp/dev_msgs.pb.cc\
        ../msg/cpp/sim_msgs.pb.cc

HEADERS  += arenaui.h

FORMS    += arenaui.ui

LIBS += -lzmq\
        -lnzmqt\
        -lprotobuf
