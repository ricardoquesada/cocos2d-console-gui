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
#include <QSettings>

namespace Ui {
class PreferencesDialog;
}

class PreferencesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit PreferencesDialog(QWidget *parent = 0);
    ~PreferencesDialog();

    static QString findCocosPath();
    static QString findSDKBOXPath();

private slots:
    void on_buttonBox_accepted();

    void on_directoryButton_cocos_clicked();

    void on_lineEdit_cocos_editingFinished();

    void on_directoryButton_sdkbox_clicked();

    void on_lineEdit_sdkbox_editingFinished();

private:
    Ui::PreferencesDialog *ui;

    QSettings _settings;
};
