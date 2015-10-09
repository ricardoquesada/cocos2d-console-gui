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

#include <QMainWindow>
#include <QStringList>
#include <QString>
#include <QSettings>

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    static constexpr int MAX_RECENT_FILES=8;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

protected:

private slots:

    void openRecentFile_triggered();

    void on_pushButtonNewGame_clicked();
    void on_actionPreferences_triggered();
    void on_actionAbout_triggered();
    void on_pushButtonOpenGame_clicked();
    void on_actionOpen_triggered();
    void on_actionClear_Recent_Games_triggered();

private:
    void updateRecentFiles();
    void setRecentFile(const QString& fileName);
    QStringList recentFiles() const;
    bool validatePath(const QString& path) const;
    void createActions();


    Ui::MainWindow *ui;
    QSettings _settings;
    QAction* _recentFiles[MAX_RECENT_FILES];
};
