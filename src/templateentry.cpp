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
    entry.name = "invalid";

    struct {
        const char* name;
        Language value;
    } languages[] =
    {
        {"c++", Language::CPP},
        {"lua", Language::LUA},
        {"javascript", Language::JAVASCRIPT}
    };
    static const int TOTAL_LANGUAGES = sizeof(languages)/sizeof(languages[0]);


    auto language = jsonObject["language"].toString();
    entry.name = jsonObject["name"].toString();

    // description: append path
    auto path = jsonObject["path"].toString();
    entry.description = path + "/" + "template_metadata" + "/" + jsonObject["description_file"].toString();

    // language
    bool found = false;
    for (int i=0; i<TOTAL_LANGUAGES; i++)
    {
        if (language == languages[i].name) {
            entry.language = languages[i].value;
            found = true;
            break;
        }
    }
    if (!found)
        qDebug() << "Invalid language: " << language;

    return entry;
}
