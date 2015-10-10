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

#include <QDebug>
#include <QString>
#include <QDirIterator>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QProcess>
#include <QProgressDialog>

#include "preferencesdialog.h"
#include "newgamedialog.h"
#include "ui_newgamedialog.h"
#include "templatewizard.h"
#include "progressdialog.h"
#include "gamestate.h"
#include "mainwindow.h"

NewGameDialog::NewGameDialog(QWidget *parent)
    : QDialog(parent)
    , _entriesCpp()
    , _entriesLua()
    , _entriesJavaScript()
    , ui(new Ui::NewGameDialog)
    , _progressDialog(nullptr)
    , _running(true)
{
    ui->setupUi(this);

    // populate items
    auto listWidget = ui->listWidget;

    // parse templates
    if (parseTemplates())
    {
        populateTemplateList("C++ Templates", &_entriesCpp, listWidget);
        populateTemplateList("JavaScript Templates", &_entriesJavaScript, listWidget);
        populateTemplateList("Lua Templates", &_entriesLua, listWidget);

        if (listWidget->count()>1)
            listWidget->item(1)->setSelected(true);
    }

    // populate text browser
    ui->textBrowser->setDocumentTitle("hello");
}

NewGameDialog::~NewGameDialog()
{
    delete ui;
}

void NewGameDialog::populateTemplateList(const QString& title, QList<TemplateEntry>* list, QListWidget* parent)
{
    if (list->count())
    {
        auto separator = new QListWidgetItem(title, parent);
        separator->setFlags(Qt::NoItemFlags);
        separator->setBackground(QBrush(QColor(224,224,224)));
        separator->setForeground(QBrush(Qt::black));
        separator->setSizeHint(QSize(0,30));

        for (int i=0; i<list->count(); i++)
        {
            auto item = new QListWidgetItem(list->at(i).name(), parent);
            QVariant v;
            v.setValue(&list->at(i));
            item->setData(Qt::UserRole,v);
        }
    }

}

void NewGameDialog::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    // unused
    (void)previous;

    auto variant = current->data(Qt::UserRole);
    auto entry = variant.value<const TemplateEntry*>();
    auto qurl = QUrl::fromLocalFile(entry->description());

    ui->textBrowser->setSource(qurl);
}


void NewGameDialog::on_buttonBox_accepted()
{
    auto variant = ui->listWidget->selectedItems().at(0)->data(Qt::UserRole);
    auto entry = variant.value<const TemplateEntry*>();
    TemplateWizard wizard(*entry, this);
    wizard.resize(this->width(), this->height());

    // 1: Done, 0: Cancel
    if (wizard.exec())
        copyFiles(wizard, *entry);
}

void NewGameDialog::copyFiles(const TemplateWizard& wizard, const TemplateEntry& templateEntry)
{
    _process = new QProcess(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    _process->setProcessEnvironment(env);

    _process->setProcessChannelMode(QProcess::MergedChannels);
    QStringList commandLine = QStringList() << PreferencesDialog::findCocosPath() + "/cocos"
                                            << "new"
                                            << wizard.field("gameName").toString()
                                            << "--template-name" << templateEntry.key()
                                            << "-d" << wizard.field("gamePath").toString();

    _progressDialog = new ProgressDialog(this);
    _progressDialog->setModal(true);
    _progressDialog->appendData(tr("Running:"));
    _progressDialog->appendData(commandLine.join(" "));

    auto executable = commandLine.at(0);
    commandLine.removeFirst();
    _process->start(executable, commandLine);


    connect(_process,SIGNAL(readyReadStandardOutput()), this, SLOT(processReadyReadStandardOutput()));
    connect(_process,SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(processFinished(int,QProcess::ExitStatus)));
    connect(_process,SIGNAL(finished(int,QProcess::ExitStatus)), _progressDialog, SLOT(processFinished()));

    if (_progressDialog->exec())
    {
        bool found = false;
        auto path = wizard.field("gamePath").toString();
        foreach (QWidget *widget, QApplication::topLevelWidgets())
        {
            if (dynamic_cast<MainWindow*>(widget))
            {
                auto mainWindow = static_cast<MainWindow*>(widget);
                auto gameState = new GameState(path);
                mainWindow->setGameState(gameState);
                found = true;
            }
        }
        if (!found)
            qDebug() << "MainWindow not found";

    }
}

void NewGameDialog::processReadyReadStandardOutput()
{
    _progressDialog->appendData(_process->read(_process->bytesAvailable()));

    // Output the data
//    qDebug() << "-- section --";
//    qDebug() << _data.data();
}

void NewGameDialog::processFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    Q_UNUSED(exitCode);
    Q_UNUSED(exitStatus);
}


///

bool NewGameDialog::parseTemplates()
{
    QProcess process(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    process.setProcessEnvironment(env);
    process.start( PreferencesDialog::findCocosPath() + "/cocos", QStringList() << "new" << "--list-templates");

    // wait for error or five seconds
    if (process.waitForFinished(5000))
    {
        const auto json(process.readAllStandardOutput());

        QJsonParseError error;
        QJsonDocument loadDoc(QJsonDocument::fromJson(json, &error));
        if (error.error != QJsonParseError::NoError) {
            qDebug() << error.errorString();
            return false;
        }
        auto objects = loadDoc.object();
        foreach (const auto& jsonObject, objects) {
            TemplateEntry entry = TemplateEntry::createFromJson(jsonObject.toObject());

            switch(entry.language()){
                case TemplateEntry::Language::CPP:
                    _entriesCpp.push_back(entry);
                    break;

                case TemplateEntry::Language::LUA:
                    _entriesLua.push_back(entry);
                    break;

                case TemplateEntry::Language::JAVASCRIPT:
                    _entriesJavaScript.push_back(entry);
                    break;
            }
        }

        return true;

    }
    else
    {
        qDebug() << "Error running cocos";
        return false;
    }
}
