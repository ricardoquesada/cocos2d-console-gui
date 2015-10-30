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
#include <QFile>
#include <QFileInfo>
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
#include <QStandardPaths>

#include "aboutdialog.h"
#include "preferencesdialog.h"
#include "welcomedialog.h"
#include "librariesdialog.h"
#include "newgamedialog.h"
#include "templateentry.h"
#include "gamestate.h"
#include "runmgr.h"
#include "highlighter.h"
#include "systemstate.h"


constexpr int MainWindow::MAX_RECENT_FILES;

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , _settings("org.cocos2d-x","Cocos2d Console GUI")
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

    QString defaultDir = QStandardPaths::standardLocations(QStandardPaths::DocumentsLocation).at(0);
    _lastDir = _settings.value("dir/lastProjectdDir", defaultDir).toString();

    ui->plainTextEdit->setReadOnly(true);
    ui->plainTextEdit->appendPlainText(tr("Cocos2d Console GUI v") + GIT_VERSION);
    auto cocosPath = PreferencesDialog::getCocosPath();
    if (cocosPath.length() == 0)
        ui->plainTextEdit->appendPlainText(tr("Error: 'cocos' not found. Open Preferences"));


    auto sdkboxPath = PreferencesDialog::getSDKBOXPath();
    if (sdkboxPath.length() == 0)
        ui->plainTextEdit->appendPlainText(tr("Error: 'sdkbox' not found. Open Preferences"));

    readSettings();

    connect(SystemState::getInstance(), &SystemState::systemTemplatesUpdated, [&](){
        ui->actionNew_Game->setEnabled(true);
    }
    );
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
        { "COCOS_PROJECT_TYPE", "Language" }
    };
    const int MAX_VALUES = sizeof(values) / sizeof(values[0]);

    enum flags {
        MAC = 1,
        WIN = 2,
    };

    struct _buttons {
        QString key;
        QString description;
        int flags;
        std::function<void()> fn;
    } buttons[] = {
        { "XCODE_PROJECT", "Xcode Project", MAC, std::bind(&MainWindow::onOpenXcode, this) },
        { "ANDROID_STUDIO_PROJECT_DIR", "Android Studio Project", MAC | WIN, std::bind(&MainWindow::onOpenAndroidStudio, this) },
        { "VISUAL_STUDIO_WIN32_PROJECT", "Visual Studio Win32", WIN, std::bind(&MainWindow::onOpenVSWin32, this) },
        { "VISUAL_STUDIO_UNIVERSAL_PROJECT", "Visual Studio Universal", WIN, std::bind(&MainWindow::onOpenVSUniversal, this) },
    };
    const int MAX_BUTTON = sizeof(buttons) / sizeof(buttons[0]);


    // upper properties
    for (int i=0; i<MAX_VALUES; i++)
    {
        int row = ui->tableWidget_gameProperties->rowCount();
        ui->tableWidget_gameProperties->insertRow(row);

        auto name = new QTableWidgetItem(values[i].description);
        name->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget_gameProperties->setItem(row, 0, name);

        auto value = new QTableWidgetItem(properties[values[i].key].toString());
        value->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget_gameProperties->setItem(row, 1, value);
    }

    // separator
    int row = ui->tableWidget_gameProperties->rowCount();
    ui->tableWidget_gameProperties->insertRow(row);
    ui->tableWidget_gameProperties->verticalHeader()->resizeSection(row, 10);

    for (int col = 0; col < ui->tableWidget_gameProperties->columnCount(); col++)
    {
        ui->tableWidget_gameProperties->setItem(row, col, new QTableWidgetItem());
        ui->tableWidget_gameProperties->item(row, col)->setBackground(QWidget::palette().color(QWidget::backgroundRole()));
    }


    // buttons
    for (int i=0; i<MAX_BUTTON; i++)
    {
        int row = ui->tableWidget_gameProperties->rowCount();
        ui->tableWidget_gameProperties->insertRow(row);

        auto button = new QPushButton(buttons[i].description, this);
        connect(button, &QPushButton::clicked, buttons[i].fn);
        ui->tableWidget_gameProperties->setCellWidget(row, 0, button);
#if defined(Q_OS_OSX)
        button->setEnabled(buttons[i].flags & MAC);
#elif defined(Q_OS_WIN32)
        button->setEnabled(buttons[i].flags & WIN);
#endif

        auto value = new QTableWidgetItem(properties[buttons[i].key].toString());
        value->setFlags(Qt::ItemIsSelectable | Qt::ItemIsEnabled);
        ui->tableWidget_gameProperties->setItem(row, 1, value);
    }

    ui->tableWidget_gameProperties->resizeColumnsToContents();

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
        ui->plainTextEdit->appendPlainText(QString("Error: Could not open file'%1'").arg(filePath));
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
    if (maybeRunProcess() && maybeSave())
    {
        NewGameDialog dialog(this);
        dialog.exec();
    }
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
    if (maybeRunProcess() && maybeSave())
    {
        QString filter = tr("Cocos2d Project");
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
            _lastDir = QFileInfo(fn).canonicalPath();
            _settings.setValue("dir/lastProjectdDir", _lastDir);
            openFile(fn);
        }
    }
}

void MainWindow::on_actionImport_Game_triggered()
{
    if (maybeRunProcess() && maybeSave())
    {
        auto fn = QFileDialog::getExistingDirectory(this,
                                               tr("Select Game Root Directory"),
                                               _lastDir
                                               );

        if (fn.length()> 0) {

            QFileInfo cocosProjPath(fn + "/.cocos-project.json");
            if (cocosProjPath.exists())
            {
                _lastDir = fn;
                _settings.setValue("dir/lastProjectdDir", _lastDir);

                // rename the file:
                auto newName = fn + "/" + QFileInfo(fn).fileName() + ".cocosproj";
                if (QFile::rename(cocosProjPath.canonicalFilePath(), newName))
                {
                    // create symlink
                    QFile::link(newName, cocosProjPath.canonicalFilePath());

                    ui->plainTextEdit->appendHtml(QString("<font color='blue'>Renamed file: %1 -> %2</font>")
                                                  .arg(QFileInfo(cocosProjPath).fileName())
                                                  .arg(QFileInfo(newName).fileName()));

                    openFile(newName);
                }
                else
                {
                    QMessageBox::warning(this, tr("Application"),
                                         tr("Error import file"),
                                         QMessageBox::Ok);
                }
            }
            else
            {
                QMessageBox::warning(this, tr("Application"),
                                     tr("Invalid directory. File '.cocos-proj.json' not found'"),
                                     QMessageBox::Ok);
            }
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

        connect(run, &RunCocosRun::dataAvailable, this, &MainWindow::onProcessDataAvailable);
        connect(run, &RunCocosRun::finished, this, &MainWindow::onProcessFinished);

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

void MainWindow::onOpenXcode()
{
    Q_ASSERT(_gameState);
    auto pbxfile = _gameState->getGameProperties()["XCODE_PROJECT"].toString();
    QDesktopServices::openUrl(QUrl("file://" + QFileInfo(pbxfile).canonicalPath()));
}

void MainWindow::onOpenVSUniversal()
{
#if 0
    auto projectfile = _gameState->getGameProperties()["VISUAL_STUDIO_UNIVERSAL_PROJECT"].toString();
    QDesktopServices::openUrl(QUrl("file://" + QFileInfo(projectfile).canonicalPath()));
#else
    auto exe = "cmd.exe";
    auto cwd = _gameState->getPath();
    QStringList args;
    args << "/c" << "start" << _gameState->getGameProperties()["VISUAL_STUDIO_UNIVERSAL_PROJECT"].toString();
    auto cmd = new RunGeneric(exe, args, cwd, this);
    RunMgr::getInstance()->runAsync(cmd);
#endif
}

void MainWindow::onOpenVSWin32()
{
#if 0
    auto projectfile = _gameState->getGameProperties()["VISUAL_STUDIO_UNIVERSAL_PROJECT"].toString();
    QDesktopServices::openUrl(QUrl("file://" + QFileInfo(projectfile).canonicalPath()));
#else
    auto exe = "cmd.exe";
    auto cwd = _gameState->getPath();
    QStringList args;
    args << "/c" << "start" << _gameState->getGameProperties()["VISUAL_STUDIO_WIN32_PROJECT"].toString();
    auto cmd = new RunGeneric(exe, args, cwd, this);
    RunMgr::getInstance()->runAsync(cmd);
#endif
}

void MainWindow::onOpenAndroidStudio()
{
    Q_ASSERT(_gameState);
    auto androidStudioDir = _gameState->getGameProperties()["ANDROID_STUDIO_PROJECT_DIR"].toString();

#if defined(Q_OS_OSX)
    auto exe = "open";
    auto cwd = _gameState->getPath();
    QStringList args;
    args << "-a" << "Android Studio" << androidStudioDir;
#elif defined(Q_OS_WIN32)
    auto exe = "studio64.exe";
    auto cwd = _gameState->getPath();
    QStringList args;
    args << androidStudioDir;
#else
    auto exe = "";
    auto cwd = _gameState->getPath();
    QStringList args;
    args << "-a" << "Android Studio" << androidStudioDir;
#endif
    auto cmd = new RunGeneric(exe, args, cwd, this);
    RunMgr::getInstance()->runAsync(cmd);
}

void MainWindow::on_actionOpen_File_Browser_triggered()
{
    Q_ASSERT(_gameState);

#if defined(Q_OS_WIN32)
    auto exe = "cmd.exe";
    auto cwd = _gameState->getPath();
    QStringList args;
    args << "/c" << "start" << ".";
    auto cmd = new RunGeneric(exe, args, cwd, this);
    RunMgr::getInstance()->runAsync(cmd);
#else
    QFileInfo fileInfo(_gameState->getFilePath());
    QDesktopServices::openUrl(QUrl("file://" + fileInfo.canonicalPath()));
#endif
}

void MainWindow::on_actionClose_Game_triggered()
{
    if (maybeRunProcess() && maybeSave())
    {
        setGameState(nullptr);
        setupModels();
    }
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

void MainWindow::on_pushButton_addLibrary_clicked()
{
    LibrariesDialog dialog(_gameState, this);
    if (dialog.exec())
    {
        auto selected = dialog.getSelectedString();
        if (selected.length() > 0) {
            auto runMgr = RunMgr::getInstance();

            auto cmdInstall = new RunSDKBOXImport(_gameState, selected, this);
            connect(cmdInstall, &RunSDKBOXImport::dataAvailable, this, &MainWindow::onProcessDataAvailable);

            auto cmdInfo = new RunSDKBOXInfo(_gameState, this);
            connect(cmdInfo, &RunSDKBOXInfo::finished, [&](Run* command)
            {
                QString json = command->getOutput().join("");
                _gameState->parseGameLibraries(json);
                ui->plainTextEdit->appendPlainText("Done parsing game libraries.");

                updateActions();
            });
            runMgr->runSync(cmdInstall);
            runMgr->runSync(cmdInfo);

            updateActions();
        }
    }
}

void MainWindow::on_pushButton_clearConsole_clicked()
{
    ui->plainTextEdit->clear();
}

void MainWindow::on_actionExit_triggered()
{
    if (maybeRunProcess() && maybeSave())
    {
        saveSettings();
        QApplication::exit();
    }
}

//
// Overrides
//
void MainWindow::closeEvent(QCloseEvent *event)
{
    if (maybeRunProcess() && maybeSave()) {
        event->accept();

        saveSettings();
        QMainWindow::closeEvent(event);
    } else {
        event->ignore();
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

    ui->actionNew_Game->setEnabled(SystemState::getInstance()->systemLibrariesParsed());
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
    return (fileinfo.isFile() && fileinfo.exists());
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
    ui->tableWidget_gameProperties->setColumnCount(2);
    QStringList labels;
    labels << tr("Description") << tr("Value");
    ui->tableWidget_gameProperties->setHorizontalHeaderLabels(labels);
//    ui->tableWidget_gameProperties->horizontalHeader()->setSectionResizeMode(0, QHeaderView::Interactive);
    ui->tableWidget_gameProperties->horizontalHeader()->setSectionResizeMode(1, QHeaderView::Stretch);
    ui->tableWidget_gameProperties->verticalHeader()->hide();
    ui->tableWidget_gameProperties->setShowGrid(false);


    //
    auto model2 = new QStringListModel(this);
    ui->listView_libraries->setModel(model2);
}

void MainWindow::readSettings()
{
    auto geom = _settings.value("MainWindow/geometry").toByteArray();
    auto state = _settings.value("MainWindow/windowState").toByteArray();

    restoreState(state);
    restoreGeometry(geom);
}

void MainWindow::saveSettings()
{
    _settings.setValue("MainWindow/geometry", saveGeometry());
    _settings.setValue("MainWindow/windowState", saveState());
}
