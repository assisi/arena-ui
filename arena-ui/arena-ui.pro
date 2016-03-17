#-------------------------------------------------
#
# Project created by QtCreator 2014-01-14T15:53:48
#
#-------------------------------------------------

QT       += core gui

QT       += widgets\
           printsupport

TARGET = arena-ui
TEMPLATE = app

CONFIG += link_pkgconfig
PKGCONFIG += Qt5GStreamer-1.0 Qt5GStreamerUi-1.0


INCLUDEPATH += ../msg/cpp

SOURCES += main.cpp\
        arenaui.cpp\
        nzmqt/nzmqt.cpp\
        ../msg/cpp/base_msgs.pb.cc\
        ../msg/cpp/dev_msgs.pb.cc\
        ../msg/cpp/sim_msgs.pb.cc \
    qcasusceneitem.cpp \
    qcasutreeitem.cpp \
    qcustomplot.cpp \
    qtrendplot.cpp \
    qtreebuffer.cpp \
    qdialogsettings.cpp \
    qdialogconnect.cpp \
    globalHeader.cpp

HEADERS  += arenaui.h\
        nzmqt/nzmqt.hpp \
    qcasusceneitem.h \
    qcasutreeitem.h \
    qcustomplot.h \
    qtrendplot.h \
    qtreebuffer.h \
    qdialogsettings.h \
    qdialogconnect.h \
    globalHeader.h

FORMS    += arenaui.ui

LIBS += -lzmq\
        -lprotobuf\
        -lyaml-cpp


RESOURCES += \
    images.qrc
