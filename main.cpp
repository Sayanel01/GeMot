/* Programme GeMot
 * Générateur de mot aléatoire ayant une consonnance similaire
 * aux mots fournit dans un dictionnaire
 *
 * Par Philémon Giraud - 2018
 */

#include <QApplication>
#include <QtWidgets>
#include "mainwindow.h"
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

using namespace std;


/*AFFTAB = 0 pas d'affichage du tableau
  AFFTAB = 1 affichage du tableau après action utilisateur (laisse le temps d'aggrandir la fenêtre
  AFFTAB = 2 affichage du tableau immédiatement*/
#define AFFTAB 0

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    /*Traduction des texte automatique en fr*/
    QString locale = QLocale::system().name().section('_', 0, 0);
    QTranslator translator;
    translator.load(QString("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&translator);
    /*fin*/

    MainWindow fenetre;
    fenetre.show();

    return app.exec();

}

/*int main()
{
    //Paramètres du programme :
    string nom_liste_def = "../Mots_FR_full_sansaccents.txt"; //liste de mot par défaut
    bool clearaccent = false; //traitement des accent par remplacement
    uint taillemax = 100; //taille maximal des mots
    uint nb = 20; //nombre de mots à générer

    cout << "Invente-moi un moupthomb\n...\n" << endl;

    //Initialisation du générateur aléatoire
    srand(time(NULL));

    //Choix de la liste de mot
    cout << "Indiquer le chemin de la liste de mot" << endl;
    cout << "     (defaut : '" << nom_liste_def << "')" << endl;
    string nom_liste;
    getline(cin,nom_liste);
    if (nom_liste.size() == 0)
        nom_liste=nom_liste_def;

    cout << "Importation de la liste de mots..." << endl;

    vector<string> mots;
    vector<float> proba;
    ifstream Liste_mots(nom_liste_def, ios::in);
    //Produit la liste des mots et leur proba associée
    if(Liste_mots) {
        extractWords(Liste_mots, mots, proba);
        cout << "Liste de mot ouverte\n" << endl;
    }
    else {
        cerr << "Impossible de lire la liste de mots\n" << endl;
        return 1;
    }

    cout << "Analyse de la liste de mots..." << endl;

    //Contruit le tableau des proba d'enchênement
    int lettertab[27][27][27] = {0}; //--> lettertab[2][1][3] = nombre d'occurence de "cab" ("3","1","2")
    int nbt=0; //nombre total de lettres traités
    for(unsigned int i=0; i<mots.size(); i++) {
        nbt += analyzeWord(mots[i], lettertab, clearaccent);
    }
    cout << "Analise terminée\n" << endl;

//    if (AFFTAB==1) {
//        cout << "\nPour l'affichage du tableau, aggrandir la fenètre, puis taper une entrée." << endl;
//        int foo;
//        cin >> foo;
//        cout << foo << endl;
//    }
//    if (AFFTAB==1 || AFFTAB==2) {
//        cout << "Affichage du tableau resultant de l'analyse :\nNombre d'occurence totales analysées : " << nbt << endl;
////        afficheTab2D(lettertab[][][0]);
//    }

    cout << "Analyse du tableau d'analyse..." << endl;
    //Création d'une table des proba cumulative : par ligne (=par doublon précédent), proba d'obtenir une lettre donnée
    //--> Cumulative : proba d'obtenir la lettre 'i' ou une lettre inférieure
    double probatab[27][27][27];
    for (uint k=0; k<27; k++) {
        for (uint j=0;j<27;j++) {
            int nbtlsuiv = 0; //nb tot de lettre suivant l'enchainement 'k-j'
            for (uint i=0;i<27;i++) {
                nbtlsuiv += lettertab[i][j][k];
            }
            for (uint i=0;i<27;i++) {
                probatab[i][j][k] =  (double)lettertab[i][j][k] / nbtlsuiv;
                if(i!=0)
                    probatab[i][j][k] += probatab[i-1][j][k]; //Transformation en proba cumulative
            }
        }
    }

    cout << "Tableau des probabilité terminé\n" << endl;


    cout << "Génération de " << nb << " mots :" << endl;
    //Génaration d'un mot aléatoire
    for (uint i=0; i<nb; i++) {
        cout << " " <<generateur(probatab, taillemax) << endl;
    }


    cout << "\nFin du programme" << endl;
    return 0;
}*/

