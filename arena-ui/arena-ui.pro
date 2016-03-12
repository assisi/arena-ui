#-------------------------------------------------
#
# Project created by QtCreator 2014-01-14T15:53:48
#
#-------------------------------------------------

QT       += core gui

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets\
                                        printsupport

TARGET = arena-ui
TEMPLATE = app


INCLUDEPATH += ../msg/cpp

SOURCES += main.cpp\
        arenaui.cpp\
        nzmqt/nzmqt.cpp\
        ../msg/cpp/base_msgs.pb.cc\
        ../msg/cpp/dev_msgs.pb.cc\
        ../msg/cpp/sim_msgs.pb.cc \
    global.cpp \
    qconnectdialog.cpp \
    qcasusceneitem.cpp \
    qcasutreeitem.cpp \
    qcustomplot.cpp \
    qtrendplot.cpp \
    qtreebuffer.cpp

HEADERS  += arenaui.h\
        nzmqt/nzmqt.hpp \
    global.h \
    qconnectdialog.h \
    qcasusceneitem.h \
    qcasutreeitem.h \
    qcustomplot.h \
    qtrendplot.h \
    qtreebuffer.h

FORMS    += arenaui.ui

LIBS += -lzmq\
        -lprotobuf\
        -lyaml-cpp


RESOURCES += \
    images.qrc
