/****************************************************************************
Copyright 2015 Chukong Technologies

Licensed under the Apache License, Version 2.0 (the "License");
you may not use this file except in compliance with the License.
You may obtain a copy of the License at

    http://www.apache.org/licenses/LICENSE-2.0

Unless required by applicable law or agreed to in writing, software
distributed under the License is distributed on an "AS IS" BASIS,
WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
See the License for the specific language governing permissions and
limitations under the License.
****************************************************************************/

#include "gamedialog.h"
#include "ui_gamedialog.h"

#include <QDesktopServices>

GameDialog::GameDialog(const QString &path, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::GameDialog)
{
    ui->setupUi(this);
    ui->textBrowser->append(path);
}

GameDialog::~GameDialog()
{
    delete ui;
}

void GameDialog::on_pushButton_close_clicked()
{

}

void GameDialog::on_pushButton_compile_clicked()
{

}

void GameDialog::on_pushButton_deploy_clicked()
{

}

void GameDialog::on_pushButton_openFolder_clicked()
{
    QDesktopServices::openUrl(QUrl("file://Users/"));
}
