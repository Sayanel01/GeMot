#include "infos.h"
#include "ui_infos.h"

Infos::Infos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Infos)
{
    ui->setupUi(this);

#ifdef __linux__
    this->setFixedSize(this->width(),this->height());
#elif _WIN32
    //Taille de fenere fixe. Windows seulement
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
#else

#endif
}

Infos::~Infos()
{
    delete ui;
}
