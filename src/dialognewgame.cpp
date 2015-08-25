#include <QDebug>
#include <QString>
#include <QDirIterator>
#include <QFileInfo>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>

#include "dialognewgame.h"
#include "ui_dialognewgame.h"
#include "templatewizard.h"

DialogNewGame::DialogNewGame(QWidget *parent)
    : QDialog(parent)
    , _entriesCpp()
    , _entriesLua()
    , _entriesJavaScript()
    , ui(new Ui::DialogNewGame)
{
    ui->setupUi(this);

    // populate items
    auto listWidget = ui->listWidget;

    // parse templates
    parseTemplates();

    populateTemplateList("C++ Templates", &_entriesCpp, listWidget);
    populateTemplateList("JavaScript Templates", &_entriesJavaScript, listWidget);
    populateTemplateList("Lua Templates", &_entriesLua, listWidget);

    //
    listWidget->item(1)->setSelected(true);

    // populate text browser
    ui->textBrowser->setDocumentTitle("hello");
}

DialogNewGame::~DialogNewGame()
{
    delete ui;
}

void DialogNewGame::populateTemplateList(const QString& title, QList<TemplateEntry>* list, QListWidget* parent)
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
            auto item = new QListWidgetItem(list->at(i).name, parent);
            QVariant v;
            v.setValue(&list->at(i));
            item->setData(Qt::UserRole,v);
        }
    }

}

void DialogNewGame::on_listWidget_currentItemChanged(QListWidgetItem *current, QListWidgetItem *previous)
{
    auto variant = current->data(Qt::UserRole);
    auto entry = variant.value<const TemplateEntry*>();
    auto qurl = QUrl::fromLocalFile(entry->description);

    ui->textBrowser->setSource(qurl);
}


void DialogNewGame::on_buttonBox_accepted()
{
    TemplateWizard wizard(this);

    wizard.resize(this->width(), this->height());

    wizard.exec();
}

void DialogNewGame::parseTemplates()
{
    QDirIterator it(":/templates", QDirIterator::Subdirectories);
    while (it.hasNext()) {
        it.next();
        auto fileInfo = it.fileInfo();

        if (fileInfo.fileName() == "config.json") {
            QFile file(fileInfo.filePath());
            TemplateEntry entry = TemplateEntry::createFromJsonFile(&file);

            switch(entry.language){
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
    }
}
