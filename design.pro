QT       += core gui
QT       += multimediawidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
CONFIG += thread

DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    Cloud.cpp \
    Cluster.cpp \
    Octree.cpp \
    circle.cpp \
    config.cpp \
    main.cpp \
    mainwindow.cpp \
    mythread.cpp \
    openglwidget.cpp \
    parallel_decoder.cpp \
    parallel_encoder.cpp \
    pco.cpp \
    playlistmodel.cpp

HEADERS += \
    Cloud.h \
    Cluster.h \
    Octree.h \
    Octree2.h \
    circle.h \
    config.h \
    mainwindow.h \
    mythread.h \
    openglwidget.h \
    parallel_decoder.h \
    parallel_encoder.h \
    pco.h \
    playlistmodel.h

FORMS += \
    circle.ui \
    mainwindow.ui \
    openglwidget.ui

LIBS += -lopengl32 -luser32 -lglu32

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

RESOURCES += \
    images.qrc


DISTFILES += \
    shapes.frag \
    shapes.vert

INCLUDEPATH += D:\libjpeg\libjpeg-turbo64\include

INCLUDEPATH += D:\zstd-v1.4.4-win64\include

LIBS += -LD:\zstd-v1.4.4-win64\static\
        -llibzstd

LIBS += -LD:\libjpeg\libjpeg-turbo64\lib\
        -ljpeg\
        -lturbojpeg

include(D:\design\pcl.pri)
