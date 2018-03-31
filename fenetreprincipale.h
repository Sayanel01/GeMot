#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include <QProgressBar>
#include "fonctions.h"
#include "fenaide.h"

namespace Ui {
class FenetrePrincipale;
}

class FenetrePrincipale : public QMainWindow
{
    Q_OBJECT

public:
    explicit FenetrePrincipale(QWidget *parent = 0);
    ~FenetrePrincipale();

    enum type_Trait {aucun, ascii, asciiplus, utf_8};
    type_Trait selectedTrait();

public slots:
    void on_bouton_analyser_clicked();
    void on_bouton_generer_clicked();

    void on_bouton_selecFichier_clicked();
    void on_bouton_details_clicked();

    void on_bouton_nettoyer_clicked();
    void on_bouton_trier_clicked();

    void on_actionAide_triggered();

    void on_check_troll_clicked();
    void unchecking();

private slots:
    //Slot de warning, analyse à refaire
    void check_analyse_changed();
    void on_spin_lcoh_valueChanged(int value);
    void traitement_modifie();
    void liste_modifie();

private:
    Ui::FenetrePrincipale *ui;

    QString nomListeMotsDefaut="WordLists/Mots_FR_avec_frequence.txt";
    double probatab[27][27][27] = {{{0}}}; //pour methode nulle
    std::map<std::vector<QChar>, std::pair<int,double>> charmap; //pour methode un peu mieux

    //Paramètre de l'analyse faite :
    QString nomListeMots;
    type_Trait analyse = aucun;
    uint lcoh;

    //Paramètre changement sur l'onglet analyse
    bool listeMots_changed=false;
    bool traitement_changed=false;
    bool lcoh_changed=false;

    FenAide *m_FenAide;
};

#endif // FENETREPRINCIPALE_H
