#include "fonctions.h"
#include <vector>
#include <string>
#include <sstream>
#include <iostream>
#include <cstdlib>

using namespace std;

void extractWords(ifstream &liste, vector<string> &mots, vector<float> &proba) {
    int nb_lignes = 0; //nombre de lignes de la liste
    string line; //ligne du fichier en train d'être lue
    for (nb_lignes; getline(liste,line); nb_lignes++) {}; //récupère le nombre de ligne de la liste

    liste.clear();
    liste.seekg(0, ios::beg); //replace le curseur au début

    getline(liste, line);
    nb_lignes--; //Passe la première ligne (commentaires)

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

void afficheTab2D(int tab[27][27], int laff, int caff) {
    cout << "\t";
    for (int i=0; i<caff; i++){ //met l'en tête
        cout << i << " "<< (char)(i+96) << "\t";
    }
    cout << endl;
    for (int j=0; j<laff; j++) {
        cout << j << " " << (char)(j+96) <<  ":\t";
        for (int i=0; i<caff; i++) {
            cout << "[" << tab[i][j] << "]\t";
        }
        cout << endl;
    }
    cout << endl;
}

int irand_a_b (int a, int b) {
    return rand()%(b-a) + a;
}

string generateur (double probatab[27][27][27], uint maxsize) {
    string monmot ="";
    int pr1=0; //lettre précédente
    int pr2=0; //avant-dernière lettre
    int pot; //lettre potentielle

    do {
        pot=0;
//        cout << probatab[pot][pr1][pr2] << "\t";
        double r = (double)rand() / RAND_MAX;
        while (r > probatab[pot][pr1][pr2]) {
            pot++;
        }
        monmot += (char)(pot+96);
        pr2=pr1;
        pr1=pot;
    } while (pot!=0 && monmot.size()<maxsize); //!\ size=taille en octet=nb carac en ascii seulement
    monmot.pop_back();

    return monmot;
}


//std::vector<string> stringsplitter(const string &line, char delim) {
//    int size = line.size();
//    int r=0;
//    vector<string> v;
//    for(int i=0; i<size; i++) {
//        if (line[i]==delim) {
//            v.push_back(line.substr(r, i-r));
//            r=i+1;
//        }
//    }
//    v.push_back(line.substr(r, size-r));
//    return v;
//}
