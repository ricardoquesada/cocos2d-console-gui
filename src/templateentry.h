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
#include <QString>
#include <QFile>
#include <QJsonObject>
#include <QVariantMap>

class TemplateEntry
{
public:
    enum Language
    {
        CPP,
        JAVASCRIPT,
        LUA
    };

    static TemplateEntry createFromJson(const QJsonObject& jsonObject);

    const QString& name() const { return _name; }
    const QString& key() const { return _key; }
    const Language& language() const { return _language; }
    const QVariantMap& libraries() const { return _libraries; }
    const QString& description() const { return _description; }

protected:
    QString _name;
    QString _description;
    QString _key;
    Language _language;
    QVariantMap _libraries;
};

// needed to use it in a qvariant
Q_DECLARE_METATYPE(const TemplateEntry*)
