#pragma once

#include <QObject>
#include <QString>
#include <QFile>

class TemplateEntry
{
public:
    enum Platform
    {
        NONE = 0,

        IOS = 1 << 0,
        ANDROID = 1 << 1,
        WINDOWSPHONE = 1 << 2,
        WINDOWS = 1 << 3,
        MAC = 1 << 4,
        LINUX = 1 << 5,
    };

    enum Language
    {
        CPP,
        JAVASCRIPT,
        LUA
    };

    static TemplateEntry createFromJsonFile(QFile* file);

    QString name;
    QString description;
    int platforms;
    Language language;
};

// needed to use it in a qvariant
Q_DECLARE_METATYPE(const TemplateEntry*)
