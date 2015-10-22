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

#include "mainwindow.h"

#include <QIcon>
#include <QSettings>
#include <QDebug>
#include <QTranslator>
#include <QCoreApplication>
#include <QLibraryInfo>

#include "welcomedialog.h"
#include "cocos2dguiapplication.h"
#include "systemstate.h"

int main(int argc, char *argv[])
{
    Cocos2dGUIApplication app(argc, argv);

    app.setOrganizationDomain(QLatin1String("cocos2d-x.org"));
    app.setApplicationVersion(QLatin1String(APP_VERSION));
    app.setApplicationName(QLatin1String("Cocos2d Console GUI"));
    app.setApplicationDisplayName(QLatin1String("Cocos2d Console GUI"));

    // translation code
    QTranslator qtTranslator;
    qtTranslator.load("qt_" + QLocale::system().name(),
                      QLibraryInfo::location(QLibraryInfo::TranslationsPath));
    app.installTranslator(&qtTranslator);


    auto translationDir = QCoreApplication::applicationDirPath();
#ifdef Q_OS_WIN32
    translationDir += QLatin1String("/translations");
#elif defined(Q_OS_MAC)
    translationDir += QLatin1String("/../Translations");
#else
    translationDir += QLatin1String("/../share/tiled/translations");
#endif

    QTranslator myappTranslator;
    myappTranslator.load("cc_console_gui" + QLocale::system().name(),
                         translationDir);
    app.installTranslator(&myappTranslator);


#ifdef Q_OS_MAC
    app.setAttribute(Qt::AA_DontShowIconsInMenus);
#endif

    QApplication::setWindowIcon(QIcon(":/logo.png"));


    MainWindow w;

    QSettings settings("org.cocos2d-x", "Cocos2d Console GUI");
    bool show_welcome = settings.value("show_welcome_dialog", true).toBool();
    QObject::connect(&app, &Cocos2dGUIApplication::fileOpenRequest, &w, &MainWindow::openFile);


#ifdef Q_OS_MAC
    // process Events before main loop in order to process the QFileOpenEvent
    app.processEvents();
#endif

    // initialize it now
    SystemState::getInstance();

    if (show_welcome && !app.triggeredByFileOpenEvent())
    {
        WelcomeDialog dialog(&w);
        dialog.exec();
    }
    w.show();


    return app.exec();
}
