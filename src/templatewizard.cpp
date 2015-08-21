/****************************************************************************
**
** Copyright (C) 2015 The Qt Company Ltd.
** Contact: http://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
    auto nameEdit = new QLineEdit;
    nameLabel->setBuddy(nameEdit);

    auto createLabel = new QLabel(tr("&Create in:"));
    auto createEdit = new QLineEdit;
    createLabel->setBuddy(createEdit);

    auto createChooseButton = new QPushButton("Choose...");

    auto locationCheckBox = new QCheckBox(tr("&Use as default project location"));


    registerField("nameEdit*", nameEdit);
    registerField("createEdit*", createEdit);
    registerField("locationCheckBox", locationCheckBox);

    QGridLayout *gridLayout = new QGridLayout;
    gridLayout->addWidget(nameLabel, 0, 0);
    gridLayout->addWidget(nameEdit, 0, 1, 1, 2);
    gridLayout->addWidget(createLabel, 1, 0);
    gridLayout->addWidget(createEdit, 1, 1);
    gridLayout->addWidget(createChooseButton, 1, 2);
    gridLayout->addWidget(locationCheckBox, 2, 0, 2, -1);

    groupBox->setLayout(gridLayout);

    auto vLayout = new QVBoxLayout;
    vLayout->addWidget(groupBox);

    setLayout(vLayout);
}

SDKPage::SDKPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("SDK Selection"));
    setSubTitle(tr("Select the SDKs that you want to have installed in your game"));
//    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo2.png"));

    auto listWidget = new QListWidget;

    QListWidgetItem *item;
    item = new QListWidgetItem(tr("Cocos2d-x v3.8"), listWidget);
    item->setCheckState(Qt::CheckState::Checked);
    item->setFlags(Qt::NoItemFlags);

    item = new QListWidgetItem(tr("SDKBOX AdMob"), listWidget);
    item->setCheckState(Qt::CheckState::Checked);

    item = new QListWidgetItem(tr("SDKBOX Facebook"), listWidget);
    item->setCheckState(Qt::CheckState::Checked);

    item = new QListWidgetItem(tr("SDKBOX Kochava"), listWidget);
    item->setCheckState(Qt::CheckState::Checked);

    item = new QListWidgetItem(tr("SDKBOX Vungle"), listWidget);
    item->setCheckState(Qt::CheckState::Unchecked);

    auto vblayout = new QVBoxLayout;
    vblayout->addWidget(listWidget);

    setLayout(vblayout);
}

void SDKPage::initializePage()
{
    QString className = field("nameEdit").toString();
}

ConclusionPage::ConclusionPage(QWidget *parent)
    : QWizardPage(parent)
{
    setTitle(tr("Project Management"));
    setSubTitle(tr("Specify where you want the wizard to put the generated "
                   "skeleton code."));
    setPixmap(QWizard::LogoPixmap, QPixmap(":/images/logo3.png"));

}

void ConclusionPage::initializePage()
{

}

