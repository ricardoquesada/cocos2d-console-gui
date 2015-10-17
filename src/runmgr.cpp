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

#include "runmgr.h"

#include <QDebug>
#include <QDir>

#include "gamestate.h"
#include "preferencesdialog.h"

RunMgr* RunMgr::getInstance()
{
    static RunMgr* _instance = nullptr;
    if (!_instance)
        _instance = new RunMgr;
    return _instance;
}

RunMgr::RunMgr(QObject *parent)
    : QObject(parent)
    , _syncCommands()
{
}

//
// public
//
void RunMgr::killAll()
{
    if (isBusy())
    {
        auto front = _syncCommands.first();
        front->getProcess()->terminate();

        Run* command;
        foreach (command, _syncCommands)
        {
            delete command;
        }
        _syncCommands.clear();
    }
}

bool RunMgr::isBusy() const
{
    return (!_syncCommands.isEmpty());
}

bool RunMgr::runAsync(Run* runCommand)
{
    runCommand->run();
    return true;
}

bool RunMgr::runSync(Run* runCommand)
{
    if (_syncCommands.isEmpty())
        runCommand->run();

    _syncCommands.append(runCommand);
    connect(runCommand, &Run::finished, this, &RunMgr::onProcessFinished);

    return true;
}

//
// Slots
//
void RunMgr::onProcessFinished(Run* cmd)
{
    Q_UNUSED(cmd);

    auto command = _syncCommands.at(0);
    Q_ASSERT(command == cmd);

    disconnect(command, &Run::finished, this, &RunMgr::onProcessFinished);

    _syncCommands.removeFirst();
    if (_syncCommands.length() == 1)
    {
        _syncCommands.first()->run();
    }

    if (_syncCommands.isEmpty())
        emit ready();
}


//
// class Run
//
Run::Run(QObject *parent)
    : QObject(parent)
    , _process(nullptr)
{
}

Run::~Run()
{
    if (_process)
        delete _process;
}

QProcess* Run::getProcess()
{
    return _process;
}

QString Run::getCommandLine() const
{
    return _cmd + " " + _args.join(" ");
}

const QStringList& Run::getOutput() const
{
    return _output;
}

bool Run::run()
{
    _process = new QProcess(this);
    QProcessEnvironment env = QProcessEnvironment::systemEnvironment();
    // needed for gettext apps, like cocos and sdkbox
    env.insert("LANG", QLocale::system().name());
    _process->setProcessEnvironment(env);

    _process->setProcessChannelMode(QProcess::MergedChannels);
    _process->setWorkingDirectory(_cwd);

    _process->start(_cmd, _args);

    connect(_process, &QProcess::readyReadStandardOutput, this, &Run::onProcessStdOutReady);
    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onProcessFinished(int,QProcess::ExitStatus)));

    return true;
}

int Run::getExitCode() const
{
    return _exitCode;
}

QProcess::ExitStatus Run::getExitStatus() const
{
    return _exitStatus;
}

void Run::onProcessStdOutReady()
{
    auto available = _process->read(_process->bytesAvailable());
    _output.append(available);

    emit dataAvailable(this, available);
}

void Run::onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus)
{
    _exitCode = exitCode;
    _exitStatus = exitStatus;

    emit finished(this);

    deleteLater();
}

//
// class  RunSDKBOXInfo
//
RunSDKBOXInfo::RunSDKBOXInfo(GameState* gameState, QObject* parent)
    : Run(parent)
{
    Q_ASSERT(gameState);

    _args << "--noupdate" << "--jsonapi" << "info";
    _cmd = PreferencesDialog::findSDKBOXPath() + "/sdkbox";
    _cwd = gameState->getPath();
}

//
// class  RunSDKBOXInfo
//
RunSDKBOXSymbols::RunSDKBOXSymbols(GameState* gameState, QObject* parent)
    : Run(parent)
{
    Q_ASSERT(gameState);

    _args << "--noupdate" << "--jsonapi" << "symbols";
    _cmd = PreferencesDialog::findSDKBOXPath() + "/sdkbox";
    _cwd = gameState->getPath();
}

//
// class  RunSDKBOXLibraries
//
RunSDKBOXLibraries::RunSDKBOXLibraries(QObject* parent)
    : Run(parent)
{
    _args << "--noupdate" << "--jsonapi" << "list";
    _cmd = PreferencesDialog::findSDKBOXPath() + "/sdkbox";
    _cwd = QDir::homePath();
}

//
// class  RunCocosNew
//
RunCocosNew::RunCocosNew(const QString& gameName, const QString& gamePath, const QString& templateName, QObject* parent)
    : Run(parent)
{
    _args << "new" << gameName << "-k" << templateName;
    _cmd = PreferencesDialog::findCocosPath() + "/cocos";
    _cwd = gamePath;
}

//
// class  RunCocosListTemplates
//
RunCocosListTemplates::RunCocosListTemplates(QObject* parent)
    : Run(parent)
{
    _args << "new" << "--list-templates";
    _cmd = PreferencesDialog::findCocosPath() + "/cocos";
    _cwd = QDir::homePath();
}

//
// RunCocosCompile
//
RunCocosCompile::RunCocosCompile(GameState* gameState, const QString& platform, QObject* parent)
    : Run(parent)
{
    _args << "compile" << "-p" << platform;
    _cmd = PreferencesDialog::findCocosPath() + "/cocos";
    _cwd = gameState->getPath();

}
