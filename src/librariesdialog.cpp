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

#include "librariesdialog.h"
#include "ui_librariesdialog.h"

#include <QStringListModel>

#include "gamestate.h"
#include "systemstate.h"

LibrariesDialog::LibrariesDialog(GameState* gameState, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LibrariesDialog)
{
    Q_UNUSED(gameState);
    ui->setupUi(this);

    auto keys = SystemState::getInstance()->getSystemLibraries().keys();

    // Remove already used libraries, including "SDKBOX"
    // "SDKBOX" library is exported, which shouldn't... ignore it.
    auto usedLibs = gameState->getGameLibraries().keys();
    usedLibs << "SDKBOX";

    QString libToRemove;
    foreach (libToRemove, usedLibs)
    {
        int idx = keys.indexOf(libToRemove, 0);
        if (idx >=0)
            keys.removeAt(idx);
    }

    auto model = new QStringListModel(this);
    model->setStringList(keys);
    ui->listView->setModel(model);
}

LibrariesDialog::~LibrariesDialog()
{
    delete ui;
}

QString LibrariesDialog::getSelectedString() const
{
    auto list = ui->listView->selectionModel()->selectedRows();

    if (list.length() > 0)
        return list.at(0).data(Qt::DisplayRole).toString();
    return "";
}

void LibrariesDialog::on_listView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);
    accept();
}
