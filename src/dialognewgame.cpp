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
    new QListWidgetItem(tr("Endless Runner Game"), listWidget);
    new QListWidgetItem(tr("Empty Game"), listWidget);

    listWidget->item(0)->setSelected(true);

    // populate text browser
    ui->textBrowser->setDocumentTitle("hello");
}

DialogNewGame::~DialogNewGame()
{
    delete ui;
}

void DialogNewGame::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    if (current->text().startsWith("Physics")) {
        auto qurl = QUrl::fromLocalFile(":/res/physics-doc.html");
        ui->textBrowser->setSource(qurl);
    }
    else if (current->text().startsWith("Endless")) {
        auto qurl = QUrl::fromLocalFile(":/res/endless-doc.html");
        ui->textBrowser->setSource(qurl);
    }
    else if (current->text().startsWith("Empty")) {
        auto qurl = QUrl::fromLocalFile(":/res/empty-doc.html");
        ui->textBrowser->setSource(qurl);
    }

}

