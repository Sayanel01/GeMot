TEMPLATE = app
CONFIG += qt
CONFIG += c++11

QT += widgets

SOURCES += main.cpp \
    fonctions.cpp \
    fenaide.cpp \
    fenetreprincipale.cpp

HEADERS += \
    fonctions.h \
    fenaide.h \
    fenetreprincipale.h

DISTFILES +=

FORMS += \
    fenaide.ui \
    fenetreprincipale.ui

RESOURCES += \
    ressources.qrc

