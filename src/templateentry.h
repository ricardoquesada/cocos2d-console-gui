#pragma once

#include <QObject>
#include <QString>
#include <QFile>

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

    QString name;
    QString description;
    Language language;
};

// needed to use it in a qvariant
Q_DECLARE_METATYPE(const TemplateEntry*)
