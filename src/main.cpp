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

#include "welcomedialog.h"
#include "cocos2dguiapplication.h"

int main(int argc, char *argv[])
{
    Cocos2dGUIApplication app(argc, argv);

    app.setOrganizationDomain(QLatin1String("cocos2d-x.org"));
    app.setApplicationVersion(QLatin1String(APP_VERSION));
    app.setApplicationName(QLatin1String("Cocos2d Console GUI"));
    app.setApplicationDisplayName(QLatin1String("Cocos2d Console GUI"));


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

    if (show_welcome && !app.triggeredByFileOpenEvent())
    {
        WelcomeDialog dialog(&w);
        dialog.exec();
    }
    w.show();


    return app.exec();
}
