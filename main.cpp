/* Programme GeMot
 * Générateur de mot aléatoire ayant une consonnance similaire
 * aux mots fournit dans un dictionnaire
 *
 * Par Sayanel - 2019
 */

#include <QtWidgets>
#include "fenetreprincipale.h"
#include "fonctions.h"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    srand( static_cast<unsigned int>(time(NULL)));

    /*Traduction of Qt texts to Fr*/
    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);
    /*end*/

    FenetrePrincipale fenetre;
    fenetre.show();

    return app.exec();

}

