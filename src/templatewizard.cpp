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

#include <QtWidgets>
#include <QStandardPaths>
#include <QSpacerItem>

#include "templatewizard.h"
#include "templateentry.h"
#include "preferencesdialog.h"

TemplateWizard::TemplateWizard(const TemplateEntry& templateEntry, QWidget *parent)
    : QWizard(parent)
    , _templateEntry(templateEntry)
    , _settings("org.cocos2d-x","Cocos2d Console GUI")
{
    auto libraries = _templateEntry.libraries();

    addPage(new LocationPage(this));

    // only display the SDKPage if 'libraries' is not empty
    if (libraries.size() > 0)
        addPage(new SDKPage(libraries, this));

    addPage(new ConclusionPage(this));

    setWindowTitle(tr("Template Wizard"));
}

void TemplateWizard::accept()
{
    QDialog::accept();
}

void TemplateWizard::setSelectedLibraries(const QVariantMap &libraries)
{
    _selectedLibraries = libraries;
}

const QVariantMap& TemplateWizard::getSelectedLibraries() const
{
    return _selectedLibraries;
}

//
// LocationPage
//

LocationPage::LocationPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introduction and Project Location"));
    setSubTitle(tr("This wizard generates a Cocos2d-x game project."));

    auto groupBox = new QGroupBox();

    auto nameLabel = new QLabel(tr("Game &Name:"));
    auto nameEdit = new QLineEdit(tr("untitled"), this);
    nameLabel->setBuddy(nameEdit);

    auto createLabel = new QLabel(tr("&Create in:"));
    _createEdit = new QLineEdit("");
    createLabel->setBuddy(_createEdit);

    auto createChooseButton = new QPushButton(tr("Choose..."));

    connect(createChooseButton, &QPushButton::pressed, [&]()
    {
        auto fn = QFileDialog::getExistingDirectory(this,
                                                    tr("Choose Directory"),
                                                    _createEdit->text()
                                                    );
        if (fn.length()>0)
        {
            _createEdit->setText(fn);
        }
    }
            );


    auto locationCheckBox = new QCheckBox(tr("&Use as default project location"));

    registerField("gameName*", nameEdit);
    registerField("gamePath*", _createEdit);
    registerField("locationCheckBox", locationCheckBox);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(nameLabel, 0, 0);
    gridLayout->addWidget(nameEdit, 0, 1, 1, 2);
    gridLayout->addWidget(createLabel, 1, 0);
    gridLayout->addWidget(_createEdit, 1, 1);
    gridLayout->addWidget(createChooseButton, 1, 2);
    gridLayout->addWidget(locationCheckBox, 2, 0, 2, -1);

    groupBox->setLayout(gridLayout);

    _label = new QLabel(this);

    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(groupBox);
    vLayout->addStretch();
    vLayout->addWidget(_label);

    setLayout(vLayout);

    // load defaults
    QSettings settings("org.cocos2d-x","Cocos2d Console GUI");
    bool enabled = settings.value("wizard/savePath", true).toBool();
    QString dir = settings.value("wizard/defaultPath", QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0)).toString();

    locationCheckBox->setChecked(enabled);
    _createEdit->setText(dir);
}

bool LocationPage::isComplete() const
{
    auto name = field("gameName").toString();
    auto path = field("gamePath").toString();

    QFileInfo fi(path);
    QFileInfo fullpath(QString(path) + "/" + name);

    bool ret = (name.length() && fi.isDir() && !fullpath.exists());

    if (name.length() > 0 && fi.isDir())
    {
         if (fullpath.exists())
            _label->setText(tr("<font color='red'>Diretory '%1' already exists</font>").arg(fullpath.canonicalPath()));
         else
             _label->setText("");
    }

    return ret;
}

//
// SDKPage
//
SDKPage::SDKPage(const QVariantMap& libraries, QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("SDK Selection"));
    setSubTitle(tr("Select the SDKs that you want to have installed in your game"));

    auto sdkCheckBox = new QCheckBox(tr("&Select all SDKs"));
    sdkCheckBox->setChecked(false);

    _listWidget = new QListWidget;
    for (auto it=libraries.begin(); it != libraries.end(); ++it)
    {
        QListWidgetItem* item = new QListWidgetItem(it.value().toString(), _listWidget);
        item->setCheckState(Qt::CheckState::Unchecked);
        item->setData(Qt::UserRole, it.key());
    }


    connect(sdkCheckBox, &QCheckBox::toggled, [&](bool enabled)
        {
            for(int row = 0; row < _listWidget->count(); row++)
            {
                QListWidgetItem *w = _listWidget->item(row);
                w->setCheckState(enabled ? Qt::CheckState::Checked
                                         : Qt::CheckState::Unchecked);
            }
    }
    );


    auto vblayout = new QVBoxLayout;
    vblayout->addWidget(sdkCheckBox);
    vblayout->addWidget(_listWidget);

    setLayout(vblayout);
}

void SDKPage::initializePage()
{
    QString className = field("gameName").toString();
}

bool SDKPage::validatePage()
{
    QVariantMap map;

    for(int row = 0; row < _listWidget->count(); row++)
    {
        QListWidgetItem* w = _listWidget->item(row);
        if (w->checkState() == Qt::Checked)
            map[w->data(Qt::UserRole).toString()] = w->text();
    }

    static_cast<TemplateWizard*>(wizard())->setSelectedLibraries(map);

    return true;
}

//
// ConclusionPage
//
ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Project Management"));
    setSubTitle(tr("Specify where you want the wizard to put the generated "
                   "skeleton code."));

    // --

    auto grid = new QGridLayout;

    auto versionLabel = new QLabel(tr("&Add to version control:"));
    QComboBox *comboBox = new QComboBox;
    comboBox->addItem(tr("<None>"));
    comboBox->addItem(tr("Git"));
    versionLabel->setBuddy(comboBox);

    grid->addWidget(versionLabel, 0, 0);
    grid->addWidget(comboBox, 0, 1);

    // --
    _textBrowser = new QTextBrowser();
    grid->addWidget(_textBrowser, 1, 0, 2, 2);

    // --
    setLayout(grid);
}

void ConclusionPage::initializePage()
{
    _textBrowser->setReadOnly(true);

    QString html;

    html.append("<html>");
    html.append(tr("<span>Game Name: %1</span>").arg(field("gameName").toString()));
    html.append("<p>");
    html.append(tr("<span>Files to be created in:</span>"));
    html.append("<p>");
    html.append(QString("<code>%1</code>").arg(field("gamePath").toString()));
    html.append("<p>");

    auto selectedLibraries = static_cast<TemplateWizard*>(wizard())->getSelectedLibraries();
    if (selectedLibraries.count() > 0)
    {
        html.append(tr("<span>Libraries to add:</span>"));
        html.append("<ul>");
        for(const auto description: selectedLibraries)
        {
            html.append("<li>");
            html.append(description.toString());
            html.append("</li>");
        }
        html.append("</ul>");
    }
    html.append(tr("<span>Press 'Done'.</span></html>"));
    _textBrowser->setHtml(html);
}
