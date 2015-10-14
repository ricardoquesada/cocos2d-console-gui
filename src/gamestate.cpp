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

#include "gamestate.h"

#include <QFileInfo>
#include <QProcessEnvironment>
#include <QDebug>

#include "preferencesdialog.h"

GameState::GameState(const QString& filePath)
    : _filePath(filePath)
    , _settings("org.cocos2d-x","Cocos2d Console GUI")
    , _runningProcess(nullptr)
    , _gameProperties()
    , _gameLibraries()
    , _buffer()
{
    QFileInfo fileinfo(filePath);
    _projectName = fileinfo.baseName();
    _path = fileinfo.canonicalPath();

    parseGameLibraries();
    parseGameProperties();
}

void GameState::parseGameProperties()
{
    QStringList args;
    args << "symbols" << "--jsonapi";
    runSDKBOXCommand(args);
}

void GameState::parseGameLibraries()
{
    QStringList args;
    args << "info" << "--jsonapi";
    runSDKBOXCommand(args);
}

const QString& GameState::getFilePath() const
{
    return _filePath;
}

const QString& GameState::getPath() const
{
    return _path;
}

const QString& GameState::getProjectName() const
{
    return _projectName;
}

const QMap<QString,QString>& GameState::getGameProperties() const
{
    return _gameProperties;
}

const QMap<QString,QString>& GameState::getGameLibraries() const
{
    return _gameLibraries;
}

//
// Helpers
//
bool GameState::runSDKBOXCommand(const QStringList& stringList)
{
    _runningProcess = new QProcess(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    env.insert("COCOS_CONSOLE_ROOT", PreferencesDialog::findCocosPath());
    env.insert("LANG", "en_US.UTF-8");
    _runningProcess->setProcessEnvironment(env);

    _runningProcess->setProcessChannelMode(QProcess::MergedChannels);
    _runningProcess->setWorkingDirectory(getPath());

    QString sdkboxFilePath = PreferencesDialog::findSDKBOXPath() + "/sdkbox";
    _runningProcess->start(sdkboxFilePath, stringList);

    bool ret = _runningProcess->waitForFinished(5000);
    if (ret)
    {
        qDebug() << _runningProcess->readAllStandardOutput();
    }
    qDebug() << "Ret: " << ret;
    return ret;
}
