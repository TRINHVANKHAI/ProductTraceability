#-------------------------------------------------
#
# Project created by QtCreator 2022-08-09T10:43:00
#
#-------------------------------------------------

QT       += core gui network sql

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ProductTraceability
TEMPLATE = app
RC_ICONS = ProductTraceability.ico

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0


SOURCES += main.cpp\
    globalapplication.cpp \
    mainmenu.cpp \
    statusitemwidget.cpp \
    tcpserver.cpp \
    tcpclient.cpp \
    userinfo.cpp \
    dialog.cpp \
    popupstatus.cpp \
    fileexport.cpp \
    dbcontroller.cpp

HEADERS  += mainmenu.h \
    globalapplication.h \
    statusitemwidget.h \
    tcpserver.h \
    tcpclient.h \
    userinfo.h \
    dialog.h \
    popupstatus.h \
    fileexport.h \
    dbcontroller.h

FORMS    += mainmenu.ui \
    statusitemwidget.ui \
    userinfo.ui \
    dialog.ui \
    popupstatus.ui \
    fileexport.ui
