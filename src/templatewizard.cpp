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

#include <QtWidgets>

#include "templatewizard.h"

TemplateWizard::TemplateWizard(QWidget *parent)
    : QWizard(parent)
{
    addPage(new LocationPage);
    addPage(new SDKPage);
    addPage(new ConclusionPage);

//    setPixmap(QWizard::BannerPixmap, QPixmap(":/images/banner.png"));
//    setPixmap(QWizard::BackgroundPixmap, QPixmap(":/images/background.png"));

    setWindowTitle(tr("Template Wizard"));
}

void TemplateWizard::accept()
{
    //
    QDialog::accept();
}

LocationPage::LocationPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Introduction and Project Location"));
    setSubTitle(tr("This wizard generates a Cocos2d-x game project."));
//    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo1.png"));

    auto groupBox = new QGroupBox();

    auto nameLabel = new QLabel(tr("Game &Name:"));
    auto nameEdit = new QLineEdit(tr("untitled"), this);
    nameLabel->setBuddy(nameEdit);

    auto createLabel = new QLabel(tr("&Create in:"));
    _createEdit = new QLineEdit(tr("~/MyGames"), this);
    createLabel->setBuddy(_createEdit);

    auto createChooseButton = new QPushButton("Choose...");

    connect(createChooseButton, &QPushButton::pressed, [&]()
    {
        auto fn = QFileDialog::getExistingDirectory(this, _createEdit->text());
        if (fn.length()>0)
        {
            _createEdit->setText(fn);
        }
    }
            );


    auto locationCheckBox = new QCheckBox(tr("&Use as default project location"));

    registerField("nameEdit*", nameEdit);
    registerField("createEdit*", _createEdit);
    registerField("locationCheckBox", locationCheckBox);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(nameLabel, 0, 0);
    gridLayout->addWidget(nameEdit, 0, 1, 1, 2);
    gridLayout->addWidget(createLabel, 1, 0);
    gridLayout->addWidget(_createEdit, 1, 1);
    gridLayout->addWidget(createChooseButton, 1, 2);
    gridLayout->addWidget(locationCheckBox, 2, 0, 2, -1);

    groupBox->setLayout(gridLayout);

    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(groupBox);

    setLayout(vLayout);
}

bool LocationPage::isComplete() const
{
    auto name = field("nameEdit").toString();
    auto create = field("createEdit").toString();

    return (name.length() && create.length());
}

SDKPage::SDKPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("SDK Selection"));
    setSubTitle(tr("Select the SDKs that you want to have installed in your game"));
//    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo2.png"));

    // --
    _listWidget = new QListWidget;

    QListWidgetItem *item;
    item = new QListWidgetItem(tr("Cocos2d-x v3.8"), _listWidget);
    item->setCheckState(Qt::CheckState::Checked);
    item->setFlags(Qt::NoItemFlags);

    item = new QListWidgetItem(tr("SDKBOX core"), _listWidget);
    item->setCheckState(Qt::CheckState::Checked);
    item->setFlags(Qt::NoItemFlags);

    item = new QListWidgetItem(tr("SDKBOX AdMob"), _listWidget);
    item->setCheckState(Qt::CheckState::Checked);

    item = new QListWidgetItem(tr("SDKBOX Facebook"), _listWidget);
    item->setCheckState(Qt::CheckState::Checked);

    item = new QListWidgetItem(tr("SDKBOX Kochava"), _listWidget);
    item->setCheckState(Qt::CheckState::Checked);

    item = new QListWidgetItem(tr("SDKBOX Vungle"), _listWidget);
    item->setCheckState(Qt::CheckState::Unchecked);


    // ---
    auto sdkCheckBox = new QCheckBox(tr("&Select all SDKs"));
    sdkCheckBox->setChecked(true);

    connect(sdkCheckBox, &QCheckBox::toggled, [&](bool enabled)
        {
            // skip cocos2d
            for(int row = 1; row < _listWidget->count(); row++)
            {
                QListWidgetItem *w = _listWidget->item(row);
                w->setCheckState(enabled?Qt::CheckState::Checked: Qt::CheckState::Unchecked);
            }
    }
    );


    // ----

    auto cocosSDK = new QLabel(tr("&Cocos2D-x SDK library type:"));
    QComboBox *comboBox = new QComboBox;
    comboBox->addItem(tr("binary"));
    comboBox->addItem(tr("source code"));
    cocosSDK->setBuddy(comboBox);
    auto hbox = new QHBoxLayout;
    hbox->addWidget(cocosSDK);
    hbox->addWidget(comboBox);

    // ----

    auto vblayout = new QVBoxLayout;
    vblayout->addWidget(sdkCheckBox);
    vblayout->addWidget(_listWidget);
    vblayout->addLayout(hbox);

    setLayout(vblayout);
}

void SDKPage::initializePage()
{
    QString className = field("nameEdit").toString();
}


// ----

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Project Management"));
    setSubTitle(tr("Specify where you want the wizard to put the generated "
                   "skeleton code."));
//    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo3.png"));


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
    auto textBrowser = new QTextBrowser();

    textBrowser->append("<html>");
    textBrowser->append("<span>Here goes some description of your project. Files to be created are:</span>");
    textBrowser->append("<p>");
    textBrowser->append("<code>/usr/local/bin/game.xxx</code>");
    textBrowser->append("<code>/usr/local/bin/game.xxx</code>");
    textBrowser->append("<code>/usr/local/bin/game.xxx</code>");
    textBrowser->append("<code>/usr/local/bin/game.xxx</code>");
    textBrowser->append("");
    textBrowser->append("<span>Press 'Done'.</span>");
    textBrowser->setReadOnly(true);

    grid->addWidget(textBrowser, 1, 0, 2, 2);

    // --
    setLayout(grid);

}

void ConclusionPage::initializePage()
{


}

