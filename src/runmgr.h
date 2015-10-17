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

#pragma once

#include <QObject>
#include <QStringList>
#include <QProcess>
#include <QByteArray>
#include <QList>

class GameState;
class Run;

class RunMgr : public QObject
{
    Q_OBJECT
public:
    static RunMgr* getInstance();

    bool runAsync(Run* runCommand);
    bool runSync(Run* runCommand);
    void killAll();
    bool isBusy() const;

signals:
    void ready();

private slots:
    void onProcessFinished(Run* command);

private:
    explicit RunMgr(QObject *parent = 0);

    QList<Run*> _syncCommands;
};

class Run : public QObject
{
    Q_OBJECT
public:
    friend class RunMgr;

    explicit Run(QObject* parent = nullptr);
    virtual ~Run();

    QProcess* getProcess();
    int getExitCode() const;
    QProcess::ExitStatus getExitStatus() const;
    QString getCommandLine() const;
    const QStringList& getOutput() const;

signals:
    void dataAvailable(Run* command, const QByteArray& available);
    void finished(Run* command);

public slots:
    void onProcessFinished(int exitCode, QProcess::ExitStatus exitStatus);
    void onProcessStdOutReady();

protected:
    virtual bool run();

    QStringList _output;
    QString _cmd;
    QStringList _args;
    QString _cwd;

    int _exitCode;
    QProcess::ExitStatus _exitStatus;
    QProcess* _process;
};

class RunSDKBOXInfo : public Run
{
    Q_OBJECT
public:
    explicit RunSDKBOXInfo(GameState* gameState, QObject* parent = nullptr);
};

class RunSDKBOXSymbols : public Run
{
    Q_OBJECT
public:
    explicit RunSDKBOXSymbols(GameState* gameState, QObject* parent = nullptr);
};

class RunSDKBOXLibraries : public Run
{
    Q_OBJECT
public:
    explicit RunSDKBOXLibraries(QObject* parent = nullptr);
};

class RunCocosNew : public Run
{
    Q_OBJECT
public:
    explicit RunCocosNew(const QString& gameName, const QString& gamePath, const QString &templateName, QObject* parent);
};

class RunCocosListTemplates : public Run
{
    Q_OBJECT
public:
    explicit RunCocosListTemplates(QObject* parent);
};

class RunCocosCompile: public Run
{
    Q_OBJECT
public:
    explicit RunCocosCompile(GameState *gameState, const QString& platform, QObject* parent);
};
