TEMPLATE = app
CONFIG += qt
CONFIG += c++11

QT += widgets

SOURCES += main.cpp \
    fonctions.cpp \
    mainwindow.cpp \
    fenaide.cpp \
    fenetreprincipale.cpp

HEADERS += \
    fonctions.h \
    mainwindow.h \
    fenaide.h \
    fenetreprincipale.h

DISTFILES +=

FORMS += \
    mainwindow.ui \
    fenaide.ui \
    fenetreprincipale.ui

