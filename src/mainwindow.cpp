#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialognewgame.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    ui->label->setText("<img src=':/res/logo.png'>");
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_pushButtonNewGame_clicked()
{
    DialogNewGame dialog(this);
    dialog.exec();
}
