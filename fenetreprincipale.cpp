#include <QFileDialog>
#include <QProgressBar>
#include <QTimer>
#include <thread>
#include <QXmlStreamReader>
#include <QXmlStreamWriter>
#include <QMessageBox>
#include "fenetreprincipale.h"
#include "ui_fenetreprincipale.h"
#include <string>

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

    //Éléments de la statusbar
    ui->statusbar->setFixedHeight(20);
    ui->statusbar->showMessage("En attente de lancement...");

    //Lecture du fichier des valeurs par défaut:
    QFile file("WordLists/Parametres.xml");
    if(file.open(QIODevice::ReadOnly)) {
        QXmlStreamReader xmlReader(&file);
        if(xmlReader.readNextStartElement()) {
            if (xmlReader.name() == "valeursDef") {
                while(xmlReader.readNextStartElement()) {
                    if (xmlReader.name() == "nomListe")
                        nomListeMotsDefaut = xmlReader.readElementText();
                    else if (xmlReader.name() == "modeTraitement") {
                        type_Trait wantedAnalyse = stringToType(xmlReader.readElementText());
                        if(wantedAnalyse==ascii)
                            ui->radio_ignore->setChecked(true);
                        else if(wantedAnalyse==asciiplus)
                            ui->radio_minable->setChecked(true);
                        else if(wantedAnalyse==utf_8)
                            ui->radio_speciaux->setChecked(true);
                    }
                    else if (xmlReader.name() == "lcoh")
                        ui->spin_lcoh->setValue(xmlReader.readElementText().toInt());
                    else if (xmlReader.name() == "nbMots")
                        ui->spin_nbMots->setValue(xmlReader.readElementText().toInt());
                    else if (xmlReader.name() == "tailleMax")
                        ui->spin_tailleMax->setValue(xmlReader.readElementText().toInt());
                    else
                        xmlReader.skipCurrentElement();
                }
            }
            xmlReader.skipCurrentElement();
        }
    }
    else{
        ui->statusbar->showMessage("Problème lors de la lecture des valeurs par défaut");
    }

    //Indication de la liste par défaut
    QFileInfo fi(nomListeMotsDefaut);
    QString textLangDef = "Utiliser la langue par défaut ("+fi.fileName()+")";
    ui->radio_langDef->setText(textLangDef);

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

    //Connexion signal/slot pour detection changement
    QObject::connect(ui->radio_ignore, SIGNAL(clicked()), this, SLOT(traitement_modifie()));
    QObject::connect(ui->radio_minable, SIGNAL(clicked()), this, SLOT(traitement_modifie()));
    QObject::connect(ui->radio_speciaux, SIGNAL(clicked()), this, SLOT(traitement_modifie()));

    QObject::connect(ui->radio_langDef, SIGNAL(clicked()), this, SLOT(liste_modifie()));
    QObject::connect(ui->radio_langPerso, SIGNAL(clicked()), this, SLOT(liste_modifie()));
    QObject::connect(ui->bouton_selecFichier, SIGNAL(clicked()), this, SLOT(liste_modifie()));

    //Quitter via le menu
    QObject::connect(ui->actionQuitter_3, SIGNAL(triggered()), qApp, SLOT(quit()));
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
    ui->statusbar->setToolTip("");
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
        ui->statusbar->showMessage("Impossible de lire la liste de mots (voir ici pour plus de détails)");
        ui->statusbar->setToolTip("La liste de mot suivante :\n"+nomListeMots+
                                  "\nn'a pas pu être lue. Vérifiez qu'elle existe bien et est une liste de mots au format .txt, 1 mot par ligne");
        ui->onglet_analyse->setEnabled(true);
        ui->centralwidget->unsetCursor();
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
    ui->progr_Analyse->setToolTip("Non, la barre ne va pas à 100%. C'est frustrant, hein ?");

    ui->onglet_generation->setEnabled(true);
    ui->onglet_analyse->setEnabled(true);
    ui->centralwidget->unsetCursor();

    //Supprime les warning de changement
    traitement_modifie();
    on_spin_lcoh_valueChanged(lcoh);
        QFile file(nomListeMots); QFileInfo fileinfo(file);
        nomListeAnalysePrecedente=fileinfo.fileName(); //récupération du nom de la liste de mots
    liste_modifie();

    ui->statusbar->showMessage("Analyse terminée ! Prêt a inventer des mots !");
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
    liste_modifie();
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

void FenetrePrincipale::on_action_propos_de_ce_programme_triggered() {
    m_Infos = new Infos();
    m_Infos->show();
}

void FenetrePrincipale::on_actionUtiliser_les_valeur_actuelles_par_defaut_triggered() {
    QMessageBox::StandardButton reply;
    reply = QMessageBox::question(this, "Changer valeurs par defaut",
                                  "Voulez vous changer les valeurs par défaut du programme ?\n"
                                  "\nLes valeurs par défaut (càd celle au lancement du programme) seront remplacées par les valeurs actuelles\n"
                                  "Sont concernés :\n"
                                  "    - La liste de mot\n"
                                  "    - La longueur de cohérence\n"
                                  "    - Le nombre de mot à générer\n"
                                  "    - La taille maximale des mots\n",
                                  QMessageBox::Yes|QMessageBox::No);
    if (reply == QMessageBox::Yes) {
        //préparation pour écriture du chemin de la liste de mot
        QFile file("WordLists/Parametres.xml");
        if(file.open(QIODevice::WriteOnly)) {
            QXmlStreamWriter xmlWriter(&file);
            xmlWriter.writeStartElement("?xml version=\"1.0\" encoding=\"utf-8\"?");
            xmlWriter.writeCharacters("\n");
            xmlWriter.writeStartElement("valeursDef");
            xmlWriter.writeCharacters("\n\t");
                xmlWriter.writeStartElement("nomListe");
                    if(ui->radio_langPerso->isChecked())
                        xmlWriter.writeCharacters(nomListeMots);
                    else
                        xmlWriter.writeCharacters(nomListeMotsDefaut);
                xmlWriter.writeEndElement();
                xmlWriter.writeCharacters("\n");
                xmlWriter.writeComment("Le chemin peut être donné en absolu ou en relatif");
                xmlWriter.writeCharacters("\n\t");
                xmlWriter.writeStartElement("modeTraitement");
                    if(ui->radio_ignore->isChecked())
                        xmlWriter.writeCharacters("ascii");
                    else if(ui->radio_minable->isChecked())
                        xmlWriter.writeCharacters("asciiplus");
                    else if(ui->radio_speciaux->isChecked())
                        xmlWriter.writeCharacters("utf_8");
                    else
                        xmlWriter.writeCharacters("aucun");
                xmlWriter.writeEndElement();
                xmlWriter.writeCharacters("\n");
                xmlWriter.writeComment("Les valeurs possible du mode de traitement sont : aucun, ascii, asciiplus et utf_8");
                xmlWriter.writeCharacters("\n\t");
                xmlWriter.writeStartElement("lcoh");
                    xmlWriter.writeCharacters(QString::number(ui->spin_lcoh->value()));
                xmlWriter.writeEndElement();
                xmlWriter.writeCharacters("\n\t");
                xmlWriter.writeStartElement("nbMots");
                    xmlWriter.writeCharacters(QString::number(ui->spin_nbMots->value()));
                xmlWriter.writeEndElement();
                xmlWriter.writeCharacters("\n\t");
                xmlWriter.writeStartElement("tailleMax");
                    xmlWriter.writeCharacters(QString::number(ui->spin_tailleMax->value()));
                xmlWriter.writeEndElement();
                xmlWriter.writeCharacters("\n");
            xmlWriter.writeEndElement();
            xmlWriter.writeCharacters("\n");
        }
        else{
            ui->statusbar->showMessage("Problème lors de l'écriture des valeurs par défaut");
        }
    }
}

void FenetrePrincipale::on_check_troll_clicked() {
    QTimer::singleShot(80, this, SLOT(unchecking()));
}

void FenetrePrincipale::unchecking() {
    std::this_thread::sleep_for(std::chrono::milliseconds(400));
    ui->check_troll->setChecked(false);
}

void FenetrePrincipale::check_analyse_changed() {
    bool changed = listeMots_changed || traitement_changed || lcoh_changed;
    if (changed && analyse!=aucun) {
        QIcon p(":/icones/icons/warning.png");
        ui->tabWidget->setTabIcon(0,p);
        ui->statusbar->showMessage("Attention, vous devez refaire l'analyse pour que les changements soient pris en compte");
    }
    else {
        ui->tabWidget->setTabIcon(0,QIcon());
        ui->statusbar->showMessage("");
    }
}

void FenetrePrincipale::on_spin_lcoh_valueChanged(int value) {
    if( (analyse==type_Trait::utf_8) & ((uint)value!=lcoh) ) {
        ui->spin_lcoh->setStyleSheet("background-color: #FFBF00");
        lcoh_changed = true; }
    else {
        ui->spin_lcoh->setStyleSheet("");
        lcoh_changed = false; }
    check_analyse_changed();
}

void FenetrePrincipale::traitement_modifie() {
    if(analyse!=aucun) {
        if(selectedTrait()!=analyse) {
            traitement_changed=true;
            if(ui->radio_ignore->isChecked()) {
                ui->radio_ignore->setStyleSheet("background-color: #FFBF00");
                ui->radio_minable->setStyleSheet("");
                ui->radio_speciaux->setStyleSheet(""); }
            else if(ui->radio_minable->isChecked()) {
                ui->radio_ignore->setStyleSheet("");
                ui->radio_minable->setStyleSheet("background-color: #FFBF00");
                ui->radio_speciaux->setStyleSheet(""); }
            else if(ui->radio_speciaux->isChecked()) {
                ui->radio_ignore->setStyleSheet("");
                ui->radio_minable->setStyleSheet("");
                ui->radio_speciaux->setStyleSheet("background-color: #FFBF00"); }
        }
        else {
            traitement_changed=false;
            ui->radio_ignore->setStyleSheet("");
            ui->radio_minable->setStyleSheet("");
            ui->radio_speciaux->setStyleSheet("");
        }
    }
    check_analyse_changed();
}

FenetrePrincipale::type_Trait FenetrePrincipale::selectedTrait() {
    if(ui->radio_ignore->isChecked())
        return ascii;
    else if(ui->radio_minable->isChecked())
        return asciiplus;
    else if(ui->radio_speciaux->isChecked())
        return utf_8;
    else
        return aucun;
}

FenetrePrincipale::type_Trait FenetrePrincipale::stringToType(QString stringType) {
    if (stringType == "ascii")
        return FenetrePrincipale::ascii;
    else if (stringType == "asciiplus")
        return FenetrePrincipale::asciiplus;
    else if (stringType == "utf_8")
        return FenetrePrincipale::utf_8;
    else
        return FenetrePrincipale::aucun;
}

void FenetrePrincipale::liste_modifie() {
    if(analyse!=aucun) {
        bool usedLangDef = (nomListeMots==nomListeMotsDefaut);

        if(ui->radio_langPerso->isChecked() && usedLangDef) {
            ui->radio_langDef->setStyleSheet("");
            ui->radio_langPerso->setStyleSheet("background-color: #FFBF00");
            listeMots_changed=true; }
        else if (ui->radio_langDef->isChecked() && usedLangDef) {
            ui->radio_langDef->setStyleSheet("");
            ui->radio_langPerso->setStyleSheet("");
            listeMots_changed=false; }
        else if(ui->radio_langDef->isChecked() && !usedLangDef) {
            ui->radio_langDef->setStyleSheet("background-color: #FFBF00");
            ui->radio_langPerso->setStyleSheet("");
            listeMots_changed=true; }
        else if(ui->bouton_selecFichier->text()!=nomListeAnalysePrecedente&& !usedLangDef) {
            ui->radio_langDef->setStyleSheet("");
            ui->radio_langPerso->setStyleSheet("background-color: #FFBF00");
            listeMots_changed=true; }
        else if(ui->radio_langPerso->isChecked() && !usedLangDef ) {
            ui->radio_langDef->setStyleSheet("");
            ui->radio_langPerso->setStyleSheet("");
            listeMots_changed=false; }
        else {
            ui->radio_langDef->setStyleSheet("");
            ui->radio_langPerso->setStyleSheet("");
            listeMots_changed=false; }
    }
    check_analyse_changed();
}
