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

#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QListWidgetItem>
#include <QDesktopServices>

#include "newgamedialog.h"
#include "templateentry.h"
#include "aboutdialog.h"
#include "preferencesdialog.h"
#include "welcomedialog.h"
#include "gamestate.h"

constexpr int MainWindow::MAX_RECENT_FILES;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _settings("org.cocos2d-x","Cocos2d Console GUI")
    , _gameState(nullptr)
{
    setUnifiedTitleAndToolBarOnMac(true);

    ui->setupUi(this);
    createActions();
}

MainWindow::~MainWindow()
{
    delete ui;
    if (_gameState)
        delete _gameState;
}

//
// Manual Slots
//
void MainWindow::openRecentFile_triggered()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
        auto path = action->data().toString();
        if (path.length() > 0)
        {
            auto gameState = new GameState(path);
            setGameState(gameState);
        }
        else
            qDebug() << "Invalid path";
    }
}

//
// QtCreator Slots
//
void MainWindow::on_actionNew_Game_triggered()
{
    NewGameDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog preferences(this);
    preferences.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::on_actionOpen_triggered()
{
    auto path = QFileDialog::getExistingDirectory(this, "Select Existing Game Directory");
    if (path.length() > 0) {
        if (validatePath(path))
        {
            setRecentFile(path);
            auto gameState = new GameState(path);
            setGameState(gameState);
        }
    }
}

void MainWindow::on_actionClear_Recent_Games_triggered()
{
    _settings.setValue(QLatin1String("recentFiles/fileNames"), QStringList());
    updateRecentFiles();
}

void MainWindow::on_actionWelcome_triggered()
{
    WelcomeDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionRun_triggered()
{

}

void MainWindow::on_actionStop_triggered()
{

}

void MainWindow::on_actionClean_triggered()
{

}

void MainWindow::on_actionOpen_Xcode_triggered()
{

}

void MainWindow::on_actionOpen_in_Visual_Studio_triggered()
{

}

void MainWindow::on_actionOpen_in_Android_Studio_triggered()
{

}

void MainWindow::on_actionOpen_File_Browser_triggered()
{
    QDesktopServices::openUrl(QUrl("file://" + _gameState->getPath()));
}

//
// Public Methods
//
void MainWindow::setGameState(GameState* gameState)
{
    if (_gameState)
        closeGameState();

    if (gameState)
    {
        setWindowFilePath(gameState->getPath());
        _gameState = gameState;
    }
}

GameState* MainWindow::getGameState() const
{
    return _gameState;
}

//
// Helpers
//
void MainWindow::closeGameState()
{
    if (!_gameState)
        return;

    delete _gameState;
    _gameState = nullptr;

    setWindowFilePath("[untitled]");
}

void MainWindow::createActions()
{
    setWindowFilePath("[untitled]");

    // Add recent file actions to the recent files menu and widgetList
    for (int i=0; i < MAX_RECENT_FILES; ++i)
    {
        _recentFilesAction[i] = new QAction(this);
        ui->menuRecentGames->insertAction(ui->actionClear_Recent_Games, _recentFilesAction[i]);
        _recentFilesAction[i]->setVisible(false);
        connect(_recentFilesAction[i], &QAction::triggered, this, &MainWindow::openRecentFile_triggered);
    }
    ui->menuRecentGames->insertSeparator(ui->actionClear_Recent_Games);
    updateRecentFiles();
}

QStringList MainWindow::recentFiles() const
{
    QVariant v = _settings.value(QLatin1String("recentFiles/fileNames"));
    return v.toStringList();
}

void MainWindow::updateRecentFiles()
{
    QStringList files = recentFiles();
    const int numRecentFiles = qMin(files.size(), MAX_RECENT_FILES);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        _recentFilesAction[i]->setText(QFileInfo(files[i]).fileName());
        _recentFilesAction[i]->setData(files[i]);
        _recentFilesAction[i]->setVisible(true);
    }
    for (int j=numRecentFiles; j<MAX_RECENT_FILES; ++j)
    {
        _recentFilesAction[j]->setVisible(false);
    }
    ui->menuRecentGames->setEnabled(numRecentFiles > 0);
}

void MainWindow::setRecentFile(const QString& fileName)
{
    // Remember the file by its canonical file path
    const QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    if (canonicalFilePath.isEmpty())
        return;

    QStringList files = recentFiles();
    files.removeAll(canonicalFilePath);
    files.prepend(canonicalFilePath);
    while (files.size() > MAX_RECENT_FILES)
        files.removeLast();

    _settings.setValue(QLatin1String("recentFiles/fileNames"), files);
    updateRecentFiles();
}

bool MainWindow::validatePath(const QString &dir) const
{
    // file .cocos-project.json must exist, but this check is very fragile
    QString path = dir + "/.cocos-project.json";
    QFile file(path);
    return file.exists();
}
