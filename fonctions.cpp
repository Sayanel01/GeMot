#include <vector>
#include <sstream>
#include <iostream>
#include "fonctions.h"
#include "fenetreprincipale.h"

//Fonction commencant par Q = fonction pour le traitement gérant tout les type de
//caratère et la longueur de cohérence (Q pour le Q de QString, le moyen le plus
//simple de gérer l'utf-8 (j'ai pas les wchar_t. C'est nul et pas pratique)

using namespace std;

void extractWords(ifstream &liste, vector<string> &mots, vector<float> &proba) {
    int nb_lignes = 0; //nombre de lignes de la liste
    string line; //ligne du fichier en train d'être lue
    for (nb_lignes; getline(liste,line); nb_lignes++) {}; //récupère le nombre de ligne de la liste
    //NON, espèce de compilateur coprolithique de bas fond, cette instruction ne fait pas rien, elle change la valeur de nb_lignes

    liste.clear();
    liste.seekg(0, ios::beg); //replace le curseur au début

    getline(liste, line);
    nb_lignes--; //Passe la première ligne (commentaires)
    /*TODO : mettre en place un vrai gestion des commentaire de début (sur une ou plusieurs lignes
     * A déjà été fait dans le projet cpp 2A traitement d'image --> à reprendre
     * */

    mots.resize(nb_lignes);
    proba.resize(nb_lignes);
    for (int i=0; i<nb_lignes; i++) {
        getline(liste, line);

        string sproba;
        stringstream ss;
        ss << line;
        ss >> mots[i] >> sproba; //divise la ligne en deux (mot / proba du mot)
        if(sproba!="") //si la liste ne comprend pas de fréquence, sproba sera une chaine vide
            proba[i]=std::stof(sproba); //converti la proba en float
        else
            proba[i]=0;
    }
    liste.close();
    return;
}

string retireAccent(string &message)
{
    string accent("ÀÁÂÃÄÅàáâãäåÒÓÔÕÖØòóôõöøÈÉÊËèéêëÌÍÎÏìíîïÙÚÛÜùúûüÿÑñÇç");
    string sansAccent("AAAAAAaaaaaaOOOOOOooooooEEEEeeeeIIIIiiiiUUUUuuuuyNnCc");
    int i=0,j=0,k=0,taille;
    taille=message.size();
    for (i=0;i<=taille;i++) {
        for(j=0;j<=104;j=j+2) {
            if((message[i]==accent[j])&&(message[i+1]==accent[j+1])) {
                message[i]=sansAccent[j/2];
                for(k=i+1;k<taille;k++) {
                    message[k]=message[k+1];
                }
                message=message.substr(0,taille-1);
                taille=message.size();
            }
        }
    }
    return message;
}

int analyzeWord(string &lemot, int lettertab[27][27][27], bool ClearAccent) {
    int pr1=0; //lettre précédente, intialisée à rien (0)
    int pr2=0; //lettre présente 2 lettre avant, intialisée à rien (0)
    int act; //lettre actuelle
    int nb=0; //nombre total de lettre traitées
    for(unsigned int i=0; i<lemot.size(); i++) {
        if(ClearAccent==true)
            lemot=retireAccent(lemot);
        act=lemot.at(i)-96; // retourne le code ascii de la lettre, puis ramené à a=1
        if(act>0 && act <27) { //on ignore tout caractère accentué
            lettertab[act][pr1][pr2]++;
            pr2=pr1;
            pr1=act; //lettre actuelle devient la précédente
            nb++;
        }
    }
    lettertab[0][pr1][pr2]++; //indique que "pr2-pr1" sont les 2 dernières lettres du mot
    return nb;
}

void QanalyzeWord(const QString &lemot, map<vector<QChar>, pair<int,double>> &charmap, uint lcoh, int &nb) {
    vector<QChar> suiteLettres(lcoh,'\0'); //vecteur de longueur lcoh, initialisé à \0

    for(int i=0; i<lemot.size(); i++) {
        suiteLettres[lcoh-1]=lemot.at(i); //lcoh = suiteLettres.size()
        charmap[suiteLettres].first++;
        nb++;
        for(uint j=0; j<lcoh-1; j++) {
            suiteLettres[j]=suiteLettres[j+1];
        }
    }
    //Indication de dernier charactère=vide
    suiteLettres[lcoh-1]='\0';
    charmap[suiteLettres].first++;
    return;
}

string generateur (double probatab[27][27][27], bool forcedSize, uint maxsize) {
    string monmot ="";
    int pr1=0; //lettre précédente
    int pr2=0; //avant-dernière lettre
    int pot; //lettre potentielle
    do {
        pot=0;
        double r = (double)rand() / RAND_MAX;
        while (r > probatab[pot][pr1][pr2] && pot<26) {
            pot++;
        }
        if (pot!=0){ //si pot=0 (eg. fin du mot) MAIS forcedSize, alors on ignore ce caractère
            monmot += (char)(pot+96);
            pr2=pr1;
            pr1=pot;
        }
    } while ((pot!=0 || forcedSize) && monmot.size()<=maxsize); //!\ size=taille en octet=nb carac en ascii seulement

    return monmot;
}

string Qgenerateur(map<vector<QChar>, pair<int,double>> &charmap, uint lcoh, bool forcedSize, uint maxsize) {
    QString monmot="";
    vector<QChar> cePr(lcoh-1,'\0');

    vector<QChar> cePrMin, cePrMax; //debut et fin des élément de la map ayant ce Pr
    cePrMin.reserve(lcoh); cePrMax.reserve(lcoh);
    map<vector<QChar>, pair<int,double>>::iterator it, itLow, itHigh;
    //itLow : itérateur vers le premier éléments de la map ayant ce Pr
    //itHigh: itérateur vers l'élément suivant le dernier éléments de la map ayant ce Pr

    do {
        cePrMin=cePr;   cePrMin.push_back(QChar::Null);
        cePrMax=cePr;   cePrMax.push_back(QChar::LastValidCodePoint);

        itLow = charmap.lower_bound(cePrMin);
        itHigh = charmap.upper_bound(cePrMax);
        it = itLow;

        double r = (double)rand() / RAND_MAX;
        while (r > it->second.second && it != itHigh) {
            it++;
        }
        monmot += QString(it->first.back());
        for(uint i=0; i<cePr.size()-1; i++) {
            cePr[i] = cePr[i+1];
        }
        cePr[cePr.size()-1] = it->first.back();
    } while ( (it->first.back()!='\0') && (uint)monmot.size() <= maxsize);
    //TODO : implémenter le forced size (pour le moment bug car si it atteint la fin (=fin de mot sans forcedsize), pb pour le remettre où il faut (je crois)(je sais plus))
    return monmot.toStdString();
}

void triParTaille(QString &liste_mots) {
    QStringList splitted(liste_mots.split('\n'));
    Qquicksort(splitted, 0, splitted.size()-1);
    liste_mots = splitted.join("\n");
}

void Qechanger(QStringList &liste, int a, int b) {
    //Fonction utilisée par le tri quicksort
    QString temp = liste.at(a);
    liste.replace(a, liste.at(b));
    liste.replace(b,temp);
}

void Qquicksort(QStringList &liste, int debut, int fin) {
    int gauche=debut-1;
    int droite=fin+1;
    const int pivot = liste.at(debut).size();

    if(debut>=fin)
        return;

    while(1) {
        do {droite--; } while(liste.at(droite).size() > pivot);
        do gauche++; while(liste.at(gauche).size() < pivot);

        if(gauche < droite)
            Qechanger(liste, gauche, droite);
        else break;
    }

    Qquicksort(liste, debut, droite);
    Qquicksort(liste, droite+1, fin);
}
