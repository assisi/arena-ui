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
        nzmqt/nzmqt.cpp\
        ../msg/cpp/base_msgs.pb.cc\
        ../msg/cpp/dev_msgs.pb.cc\
        ../msg/cpp/sim_msgs.pb.cc \
    casutreeitem.cpp \
    casusceneitem.cpp \
    connectdialog.cpp \
    global.cpp

HEADERS  += arenaui.h\
        nzmqt/nzmqt.hpp \
    casutreeitem.h \
    casusceneitem.h \
    connectdialog.h \
    global.h

FORMS    += arenaui.ui

LIBS += -lzmq\
        -lprotobuf\
        -lyaml-cpp\
        -lboost_system\
        -lboost_filesystem

RESOURCES += \
    images.qrc
