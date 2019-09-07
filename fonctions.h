#ifndef FONCTIONS
#define FONCTIONS

#include <map>
#include <vector>
#include <fstream>
#include <QString>

//Get the words listed in the wordlist (text file)
void extractWords(std::ifstream &liste, std::vector<std::string> &mots, std::vector<float> &proba);

//Remove accentuated letters (for 'analyze word')
std::string retireAccent(std::string &message);


//Fonctions beginning by 'Q' use words made of Qchar instead of char and hence are able to handle any character

//analyse one word and completes lettertab (of "fenetreprincipale"). ClearAccent=true handles accentuated letter by replacing them by their unaccentuated version
int analyzeWord(std::string &lemot, int lettertab[27][27][27], bool ClearAccent=false);
//analyse on word and completes the charmap (of "fenetreprincipale")
void QanalyzeWord(const QString &lemot, std::map<std::vector<QChar>, std::pair<int,double>> &charmap, uint lcoh, int &nb);


//Generates a word based on the table 'probatab'
std::string generateur(double probatab[27][27][27], bool forcedSize=false, uint maxsize=100);
//Generates a word based on the character map (charmap)
std::string Qgenerateur(std::map<std::vector<QChar>, std::pair<int,double>> &charmap, uint lcoh=3, uint maxsize=100);


//triParTaille : sort word displayed by size. Calls quicksort and echanger
void triParTaille(QString &liste_mots);
    void Qechanger(QStringList &liste, int a, int b);
    void Qquicksort(QStringList &liste, int debutR, int fin);

#endif // FONCTIONS
