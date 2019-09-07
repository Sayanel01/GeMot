#ifndef FENAIDE_H
#define FENAIDE_H

#include <QDialog>
#include <memory>

namespace Ui {
class FenAide; //=Help window
}

class FenAide : public QDialog
{
    Q_OBJECT

public:
    explicit FenAide(QWidget *parent = 0);
    ~FenAide();

public slots:
    void on_bouton_moreInfo_clicked();

private:
    //Soit la methode par defaut avec un delete dans le destructeur, soit un unique_ptr
//    Ui::FenAide *ui;
    std::unique_ptr<Ui::FenAide> ui;
};

#endif // FENAIDE_H
