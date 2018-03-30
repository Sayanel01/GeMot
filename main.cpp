/* Programme GeMot
 * Générateur de mot aléatoire ayant une consonnance similaire
 * aux mots fournit dans un dictionnaire
 *
 * Par Philémon Giraud - 2018
 */

#include <QtWidgets>
#include "fenetreprincipale.h"
#include "fonctions.h"

/*
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <time.h>
*/


int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /*Traduction des texte automatique en fr*/
    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);
    /*fin*/

    FenetrePrincipale fenetre;
    fenetre.show();

    return app.exec();

}

