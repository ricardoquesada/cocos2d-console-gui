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
#include "ui_mainwindow.h"

#include <QFileDialog>
#include <QDebug>
#include <QSettings>
#include <QListWidgetItem>
#include <QDesktopServices>
#include <QFileInfo>
#include <QMessageBox>
#include <QStringList>
#include <QStandardItemModel>
#include <QStandardItem>
#include <QStringListModel>
#include <QProgressBar>
#include <QToolBar>
#include <QComboBox>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonValue>

#include "aboutdialog.h"
#include "preferencesdialog.h"
#include "welcomedialog.h"
#include "librariesdialog.h"
#include "newgamedialog.h"

#include "templateentry.h"
#include "gamestate.h"
#include "runmgr.h"
#include "highlighter.h"


constexpr int MainWindow::MAX_RECENT_FILES;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _settings("org.cocos2d-x","Cocos2d Console GUI")
    , _lastDir(QDir::homePath())
    , _gameState(nullptr)
    , _comboBoxMode(nullptr)
    , _comboBoxPlatforms(nullptr)
{
    setUnifiedTitleAndToolBarOnMac(true);

    ui->setupUi(this);
    setupStatusBar();
    createActions();
    setGameState(nullptr);

    setupModels();

    // other setups
    _highlighter = new Highlighter(ui->plainTextEdit->document());

    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->appendPlainText(QString("Cocos2d Console GUI v") + APP_VERSION);
    auto cocosPath = PreferencesDialog::findCocosPath();
    if (cocosPath.length() == 0)
        ui->plainTextEdit->appendPlainText(QString("Error: 'cocos' not found. Open Preferences"));


    auto sdkboxPath = PreferencesDialog::findSDKBOXPath();
    if (sdkboxPath.length() == 0)
        ui->plainTextEdit->appendPlainText(QString("Error: 'sdkbox' not found. Open Preferences"));
}

MainWindow::~MainWindow()
{
    delete ui;
    if (_gameState)
        delete _gameState;
}

//
// Manual Slots
//
void MainWindow::gameUpdateLibraries()
{
    auto keys = _gameState->getGameLibraries().keys();
    auto model = dynamic_cast<QStringListModel*>(ui->listView_libraries->model());
    model->setStringList(keys);

    if (_gameState->isReady())
        statusBar()->showMessage("Ready", 3000);
}

void MainWindow::gameUpdateProperties()
{
    Q_ASSERT(_gameState);

    auto properties = _gameState->getGameProperties();

    struct _values {
        QString key;
        QString description;
    } values[] = {
        { "COCOS_2DX_VERSION", "Cocos2d-x Version" },
        { "COCOS_PROJECT_TYPE", "Language" },
        { "XCODE_PROJECT", "Xcode Project File" },
        { "ANDROID_STUDIO_PROJECT_DIR", "Android Studio Project Path" },
    };
    const int MAX_VALUES = sizeof(values) / sizeof(values[0]);

    for (int i=0; i<MAX_VALUES; i++)
    {
        auto name = new QStandardItem(values[i].description);
        auto value = new QStandardItem(properties[values[i].key].toString());
        name->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        value->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);

        auto model = dynamic_cast<QStandardItemModel*>(ui->tableView_gameProperties->model());
        model->setItem(i, 0, name);
        model->setItem(i, 1, value);
    }

    if (_gameState->isReady())
        statusBar()->showMessage("Ready", 3000);
}

void MainWindow::gameUpdatePlatforms()
{
    auto platforms = _gameState->getGamePlatforms();
    foreach (const QJsonValue& value , platforms) {
        auto str = value.toString();
        _comboBoxPlatforms->addItem(str);
    }

    if (_gameState->isReady())
        statusBar()->showMessage("Ready", 3000);
}

void MainWindow::displayCommand(const QString& commandLine)
{
    ui->plainTextEdit->appendPlainText("Running: " + commandLine);
}


void MainWindow::openFile(const QString& filePath)
{
    if (filePath.length() > 0 && validatePath(filePath))
    {
        setRecentFile(filePath);
        auto gameState = new GameState(filePath);
        setGameState(gameState);
    }
    else
    {
        qDebug() << "Invalid path: " << filePath;
    }
}

void MainWindow::onProcessFinished(Run* command)
{
    if (command->getExitStatus() == QProcess::NormalExit)
        ui->plainTextEdit->appendPlainText("Process finished with exit code:" + QString::number(command->getExitCode()));
    else
        ui->plainTextEdit->appendPlainText("Process stopped with exit code: " + QString::number(command->getExitCode()));

    updateActions();
}

void MainWindow::onProcessDataAvailable(Run* command, const QString& data)
{
    Q_UNUSED(command);
    ui->plainTextEdit->appendPlainText(data);
}

//
// QtCreator Slots
//
void MainWindow::openRecentFile_triggered()
{
    QAction *action = qobject_cast<QAction *>(sender());

    if (action) {
        auto path = action->data().toString();
        if (path.length() > 0)
        {
            auto gameState = new GameState(path);
            setGameState(gameState);
        }
        else
            qDebug() << "Invalid path";
    }
}

void MainWindow::on_actionNew_Game_triggered()
{
    NewGameDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionPreferences_triggered()
{
    PreferencesDialog preferences(this);
    preferences.exec();
}

void MainWindow::on_actionAbout_triggered()
{
    AboutDialog aboutDialog(this);
    aboutDialog.exec();
}

void MainWindow::on_actionOpen_triggered()
{
    if (maybeSave())
    {
        QString filter = _settings.value("dir/lastUsedOpenFilter", "Cocos2d Project").toString();
        auto fn = QFileDialog::getOpenFileName(this,
                                               tr("Select File"),
                                               _lastDir,
                                               tr(
                                                   "All files (*);;" \
                                                   "Cocos2d Project (*.cocosproj);;"
                                               ),
                                               &filter
                                               /*,QFileDialog::DontUseNativeDialog*/
                                               );

        if (fn.length()> 0) {
            _settings.setValue("dir/lastUsedOpenFilter", filter);
            openFile(fn);
        }
    }
}

void MainWindow::on_actionClear_Recent_Games_triggered()
{
    _settings.setValue(QLatin1String("recentFiles/fileNames"), QStringList());
    updateRecentFiles();
}

void MainWindow::on_actionWelcome_triggered()
{
    WelcomeDialog dialog(this);
    dialog.exec();
}

void MainWindow::on_actionRun_triggered()
{
    Q_ASSERT(_gameState);

    if (maybeRunProcess())
    {
        auto platform = _comboBoxPlatforms->itemText(_comboBoxPlatforms->currentIndex());
        auto mode = _comboBoxMode->itemText(_comboBoxMode->currentIndex());
        auto run = new RunCocosRun(_gameState, platform, mode, this);
        RunMgr::getInstance()->runSync(run);

        connect(run, &RunCocosCompile::dataAvailable, this, &MainWindow::onProcessDataAvailable);
        connect(run, &RunCocosCompile::finished, this, &MainWindow::onProcessFinished);

        updateActions();
    }
}

bool MainWindow::on_actionStop_triggered()
{
    if (RunMgr::getInstance()->isBusy())
    {
        ui->plainTextEdit->appendPlainText("Stoping process...");
        RunMgr::getInstance()->killAll();
    }

    return true;
}

void MainWindow::on_actionClean_triggered()
{
}

void MainWindow::on_actionOpen_Xcode_triggered()
{
    Q_ASSERT(_gameState);

    QFileInfo fileInfo(_gameState->getFilePath());
    QString xcodefile = fileInfo.canonicalPath() + "/proj.ios_mac/" + _gameState->getProjectName() + ".xcodeproj";
    QDesktopServices::openUrl(QUrl("file://" + xcodefile));
}

void MainWindow::on_actionOpen_in_Visual_Studio_triggered()
{

}

void MainWindow::on_actionOpen_in_Android_Studio_triggered()
{

}

void MainWindow::on_actionOpen_File_Browser_triggered()
{
    Q_ASSERT(_gameState);

    QFileInfo fileInfo(_gameState->getFilePath());
    QDesktopServices::openUrl(QUrl("file://" + fileInfo.canonicalPath()));
}

void MainWindow::on_actionClose_Game_triggered()
{
    setGameState(nullptr);
    setupModels();
}

void MainWindow::on_actionBuild_triggered()
{
    Q_ASSERT(_gameState);

    if (maybeRunProcess())
    {
        auto platform = _comboBoxPlatforms->itemText(_comboBoxPlatforms->currentIndex());
        auto mode = _comboBoxMode->itemText(_comboBoxMode->currentIndex());
        auto run = new RunCocosCompile(_gameState, platform, mode, this);
        RunMgr::getInstance()->runSync(run);

        connect(run, &RunCocosCompile::dataAvailable, this, &MainWindow::onProcessDataAvailable);
        connect(run, &RunCocosCompile::finished, this, &MainWindow::onProcessFinished);

        updateActions();
    }
}

//
// Public Methods
//
void MainWindow::setGameState(GameState* gameState)
{
    if (_gameState)
        closeGameState();

    if (gameState)
    {
        _gameState = gameState;
        setWindowFilePath(gameState->getFilePath());
        setRecentFile(gameState->getFilePath());

        connect(_gameState, &GameState::gameLibrariesUpdated, this, &MainWindow::gameUpdateLibraries);
        connect(_gameState, &GameState::gamePropertiesUpdated, this, &MainWindow::gameUpdateProperties);
        connect(_gameState, &GameState::gamePlatformsUpdated, this, &MainWindow::gameUpdatePlatforms);

        updateActions();
        ui->plainTextEdit->appendPlainText("Parsing " + gameState->getFilePath() + "...");
        statusBar()->showMessage("Parsing " + gameState->getFilePath());

        auto runMgr = RunMgr::getInstance();

        auto cmdInfo = new RunSDKBOXInfo(gameState, this);
        auto cmdSymbols = new RunSDKBOXSymbols(gameState, this);
        auto cmdGamePlatforms = new RunCocosListPlatforms(gameState, this);

        connect(cmdInfo, &RunSDKBOXInfo::finished, [&](Run* command)
        {
            QString json = command->getOutput().join("");
            _gameState->parseGameLibraries(json);
            ui->plainTextEdit->appendPlainText("Done parsing game libraries.");
            updateActions();
        });

        connect(cmdSymbols, &RunSDKBOXSymbols::finished, [&](Run* command)
        {
            QString json = command->getOutput().join("");
            _gameState->parseGameProperties(json);
            ui->plainTextEdit->appendPlainText("Done parsing game properties.");
            updateActions();
        });

        connect(cmdGamePlatforms, &RunCocosListPlatforms::finished, [&](Run* command)
        {
            QString json = command->getOutput().join("");
            _gameState->parseGamePlatforms(json);
            ui->plainTextEdit->appendPlainText("Done parsing game platforms.");
            updateActions();
        });

        runMgr->runSync(cmdInfo);
        runMgr->runSync(cmdSymbols);
        runMgr->runSync(cmdGamePlatforms);
    }
    updateActions();
}

GameState* MainWindow::getGameState() const
{
    return _gameState;
}

//
// Helpers
//
void MainWindow::updateActions()
{
    QAction* actions[] = {
        ui->actionClose_Game,
        ui->actionOpen_File_Browser,
        ui->actionOpen_in_Android_Studio,
#ifdef Q_OS_OSX
        ui->actionOpen_Xcode,
#elif defined(Q_OS_WIN)
        ui->actionOpen_in_Visual_Studio,
#endif
        ui->actionRun,
        ui->actionStop,
        ui->actionBuild,
        ui->actionClean,
    };
    const int ACTIONS_MAX = sizeof(actions) / sizeof(actions[0]);

    bool gameStateReady = ( _gameState && _gameState->isReady());

    for (int i=0; i<ACTIONS_MAX; i++)
    {
        actions[i]->setEnabled(gameStateReady);
    }
    ui->pushButton_addLibrary->setEnabled(gameStateReady);
    _comboBoxMode->setEnabled(gameStateReady);
    _comboBoxPlatforms->setEnabled(gameStateReady);

    // progress bar
    bool processRunning = RunMgr::getInstance()->isBusy();
    int min_max = processRunning ? 0 : 1;
    _progressBar->setMinimum(min_max);
    _progressBar->setMaximum(min_max);

    if (gameStateReady)
    {
        ui->actionRun->setEnabled(!processRunning);
        ui->actionBuild->setEnabled(!processRunning);
        ui->actionRun->setEnabled(!processRunning);

        ui->actionStop->setEnabled(processRunning);

        ui->pushButton_addLibrary->setEnabled(!processRunning);
    }
}

void MainWindow::closeGameState()
{
    if (!_gameState)
        return;

    disconnect(_gameState, &GameState::gameLibrariesUpdated, this, &MainWindow::gameUpdateLibraries);
    disconnect(_gameState, &GameState::gamePropertiesUpdated, this, &MainWindow::gameUpdateProperties);
    disconnect(_gameState, &GameState::gamePlatformsUpdated, this, &MainWindow::gameUpdatePlatforms);

    delete _gameState;
    _gameState = nullptr;

    // remove all before adding new, to avoid duplicates
    _comboBoxPlatforms->clear();

    setWindowFilePath("[untitled]");
}

void MainWindow::createActions()
{
    setWindowFilePath("[untitled]");

    // Add recent file actions to the recent files menu and widgetList
    for (int i=0; i < MAX_RECENT_FILES; ++i)
    {
        _recentFilesAction[i] = new QAction(this);
        ui->menuRecentGames->insertAction(ui->actionClear_Recent_Games, _recentFilesAction[i]);
        _recentFilesAction[i]->setVisible(false);
        connect(_recentFilesAction[i], &QAction::triggered, this, &MainWindow::openRecentFile_triggered);
    }
    ui->menuRecentGames->insertSeparator(ui->actionClear_Recent_Games);
    updateRecentFiles();

#ifdef Q_OS_OSX
    ui->actionOpen_in_Visual_Studio->setEnabled(false);
#elif defined (Q_OS_WIN)
    ui->actionOpen_Xcode->setEnabled(false);
#endif

    connect(RunMgr::getInstance(), &RunMgr::isReady, this, &MainWindow::updateActions);
    connect(RunMgr::getInstance(), &RunMgr::commandRun, this, &MainWindow::displayCommand);

    // toolbar actions
    auto toolbars = findChildren<QToolBar *>();
    if (toolbars.count() == 1)
    {
        auto toolbar = toolbars.at(0);

        // debug / release
        _comboBoxMode = new QComboBox;
        _comboBoxMode->addItem(tr("Debug"));
        _comboBoxMode->addItem(tr("Release"));
        toolbar->insertWidget(ui->actionBuild, _comboBoxMode);

        // supported platforms. Empty by default. Will be filled once it is parsed
        _comboBoxPlatforms = new QComboBox;
        toolbar->insertWidget(ui->actionBuild, _comboBoxPlatforms);
    }
}

void MainWindow::setupStatusBar()
{
    _progressBar = new QProgressBar(this);
    ui->statusBar->addPermanentWidget(_progressBar);

    _progressBar->setMinimum(0);
    _progressBar->setMaximum(0);
}

QStringList MainWindow::recentFiles() const
{
    QVariant v = _settings.value(QLatin1String("recentFiles/fileNames"));
    return v.toStringList();
}

void MainWindow::updateRecentFiles()
{
    QStringList files = recentFiles();
    const int numRecentFiles = qMin(files.size(), MAX_RECENT_FILES);

    for (int i = 0; i < numRecentFiles; ++i)
    {
        _recentFilesAction[i]->setText(QFileInfo(files[i]).fileName());
        _recentFilesAction[i]->setData(files[i]);
        _recentFilesAction[i]->setVisible(true);
    }
    for (int j=numRecentFiles; j<MAX_RECENT_FILES; ++j)
    {
        _recentFilesAction[j]->setVisible(false);
    }
    ui->menuRecentGames->setEnabled(numRecentFiles > 0);
}

void MainWindow::setRecentFile(const QString& fileName)
{
    // Remember the file by its canonical file path
    const QString canonicalFilePath = QFileInfo(fileName).canonicalFilePath();

    if (canonicalFilePath.isEmpty())
        return;

    QStringList files = recentFiles();
    files.removeAll(canonicalFilePath);
    files.prepend(canonicalFilePath);
    while (files.size() > MAX_RECENT_FILES)
        files.removeLast();

    _settings.setValue(QLatin1String("recentFiles/fileNames"), files);
    updateRecentFiles();
}

bool MainWindow::validatePath(const QString &filePath) const
{
    // FIXME: must validate that the cocosproj is valid...
    QFileInfo fileinfo(filePath);
    return fileinfo.isFile();
}

bool MainWindow::maybeSave()
{
    return true;
}

bool MainWindow::maybeRunProcess()
{
    if (RunMgr::getInstance()->isBusy())
    {
        QMessageBox::StandardButton ret;
        ret = QMessageBox::warning(this, tr("Application"),
                     tr("There is another process running.\n"
                        "Stop current process?"),
                     QMessageBox::Ok | QMessageBox::Cancel);
        if (ret == QMessageBox::Ok)
            return on_actionStop_triggered();
        else if (ret == QMessageBox::Cancel)
            return false;
    }
    return true;
}



void MainWindow::setupModels()
{
    // Game Properties

    // 2 Rows and 2 Columns
    auto model = new QStandardItemModel(0, 2, this);
    model->setHorizontalHeaderItem(0, new QStandardItem("Description"));
    model->setHorizontalHeaderItem(1, new QStandardItem("Value"));
    ui->tableView_gameProperties->setModel(model);

    ui->tableView_gameProperties->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Stretch);
    ui->tableView_gameProperties->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);

    //
    auto model2 = new QStringListModel(this);
    ui->listView_libraries->setModel(model2);
}

void MainWindow::on_pushButton_addLibrary_clicked()
{
    LibrariesDialog dialog(_gameState, this);
    if (dialog.exec())
    {
        auto selected = dialog.getSelectedString();
        if (selected.length() > 0) {
            auto runMgr = RunMgr::getInstance();
            auto cmdInfo = new RunSDKBOXInfo(_gameState);

            connect(cmdInfo, &RunSDKBOXInfo::finished, [&](Run* command)
            {
                QString json = command->getOutput().join("");
                _gameState->parseGameLibraries(json);
                ui->plainTextEdit->appendPlainText("Done parsing game libraries.");
            });
            runMgr->runSync(cmdInfo);
        }
    }
}

void MainWindow::on_pushButton_clearConsole_clicked()
{
    ui->plainTextEdit->clear();
}
