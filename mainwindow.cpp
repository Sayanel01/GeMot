#include <vector>
#include <string>
#include <fstream>
#include <QCoreApplication>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_bouton_LancerAnalyse_clicked() {
    std::string nom_liste;
    ui->progr_Analyse->setValue(0);
    int avRecup(5), avAnal(90), avProbatab(5); //doit sommer a 100. Avancement (%) de chaque étape

    //Choix liste mot (selon valeur radio)
    if (ui->radio_LangDef->isChecked()) {
        nom_liste = "../Mots_FR_full_sansaccents.txt"; //TODO : placer un paramètre string nom_liste_defaut
    }
    else if (ui->radio_LanPerso->isChecked()) {
        //Not implemented yet
        ui->label_ResAnalyse->setText("Aborted. Not implemented yet");
        return;
    }
    else {
        ui->progr_Analyse->setValue(42);
        ui->label_ResAnalyse->setText("How did you do that ?");
        return; //Si aucun bouton coché : arrêter tout
    }
    //Importation liste mots
    std::vector<std::string> mots;
    std::vector<float> proba;
    std::ifstream Liste_mots(nom_liste, std::ios::in);
    if(Liste_mots) {
        extractWords(Liste_mots, mots, proba);
        ui->label_ResAnalyse->setText("Liste de mots récupérée. Analyse en cours...");
        ui->progr_Analyse->setValue(avRecup);
    }
    else {
        ui->label_ResAnalyse->setText("Impossible de lire la liste de mots. ");
        return;
    }

    //Analyse liste mots
    int lettertab[27][27][27] = {0}; //--> lettertab[2][1][3] = nombre d'occurence de "cab" ("3","1","2")
    int nbt=0; //nombre total de lettres traités
    for(unsigned int i=0; i<mots.size(); i++) {
        nbt += analyzeWord(mots[i], lettertab, false); //TODO : placer un paramètre bool clearaccent
        if (i%100==0) { //Possibilité : 10000 (haché), 100000 (3 étapes) //TODO : choix de la précision ici ?
            ui->progr_Analyse->setValue(avRecup+i/(float)mots.size()*avAnal);
            QCoreApplication::processEvents(); //permet l'actualisation du gui. Ralenti les calcul...
        }
    }
    ui->label_ResAnalyse->setText("Liste de mots analysée. Analyse de l'analyse en cours...");
//    ui->progr_Analyse->setValue(66);

    //Création du tableau des probas cumulatives
    double probatab[27][27][27];
    for (uint k=0; k<27; k++) {
        for (uint j=0;j<27;j++) {
            int nbtlsuiv = 0; //nb tot de lettre suivant l'enchainement 'k-j'
            for (uint i=0;i<27;i++) {
                nbtlsuiv += lettertab[i][j][k];
            }
            for (uint i=0;i<27;i++) {
                probatab[i][j][k] =  (double)lettertab[i][j][k] / nbtlsuiv;
                if(i!=0)
                    probatab[i][j][k] += probatab[i-1][j][k]; //Transformation en proba cumulative
            }
        }
    }
    ui->progr_Analyse->setValue(avRecup+avAnal+avProbatab-1);
    ui->label_ResAnalyse->setText("Tableau de probabilité construit. Prêt à inventer des mots !");
    ui->progr_Analyse->setToolTip("Non, la barre ne va pas à 100%. C'est frustrant, hein ?");
}

void MainWindow::on_bouton_GenMots_clicked() {
    
    
    ui->text_ResMots->setText("hgd");
}
