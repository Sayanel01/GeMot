#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "fonctions.h"

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

public slots:
    void on_bouton_LancerAnalyse_clicked();
    void on_bouton_GenMots_clicked();

private:
    Ui::MainWindow *ui;
    double proba
};

#endif // MAINWINDOW_H
