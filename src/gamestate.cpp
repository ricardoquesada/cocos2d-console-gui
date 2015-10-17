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
    , _gamePropertiesParsed(false)
    , _gameLibrariesParsed(false)
{
    QFileInfo fileinfo(filePath);
    _projectName = fileinfo.baseName();
    _path = fileinfo.canonicalPath();
}

bool GameState::isReady() const
{
    return (_gameLibrariesParsed && _gamePropertiesParsed);
}

bool GameState::parseGameProperties(const QString& json)
{
    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(json.toUtf8(), &error));
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parsing JSON:" << error.errorString();
        return false;
    }

    _gameProperties = loadDoc.object();
    _gamePropertiesParsed = true;
    emit gamePropertiesUpdated();
    return true;
}

bool GameState::parseGameLibraries(const QString& json)
{
    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(json.toUtf8(), &error));
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parsing JSON:" << error.errorString();
        return false;
    }
    _gameLibraries = loadDoc.object();
    _gameLibrariesParsed = true;
    emit gameLibrariesUpdated();
    return true;
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
