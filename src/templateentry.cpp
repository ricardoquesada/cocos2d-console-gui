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

#include "templateentry.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


TemplateEntry TemplateEntry::createFromJson(const QJsonObject& jsonObject)
{
    TemplateEntry entry;
    entry._name = "invalid";

    struct {
        const char* name;
        Language value;
    } languages[] =
    {
        {"cpp", Language::CPP},
        {"lua", Language::LUA},
        {"js", Language::JAVASCRIPT}
    };
    static const int TOTAL_LANGUAGES = sizeof(languages)/sizeof(languages[0]);


    auto language = jsonObject["language"].toString();
    entry._name = jsonObject["name"].toString();
    entry._options = jsonObject["options"].toObject();
    entry._key = jsonObject["key"].toString();

    // description: append path
    auto path = jsonObject["path"].toString();
    entry._description = path + "/" + "template_metadata" + "/" + jsonObject["description_file"].toString();

    // language
    bool found = false;
    for (int i=0; i<TOTAL_LANGUAGES; i++)
    {
        if (language == languages[i].name) {
            entry._language = languages[i].value;
            found = true;
            break;
        }
    }
    if (!found)
        qDebug() << "Invalid language: " << language;

    return entry;
}
