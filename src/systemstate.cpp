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

#include "systemstate.h"

#include <QJsonDocument>
#include <QDebug>

#include "runmgr.h"

SystemState* SystemState::getInstance()
{
    static SystemState* _instance = nullptr;
    if (!_instance)
        _instance = new SystemState;
    return _instance;
}

SystemState::SystemState(QObject *parent) : QObject(parent)
{
    auto command = new RunSDKBOXLibraries(this);
    RunMgr::getInstance()->runAsync(command);
    connect(command, &RunSDKBOXLibraries::finished, [&](const QStringList& output)
    {
        QString json = output.join("");
        parseSystemLibraries(json);
    });
}

const QJsonObject& SystemState::getSystemLibraries() const
{
    if (!_systemLibrariesParsed)
    {
        // FIXME:
        qDebug() << "System State not parsed";
        while(!_systemLibrariesParsed)
            ;
    }

    return _systemLibraries;
}

bool SystemState::parseSystemLibraries(const QString &json)
{
    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(json.toUtf8(), &error));
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parsing JSON:" << error.errorString();
        return false;
    }
    _systemLibraries = loadDoc.object();
    _systemLibrariesParsed = true;
    emit systemLibrariesUpdated();
    return true;
}

