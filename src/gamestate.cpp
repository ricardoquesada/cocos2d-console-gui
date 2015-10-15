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
#include <QLocale>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonParseError>

#include "preferencesdialog.h"

GameState::GameState(const QString& filePath)
    : _filePath(filePath)
    , _settings("org.cocos2d-x","Cocos2d Console GUI")
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
    runSDKBOXCommand(args, &_gameProperties);
}

void GameState::parseGameLibraries()
{
    QStringList args;
    args << "info" << "--jsonapi";
    runSDKBOXCommand(args, &_gameLibraries);
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

const QJsonObject& GameState::getGameProperties() const
{
    return _gameProperties;
}

const QJsonObject& GameState::getGameLibraries() const
{
    return _gameLibraries;
}

//
// Helpers
//
bool GameState::runSDKBOXCommand(const QStringList& stringList, QJsonObject* outObject)
{
    QProcess process(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // needed for gettext apps, like cocos and sdkbox
    env.insert("LANG", QLocale::system().name());
    process.setProcessEnvironment(env);

    process.setProcessChannelMode(QProcess::MergedChannels);
    process.setWorkingDirectory(getPath());

    QString sdkboxFilePath = PreferencesDialog::findSDKBOXPath() + "/sdkbox";
    process.start(sdkboxFilePath, stringList);

    bool ret = process.waitForFinished(5000);
    if (ret)
    {
        const auto json(process.readAllStandardOutput());

        QJsonParseError error;
        QJsonDocument loadDoc(QJsonDocument::fromJson(json, &error));
        if (error.error != QJsonParseError::NoError) {
            qDebug() << error.errorString();
            return false;
        }
        *outObject = loadDoc.object();
    }
    return ret;
}
