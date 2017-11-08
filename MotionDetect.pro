#-------------------------------------------------
#
# Project created by QtCreator 2016-09-22T14:55:17
#
#-------------------------------------------------
QT       += core gui widgets network

CONFIG   += c++14

TARGET = MotionDetect
TEMPLATE = app


SOURCES += main.cpp


INCLUDEPATH += /usr/local/include
LIBS += -L /usr/local/lib/*.so

CONFIG += c++14

CONFIG(debug, debug|release){
    DEFINES += DEBUG
}
