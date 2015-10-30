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
#include "runmgr.h"
#include "highlighter.h"
#include "runmgr.h"

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(false);

    ui->plainTextEdit->setReadOnly(true);
    new Highlighter(ui->plainTextEdit->document());

    connect(RunMgr::getInstance(), &RunMgr::commandRun, this, &ProgressDialog::appendCLI);
    connect(RunMgr::getInstance(), &RunMgr::commandError, this, &ProgressDialog::appendCLI);

}

ProgressDialog::~ProgressDialog()
{
    disconnect(RunMgr::getInstance(), &RunMgr::commandRun, this, &ProgressDialog::appendCLI);
    disconnect(RunMgr::getInstance(), &RunMgr::commandError, this, &ProgressDialog::appendCLI);
    delete ui;
}

void ProgressDialog::appendData(const QString& str)
{
    ui->plainTextEdit->appendPlainText(str);
}

void ProgressDialog::appendCLI(const QString& str)
{
    ui->plainTextEdit->appendPlainText(str);
}

void ProgressDialog::processFinished(Run* command)
{
    Q_UNUSED(command);

    ui->progressBar->setMaximum(1);
    ui->progressBar->setValue(1);
    ui->buttonBox->button(QDialogButtonBox::Ok)->setEnabled(true);
}

void ProgressDialog::on_buttonBox_accepted()
{

}
