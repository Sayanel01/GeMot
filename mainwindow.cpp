#include <vector>
#include <string>
#include <fstream>
#include <QCoreApplication>
#include <QFileDialog>
#include <QFile>
#include <QFileInfo>
#include <QTimer>
#include <thread>
#include <chrono>
#include <algorithm>
#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    QString ListeMotsInstruction("La liste de mot doit être au format .txt, avec 1 seul mot par ligne\n"
                                 "Si plusieurs mots par lignes, seul le premier est pris en compte");
    ui->radio_LanPerso->setToolTip(ListeMotsInstruction);
    ui->bouton_selecFichier->setToolTip(ui->radio_LanPerso->toolTip());

    connect(ui->action_aide, SIGNAL(triggered()), this, SLOT(openHelp()));
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
        nomListeMots = "../Mots_FR_avec_frequence.txt"; //TODO : placer un paramètre string nom_liste_defaut
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
        ui->label_ResAnalyse->setText("Liste de mots récupérée. Analyse...");
        ui->progr_Analyse->setValue(avRecup);
    }
    else {
        ui->label_ResAnalyse->setText("Impossible de lire la liste de mots. ");
        return;
    }

    //Analyse liste mots
    int nb=0; //nombre de lettres traitées
    uint lcoh = ui->spin_lcoh->value();
    for(uint i=0; i<mots.size(); i++) {
        QString qmot=QString::fromStdString(mots[i]);
        QanalyzeWord(qmot,charmap, lcoh, nb);
        if (i%100==0) { //Possibilité : 10000 (haché), 100000 (3 étapes)
            ui->progr_Analyse->setValue(avRecup+i/(float)mots.size()*avAnal);
            QCoreApplication::processEvents(); //permet l'actualisation du gui. Ralenti les calcul...
        }
    }
    //Ancien code
    /*int nbt=0; //nombre total de lettres traités
    int lettertab[27][27][27] = {0}; //--> lettertab[2][1][3] = nombre d'occurence de "cab" ("3","1","2")
    int nbt=0; //nombre total de lettres traités
    for(unsigned int i=0; i<mots.size(); i++) {
        nbt += analyzeWord(mots[i], lettertab, ui->check_accent->isChecked());
        if (i%100==0) { //Possibilité : 10000 (haché), 100000 (3 étapes)
            ui->progr_Analyse->setValue(avRecup+i/(float)mots.size()*avAnal);
            QCoreApplication::processEvents(); //permet l'actualisation du gui. Ralenti les calcul...
        }
    }*/

    ui->label_ResAnalyse->setText("Liste de mots analysée. Analyse de l'analyse en cours...");

    //Création du tableau des probas cumulatives probatab
    std::map<std::vector<QChar>, std::pair<int,double>>::iterator it = charmap.begin() , cePrDebut , cePrFin;
    //cePrDebut et cePrFin sont des itérateur indiquant le début et la fin de l'enchaînement
    //de lettre examiné actuellement

    while(it!=charmap.end()) {
        //pr = sous vecteur de it->first de [0] à [lcoh-2] : contient lcoh-1 éléments
        //c'est l'enchainement de lettres précédente --> utilisé pour déterminer la proba de l'actuelle
        std::vector<QChar> pr(&it->first[0], &it->first[lcoh-1]);
        cePrDebut = it;
        int nbtlsuiv = 0; //nb tot de lettre suivant l'enchainement 'pr[0]pr[1]...'

        //1er parcours : compter occurence de chaque pr
        // /!\ à l'odre des condition du while : il ne faut pas appeler it si it=end() --> vérification end en premier
        while( (it!=charmap.end()) && (pr==std::vector<QChar>(&it->first[0], &it->first[lcoh-1])) ) {
            //tant que l'enchainement des lettres précédentes (défini dans pr) ne change pas...
            //on additionne le nombre d'occurence
            nbtlsuiv += it->second.first; //it->second = la pair / .first -> le 'int'=nb d'occurence
            it++; //passage au membre suivant
        }
        cePrFin = it;

        //2e  parcours : diviser enchainement/occurence total du pr + additionner (pour proba cumul)
        //on repasse sur la partie déjà vue : de cePrDebut à cePrFin
        for(std::map<std::vector<QChar>, std::pair<int,double>>::iterator repasse = cePrDebut; repasse!=cePrFin && repasse!=charmap.end(); ++repasse) {
            repasse->second.second = (double)repasse->second.first / nbtlsuiv;
            if (repasse != cePrDebut && repasse!=charmap.begin() )
                repasse->second.second += prev(repasse)->second.second; //prev() = élément précédent
        }
    }
    //Ancien code
    /*for (uint k=0; k<27; k++) {
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
    }*/
    ui->progr_Analyse->setValue(avRecup+avAnal+avProbatab-1);
    ui->label_ResAnalyse->setText("Tableau de probabilité construit.\n Prêt à inventer des mots !");
    ui->progr_Analyse->setToolTip("Non, la barre ne va pas à 100%. C'est frustrant, hein ?");

    analysed=true;
    ui->frame_generation->setEnabled(true);
}

void MainWindow::on_bouton_GenMots_clicked() {
    if (!analysed)
        return;
    ui->check_troll->setChecked(false);

    uint taille_max = ui->spin_tailleMax->value();

    if(taille_max==0)
        taille_max=100;

    for (int i=0; i<ui->spin_NbMots->value(); i++) {
        std::string mot(Qgenerateur(charmap,ui->spin_lcoh->value(),taille_max, ui->check_forceTaille->isChecked()));
        ui->text_ResMots->append(QString::fromStdString(mot));
    }
    //TODO : obliger la réanalyse si on change lcoh
    //et --> prendre la fin de lcoh, pas le début ? permet de réduire lcoh pour la generation

    //Ancien code
    /*for (int i=0; i<ui->spin_NbMots->value(); i++) {
        std::string mot(generateur(probatab, taille_max, ui->check_forceTaille->isChecked()));
        ui->text_ResMots->append(QString::fromStdString(mot));
    }*/
}

void MainWindow::on_bouton_clear_clicked() {
    ui->text_ResMots->clear();
}

void MainWindow::on_bouton_selecFichier_clicked() {
    nomListeMots = QFileDialog::getOpenFileName(this, "Selectionner une liste de mot .txt", "", "Fichier texte (*.txt)");
    QFile file(nomListeMots);
    QFileInfo fileinfo(file);
    ui->bouton_selecFichier->setText(fileinfo.fileName());
    if(ui->bouton_selecFichier->text()!="")
        ui->radio_LanPerso->setChecked(true);
    else
        ui->bouton_selecFichier->setText("(Aucun)");
}

void MainWindow::on_bouton_triTaille_clicked() {
    //TODO : immplémenter le tri par taille des mots
}

void MainWindow::openHelp() {
    m_FenAide = new FenAide();
    m_FenAide->show();
}

void MainWindow::on_check_troll_clicked() {
    QTimer::singleShot(80, this, SLOT(uncheking()));
}

void MainWindow::uncheking() {
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    ui->check_troll->setChecked(false);
}
