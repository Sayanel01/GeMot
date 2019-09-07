#include "fenaide.h"
#include "ui_fenaide.h"

FenAide::FenAide(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::FenAide)
{
    ui->setupUi(this);

    ui->text_details->setVisible(false);
    this->adjustSize();

#ifdef __linux__
    this->setFixedSize(this->width(),this->height());
#elif _WIN32
    //Taille de fenetre fixe. Windows seulement
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
#else

#endif
}

FenAide::~FenAide()
{
//    delete ui;
}

void FenAide::on_bouton_moreInfo_clicked() {
    ui->text_details->setVisible(!ui->text_details->isVisible());
    this->adjustSize();
}

//#ifdef __linux__
//    code
//#elif _WIN32
//    code
//#else
//    code
//#endif
