#-------------------------------------------------
#
# Project created by QtCreator 2014-01-14T15:53:48
#
#-------------------------------------------------

QT       += core gui\
            widgets\
            printsupport

TARGET = arena-ui
TEMPLATE = app
CONFIG += warn_on

#CONFIG += link_pkgconfig
#PKGCONFIG += Qt5GStreamer-1.0 Qt5GStreamerUi-1.0


DESTDIR = ../build/bin
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
OBJECTS_DIR = ../build/o

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
    globalHeader.cpp \
    qdialogsetpoint.cpp \
    flowlayout.cpp \
    qdeploy.cpp \
    qcolorbar.cpp

HEADERS  += arenaui.h\
        nzmqt/nzmqt.hpp \
    qcasusceneitem.h \
    qcasutreeitem.h \
    qcustomplot.h \
    qtrendplot.h \
    qtreebuffer.h \
    qdialogsettings.h \
    qdialogconnect.h \
    globalHeader.h \
    qdialogsetpoint.h \
    flowlayout.h \
    qdeploy.h \
    qcolorbar.h

FORMS    += arenaui.ui

LIBS += -lzmq\
        -lprotobuf\
        -lyaml-cpp


RESOURCES += \
    images.qrc
