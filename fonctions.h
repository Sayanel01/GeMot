#ifndef FONCTIONS
#define FONCTIONS


#include <vector>
#include <fstream>
#include <string>
#include <map>
#include <QString>

void extractWords(std::ifstream &liste, std::vector<std::string> &mots, std::vector<float> &proba);

int analyzeWord(std::string &lemot, int lettertab[27][27][27], bool ClearAccent=false);
void QanalyzeWord(const QString &lemot, std::map<std::vector<QChar>, std::pair<int,double>> &charmap, uint lcoh, int &nb);

std::string generateur(double probatab[27][27][27], uint maxsize=100, bool forcedSize=false);
std::string Qgenerateur(std::map<std::vector<QChar>, std::pair<int,double>> &charmap, uint lcoh, uint maxsize, bool forcedSize);

std::string retireAccent(std::string &message);


#endif // FONCTIONS
