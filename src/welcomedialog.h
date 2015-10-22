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
#include <QStringList>
#include <QString>
#include <QSettings>

QT_BEGIN_NAMESPACE
class QListWidgetItem;
QT_END_NAMESPACE


namespace Ui {
class WelcomeDialog;
}

class WelcomeDialog : public QDialog
{
    Q_OBJECT
    static constexpr int MAX_RECENT_FILES=8;

public:
    explicit WelcomeDialog(QWidget *parent = 0);
    ~WelcomeDialog();

private slots:
    void itemDoubleClicked(QListWidgetItem* item);

    void on_pushButton_NewGame_clicked();

    void on_pushButton_OpenGame_clicked();

    void on_checkBox_clicked();

private:
    void updateRecentFiles();
    void setRecentFile(const QString& fileName);
    QStringList recentFiles() const;
    void createActions();

    Ui::WelcomeDialog *ui;
    QSettings _settings;
    QListWidgetItem* _recentFilesWidget[MAX_RECENT_FILES];

};
