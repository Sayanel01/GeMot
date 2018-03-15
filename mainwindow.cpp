#include <vector>
#include <string>
#include <fstream>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
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
    ui->progr_Analyse->setValue(0);
    int avRecup(5), avAnal(90), avProbatab(5); //doit sommer a 100. Avancement (%) de chaque étape

    //Choix liste mot (selon valeur radio)
    if (ui->radio_LangDef->isChecked()) {
        nomListeMots = "../Mots_FR_full_sansaccents.txt"; //TODO : placer un paramètre string nom_liste_defaut
    }
    else if (ui->radio_LanPerso->isChecked()) {
//        nomListeMots = nomListeMots; //on a déjà ça ><
    }
    else {
        ui->progr_Analyse->setValue(42);
        ui->label_ResAnalyse->setText("How did you do that ?");
        return; //Si aucun bouton coché : arrêter tout
    }
    //Importation liste mots
    std::vector<std::string> mots;
    std::vector<float> proba;
    std::ifstream Liste_mots(nomListeMots.toStdString(), std::ios::in);
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
        nbt += analyzeWord(mots[i], lettertab, ui->check_accent->isChecked());
        if (i%100==0) { //Possibilité : 10000 (haché), 100000 (3 étapes)
            ui->progr_Analyse->setValue(avRecup+i/(float)mots.size()*avAnal);
            QCoreApplication::processEvents(); //permet l'actualisation du gui. Ralenti les calcul...
        }
    }
    ui->label_ResAnalyse->setText("Liste de mots analysée. Analyse de l'analyse en cours...");
//    ui->progr_Analyse->setValue(66);

    //Création du tableau des probas cumulatives probatab
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
    ui->label_ResAnalyse->setText("Tableau de probabilité construit.\n Prêt à inventer des mots !");
    ui->progr_Analyse->setToolTip("Non, la barre ne va pas à 100%. C'est frustrant, hein ?");

    analysed=true;
    ui->frame_generation->setEnabled(true);
}

void MainWindow::on_bouton_GenMots_clicked() {
    if (!analysed)
        return;
    uint taille_max = ui->spin_tailleMax->value();

    if(taille_max==0)
        taille_max=100;

    for (int i=0; i<ui->spin_NbMots->value(); i++) {
        std::string mot(generateur(probatab, taille_max, ui->check_forceTaille->isChecked()));
        ui->text_ResMots->append(QString::fromStdString(mot));
    }
}

void MainWindow::on_bouton_clear_clicked() {
    ui->text_ResMots->clear();
}

void MainWindow::on_bouton_selecFichier_clicked() {
    nomListeMots = QFileDialog::getOpenFileName(this, "Selectionner une liste de mot .txt", "", "Fichier texte (*.txt)");
    QFile file(nomListeMots);
    QFileInfo fileinfo(file);
    ui->bouton_selecFichier->setText(fileinfo.fileName());

    ui->radio_LanPerso->setChecked(true);
}

