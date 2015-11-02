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

QT_BEGIN_NAMESPACE
class QListWidgetItem;
class QProgressBar;
class QComboBox;
QT_END_NAMESPACE

class GameState;
class Run;
class Highlighter;

namespace Ui {
class MainWindow;
}

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    enum {
        COCOS_COMMAND,
        SDKBOX_COMMAND
    };

    static constexpr int MAX_RECENT_FILES=15;

    explicit MainWindow(QWidget *parent = 0);
    ~MainWindow();

    void setGameState(GameState* gameState);
    GameState* getGameState() const;

public slots:
    void openFile(const QString& path);
    void on_actionOpen_triggered();

signals:
    void processFinished();

protected:
    void closeEvent(QCloseEvent *event) Q_DECL_OVERRIDE;

private slots:
    void gameUpdateProperties();
    void gameUpdateLibraries();
    void gameUpdatePlatforms();

    void onOpenXcode();
    void onOpenVSWin32();
    void onOpenVS10();
    void onOpenVSUniversal();
    void onOpenAndroidStudio();

    void displayCommand(const QString& commandLine);

    void openRecentFile_triggered();
    void onProcessFinished(Run* command);
    void onProcessDataAvailable(Run* command, const QString& data);

    void on_actionPreferences_triggered();
    void on_actionAbout_triggered();
    void on_actionClear_Recent_Games_triggered();
    void on_actionNew_Game_triggered();
    void on_actionRun_triggered();
    bool on_actionStop_triggered();
    void on_actionClean_triggered();
    void on_actionOpen_File_Browser_triggered();
    void on_actionWelcome_triggered();
    void on_actionClose_Game_triggered();
    void on_actionBuild_triggered();

    void on_pushButton_addLibrary_clicked();

    void on_pushButton_clearConsole_clicked();

    void on_actionExit_triggered();

    void on_actionImport_Game_triggered();

private:
    void updateActions();
    void updateRecentFiles();
    void setRecentFile(const QString& fileName);
    QStringList recentFiles() const;
    bool validatePath(const QString& filePath) const;
    void createActions();
    void closeGameState();
    bool maybeSave();
    bool maybeRunProcess();
    void setupModels();
    void setupStatusBar();

    void readSettings();
    void saveSettings();

    Ui::MainWindow *ui;
    QSettings _settings;
    QAction* _recentFilesAction[MAX_RECENT_FILES];
    QString _lastDir;

    GameState* _gameState;
    QProgressBar* _progressBar;

    QComboBox* _comboBoxMode;
    QComboBox* _comboBoxPlatforms;

    Highlighter* _highlighter;
};
