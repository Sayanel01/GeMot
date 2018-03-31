#include <QFileDialog>
#include <QProgressBar>
#include <QTimer>
#include <thread>
#include "fenetreprincipale.h"
#include "ui_fenetreprincipale.h"

#include <iostream>

FenetrePrincipale::FenetrePrincipale(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::FenetrePrincipale)
{
    ui->setupUi(this);

    //Indication des tooltip longs
    QString ListeMotsInstruction("La liste de mot doit être au format .txt, avec 1 seul mot par ligne\n"
                                 "Si plusieurs mots par lignes, seul le premier est pris en compte");
        ui->radio_langPerso->setToolTip(ListeMotsInstruction);
        ui->bouton_selecFichier->setToolTip(ui->radio_langPerso->toolTip());

    //Indication de la liste par défaut
    QFileInfo fi(nomListeMotsDefaut);
    QString textLangDef = "Utiliser la langue par défaut ("+fi.fileName()+")";
    ui->radio_langDef->setText(textLangDef);

    //Éléments de la statusbar
    ui->statusbar->setFixedHeight(20);
    ui->statusbar->showMessage("En attente de lancement...");

    //Efface tous les éléments affiché par le bouton "détail"
    ui->Bof->setVisible(false);
    ui->Bof_2->setVisible(false);
    ui->Minable->setVisible(false);
    ui->Minable_2->setVisible(false);
    ui->Parfait->setVisible(false);
    ui->Parfait_2->setVisible(false);
    ui->Qualite->setVisible(false);
    ui->Rapidite->setVisible(false);
    ui->line_details->setVisible(false);

    //Selectionne l'onglet de départ
    ui->tabWidget->setCurrentIndex(0);


}

FenetrePrincipale::~FenetrePrincipale()
{
    delete ui;
}

void FenetrePrincipale::on_bouton_analyser_clicked() {
    ui->centralwidget->setCursor(Qt::WaitCursor);
    ui->onglet_analyse->setEnabled(false);
    ui->progr_Analyse->setValue(0); //inutile.
    ui->tabWidget->setTabIcon(0,QIcon());
    int avRecup(5), avAnal(90), avProbatab(5); //doit sommer a 100. Avancement (%) de chaque étape

//Partie 1 : Récupération de la liste de mots
    ui->statusbar->showMessage("Récupération de la liste de mots...");
    //Choix liste mot (selon valeur radio)
    if (ui->radio_langDef->isChecked()) {
        nomListeMots = nomListeMotsDefaut;
    }
    else if (ui->radio_langPerso->isChecked()) {} //rien à faire...
    else { //Trololol inutile
        ui->progr_Analyse->setValue(42);
        ui->statusbar->showMessage("How did you do that ? En plus maintenant, t'as un pointeur de merde :p");
        return; //Si aucun bouton coché : arrêter tout
    }
    //Importation liste mots
    std::vector<std::string> mots;
    std::vector<float> proba;
    std::ifstream Liste_mots(nomListeMots.toStdString(), std::ios::in);
    if(Liste_mots) {
        extractWords(Liste_mots, mots, proba);
        ui->progr_Analyse->setValue(avRecup);
    }
    else {
        ui->statusbar->showMessage("Impossible de lire la liste de mots");
        //TODO : indiquer erreur sur la liste de mot
        return;
    }

//Partie 2 et 3 : Analyse liste mots / construction des proba
    ui->statusbar->showMessage("Analyse de la liste de mots...");
    int nb=0; //nombre de lettres traitées
//Partie 2 version A :
    if(ui->radio_speciaux->isChecked()) { //Traitement moderne
        lcoh = ui->spin_lcoh->value();
        for(uint i=0; i<mots.size(); i++) {
            QString qmot=QString::fromStdString(mots[i]);
            QanalyzeWord(qmot,charmap, lcoh, nb);
            if (i%500==0) { //Possibilité : 10000 (haché), 100000 (3 étapes)
                ui->progr_Analyse->setValue(avRecup+i/(float)mots.size()*avAnal);
                QCoreApplication::processEvents(); //permet l'actualisation du gui. Ralenti les calcul...
            }
        }

//Partie 3 vA : Création des proba de chaque enchaînement de lettre
        ui->statusbar->showMessage("Liste de mots analysée. Analyse de l'analyse en cours...");
        std::map<std::vector<QChar>, std::pair<int,double>>::iterator it = charmap.begin() , cePrDebut , cePrFin;
        //cePrDebut et cePrFin sont des itérateur indiquant le début et la fin de l'enchaînement de lettre examiné actuellement

        while(it!=charmap.end()) {
            //pr = sous vecteur de it->first de [0] #FFBF00à [lcoh-2] : contient lcoh-1 éléments
            //c'est l'enchainement de lettres précédente --> utilisé pour déterminer la proba de l'actuelle
            std::vector<QChar> pr(&it->first[0], &it->first[lcoh-1]);
            cePrDebut = it;
            int nbtlsuiv = 0; //nb tot de lettre suivant l'enchainement 'pr[0]pr[1]...'

            //1er parcours : compter occurence de chaque pr
            // /!\ à l'odre des condition du while : il ne faut pas appeler it si it=end() --> vérification end en premier
            while( (it!=charmap.end()) && (pr==std::vector<QChar>(&it->first[0], &it->first[lcoh-1])) ) {
                //tant que l'enchainement des lettres précédentes (défini dans pr) ne change pas, on additionne le nombre d'occurence
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
//Traitement terminé. Indication lié au traitement utf_8
        analyse = utf_8;
        ui->check_forcerTaille->setEnabled(false);
        ui->check_forcerTaille->setToolTip("Un bug très (très) idiot empêche de forcer la taille des mots "
                                           "avec la méthode gérant tous les type de caractères\n"
                                           "Non, je n'ai pas réussi à corriger ça, désolé.");
        ui->label_resume_analyse->setText("Type d'analyse : tous caractères spéciaux");
        ui->label_resume_lcoh->setText("Longueur de cohérence : "+QString::number(lcoh));
    }
//Patie 2 version B :
    else { //Traitement à l'ancienne (radio Ignore et radio gère très mal
        int lettertab[27][27][27] = {0}; //--> lettertab[2][1][3] = nombre d'occurence de "cab" ("3","1","2")
        int nb=0; //nombre total de lettres traités
        bool clearAccent = ui->radio_minable->isChecked(); //ascii ou asciiplus
        for(unsigned int i=0; i<mots.size(); i++) {
            nb += analyzeWord(mots[i], lettertab, clearAccent);
            if (i%500==0) { //Possibilité : 10000 (haché), 100000 (3 étapes)
                ui->progr_Analyse->setValue(avRecup+i/(float)mots.size()*avAnal);
                QCoreApplication::processEvents(); //permet l'actualisation du gui. Ralenti les calcul...
            }
        }

//Partie 3 vB: Création des proba de chaque enchaînement de lettre
        ui->statusbar->showMessage("Liste de mots analysée. Analyse de l'analyse en cours...");
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
//Traitement terminé. Indication lié au traitement ascii
        if (clearAccent) {
            ui->label_resume_analyse->setText("Type d'analyse : accents désaccentés");
            analyse = asciiplus; }
        else {
            ui->label_resume_analyse->setText("Type d'analyse : sans accents");
            analyse = ascii; }
        //Autorise le "forcer_taille"
        ui->check_forcerTaille->setEnabled(true);
        ui->check_forcerTaille->setToolTip("");

        lcoh=0; //pas de lcoh ici. lcoh=0 <=> traitement ascii
        ui->label_resume_lcoh->setVisible(false); //cache la valeur lcoh
    }

//Traitement terminé. Indications générales
    ui->progr_Analyse->setValue(avRecup+avAnal+avProbatab-1);
    ui->statusbar->showMessage("Analyse terminée ! Prêt a inventer des mots !");
    ui->progr_Analyse->setToolTip("Non, la barre ne va pas à 100%. C'est frustrant, hein ?");

    ui->onglet_generation->setEnabled(true);
    ui->onglet_analyse->setEnabled(true);
    ui->centralwidget->unsetCursor();

    ui->tabWidget->setCurrentIndex(1);
}

void FenetrePrincipale::on_bouton_generer_clicked() {
    ui->check_troll->setChecked(false);
    if ((analyse==aucun)) {
        ui->statusbar->showMessage("Vous devez analyser une liste de mots avant de générer des mots");
        return; }

    uint taille_max = ui->spin_tailleMax->value();
    if(taille_max==0)
        taille_max=100;

    //TODO : obliger la réanalyse si on change lcoh (ou autre chose)

    std::string mot;
    for (int i=0; i<ui->spin_nbMots->value(); i++) {
        if(analyse==utf_8)
            mot = Qgenerateur(charmap,lcoh, ui->check_forcerTaille->isChecked(), taille_max);
        if(analyse==ascii)
            mot = generateur(probatab, ui->check_forcerTaille->isChecked(), taille_max);
        ui->text_mots->append(QString::fromStdString(mot));
    }
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

void FenetrePrincipale::on_bouton_nettoyer_clicked() {
    ui->text_mots->clear();
}

void FenetrePrincipale::on_bouton_trier_clicked() {
    //TODO : immplémenter le tri par taille des mots
}

void FenetrePrincipale::on_actionAide_triggered() {
    m_FenAide = new FenAide();
    m_FenAide->show();
}

void FenetrePrincipale::on_check_troll_clicked() {
    QTimer::singleShot(80, this, SLOT(unchecking()));
}

void FenetrePrincipale::unchecking() {
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    ui->check_troll->setChecked(false);
}

void FenetrePrincipale::analyse_changed(bool changed) {
    if (changed) {
        QIcon p(":/icones/icons/warning.png");
        ui->tabWidget->setTabIcon(0,p);
        ui->statusbar->showMessage("Attention, vous devez refaire l'analyse pour que les changements soient pris en compte");
    }
    else {
        ui->tabWidget->setTabIcon(0,QIcon());
        ui->statusbar->showMessage("");
    }
}

void FenetrePrincipale::on_spin_lcoh_valueChanged(int value)
{
    if( (analyse==type_Trait::utf_8) & ((uint)value!=lcoh) ) {
        ui->spin_lcoh->setStyleSheet("background-color: #FFBF00");
        analyse_changed(true);
    }
    else {
        ui->spin_lcoh->setStyleSheet("");
        analyse_changed(false);
    }
}

void FenetrePrincipale::on_radio_ignore_toggled(bool checked)
{
    if(checked) {

    }
}

void FenetrePrincipale::on_radio_minable_toggled(bool checked)
{

}

void FenetrePrincipale::on_radio_speciaux_toggled(bool checked)
{

}
