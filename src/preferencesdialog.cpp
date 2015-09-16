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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QFileDialog>
#include <QSettings>

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog)
{
    ui->setupUi(this);

    QSettings settings;
    auto defaultDir = settings.value("cocos2d_path", "/Applications/cocos").toString();
    ui->lineEdit->setText(defaultDir);
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_directoryButton_clicked()
{
    QSettings settings;
    auto filename = QFileDialog::getExistingDirectory(this,
                                                 "Cocos2d-x Path",
                                                 ui->lineEdit->text());

    if (!filename.isEmpty())
    {
        ui->lineEdit->setText(filename);
        settings.setValue("cocos2d_path", filename);
    }
}
