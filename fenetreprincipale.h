#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include <QProgressBar>
#include <memory>
#include "fonctions.h"
#include "fenaide.h"
#include "infos.h"

namespace Ui {
class FenetrePrincipale; //=Main Window
}

class FenetrePrincipale : public QMainWindow
{
    Q_OBJECT

public:
    explicit FenetrePrincipale(QWidget *parent = 0);
    ~FenetrePrincipale();

    enum type_Trait {aucun, ascii, asciiplus, utf_8};
    type_Trait selectedTrait();
    type_Trait stringToType(QString stringType);

public slots:
    //Analyses a wordlist
    void on_bouton_analyser_clicked();
    //Generates a word based on previously analysed wordlist
    void on_bouton_generer_clicked();

    //
    void on_bouton_selecFichier_clicked();
    //Adds detail on the 3 possible analyses
    void on_bouton_details_clicked();

    //Removes all generated words
    void on_bouton_nettoyer_clicked();
    //Sort generated words by lenght
    void on_bouton_trier_clicked();

    //Menu icon 'Aide'
    void on_actionAide_triggered();
    //Menu icon 'About'
    void on_action_propos_de_ce_programme_triggered();
    //Change the default values of the Analyse tab to the current values.
    void on_actionUtiliser_les_valeur_actuelles_par_defaut_triggered();

    // Just a test function : unckecks the box 0.4 second after it has been clicked
    void on_check_troll_clicked();
    void unchecking();

    //Message when quitting through menu
    void quit_troll();

private slots:
    //Indicates if the current parameters of the 'Analyse tab' have been modified since previous analisys
    void check_analyse_changed();
    //Checks if the current coherence lengh have been modified since previous analisys
    void on_spin_lcoh_valueChanged(int value);
    //Checks if the current analysis method have been modified since previous analisys
    void traitement_modifie();
    //Checks if the current wordlist have been modified since previous analisys
    void liste_modifie();

private:
//    Ui::FenetrePrincipale *ui;
    std::unique_ptr<Ui::FenetrePrincipale> ui;

//    QString nomListeMotsDefaut;
    QString nomListeMotsDefaut="WordLists/Mots_FR_full.txt";
    double probatab[27][27][27] = {{{0}}}; //result of the analysis of the wordlist (for 'ascii' and 'ascii+')
    std::map<std::vector<QChar>, std::pair<int,double>> charmap; //result of the analysis (for 'utf-8')

    //Parameter of the previous analysis:
    QString nomListeMots; //name of the wordlist
    type_Trait analyse = aucun; //indicates what analysis has previously been done. Acun = nothing
    uint lcoh; //coherence lengh of previous analyse

    //Indicates if a change has been made on the 'Analysis' tab
    bool listeMots_changed=false;
    bool traitement_changed=false;
    bool lcoh_changed=false;
    QString nomListeAnalysePrecedente;

    FenAide *m_FenAide;
    Infos *m_Infos;
};

#endif // FENETREPRINCIPALE_H
