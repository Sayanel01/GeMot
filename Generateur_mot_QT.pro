TEMPLATE = app
CONFIG += qt
CONFIG += c++11

QT += widgets

SOURCES += main.cpp \
    fonctions.cpp \
    mainwindow.cpp \
    fenaide.cpp

HEADERS += \
    fonctions.h \
    mainwindow.h \
    fenaide.h

DISTFILES +=

FORMS += \
    mainwindow.ui \
    fenaide.ui

