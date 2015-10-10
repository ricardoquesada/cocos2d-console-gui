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

#include "welcomedialog.h"
#include "ui_welcomedialog.h"

#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QListWidgetItem>

#include "newgamedialog.h"
#include "gamedialog.h"
#include "templateentry.h"
#include "aboutdialog.h"
#include "preferencesdialog.h"

constexpr int WelcomeDialog::MAX_RECENT_FILES;


WelcomeDialog::WelcomeDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::WelcomeDialog),
    _settings("org.cocos2d-x","Cocos2d Console GUI")
{
    ui->setupUi(this);
    ui->label_icon->setText("<img height='128' width='128' src=':/logo.png'>");

    createActions();
}

WelcomeDialog::~WelcomeDialog()
{
    delete ui;
}

//
// Manual Callback events
//
void WelcomeDialog::itemDoubleClicked(QListWidgetItem* item)
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
void WelcomeDialog::on_pushButton_NewGame_clicked()
{
    NewGameDialog dialog(this);
    dialog.exec();
}

void WelcomeDialog::on_pushButton_OpenGame_clicked()
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

//
// Helpers
//
void WelcomeDialog::createActions()
{
    // Add recent file actions to the recent files menu and widgetList
    for (int i=0; i < MAX_RECENT_FILES; ++i)
    {
        _recentFilesWidget[i] = new QListWidgetItem(ui->listWidget);
        _recentFilesWidget[i]->setHidden(true);
    }
    updateRecentFiles();

    //
    connect(ui->listWidget, SIGNAL(itemDoubleClicked(QListWidgetItem*)) , this, SLOT(itemDoubleClicked(QListWidgetItem*)));
}

QStringList WelcomeDialog::recentFiles() const
{
    QVariant v = _settings.value(QLatin1String("recentFiles/fileNames"));
    return v.toStringList();
}

void WelcomeDialog::updateRecentFiles()
{
    QStringList files = recentFiles();
    const int numRecentFiles = qMin(files.size(), MAX_RECENT_FILES);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        _recentFilesWidget[i]->setText(QFileInfo(files[i]).fileName());
        _recentFilesWidget[i]->setHidden(false);
        _recentFilesWidget[i]->setData(Qt::UserRole, files[i]);
    }
    for (int j=numRecentFiles; j<MAX_RECENT_FILES; ++j)
    {
        _recentFilesWidget[j]->setHidden(true);
    }
}

void WelcomeDialog::setRecentFile(const QString& fileName)
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

bool WelcomeDialog::validatePath(const QString &dir) const
{
    // file .cocos-project.json must exist, but this check is very fragile
    QString path = dir + "/.cocos-project.json";
    QFile file(path);
    return file.exists();
}


void WelcomeDialog::on_checkBox_clicked()
{
    _settings.setValue("show_welcome_dialog", (ui->checkBox->checkState() == Qt::Checked));
}
