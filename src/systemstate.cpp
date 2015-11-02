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
    auto commandLibs = new RunSDKBOXLibraries(this);
    RunMgr::getInstance()->runAsync(commandLibs);
    connect(commandLibs, &RunSDKBOXLibraries::finished, [&](Run* command)
    {
        QString json = command->getOutput().join("");
        parseSystemLibraries(json);
    });

    auto commandTemplates = new RunCocosListTemplates(this);
    RunMgr::getInstance()->runAsync(commandTemplates);
    connect(commandTemplates, &RunCocosListTemplates::finished, [&](Run* command)
    {
        QString json = command->getOutput().join("");
        parseSystemTemplates(json);
    });

}

bool SystemState::systemLibrariesParsed() const
{
    return _systemLibrariesParsed;
}

bool SystemState::systemTemplatesParsed() const
{
    return _systemTemplatesParsed;
}

const QJsonObject& SystemState::getSystemLibraries() const
{
    if (!_systemLibrariesParsed)
    {
        // FIXME:
        qDebug() << "System Libraries not parsed yet. Try again later";
    }

    return _systemLibraries;
}

const QJsonObject& SystemState::getSystemTemplates() const
{
    if (!_systemTemplatesParsed)
    {
        // FIXME:
        qDebug() << "System Templates not parsed yet. Try again later";
    }

    return _systemTemplates;
}

bool SystemState::parseSystemLibraries(const QString &json)
{
    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(json.toUtf8(), &error));
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parsing System Libraries JSON:" << error.errorString();
        qDebug() << json.simplified();
        return false;
    }
    _systemLibraries = loadDoc.object();
    _systemLibrariesParsed = true;
    emit systemLibrariesUpdated();
    return true;
}

bool SystemState::parseSystemTemplates(const QString &json)
{
    QJsonParseError error;
    QJsonDocument loadDoc(QJsonDocument::fromJson(json.toUtf8(), &error));
    if (error.error != QJsonParseError::NoError)
    {
        qDebug() << "Error parsing System Templates JSON:" << error.errorString();
        qDebug() << json.simplified();
        return false;
    }
    _systemTemplates = loadDoc.object();
    _systemTemplatesParsed = true;
    emit systemTemplatesUpdated();
    return true;
}
