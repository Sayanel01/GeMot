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

public slots:
    void on_bouton_analyser_clicked();
    void on_bouton_generer_clicked();

    void on_bouton_selecFichier_clicked();
    void on_bouton_details_clicked();

    void on_bouton_nettoyer_clicked();
    void on_bouton_trier_clicked();

    void openHelp();

    void on_check_troll_clicked();
    void unchecking();

private:
    Ui::FenetrePrincipale *ui;

    QString nomListeMots;
    double probatab[27][27][27] = {{{0}}}; //pour methode nulle
    std::map<std::vector<QChar>, std::pair<int,double>> charmap; //pour methode un peu mieux

    bool analysed = false;

    FenAide *m_FenAide;

    QProgressBar *progr_Analyse;
};

#endif // FENETREPRINCIPALE_H
