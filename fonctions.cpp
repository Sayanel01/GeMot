#include <vector>
#include <sstream>
#include <iostream>
#include "fonctions.h"
#include "fenetreprincipale.h"

//Fonction commencant par Q = fonction pour le traitement gérant tout les type de
//caratère et la longueur de cohérence (Q pour le Q de QString, le moyen le plus
//simple de gérer l'utf-8 (j'aime pas les wchar_t. C'est nul et pas pratique)

using namespace std;

void extractWords(ifstream &liste, vector<string> &mots, vector<float> &proba) {
    uint nb_lignes = 0; //number of lines on the wordlist
    string line; //last read line
    for (; getline(liste,line); nb_lignes++) {}; //Get number of lines

    liste.clear();
    liste.seekg(0, ios::beg); //going back to beginning of the file

    getline(liste, line);
    nb_lignes--; //Ignore first line
    //TODO : manage comments on the file, e.g. lines beginning by //

    mots.resize(nb_lignes);
    proba.resize(nb_lignes);
    for (uint i=0; i<nb_lignes; i++) {
        getline(liste, line);

        string sproba;
        stringstream ss;
        ss << line;
        ss >> mots[i] >> sproba; // get 1st word = actual word and 2nd word = occurence probability
        if(sproba!="")
            proba[i]=stof(sproba); //conversion to float
        else  //if no word probability is indicated, set it at 0
            proba[i]=0;
    }
    liste.close();
    return;
}

string retireAccent(string &message)
{
    string accent("ÀÁÂÃÄÅàáâãäåÒÓÔÕÖØòóôõöøÈÉÊËèéêëÌÍÎÏìíîïÙÚÛÜùúûüÿÑñÇç");
    string sansAccent("AAAAAAaaaaaaOOOOOOooooooEEEEeeeeIIIIiiiiUUUUuuuuyNnCc");
    uint i=0,j=0,k=0;
    size_t taille;
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

int analyzeWord(string &word, int lettertab[27][27][27], bool ClearAccent) {
    int pr1=0; //previous letter, default is nothing (0)
    int pr2=0; //second-to-last letter, default is nothing (0)
    int curr; //lettre actuelle
    int nb=0; //nombre total de lettre traitées
    for(unsigned int i=0; i<word.size(); i++) {
        if(ClearAccent==true)
            word=retireAccent(word);
        curr=word.at(i)-96; // returns ascii code for the letter
        if(curr>0 && curr <27) { //ignoring accentuated letter
            lettertab[curr][pr1][pr2]++;
            pr2=pr1;
            pr1=curr; //current letter is now previous letter
            nb++;
        }
    }
    lettertab[0][pr1][pr2]++; //indicates the word finishes by "...."pr2""pr1"
    return nb;
}

void QanalyzeWord(const QString &word, map<vector<QChar>, pair<int,double>> &charmap, uint lcoh, int &nb) {
    vector<QChar> letterChain(lcoh,'\0');

    for(int i=0; i<word.size(); i++) {
        letterChain[lcoh-1]=word.at(i);
        charmap[letterChain].first++;
        nb++;
        for(uint j=0; j<lcoh-1; j++) {
            letterChain[j]=letterChain[j+1];
        }
    }

    //Indicates last character is void
    letterChain[lcoh-1]='\0';
    charmap[letterChain].first++;
    return;
}

string generateur (double probatab[27][27][27], bool forcedSize, uint maxsize) {
    string monmot ="";
    int pr1=0; //lettre précédente
    int pr2=0; //avant-dernière lettre
    int pot; //lettre potentielle
    do {
        pot=0;
        double r = double(rand()) / RAND_MAX;
        while (r > probatab[pot][pr1][pr2] && pot<26) {
            pot++;
        }
        if (pot!=0){ //si pot=0 (eg. fin du mot) MAIS forcedSize, alors on ignore ce caractère
            monmot += char((pot+96));
            pr2=pr1;
            pr1=pot;
        }
    } while ((pot!=0 || forcedSize) && monmot.size()<=maxsize); //!\ size=taille en octet=nb carac en ascii seulement

    return monmot;
}

string Qgenerateur(std::map<std::vector<QChar>, pair<int,double>> &charmap, uint lcoh, uint maxsize) {
    QString myword="";

    vector<QChar> cePr(lcoh-1,'\0'); //represents the chain of previous characters
    vector<QChar> cePrMin, cePrMax; //represents the chain of previous character + the first (resp. last) possible character
    cePrMin.reserve(lcoh); cePrMax.reserve(lcoh);
    map<vector<QChar>, pair<int,double>>::iterator it, itLow, itHigh;
    //itLow : iterator to the first element of the map having this chain of previous character (=cePr)
    //itHigh: iterator to the element after the last element of the map having this chain of previous character (=cePr)

    do {
        cePrMin=cePr;   cePrMin.push_back(QChar::Null);
        cePrMax=cePr;   cePrMax.push_back(QChar::LastValidCodePoint);

        itLow = charmap.lower_bound(cePrMin);
        itHigh = charmap.upper_bound(cePrMax);
        it = itLow;

        double r = double(rand())/ RAND_MAX; // 0 < r < 1
        while (r > it->second.second && it != itHigh) { //places iterator to the 1st caracter having a probability less than r
            it++;
        }
        myword += QString(it->first.back()); //append this caracter to the word

        for(uint i=0; i<cePr.size()-1; i++) {
            cePr[i] = cePr[i+1];
        }
        cePr[cePr.size()-1] = it->first.back();
    } while ( (it->first.back()!='\0') && uint(myword.size()) <= maxsize);
    return myword.toStdString();
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
