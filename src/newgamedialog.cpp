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
#include <QProgressDialog>

#include "preferencesdialog.h"
#include "newgamedialog.h"
#include "ui_newgamedialog.h"
#include "templatewizard.h"
#include "progressdialog.h"
#include "gamestate.h"
#include "systemstate.h"
#include "mainwindow.h"
#include "runmgr.h"

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
        populateTemplateList(tr("C++ Templates"), &_entriesCpp, listWidget);
        populateTemplateList(tr("JavaScript Templates"), &_entriesJavaScript, listWidget);
        populateTemplateList(tr("Lua Templates"), &_entriesLua, listWidget);

        if (listWidget->count()>1)
            listWidget->item(1)->setSelected(true);
    }

    // populate text browser
//    ui->textBrowser->setDocumentTitle("hello");
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
    Q_UNUSED(previous);

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
    {
        // save defaults
        QSettings settings("org.cocos2d-x","Cocos2d Console GUI");

        auto saveLocation = wizard.field("locationCheckBox").toBool();
        settings.setValue("wizard/savePath", saveLocation);
        if (saveLocation)
            settings.setValue("wizard/defaultPath", wizard.field("gamePath").toString());

        copyFiles(wizard, *entry);
    }
}

void NewGameDialog::copyFiles(const TemplateWizard& wizard, const TemplateEntry& templateEntry)
{
    Run* lastQueuedCommand = nullptr;
    auto gamePath = wizard.field("gamePath").toString();
    auto gameName = wizard.field("gameName").toString();

    auto cocosNew = new RunCocosNew(gameName,
                                    gamePath,
                                    templateEntry.key(),
                                    this);

    lastQueuedCommand = cocosNew;

    _progressDialog = new ProgressDialog(this);
    _progressDialog->setModal(true);

    RunMgr::getInstance()->runSync(cocosNew);

    connect(cocosNew, &RunCocosNew::dataAvailable, this, &NewGameDialog::processOutputReady);

    auto selectedLibraries = wizard.getSelectedLibraries();
    if (selectedLibraries.count() > 0)
    {
        auto keys = selectedLibraries.keys();
        for (const auto& key: keys)
        {
            auto sdkboxImport = new RunSDKBOXImport(gamePath + "/" + gameName, key, this);
            RunMgr::getInstance()->runSync(sdkboxImport);
            connect(sdkboxImport, &RunSDKBOXImport::dataAvailable, this, &NewGameDialog::processOutputReady);

            lastQueuedCommand = sdkboxImport;
        }
    }

    // only care for the last queued command's "finished" event
    connect(lastQueuedCommand, &Run::finished, this, &NewGameDialog::processFinished);
    connect(lastQueuedCommand, &Run::finished, _progressDialog, &ProgressDialog::processFinished);

    if (_progressDialog->exec())
    {
        bool found = false;
        auto path = wizard.field("gamePath").toString();
        foreach (QWidget *widget, QApplication::topLevelWidgets())
        {
            if (dynamic_cast<MainWindow*>(widget))
            {
                const QString gameName = wizard.field("gameName").toString();
                const QString filePath = path + "/" + gameName + "/" + gameName + ".cocosproj";
                static_cast<MainWindow*>(widget)->openFile(filePath);
                found = true;
            }
        }
        if (!found)
            qDebug() << "MainWindow not found";
    }
}

void NewGameDialog::processOutputReady(Run* command, const QByteArray& data)
{
    Q_UNUSED(command);
    _progressDialog->appendData(data);
}

void NewGameDialog::processFinished(Run* command)
{
    Q_UNUSED(command);
}


///

bool NewGameDialog::parseTemplates()
{
    auto objects = SystemState::getInstance()->getSystemTemplates();

    foreach (const auto& jsonObject, objects)
    {
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
