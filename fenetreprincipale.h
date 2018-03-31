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

    enum type_Trait {
        aucun=0,
        ascii=1,
        asciiplus=2,
        utf_8=3
    };

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
    void analyse_changed(bool changed);
    void on_spin_lcoh_valueChanged(int value);

    void on_radio_ignore_toggled(bool checked);
    void on_radio_minable_toggled(bool checked);
    void on_radio_speciaux_toggled(bool checked);

private:
    Ui::FenetrePrincipale *ui;

    QString nomListeMotsDefaut="WordLists/Mots_FR_avec_frequence.txt";
    double probatab[27][27][27] = {{{0}}}; //pour methode nulle
    std::map<std::vector<QChar>, std::pair<int,double>> charmap; //pour methode un peu mieux

    //Paramètre de l'analyse faite :
    QString nomListeMots;
    type_Trait analyse = aucun;
//    bool A_analysed = false; //indique que l'analyse à été fait par la méthode A
//    bool B_analysed = false; // --- --- par la méthode B
//    bool clearAcc_analysed = false;
    uint lcoh;

    FenAide *m_FenAide;
};

#endif // FENETREPRINCIPALE_H
