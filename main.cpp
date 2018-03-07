/* Programme GeMot
 * Générateur de mot aléatoire ayant une consonnance similaire
 * aux mots fournit dans un dictionnaire
 *
 * Par Philémon Giraud - 2018
 */

#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <sstream>
#include <cstdlib>
#include <time.h>
#include "fonctions.h"

using namespace std;


/*AFFTAB = 0 pas d'affichage du tableau
  AFFTAB = 1 affichage du tableau après action utilisateur (laisse le temps d'aggrandir la fenêtre
  AFFTAB = 2 affichage du tableau immédiatement*/
#define AFFTAB 0

int main()
{
    cout << "Starting Program\n...\n" << endl;

    //Initialisation du générateur aléatoire
    srand(time(NULL));

    cout << "Importation de la liste de mots..." << endl;

    vector<string> mots;
    vector<float> proba;
    ifstream Liste_mots("../Mots_FR.txt", ios::in);
    //Produit la liste des mots et leur proba associée
    if(Liste_mots) {
        extractWords(Liste_mots, mots, proba);
        cout << "Liste de mot ouverte\n" << endl;
    }
    else {
        cerr << "Impossible de lire la liste de mots\n";
        return 1;
    }

    cout << "Analyse de la liste de mots..." << endl;

    //Contruit le tableau des proba d'enchênement
    int lettertab[27][27][27] = {0}; //--> lettertab[2][1][3] = nombre d'occurence de "cab" ("3","1","2")
    int nbt=0; //nombre total de lettres traités
    for(unsigned int i=0; i<mots.size(); i++) {
//        cout << i << "   ...   "<< mots[i] << endl;
        nbt += analyzeWord(mots[i], lettertab);
    }

    cout << "Analise terminée\n" << endl;

    if (AFFTAB==1) {
        cout << "\nPour l'affichage du tableau, aggrandir la fenètre, puis taper une entrée." << endl;
        int foo;
        cin >> foo;
        cout << foo << endl;
    }
    if (AFFTAB==1 || AFFTAB==2) {
        cout << "Affichage du tableau resultant de l'analyse :\nNombre d'occurence totales analysées : " << nbt << endl;
//        afficheTab2D(lettertab[][][0]);
    }

    //Création d'une table des proba cumulative : par ligne (=par lettre précédente), proba d'obtenir une lettre donnée
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
    //        cout << (char)(j+96) << " " << nbtlsuiv << endl;
        }
    }

    //Génaration d'un mot aléatoire
    for (int i=0; i<20; i++) {
        cout << generateur(probatab) << endl;
    }


    cout << "\nFin du programme" << endl;
    return 0;
}

