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

#include <QWizard>
#include <QSettings>

QT_BEGIN_NAMESPACE
class QCheckBox;
class QGroupBox;
class QLabel;
class QLineEdit;
class QRadioButton;
class QListWidget;
class QLineEdit;
class QTextBrowser;
QT_END_NAMESPACE

class TemplateEntry;

class TemplateWizard: public QWizard
{
    Q_OBJECT

public:
    TemplateWizard(const TemplateEntry& templateEntry, QWidget *parent = 0);

    void accept() Q_DECL_OVERRIDE;

protected:
    const TemplateEntry& _templateEntry;
    QSettings _settings;
};

class LocationPage: public QWizardPage
{
    Q_OBJECT

public:
    LocationPage(QWidget *parent = 0);

    bool isComplete() const Q_DECL_OVERRIDE;
    virtual bool validatePage() Q_DECL_OVERRIDE;

private:
    QLineEdit* _createEdit;
};

class SDKPage: public QWizardPage
{
    Q_OBJECT

public:
       SDKPage(QWidget *parent = 0);

protected:
    void initializePage() Q_DECL_OVERRIDE;
    virtual bool validatePage() Q_DECL_OVERRIDE;

private:
    QListWidget* _listWidget;
};


class ConclusionPage: public QWizardPage
{
    Q_OBJECT

public:
    ConclusionPage(QWidget *parent = 0);

protected:
    void initializePage() Q_DECL_OVERRIDE;
    virtual bool validatePage() Q_DECL_OVERRIDE;

private:
    QTextBrowser* _textBrowser;
};

