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
    , _gamePlatformsParsed(false)
{
    QFileInfo fileinfo(filePath);
    _projectName = fileinfo.baseName();
    _path = fileinfo.canonicalPath();
}

bool GameState::isReady() const
{
    return (_gameLibrariesParsed && _gamePropertiesParsed && _gamePlatformsParsed);
}

bool GameState::parseGameProperties(const QString& json)
{
    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(json.toUtf8(), &error));
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parsing Properties JSON:" << error.errorString();
        return false;
    }

    _gameProperties = loadDoc.object();

    {
        struct _win_versions
        {
            const char* path;
            const char* prop;
        } win_versions[] = {
            {"/proj.win32/", "VISUAL_STUDIO_WIN32_PROJECT"},
            {"/proj.win8.1-universal/", "VISUAL_STUDIO_UNIVERSAL_PROJECT"},
            {"/proj.win10/", "VISUAL_STUDIO_WIN10_PROJECT"},
        };

        const int COUNT_VERSIONS = sizeof(win_versions) / sizeof(win_versions[0]);

        // FIXME. SDKBOX Does not report VS project.
        // Use Android Studio project dir as reference. XCODE_PROJECT is not defined on Windows SDKBOX.
        auto androidStudio = _gameProperties["ANDROID_STUDIO_PROJECT_DIR"].toString();
        // remove last dir
        QFileInfo fi(androidStudio);
        auto base = fi.canonicalPath();

        for (int i=0; i < COUNT_VERSIONS; ++i)
        {
            QString winpath = base + win_versions[i].path + _projectName + ".sln";
            QFileInfo fi(winpath);
            if (fi.exists())
                _gameProperties[win_versions[i].prop] = winpath;
        }
    }

    _gamePropertiesParsed = true;
    emit gamePropertiesUpdated();
    return true;
}

bool GameState::parseGameLibraries(const QString& json)
{
    // WORKAROUND:
    // json could be empty instead of "{}"

    QByteArray byteArray;

    byteArray = (json.length() > 0) ? json.toUtf8() : "{}";
    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(byteArray, &error));
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parsing Libraries JSON:" << error.errorString();
        qDebug() << json;
        return false;
    }
    _gameLibraries = loadDoc.object();
    _gameLibrariesParsed = true;
    emit gameLibrariesUpdated();
    return true;
}

bool GameState::parseGamePlatforms(const QString& json)
{
    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(json.toUtf8(), &error));
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parsing Platforms JSON";
        return false;
    }
    _gamePlatformsParsed = true;
    _gamePlatforms = loadDoc.object()["platforms"].toArray();
    emit gamePlatformsUpdated();
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
    Q_ASSERT(_gamePropertiesParsed);
    return _gameProperties;
}

const QJsonObject& GameState::getGameLibraries() const
{
    Q_ASSERT(_gameLibrariesParsed);
    return _gameLibraries;
}

const QJsonArray& GameState::getGamePlatforms() const
{
    Q_ASSERT(_gamePlatformsParsed);
    return _gamePlatforms;
}
