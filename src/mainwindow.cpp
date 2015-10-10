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

#include "dialognewgame.h"
#include "gamedialog.h"
#include "templateentry.h"
#include "aboutdialog.h"
#include "preferencesdialog.h"

constexpr int MainWindow::MAX_RECENT_FILES;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _settings("org.cocos2d-x","Cocos2d Console GUI")
{
    ui->setupUi(this);
    ui->label->setText("<img height='192' width='192' src=':/logo.png'>");

    createActions();
}

MainWindow::~MainWindow()
{
    delete ui;
}

//
// Manual Callback events
//
void MainWindow::openRecentFile_triggered()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
        auto path = action->data().toString();
        if (path.length() > 0)
            GameDialog(path,this).exec();
        else
            qDebug() << "Invalid path";
    }
}
void MainWindow::itemDoubleClicked(QListWidgetItem* item)
{
    auto path = item->data(Qt::UserRole).toString();
    if (path.length() > 0)
    {
        GameDialog dialog(path, this);
        dialog.exec();
    }
    else
    {
        qDebug() << "Invalid path";
    }
}

//
// QtCreator Callback events
//
void MainWindow::on_pushButtonNewGame_clicked()
{
    DialogNewGame dialog(this);
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

void MainWindow::on_pushButtonOpenGame_clicked()
{
    auto path = QFileDialog::getExistingDirectory(this, "Select Existing Game Directory");
    if (path.length() > 0) {
        if (validatePath(path))
        {
            setRecentFile(path);
            GameDialog(path, this).exec();
        }
    }
}

void MainWindow::on_actionOpen_triggered()
{
    on_pushButtonOpenGame_clicked();
}

void MainWindow::on_actionClear_Recent_Games_triggered()
{
    _settings.setValue(QLatin1String("recentFiles/fileNames"), QStringList());
    updateRecentFiles();
}

//
// Helpers
//
void MainWindow::createActions()
{
    // Add recent file actions to the recent files menu and widgetList
    for (int i=0; i < MAX_RECENT_FILES; ++i)
    {
        _recentFilesAction[i] = new QAction(this);
        ui->menuRecentGames->insertAction(ui->actionClear_Recent_Games, _recentFilesAction[i]);
        _recentFilesAction[i]->setVisible(false);
        connect(_recentFilesAction[i], &QAction::triggered, this, &MainWindow::openRecentFile_triggered);

        _recentFilesWidget[i] = new QListWidgetItem(ui->listWidget);
        _recentFilesWidget[i]->setHidden(true);
    }
    ui->menuRecentGames->insertSeparator(ui->actionClear_Recent_Games);
    updateRecentFiles();

    //
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)) , this, SLOT(itemDoubleClicked(QListWidgetItem*)));
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

        _recentFilesWidget[i]->setText(QFileInfo(files[i]).fileName());
        _recentFilesWidget[i]->setHidden(false);
        _recentFilesWidget[i]->setData(Qt::UserRole, files[i]);
    }
    for (int j=numRecentFiles; j<MAX_RECENT_FILES; ++j)
    {
        _recentFilesAction[j]->setVisible(false);
        _recentFilesWidget[j]->setHidden(true);
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
