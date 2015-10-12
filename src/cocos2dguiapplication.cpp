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

#include "cocos2dguiapplication.h"

#include <QFileOpenEvent>
#include <QDebug>

Cocos2dGUIApplication::Cocos2dGUIApplication(int &argc, char **argv)
    : QApplication(argc, argv)
    , _triggeredByFileOpenEvent(false)
{
}

bool Cocos2dGUIApplication::triggeredByFileOpenEvent() const
{
    return _triggeredByFileOpenEvent;
}

bool Cocos2dGUIApplication::event(QEvent *event)
{
        switch (event->type())
        {
        case QEvent::FileOpen:
            _triggeredByFileOpenEvent = true;
            emit fileOpenRequest(static_cast<QFileOpenEvent*>(event)->file());
            return true;
        default:
            return QApplication::event(event);
    }
}
