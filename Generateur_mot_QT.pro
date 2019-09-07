TEMPLATE = app
CONFIG += qt
CONFIG += c++11

QT += widgets

SOURCES += main.cpp \
    fonctions.cpp \
    fenaide.cpp \
    fenetreprincipale.cpp \
    infos.cpp

HEADERS += \
    fonctions.h \
    fenaide.h \
    fenetreprincipale.h \
    infos.h

DISTFILES +=

FORMS += \
    fenaide.ui \
    fenetreprincipale.ui \
    infos.ui

RESOURCES += \
    ressources.qrc

