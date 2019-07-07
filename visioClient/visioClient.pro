QT       += core gui widgets network

TARGET = visioClient
TEMPLATE = app


SOURCES += main.cpp\
        tcpClient.cpp

HEADERS  += tcpClient.h

FORMS    += tcpClient.ui
