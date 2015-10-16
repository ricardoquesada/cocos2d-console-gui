#include "librariesdialog.h"
#include "ui_librariesdialog.h"

#include <QStringListModel>

#include "gamestate.h"

LibrariesDialog::LibrariesDialog(GameState* gameState, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::LibrariesDialog)
{
    ui->setupUi(this);

    auto keys = gameState->getSystemLibraries().keys();

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
