#ifndef INFOS_H
#define INFOS_H

#include <QDialog>

namespace Ui {
class Infos;
}

class Infos : public QDialog
{
    Q_OBJECT

public:
    explicit Infos(QWidget *parent = 0);
    ~Infos();

private:
    Ui::Infos *ui;
};

#endif // INFOS_H
