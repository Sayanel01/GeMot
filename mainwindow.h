#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>
#include "fonctions.h"
#include "fenaide.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_bouton_LancerAnalyse_clicked();
    void on_bouton_GenMots_clicked();
    void on_bouton_clear_clicked();
    void on_bouton_selecFichier_clicked();
    void on_bouton_triTaille_clicked();
    void openHelp();
    void on_check_troll_clicked();
    void uncheking();

private:
    Ui::MainWindow *ui;

    double probatab[27][27][27] = {{{0}}};
    std::map<std::vector<QChar>, std::pair<int,double>> charmap;

    bool analysed = false;
    QString nomListeMots;
    FenAide *m_FenAide;
};

#endif // MAINWINDOW_H
