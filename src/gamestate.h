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

#include <QString>
#include <QJsonObject>
#include <QJsonArray>
#include <QSettings>
#include <QStringList>
#include <QObject>
#include <QStringList>


class GameState : public QObject
{
    Q_OBJECT

public:
    explicit GameState(const QString& filePath);

    const QString& getPath() const;
    const QString& getFilePath() const;
    const QString& getProjectName() const;

    const QJsonObject& getGameProperties() const;
    const QJsonObject& getGameLibraries() const;
    const QJsonArray& getGamePlatforms() const;

    bool isReady() const;

    bool parseGameProperties(const QString& json);
    bool parseGameLibraries(const QString& json);
    bool parseGamePlatforms(const QString& json);

signals:
    void gameLibrariesUpdated();
    void gamePropertiesUpdated();
    void gamePlatformsUpdated();

private slots:

private:
    const QString _filePath;
    QSettings _settings;

    QString _path;
    QString _projectName;

    QJsonObject _gameProperties;
    QJsonObject _gameLibraries;
    QJsonArray _gamePlatforms;

    bool _gamePropertiesParsed;
    bool _gameLibrariesParsed;
    bool _gamePlatformsParsed;
};
