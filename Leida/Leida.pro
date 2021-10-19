#-------------------------------------------------
#
# Project created by QtCreator 2021-09-28T09:07:57
#
#-------------------------------------------------

QT       += core gui

win32{
LIBS += -luser32
}

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets printsupport network

TARGET = Leida
TEMPLATE = app

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS
DEFINES += QT_MESSAGELOGCONTEXT

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

CONFIG += c++11

SOURCES += \
        ipaddredit.cpp \
        ippartlineedit.cpp \
        main.cpp \
        leida.cpp \
        qcustomplot.cpp

HEADERS += \
        common.h \
        ipaddredit.h \
        ippartlineedit.h \
        leida.h \
        log.hpp \
        qcustomplot.h \
        recvdata.h

FORMS += \
        leida.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target
