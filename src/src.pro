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
CONFIG += warn_on\
          c++11

DESTDIR = ../build/bin
MOC_DIR = ../build/moc
RCC_DIR = ../build/rcc
UI_DIR = ../build/ui
OBJECTS_DIR = ../build/o

INCLUDEPATH += msg \

SOURCES += main.cpp\
        arenaui.cpp\
        nzmqt/nzmqt.cpp\
        msg/base_msgs.pb.cc\
        msg/dev_msgs.pb.cc\
        msg/sim_msgs.pb.cc \
        QCustomPlot/qcustomplot.cpp \
        qcasusceneitem.cpp \
        qcasutreeitem.cpp \
        qtrendplot.cpp \
        qdialogsettings.cpp \
        globalHeader.cpp \
        qdialogsetpoint.cpp \
        flowlayout.cpp \
        qdeploy.cpp \
#        qgstreamerview.cpp
        qcasuscenegroup.cpp \
    qgraphicsviewzoom.cpp \
    qarenascene.cpp \
    qcasuzmq.cpp \
    qcasutreegroup.cpp \
    qabstractsceneitem.cpp \
    qabstracttreeitem.cpp \
    qdialogconnect.cpp


HEADERS  += arenaui.h\
        nzmqt/nzmqt.hpp \
        QCustomPlot/qcustomplot.h \
        qcasusceneitem.h \
        qcasutreeitem.h \
        qtrendplot.h \
        qdialogsettings.h \
        globalHeader.h \
        qdialogsetpoint.h \
        flowlayout.h \
        qdeploy.h \
#        qgstreamerview.h
    qcasuscenegroup.h \
    qgraphicsviewzoom.h \
    qarenascene.h \
    qcasuzmq.h \
    qabstractsceneitem.h \
    qabstracttreeitem.h \
    qcasutreegroup.h \
    qdialogconnect.h


FORMS    += arenaui.ui \
    qdialogconnect.ui

LIBS += -lzmq\
        -lprotobuf\
        -lyaml-cpp


#CONFIG += link_pkgconfig
#PKGCONFIG += Qt5GStreamer-1.0 Qt5GStreamerUi-1.0

RESOURCES += \
    images.qrc
