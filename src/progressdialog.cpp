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

#include <QPushButton>

#include "progressdialog.h"
#include "ui_progressdialog.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::appendData(const QString& str)
{
    ui->textBrowser->append(str);
}

void ProgressDialog::processFinished()
{
    ui->progressBar->setMaximum(1);
    ui->progressBar->setValue(1);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}