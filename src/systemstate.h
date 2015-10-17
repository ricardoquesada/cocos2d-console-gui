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

#include <QObject>
#include <QJsonObject>

class SystemState : public QObject
{
    Q_OBJECT
public:
    static SystemState* getInstance();

    const QJsonObject& getSystemLibraries() const;
    const QJsonObject& getSystemTemplates() const;

signals:
    void systemLibrariesUpdated();
    void systemTemplatesUpdated();

public slots:

protected:
    bool parseSystemLibraries(const QString& json);
    bool parseSystemTemplates(const QString& json);

    explicit SystemState(QObject *parent = 0);

    QJsonObject _systemLibraries;
    bool _systemLibrariesParsed;

    QJsonObject _systemTemplates;
    bool _systemTemplatesParsed;

};
