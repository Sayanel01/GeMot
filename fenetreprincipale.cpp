#include "fenetreprincipale.h"
#include "ui_fenetreprincipale.h"

FenetrePrincipale::FenetrePrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    QString ListeMotsInstruction("La liste de mot doit être au format .txt, avec 1 seul mot par ligne\n"
                                 "Si plusieurs mots par lignes, seul le premier est pris en compte");
    ui->radio_langPerso->setToolTip(ListeMotsInstruction);
    ui->bouton_selecFichier->setToolTip(ui->radio_langPerso->toolTip());

    ui->Bof->setVisible(false);
    ui->Bof_2->setVisible(false);
    ui->Minable->setVisible(false);
    ui->Minable_2->setVisible(false);
    ui->Parfait->setVisible(false);
    ui->Parfait_2->setVisible(false);
    ui->Qualite->setVisible(false);
    ui->Rapidite->setVisible(false);
    ui->line_details->setVisible(false);

}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
}

void FenetrePrincipale::on_bouton_analyser_clicked() {
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

void FenetrePrincipale::on_bouton_generer_clicked() {
    ui->check_troll->setChecked(false);
    if (!analysed)
        return;

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

void FenetrePrincipale::on_bouton_selecFichier_clicked() {
    nomListeMots = QFileDialog::getOpenFileName(this, "Selectionner une liste de mot .txt", "", "Fichier texte (*.txt)");
    QFile file(nomListeMots);
    QFileInfo fileinfo(file);
    ui->bouton_selecFichier->setText(fileinfo.fileName());
    if(ui->bouton_selecFichier->text()!="")
        ui->radio_langPerso->setChecked(true);
    else
        ui->bouton_selecFichier->setText("(Aucun)");
}













void FenetrePrincipale::on_bouton_details_clicked() {
    ui->Bof->setVisible(!ui->Bof->isVisible());
    ui->Bof_2->setVisible(!ui->Bof_2->isVisible());
    ui->Minable->setVisible(!ui->Minable->isVisible());
    ui->Minable_2->setVisible(!ui->Minable_2->isVisible());
    ui->Parfait->setVisible(!ui->Parfait->isVisible());
    ui->Parfait_2->setVisible(!ui->Parfait_2->isVisible());
    ui->Qualite->setVisible(!ui->Qualite->isVisible());
    ui->Rapidite->setVisible(!ui->Rapidite->isVisible());
    ui->line_details->setVisible(!ui->line_details->isVisible());
}
