#include "templateentry.h"

#include <QFile>
#include <QFileInfo>
#include <QDebug>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>


TemplateEntry TemplateEntry::createFromJsonFile(QFile* file)
{
    TemplateEntry entry;
    entry.name = "invalid";

    if (!file->open(QIODevice::ReadOnly)) {
        qWarning("Couldn't open open file.");
        return entry;
    }

    QByteArray data = file->readAll();
    QJsonDocument loadDoc(QJsonDocument::fromJson(data));


    auto object = loadDoc.object();

    struct {
        const char* name;
        int value;
    } platforms[] =
    {
        {"ios", Platform::IOS},
        {"android", Platform::ANDROID},
        {"winphone", Platform::WINDOWSPHONE},
        {"mac", Platform::MAC},
        {"windows", Platform::WINDOWS},
        {"linux", Platform::LINUX}
    };
    static const int TOTAL_PLATFORMS = sizeof(platforms)/sizeof(platforms[0]);

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


    auto language = object["language"].toString();
    auto array_platforms = object["platforms"].toArray();


    entry.name = object["name"].toString();

    // description: append path
    QFileInfo fileinfo(file->fileName());
    entry.description = fileinfo.absolutePath() + "/" + object["description"].toString();
    qDebug() << entry.description;

    entry.platforms = Platform::NONE;


    for (int i=0; i<array_platforms.count(); i++)
    {
        QString plat = array_platforms[0].toString();

        for (int j=0; j<TOTAL_PLATFORMS; j++)
        {
            if (platforms[j].name == plat)
                entry.platforms |= platforms[j].value;
        }
    }

    for (int i=0; i<TOTAL_LANGUAGES; i++)
    {
        if (language == languages[i].name) {
            entry.language = languages[i].value;
            break;
        }
    }

    return entry;
}
