QT       += core gui widgets network

TARGET = gamerClient
TEMPLATE = app


SOURCES += main.cpp\
        tcpclient.cpp

HEADERS  += tcpclient.h

FORMS    += tcpclient.ui
