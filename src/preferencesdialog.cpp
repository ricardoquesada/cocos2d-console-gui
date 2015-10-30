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

#include "preferencesdialog.h"
#include "ui_preferencesdialog.h"

#include <QFileDialog>
#include <QSettings>
#include <QDir>
#include <QFileInfo>
#include <QDebug>
#include <QStandardPaths>
#include <QProcess>
#include <QProcessEnvironment>

#if defined(Q_OS_WIN32)
static const char* COCOS_EXE = "cocos.bat";
static const char* SDKBOX_EXE = "sdkbox.bat";
#else
static const char* COCOS_EXE = "cocos";
static const char* SDKBOX_EXE = "sdkbox";
#endif

// function taken from here:
// http://stackoverflow.com/a/26991243/1119460
static bool fileExists(const QString& path)
{
    QFileInfo checkFile(path);
    // check if file exists and if yes: Is it really a file and no directory?
    return checkFile.exists() && checkFile.isFile();
}

QString PreferencesDialog::getCmdFilepath(const QString& cmd)
{
    QSettings settings("org.cocos2d-x","Cocos2d Console GUI");
    QString settingsPath = settings.value(cmd + "_path").toString();

    if (fileExists(settingsPath + "/" + cmd))
        return settingsPath + "/" + cmd;

    QString ret = "";
#if defined(Q_OS_OSX) || defined(Q_OS_UNIX)
    {
        QProcess process;
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        process.setProcessEnvironment(env);

        process.setProcessChannelMode(QProcess::MergedChannels);

        QStringList stringList;
        stringList << "-l" << "-c" << "which " + cmd;
        process.start("/bin/bash", stringList);

        if (process.waitForFinished(5000))
        {
            auto filepath = process.readAllStandardOutput();
            if (fileExists(filepath))
            {
                ret = filepath;
            }
        }
    }
#elif defined(Q_OS_WIN32)
    {
        QProcess process;
        QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
        process.setProcessEnvironment(env);

        process.setProcessChannelMode(QProcess::MergedChannels);

        QStringList stringList;
        stringList << cmd;
        process.start("c:/Windows/System32/where.exe", stringList);

        if (process.waitForFinished(5000))
        {
            auto filepath = process.readAllStandardOutput();
            filepath = filepath.simplified();
            if (fileExists(filepath))
            {
                ret = filepath;
            }
        }
    }
#endif // Q_OS_WIN32

    if (ret.length() == 0)
    {

        QString cwd = QDir::currentPath();
        if (fileExists(cwd + "/" + cmd))
            ret = cwd;

        else if (fileExists(cwd + "/../../" + cmd))
            ret = cwd + "/../../";

       // Some wild guesses
        else if (fileExists(QDir::homePath() + "/cocos2d-x/tools/cocos2d-console/bin/" + cmd))
            ret = QDir::homePath() + "/cocos2d-x/tools/cocos2d-console/bin";

        else if (fileExists(QDir::homePath() + "/progs/cocos2d-x/tools/cocos2d-console/bin/" + cmd))
            ret = QDir::homePath() + "/progs/cocos2d-x/tools/cocos2d-console/bin";

        else
        {
            auto list = QStandardPaths::standardLocations(QStandardPaths::ApplicationsLocation);
            foreach(const auto& element, list)
            {
                if (fileExists(element + "/Cocos/bin/" + cmd))
                {
                    ret = element + "/Cocos/bin/" + cmd;
                    break;
                }
            }
        }
    }

    if (!ret.isEmpty())
    {
        ret = QFileInfo(ret).canonicalPath();
        settings.setValue(cmd + "_path", ret);
        return ret + "/" + cmd;
    }

    return cmd;
}

QString PreferencesDialog::getCocosFilepath()
{
    return getCmdFilepath(COCOS_EXE);
}

QString PreferencesDialog::getSDKBOXFilepath()
{
    return getCmdFilepath(SDKBOX_EXE);
}

QString PreferencesDialog::getCocosPath()
{
    QFileInfo info(getCocosFilepath());
    return info.canonicalPath();
}

QString PreferencesDialog::getSDKBOXPath()
{
    QFileInfo info(getSDKBOXFilepath());
    return info.canonicalPath();
}

PreferencesDialog::PreferencesDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::PreferencesDialog),
    _settings("org.cocos2d-x","Cocos2d Console GUI")
{
    ui->setupUi(this);
    ui->lineEdit_cocos->setText(PreferencesDialog::getCocosPath());
    ui->lineEdit_sdkbox->setText(PreferencesDialog::getSDKBOXPath());

    // report error in case there is an error
//    on_lineEdit_cocos_editingFinished();
//    on_lineEdit_sdkbox_editingFinished();
}

PreferencesDialog::~PreferencesDialog()
{
    delete ui;
}

void PreferencesDialog::on_directoryButton_cocos_clicked()
{
    auto filename = QFileDialog::getExistingDirectory(this,
                                                 tr("Cocos2D Console Path"),
                                                 ui->lineEdit_cocos->text());

    if (!filename.isEmpty())
        ui->lineEdit_cocos->setText(filename);
}

void PreferencesDialog::on_buttonBox_accepted()
{
    _settings.setValue(QString(COCOS_EXE)+ "_path", ui->lineEdit_cocos->text());
    _settings.setValue(QString(SDKBOX_EXE) + "_path", ui->lineEdit_sdkbox->text());
}

void PreferencesDialog::on_lineEdit_cocos_editingFinished()
{
    // try to find "cocos". If not, report it as red
    auto cocos_path = ui->lineEdit_cocos->text();
    QFileInfo fi(cocos_path + "/" + COCOS_EXE);
    if (!fi.exists())
        ui->label_error->setText(tr("<font color='red'>Invalid path. 'cocos' not found</font>"));
    else
        ui->label_error->setText("");
}

void PreferencesDialog::on_directoryButton_sdkbox_clicked()
{
    auto filename = QFileDialog::getExistingDirectory(this,
                                                 tr("SDKBOX Path"),
                                                 ui->lineEdit_sdkbox->text());

    if (!filename.isEmpty())
        ui->lineEdit_sdkbox->setText(filename);
}

void PreferencesDialog::on_lineEdit_sdkbox_editingFinished()
{
    // try to find "sdkbox". If not, report it as red
    auto sdkbox_path = ui->lineEdit_sdkbox->text();
    QFileInfo fi(sdkbox_path + "/" + SDKBOX_EXE);
    if (!fi.exists())
        ui->label_error->setText(tr("<font color='red'>Invalid path. 'sdkbox' not found</font>"));
    else
        ui->label_error->setText("");
}
