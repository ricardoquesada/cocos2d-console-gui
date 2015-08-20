#include <QDebug>
#include <QString>

#include "dialognewgame.h"
#include "ui_dialognewgame.h"

DialogNewGame::DialogNewGame(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::DialogNewGame)
{
    ui->setupUi(this);

    // populate items
    auto listWidget = ui->listWidget;

    new QListWidgetItem(tr("Physics Game"), listWidget);
    new QListWidgetItem(tr("Flappy Bird Game"), listWidget);
    new QListWidgetItem(tr("Empty Game"), listWidget);


    // populate text browser
    ui->textBrowser->setDocumentTitle("hello");
}

DialogNewGame::~DialogNewGame()
{
    delete ui;
}

void DialogNewGame::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{

}

