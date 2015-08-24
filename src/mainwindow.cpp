#include <string>

#include <QDirIterator>
#include <QDebug>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "dialognewgame.h"
#include "templateentry.h"

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    ui->label->setText("<img src=':/res/logo.png'>");

    parseTemplates();
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

void MainWindow::on_pushButtonEditGame_clicked()
{
}

//

void MainWindow::parseTemplates()
{
    QDirIterator it(":/templates", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        auto fileInfo = it.fileInfo();

        if (fileInfo.fileName() == "config.json") {
            QFile file(fileInfo.filePath());
            TemplateEntry entry = TemplateEntry::createFromJsonFile(&file);
        }
    }
}

