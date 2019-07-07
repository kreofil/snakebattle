QT       += core gui widgets network

TARGET = snakeServer
TEMPLATE = app


SOURCES += main.cpp\
        game.cpp \
        tcpServer.cpp

HEADERS  += tcpServer.h \
        game.h

FORMS    += tcpServer.ui
