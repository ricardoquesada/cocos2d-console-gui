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

#pragma once

#include <QDialog>
#include <QListWidgetItem>
#include <QList>
#include <QProcess>

#include "templateentry.h"
#include "templatewizard.h"

namespace Ui {
class NewGameDialog;
}

class ProgressDialog;

class NewGameDialog : public QDialog
{
    Q_OBJECT

public:
    explicit NewGameDialog(QWidget *parent = 0);
    ~NewGameDialog();

protected:
    bool parseTemplates();
    void populateTemplateList(const QString& title, QList<TemplateEntry>* list, QListWidget* parent);
    void copyFiles(const TemplateWizard& wizard, const TemplateEntry &templateEntry);

private slots:
    void on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous);
    void on_buttonBox_accepted();
    void processReadyReadStandardOutput();
    void processFinished(int exitCode, QProcess::ExitStatus exitStatus);

private:
    QList<TemplateEntry> _entriesCpp;
    QList<TemplateEntry> _entriesLua;
    QList<TemplateEntry> _entriesJavaScript;

    Ui::NewGameDialog *ui;

    QByteArray _data;
    QProcess* _process;
    ProgressDialog* _progressDialog;
    bool _running;
};
