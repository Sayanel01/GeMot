#include "infos.h"
#include "ui_infos.h"

Infos::Infos(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::Infos)
{
    ui->setupUi(this);

// Make window fixed size
#ifdef __linux__
    this->setFixedSize(this->width(),this->height());
#elif _WIN32
    setWindowFlags(Qt::Dialog | Qt::MSWindowsFixedSizeDialogHint);
#else

#endif
}

Infos::~Infos()
{
    delete ui;
}
