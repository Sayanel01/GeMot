#ifndef FONCTIONS
#define FONCTIONS

#endif // FONCTIONS

#include <vector>
#include <fstream>
#include <string>

void extractWords(std::ifstream &liste, std::vector<std::string> &mots, std::vector<float> &proba);
int analyzeWord(const std::string &lemot, int lettertab[27][27][27]);
void afficheTab2D(int tab[27][27], int laff=27, int caff=27);
int irand_a_b(int a, int b);
std::string generateur(double probatab[27][27][27], uint maxsize=100);

//std::vector<string> stringsplitter(const string &line, char delim = 9);
