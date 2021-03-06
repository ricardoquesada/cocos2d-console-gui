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
#include <QFileInfo>

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
        front->kill();

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
    connect(runCommand, &Run::finished, this, &RunMgr::onProcessAsyncFinished);
    connect(runCommand, &Run::error, this, &RunMgr::onProcessAsyncError);

    emit commandRun(tr("Running: ") + runCommand->getCommandLine());
    runCommand->run();
    return true;
}

bool RunMgr::runSync(Run* runCommand)
{
    connect(runCommand, &Run::finished, this, &RunMgr::onProcessFinished);
    connect(runCommand, &Run::error, this, &RunMgr::onProcessError);
    _syncCommands.append(runCommand);

    // only self?
    if (_syncCommands.length() == 1)
    {
        emit commandRun(tr("Running: ") + runCommand->getCommandLine());
        runCommand->run();
    }
    return true;
}

//
// Slots
//
void RunMgr::onProcessFinished(Run* cmd)
{
    Q_UNUSED(cmd);

    // FIXME: if not, then probably the
    // process was cancelled ?
    if ( _syncCommands.size() > 0)
    {
        auto command = _syncCommands.at(0);
        Q_ASSERT(command == cmd);

        disconnect(command, &Run::finished, this, &RunMgr::onProcessFinished);

        _syncCommands.removeFirst();
        if (_syncCommands.length() > 0)
        {
            _syncCommands.first()->run();
            emit commandRun(tr("Running: ") + _syncCommands.first()->getCommandLine());
        }

        if (_syncCommands.isEmpty())
            emit isReady();
    }
}

void RunMgr::onProcessError(Run* command)
{
    emit commandError(tr("Error: ") + command->getProcess()->errorString());

    disconnect(command, &Run::error, this, &RunMgr::onProcessError);

    _syncCommands.removeFirst();
    if (_syncCommands.length() > 0)
    {
        _syncCommands.first()->run();
        emit commandRun(_syncCommands.first()->getCommandLine());
    }

    if (_syncCommands.isEmpty())
        emit isReady();
}

void RunMgr::onProcessAsyncFinished(Run* cmd)
{
    disconnect(cmd, &Run::finished, this, &RunMgr::onProcessAsyncFinished);
}


void RunMgr::onProcessAsyncError(Run* cmd)
{
    emit commandError(tr("Error: ") + cmd->getProcess()->errorString());
    disconnect(cmd, &Run::error, this, &RunMgr::onProcessAsyncError);
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

void Run::kill()
{
    _process->kill();
    _process->terminate();
}

QProcess* Run::getProcess()
{
    return _process;
}

QString Run::getCommandLine() const
{
    return QFileInfo(_cmd).baseName() + " " + _args.join(" ");
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

    _process->setProcessChannelMode(QProcess::SeparateChannels);
    _process->setReadChannel(QProcess::StandardOutput);
    _process->setWorkingDirectory(_cwd);

    connect(_process, &QProcess::readyReadStandardOutput, this, &Run::onProcessStdOutReady);
    connect(_process, SIGNAL(finished(int,QProcess::ExitStatus)), this, SLOT(onProcessFinished(int,QProcess::ExitStatus)));
    connect(_process, SIGNAL(error(QProcess::ProcessError)), this, SLOT(onProcessError(QProcess::ProcessError)));

#if defined(Q_OS_WIN32)
    _process->start(_cmd, _args);
#elif defined(Q_OS_UNIX)
    QString cmd = "/bin/bash";
    QStringList args;
    args << _cmd << _args;
    QStringList args2;
    args2 << "-l" << "-c" << args.join(" ");
    _process->start(cmd, args2);
#endif

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

void Run::onProcessError(QProcess::ProcessError err)
{
    Q_UNUSED(err);
    emit error(this);
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
    _cmd = PreferencesDialog::getSDKBOXFilepath();
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
    _cmd = PreferencesDialog::getSDKBOXFilepath();
    _cwd = gameState->getPath();
}

//
// class  RunSDKBOXLibraries
//
RunSDKBOXLibraries::RunSDKBOXLibraries(QObject* parent)
    : Run(parent)
{
    _args << "--noupdate" << "--jsonapi" << "list";
    _cmd = PreferencesDialog::getSDKBOXFilepath();
    _cwd = QDir::homePath();
}

//
// class  RunSDKBOXImport
//
RunSDKBOXImport::RunSDKBOXImport(GameState* gameState, const QString& library, QObject* parent)
    : Run(parent)
{
    _args << "--noupdate" << "--jsonapi" << "import" << library;
    _cmd = PreferencesDialog::getSDKBOXFilepath();
    _cwd = gameState->getPath();
}

RunSDKBOXImport::RunSDKBOXImport(const QString& gamePath, const QString& library, QObject* parent)
    : Run(parent)
{
    _args << "--noupdate" << "--jsonapi" << "import" << library;
    _cmd = PreferencesDialog::getSDKBOXFilepath();
    _cwd = gamePath;
}

//
// class  RunCocosNew
//
RunCocosNew::RunCocosNew(const QString& gameName, const QString& gamePath, const QString& templateName, QObject* parent)
    : Run(parent)
{
    _args << "new" << gameName << "-k" << templateName;
    _cmd = PreferencesDialog::getCocosFilepath();
    _cwd = gamePath;
}

//
// class  RunCocosListTemplates
//
RunCocosListTemplates::RunCocosListTemplates(QObject* parent)
    : Run(parent)
{
    _args << "new" << "--list-templates";
    _cmd = PreferencesDialog::getCocosFilepath();
    _cwd = QDir::homePath();
}

//
// RunCocosCompile
//
RunCocosCompile::RunCocosCompile(GameState* gameState, const QString& platform, const QString &mode, QObject* parent)
    : Run(parent)
{
    _args << "compile" << "-p" << platform << "-m" << mode;
    _cmd = PreferencesDialog::getCocosFilepath();
    _cwd = gameState->getPath();
}


//
// RunCocosRun
//
RunCocosRun::RunCocosRun(GameState* gameState, const QString& platform, const QString &mode, QObject* parent)
    : Run(parent)
{
    _args << "run" << "-p" << platform << "-m" << mode;
    _cmd = PreferencesDialog::getCocosFilepath();
    _cwd = gameState->getPath();
}

//
// class  RunCocosListPlatforms
//
RunCocosListPlatforms::RunCocosListPlatforms(GameState* gameState, QObject* parent)
    : Run(parent)
{
    _args << "compile" << "--list-platforms";
    _cmd = PreferencesDialog::getCocosFilepath();
    _cwd = gameState->getPath();
}

//
// class  RunGeneric
//
RunGeneric::RunGeneric(const QString& exe, const QStringList& args, const QString& cwd, QObject* parent)
    : Run(parent)
{
    _args << args;
    _cmd = exe;
    _cwd = cwd;
}
