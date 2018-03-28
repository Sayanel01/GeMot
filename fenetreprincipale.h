#ifndef FENETREPRINCIPALE_H
#define FENETREPRINCIPALE_H

#include <QMainWindow>
#include "fenaide.h"
#include "fonctions.h"

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
    double probatab[27][27][27] = {{{0}}};
    std::map<std::vector<QChar>, std::pair<int,double>> charmap;

    bool analysed = false;

    FenAide *m_FenAide;
};

#endif // FENETREPRINCIPALE_H
