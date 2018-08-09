
#include <QGraphicsRectItem>
#include <QFileDialog>
#include <QResizeEvent>
#include <QThread>
#include <QMessageBox>
#include <QDesktopServices>
#include <QDebug>
#include <QtConcurrent>

#include "common/common.h"
#include "common/dataManager.h"
#include "common/faboolApi.h"

#include "ui/mainDialog.h"
#include "ui_mainDialog.h"
#include "ui/graphicArea/mainView.h"
#include "ui/graphicArea/maintenanceView.h"
#include "ui/graphicArea/mainScene.h"
#include "ui/widgets/paramWidget.h"
#include "ui/widgets/itemShapeWidget.h"
#include "ui/dialogs/machineSettingDialog.h"
#include "ui/dialogs/errMesDialog.h"
#include "ui/dialogs/aboutDialog.h"
#include "ui/listView/libraryListModel.h"

#include "graphics/vectorImageItem.h"

#include "command/commandManager.h"


MainDialog::MainDialog(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainDialog)
{
    mainUIInit();

    systemInit();
    graphicsAreaInit();
    maintenanceAreaInit();
    commandInit();
    optionAreaInit();
    projectMenuInit();
    deleteMenuInit();
    statusbarInit();
    serialPortInit();
    progressInit();
    libraryInit();
    materialsInit();
    connectionInit();
    projectInit();
    autoDisplayUpdateMessage(); // アップデート可能メッセージの自動表示
}

MainDialog::~MainDialog()
{
    delete ui;
}

void MainDialog::mainUIInit()
{
    ui->setupUi(this);
    qApp->setStyleSheet(styleSheet() + "QToolTip { height: 10px; color:white; background-color:steelblue; border-radius:3px; }");
    setWindowTitle(QString(APP_NAME) + " _ " + tr("Undefined"));

    ui->menuBar->hide();

    if(QSysInfo::productType().indexOf("MacOS", 0, Qt::CaseInsensitive) != -1 ||
            QSysInfo::productType().indexOf("osx", 0, Qt::CaseInsensitive) != -1  ) {
        ui->RedoButton->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_Z));
    }
    mProcessTimeCalculator = new ProcessTimeCalculator();

    INFO_STRING("MainUI Initialized.");
}

void MainDialog::systemInit()
{
    mSettingDialog = new MachineSettingDialog(this);
    connect(mSettingDialog, SIGNAL(systemSaved()), this, SLOT(sendParaChange()));
    connect(mSettingDialog, SIGNAL(systemSaved()), this, SLOT(materialsUpdate()));
}

void MainDialog::progressInit()
{
    mProgressDialog = new ProgressDialog(this);
    mProgressDialog->hide();

    connect(mProgressDialog, SIGNAL(onProcess(bool)),           this, SLOT(onClickPauseProcess(bool)));
    connect(mProgressDialog, SIGNAL(onReset()),                 this, SLOT(onClickResetProcess()));

    INFO_STRING("ProgressDialog Initialized.");
}

void MainDialog::commandInit()
{
    mDMInstance = DataManager::getInstance();
    mCmdMgr     = new CommandManager(this->mScene);

    INFO_STRING("CommandManager Initialized.");
}

void MainDialog::commandFinal()
{
    if(mCmdMgr != NULL) {
        delete mCmdMgr;
        mCmdMgr = NULL;
    }
    INFO_STRING("CommandManager Finalized.");
}

void MainDialog::optionAreaInit()
{
    mOptionMenu = new QMenu(this);

    QAction* optionAct  = new QAction(tr("Setting"), this);
    connect(optionAct, SIGNAL(triggered()), this, SLOT(onClickSettingMenu()));
    optionAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_Comma));
    mOptionMenu->addAction(optionAct);

    QAction* aboutAct = new QAction(tr("About"), this);
    connect(aboutAct, SIGNAL(triggered()), this, SLOT(onClickAbout()));
    mOptionMenu->addAction(aboutAct);

    QAction* releasenoteAct = new QAction(tr("Release Note"), this);
    connect(releasenoteAct, SIGNAL(triggered()), this, SLOT(onClickReleaseNote()));
    mOptionMenu->addAction(releasenoteAct);

    ui->OptionButton->setMenu(mOptionMenu);
}

// Graphicsを表示するエリアの初期化
void MainDialog::graphicsAreaInit()
{
    mScene = new MainScene();
    SysParam* sysPara = SysParam::getInstance();

    mScene->setMovableRange(sysPara->getWorkArea());
    mScene->createGrid();

    mView = new MainView(ui->GraphicsViewArea);
    mView->setSceneAndRange(mScene);

    mMaintenanceView = new MaintenanceView(mScene, ui->GraphicsViewArea);
    mMaintenanceView->hide();
    mLaserMoveStep = 3;
    connect(mMaintenanceView, SIGNAL(sendPos(QPointF)), this, SLOT(onMoveDirectly(QPointF)));

    mScene->installEventFilter(this);

    INFO_STRING("GraphicsArea Initialized.");
}

void MainDialog::graphicsAreaFinal()
{
    if(mView != NULL) {
        delete mView;
        mView = NULL;
    }
    if(mScene != NULL) {
        delete mScene;
        mScene = NULL;
    }
    INFO_STRING("GraphicsArea Finalized.");
}

void MainDialog::maintenanceAreaInit()
{
    mMaintenanceWidget = new MaintenanceWidget();
    mMaintenanceWidget->setObjectName("MaintenanceWidget");
    ui->ParamAndMaintenance->addWidget(mMaintenanceWidget);

    connect(mMaintenanceWidget,  SIGNAL(close()),       this,  SLOT(onConfirmPositionClose()));
    connect(mMaintenanceWidget,  SIGNAL(MoveUp()),      this,  SLOT(onMoveUp()));
    connect(mMaintenanceWidget,  SIGNAL(MoveDown()),    this,  SLOT(onMoveDown()));
    connect(mMaintenanceWidget,  SIGNAL(MoveRight()),   this,  SLOT(onMoveRight()));
    connect(mMaintenanceWidget,  SIGNAL(MoveLeft()),    this,  SLOT(onMoveLeft()));

    INFO_STRING("MaintenanceArea Initialized.");
}

void MainDialog::projectMenuInit()
{
    mSaveProjectMenu = new QMenu(this);
    QAction* saveAct  = new QAction(tr("Save"), this);
    connect(saveAct, SIGNAL(triggered()), this, SLOT(onClickSave()));
    saveAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_S));
    mSaveProjectMenu->addAction(saveAct);

    QAction* saveAsAct  = new QAction(tr("Save as"), this);
    connect(saveAsAct, SIGNAL(triggered()), this, SLOT(onClickSaveAs()));
    saveAsAct->setShortcut(QKeySequence(Qt::CTRL + Qt::SHIFT + Qt::Key_S));
    mSaveProjectMenu->addAction(saveAsAct);
    ui->ProjectSaveButton->setMenu(mSaveProjectMenu);

    mLoadProjectMenu = new QMenu(this);
    QAction* loadAct  = new QAction(tr("Load"), this);
    connect(loadAct, SIGNAL(triggered()), this, SLOT(onClickLoad()));
    loadAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_O));
    mLoadProjectMenu->addAction(loadAct);

    QAction* newProjAct  = new QAction(tr("New Project"), this);
    connect(newProjAct, SIGNAL(triggered()), this, SLOT(onClickNewProject()));
    newProjAct->setShortcut(QKeySequence(Qt::CTRL + Qt::Key_N));
    mLoadProjectMenu->addAction(newProjAct);

    QAction* importProjAct  = new QAction(tr("Import Project"), this);
    connect(importProjAct, SIGNAL(triggered()), this, SLOT(onClickImportProject()));
    mLoadProjectMenu->addAction(importProjAct);
    ui->ProjectLoadButton->setMenu(mLoadProjectMenu);

    INFO_STRING("ProjectMenu Initialized.");
}

void MainDialog::deleteMenuInit()
{
    mDeleteMenu = new QMenu(this);

    QAction* deleteAct  = new QAction(tr("Delete"), this);
    connect(deleteAct, SIGNAL(triggered()), mCmdMgr,   SLOT(deleteCommand()));
    deleteAct->setShortcut(QKeySequence(Qt::Key_Delete));
    mDeleteMenu->addAction(deleteAct);

    QAction* deleteAllAct  = new QAction(tr("Delete all"), this);
    connect(deleteAllAct, SIGNAL(triggered()), mCmdMgr,   SLOT(deleteAllCommand()));
    deleteAllAct->setShortcut(QKeySequence(Qt::SHIFT + Qt::Key_Delete));
    mDeleteMenu->addAction(deleteAllAct);

    ui->DeleteButton->setMenu(mDeleteMenu);

    INFO_STRING("ProjectMenu Initialized.");
}

void MainDialog::serialPortInit()
{
    mFaboolMachine = new FaboolMachine();
    INFO_STRING("SerialPort Initialized.");

    // For Debug....
//    mDebugPanel = new DebugPanel(this, mFaboolMachine);
//    mDebugPanel->show();
}

void MainDialog::serialPortFinal()
{
    delete(mFaboolMachine);
    mFaboolMachine = NULL;
    INFO_STRING("SerialPort Finalized.");

    // For Debug....
//    delete(mDebugPanel);
//    mDebugPanel = NULL;
}

void MainDialog::projectInit()
{
    SysParam* sysPara = SysParam::getInstance();
    QString currProject = sysPara->getCurrentProject();
    if(currProject.isEmpty() == true) {
        clearCurrentProject();
        applyProject(SUCCESS);
    } else {
        int ret = mDMInstance->loadProject(currProject);
        if(ret != SUCCESS) {
            clearCurrentProject();
        }
        applyProject(ret);
    }
}

void MainDialog::projectFinal()
{
    QString currProject = mDMInstance->projectPath();
    SysParam* sysPara = SysParam::getInstance();

    if(currProject != QString(tr("Undefined"))) {
        sysPara->setCurrentProject(currProject);
    }
}

void MainDialog::clearCurrentProject()
{
    mScene->unsetGroup();
    mScene->toggleActionGroup();

    // 今持っているデータの削除
    foreach (ItemInfo* item, mDMInstance->ImageItem()) {
        item->removeItemFromScene(mScene);
    }

    QList<ParamWidget* > widList = ui->ParamDisplayArea->findChildren<ParamWidget* >();
    foreach (ParamWidget* wid, widList) {
        delete wid;
    }

    mDMInstance->dataClear();
    mCmdMgr->stackClear();
}

void  MainDialog::applyProject(int loadRet)
{
    onUpdateParam();
    onUpdateGraphics();

    QString projectName = mDMInstance->projectName();
    if(projectName.isEmpty() == true) { projectName = tr("Undefined"); }
    setWindowTitle(QString(APP_NAME) + " _ " + projectName);

    if(loadRet == ERR_PROJ_LOAD_FILE_OPEN) {
        showErrMes(QtInfoMsg, tr("Failed to open project."));
    } else if(loadRet == ERR_PROJ_LOAD_INVAILD_DATA) {
        showErrMes(QtInfoMsg, tr("Existing invalid data."));
    } else {
        INFO_STRING("Load done.");
        wrapperEstimationTime();
    }
}

bool MainDialog::confirmChangeProject()
{
    if(mDMInstance->isSaved() == false) {
        int putButton = QMessageBox::question(this, tr("confirmation"),
                                              tr("This project is modified. \nDo you want to save changes?"),
                                              QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel);
        if(putButton == QMessageBox::Cancel) {
            return false;
        } else if (putButton == QMessageBox::Yes) {
            if(onClickSave() != SUCCESS) {
                return false;
            }
        }
    }
    return true;
}

void MainDialog::statusbarInit()
{
    mStatus       = new QWidget(this);
    mPortStatus   = new QStackedWidget(this);
    mSafetyStatus = new QWidget(this);

    mConnectedStatus = new QLabel(tr("Connected."));
    mConnectedStatus->setStyleSheet("background-color:green; color:white");
    mConnectedStatus->setAlignment(Qt::AlignCenter);
    mConnectedStatus->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mPortStatus->addWidget(mConnectedStatus);

    mDisconnectedStatus = new QLabel(tr("Disconnected."));
    mDisconnectedStatus->setStyleSheet("background-color:red; color:white");
    mDisconnectedStatus->setAlignment(Qt::AlignCenter);
    mDisconnectedStatus->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));
    mPortStatus->addWidget(mDisconnectedStatus);

    mDoorOpenedStatus = new QLabel(tr("Door is open."));
    mDoorOpenedStatus->setStyleSheet("background-color:transparent; color:yellow");

    mChillerOffStatus = new QLabel(tr("Chiller is off."));
    mChillerOffStatus->setStyleSheet("background-color:transparent; color:yellow");

    mLimitHitOnStatus = new QLabel(tr("It touched to limit switch. Please check the work area, and adjust your machine."));
    mLimitHitOnStatus->setStyleSheet("background-color:transparent; color:yellow");

    // Status infomation widgets
    QHBoxLayout* mStatusLayout = new QHBoxLayout;
    mStatusLayout->setContentsMargins(0, 0, 0, 0);
    mStatusLayout->addWidget(mPortStatus);
    mStatusLayout->addWidget(mSafetyStatus, 1);
    mStatusLayout->setAlignment(mPortStatus, Qt::AlignCenter);

    mStatus->setLayout(mStatusLayout);
    mStatus->setContentsMargins(0, 0, 0, 0);
    mStatus->setGeometry(0, 0, ui->statusBar->size().width(), ui->statusBar->size().height());
    mStatus->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    ui->statusBar->addPermanentWidget(mStatus, 1);
    ui->statusBar->setContentsMargins(0, 0, 0, 0);

    // Port status infomation.
    mPortStatus->setCurrentWidget(mDisconnectedStatus);
    mPortStatus->setContentsMargins(0, 0, 0, 0);
    mPortStatus->setFixedWidth(100);
    mPortStatus->setFixedHeight(ui->statusBar->size().height() - 4); // 4 is fix height
    mPortStatus->setSizePolicy(QSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding));

    // Safety status caution.
    mSafetyStatusLayout = new QHBoxLayout;
    mSafetyStatusLayout->setContentsMargins(0, 0, 0, 0);
    mSafetyStatusLayout->addWidget(mDoorOpenedStatus, 1);
    mSafetyStatusLayout->addWidget(mChillerOffStatus, 1);
    mSafetyStatusLayout->addWidget(mLimitHitOnStatus, 1);
    mSafetyStatusLayout->addStretch(100);

    mSafetyStatus->setLayout(mSafetyStatusLayout);
    mDoorOpenedStatus->hide();
    mChillerOffStatus->hide();
    mLimitHitOnStatus->hide();
}

int  MainDialog::libraryInit()
{
    mLibraryDialog = new SelectListDialog(this);
    mLibraryDialog->setWindowTitle(tr("Image Library"));
    mLibraryDialog->hide();
    connect(mLibraryDialog, SIGNAL(select()), this, SLOT(selectLibrary()));
    connect(mLibraryDialog, SIGNAL(cancel()), mLibraryDialog, SLOT(hide()));

    createLibraryModel();
    mLibraryDialog->setListModel(mLibraryModel);

    return SUCCESS;
}

void MainDialog::libraryFinal()
{
    if(mLibraryDialog != NULL) {
        delete mLibraryDialog;
        //モデルの削除が先か？
        mLibraryDialog = NULL;
    }
}

int  MainDialog::createLibraryModel()
{
    // model load & set
    mLibraryModel = new LibraryListModel();

    QFileInfo fInfo(QCoreApplication::applicationFilePath());
    QString path = fInfo.absolutePath() + "/data/ImgPreset" + FILE_SUFFIX_IMG_PRESET;
    QFile file(path);
    if(file.open(QIODevice::ReadOnly) == false) {
        return ERR_PROJ_LOAD_FILE_OPEN;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj = doc.object();
    if(obj[JSON_TAG_FILE_TYPE] != FILE_TYPE_LIBRARY)
    { return ERR_LIBRARY_OPEN; }

    mLibraryModel->setData(obj);

    return SUCCESS;
}

int  MainDialog::materialsInit()
{
    mMaterialDialog = new SelectListDialog(this);
    mMaterialDialog->setWindowTitle(tr("Parameter Library"));
    mMaterialDialog->hide();
    connect(mMaterialDialog, SIGNAL(select()), this, SLOT(selectMaterial()));
    connect(mMaterialDialog, SIGNAL(cancel()), mMaterialDialog, SLOT(hide()));

    createMaterialModel();
    mMaterialDialog->setListModel(mMaterialModel);

    return SUCCESS;
}

void MainDialog::materialsUpdate()
{
    mMaterialDialog->hide();

    if (mMaterialModel != NULL) {
        delete mMaterialModel;
        mMaterialModel = NULL;
    }
    createMaterialModel();

    mMaterialDialog->setListModel(mMaterialModel);
}

void MainDialog::materialsFinal()
{
    if(mMaterialDialog != NULL) {
        delete mMaterialDialog;
        //モデルの削除が先か？
        mMaterialDialog = NULL;
    }
}

int  MainDialog::createMaterialModel()
{
    // model load & set
    mMaterialModel = new MaterialListModel();

    QFileInfo fInfo(QCoreApplication::applicationFilePath());
    QString path = fInfo.absolutePath() + "/data/PrmPreset" + FILE_SUFFIX_PRM_PRESET;
    QFile file(path);
    if(file.open(QIODevice::ReadOnly) == false) {
        return ERR_PROJ_LOAD_FILE_OPEN;
    }

    QByteArray data = file.readAll();
    QJsonDocument doc(QJsonDocument::fromJson(data));
    QJsonObject obj = doc.object();
    if(obj[JSON_TAG_FILE_TYPE].toString() != FILE_TYPE_MATERIAL)
    { return ERR_MATERIAL_OPEN; }

    QJsonArray matData = obj[JSON_TAG_MATERIAL_DATA].toArray();

    QJsonObject matObj;
    foreach (QJsonValue matVal, matData) {
        matObj = matVal.toObject();
        QString laserType = matObj[JSON_TAG_LASER_TYPE].toString();
        if(laserType == SysParam::getInstance()->getLaserTypeString()) {
            break;
        }
        matObj.empty();
    }
    if(matObj.isEmpty() == true)
    { return ERR_PROJ_LOAD_FILE_OPEN; }

    mMaterialModel->setData(matObj);

    return SUCCESS;
}

void MainDialog::connectionInit()
{
// project
    connect(ui->ProjectSaveButton,      SIGNAL(clicked()),            this,      SLOT(onClickSave()));
    connect(ui->ProjectLoadButton,      SIGNAL(clicked()),            this,      SLOT(onClickLoad()));

// web site
    connect(ui->ManualButton,           SIGNAL(clicked()),            this,      SLOT(openManual()));
    connect(ui->ForumButton,            SIGNAL(clicked()),            this,      SLOT(openForum()));

// graphic control Area
    connect(ui->LoadImageButton,        SIGNAL(clicked()),            mCmdMgr,   SLOT(importCommnad()));
    connect(mScene,                     SIGNAL(dropFiles(QStringList)), this,   SLOT(importFiles(QStringList)));
    connect(ui->LibraryButton,          SIGNAL(clicked()),            this,      SLOT(openLibrary()));
    connect(ui->MaterialButton,         SIGNAL(clicked()),            this,      SLOT(openMaterial()));
    connect(ui->CopyButton,             SIGNAL(clicked()),            this,      SLOT(onClickCopy()));
    connect(ui->PasteButton,            SIGNAL(clicked()),            mCmdMgr,   SLOT(pasteCommnad()));
    connect(ui->RedoButton,             SIGNAL(clicked()),            mCmdMgr,   SLOT(redo()));
    connect(ui->UndoButton,             SIGNAL(clicked()),            mCmdMgr,   SLOT(undo()));
    connect(ui->ZoomInButton,           SIGNAL(clicked()),            mView,     SLOT(zoomIn()));
    connect(ui->ZoomOutButton,          SIGNAL(clicked()),            mView,     SLOT(zoomOut()));
    connect(ui->ActualSizeButton,       SIGNAL(clicked()),            mView,     SLOT(actualSize()));
    connect(ui->ShowGridButton,         SIGNAL(toggled(bool)),        this,      SLOT(onClickGridOn(bool)));
    connect(ui->shapeControlWidget,     SIGNAL(shapeChanged(bool)),   mCmdMgr,   SLOT(shapeChangeCommand(bool)));

//process Area
    connect(ui->CheckProcessRange,      SIGNAL(clicked()),            this,      SLOT(onClickCheckProcessRange()));
    connect(ui->ResetProcessing,        SIGNAL(clicked()),            this,      SLOT(onClickResetProcess()));
    connect(ui->StartProcessing,        SIGNAL(clicked()),            this,      SLOT(onClickProcess()));
    connect(ui->ConfirmPosition,        SIGNAL(clicked()),            this,      SLOT(onConfirmPositionOpen()));
    connect(ui->ReturnToOrigin,         SIGNAL(clicked()),            this,      SLOT(onClickReturnOrigin()));
    connect(ui->CalcNeedTime,           SIGNAL(clicked()),            this,      SLOT(wrapperEstimationTime()));
    connect(mProcessTimeCalculator,     SIGNAL(calcurateFinished(QTime)),  this, SLOT(updateEstimationTime(QTime)));
    connect(mProcessTimeCalculator,     SIGNAL(callRecalcurateRequest()),  this, SLOT(wrapperEstimationTime()));

// responce
    connect(mCmdMgr,                    SIGNAL(update()),                           this,                   SLOT(onUpdateParam()));
    connect(mCmdMgr,                    SIGNAL(loadErr(QMap<QString, QString>)),    this,                   SLOT(showErrStrings(QMap<QString, QString>)));

    connect(mFaboolMachine,             SIGNAL(processRateDidUpdated(int)),         mProgressDialog,        SLOT(updateProgress(int)));
    connect(mFaboolMachine,             SIGNAL(sendGCodeDidFinished()),             this,                   SLOT(onFinishProcess()));
    connect(mFaboolMachine,             SIGNAL(machineStateDidUpdated()),           this,                   SLOT(onUpdateMachineState()));
    connect(mFaboolMachine,             SIGNAL(stopProcessBySafetyWarning()),       this,                   SLOT(onStopProcessBySafetyWarning()));

    connect(mScene,                     SIGNAL(grouped()),                          mCmdMgr,                SLOT(groupCommand()));
    connect(mScene,                     SIGNAL(ungrouped()),                        mCmdMgr,                SLOT(ungroupCommand()));
    connect(mScene,                     SIGNAL(modify(bool)),                       mCmdMgr,                SLOT(shapeChangeCommand(bool)));
    connect(mScene,                     SIGNAL(copied()),                           this,                   SLOT(onClickCopy()));
    connect(mScene,                     SIGNAL(pasted()),                           mCmdMgr,                SLOT(pasteCommnad()));
    connect(mScene,                     SIGNAL(pastedOnMenu(QPointF)),              mCmdMgr,                SLOT(pasteCommnad(QPointF)));
    connect(mScene,                     SIGNAL(deleted()),                          mCmdMgr,                SLOT(deleteCommand()));

    connect(mScene,                     SIGNAL(selectionChanged()),                 mScene,                 SLOT(sendChanged()));
    connect(mScene,                     SIGNAL(selectionChanged(QGraphicsItem*)),   ui->shapeControlWidget, SLOT(selectChange(QGraphicsItem*)));
    connect(mScene,                     SIGNAL(changed(QList<QRectF>)),             ui->shapeControlWidget, SLOT(updateEdit()));

    connect(ui->scrollAreaWidgetContents, SIGNAL(changeOrder(int, int)),            this,                  SLOT(onChangeOrder(int, int)));

    INFO_STRING("Connection done.");
}

//　コピーボタン
void MainDialog::onClickCopy()
{
    mScene->copyFlexFrame();
}

void MainDialog::applyGroupModify()
{
    if(mScene->hasGroup() == true) {
        mCmdMgr->ungroupCommand();
    }
}

int MainDialog::checkProcessAreaOfImages()
{
    QRectF scnRect = QRectF(-5, -5, mScene->sceneRect().width() + 5, mScene->sceneRect().height() + 5); //Gripper Size offset
    QMap<QString, QString> ngMes = mDMInstance->dataCheck(scnRect);
    if (ngMes.count() > 0) {
        //QString discription(tr("Some images are out of process range.\nPlease confirm Size and Position."));
        showErrStrings(ngMes);
        return ERR_PROJ_SAVE_SIZE_CHECK;
    } else {
        return SUCCESS;
    }
}

// Projectファイルの保存
int MainDialog::onClickSave()
{
    DEBUG_STRING("Save function in.");

    if(mDMInstance->isSaved() == true)          { return SUCCESS; }
    if(mDMInstance->ImageItem().count() == 0)   {
        INFO_STRING("There is no object when saving.");
    }

    int ret = 0;

// グループ化強制解除
    applyGroupModify();

// SizeCheck
    if (checkProcessAreaOfImages() != SUCCESS) {
        return ERR_PROJ_SAVE_SIZE_CHECK;
    }

// ProjectNameCheck
    if(windowTitle() == (QString(APP_NAME) + " _ " + tr("Undefined"))) {
        ret = onClickSaveAs();
        return ret;
    }

// DoSave
    ret = mDMInstance->saveProject();
    if(ret == ERR_PROJ_SAVE_FILE_OPEN) {
        showErrMes(QtInfoMsg, tr("Failed to open project."));
    } else if(ret == ERR_PROJ_SAVE_INVAILD_DATA) {
        showErrMes(QtInfoMsg, tr("Existing invaild data."));
    } else {
        mDMInstance->saveDone();
        INFO_STRING("Save done.");
        setWindowTitle(QString(APP_NAME) + " _ " + mDMInstance->projectName());
    }
    return ret;
}

// Projectファイルの命名保存
int MainDialog::onClickSaveAs()
{
    DEBUG_STRING("SaveAs function in.");
    if(mDMInstance->ImageItem().count() == 0)   {
        INFO_STRING("There is no object when saving.");
    }

// グループ化強制解除
    applyGroupModify();

// SizeCheck
    if (checkProcessAreaOfImages() != SUCCESS) {
        return ERR_PROJ_SAVE_SIZE_CHECK;
    }

    static QString memoriedPath = "/";
    QString projPath = QFileDialog::getSaveFileName(this, tr("Save As"), memoriedPath, "projectFile(*" + FILE_SUFFIX_PROJECT + ")");
    if(projPath == "") {
        return ERR_PROJ_SAVE_CANCEL;
    }
    int pathIndx = projPath.lastIndexOf("/");
    memoriedPath = projPath.left(pathIndx);

    if(projPath.endsWith(FILE_SUFFIX_PROJECT) == false) {
        projPath.append(FILE_SUFFIX_PROJECT);
    }

    int ret = mDMInstance->saveProject(projPath);
    if(ret == ERR_PROJ_SAVE_FILE_OPEN) {
        showErrMes(QtInfoMsg, tr("Failed to open project."));
    } else if(ret == ERR_PROJ_SAVE_INVAILD_DATA) {
        showErrMes(QtInfoMsg, tr("Existing invaild data."));
    } else {
        mDMInstance->saveDone();
        INFO_STRING("Save done.");
        setWindowTitle(QString(APP_NAME) + " _ " + mDMInstance->projectName());
    }
    return ret;
}

// Projectファイルの展開
void MainDialog::onClickLoad()
{
    DEBUG_STRING("Load function in.");

    int ret = 0;
//　プロジェクトの変更状態を確認
    if(confirmChangeProject() == false) { return; }

    static QString memoriedPath = "/";
    QString filePath = QFileDialog::getOpenFileName(this, tr("Load Project"),
                       memoriedPath, "projectFile(*" + FILE_SUFFIX_PROJECT + ")");
    if(filePath == "") { ret = ERR_PROJ_LOAD_CANCEL; }
    else {
        int pathIndx = filePath.lastIndexOf("/");
        memoriedPath = filePath.left(pathIndx);

        clearCurrentProject();
        int ret = mDMInstance->loadProject(filePath);
        applyProject(ret);
    }
}

// Projectファイルの展開
void MainDialog::onClickNewProject()
{
    DEBUG_STRING("Project refresh in.");

//　プロジェクトの変更状態を確認
    if(confirmChangeProject() == false) { return; }

    clearCurrentProject();
    applyProject(SUCCESS);
}

void MainDialog::onClickImportProject()
{
    DEBUG_STRING("Import FABOOLSoftware Project in.");

    int ret = 0;
//　プロジェクトの変更状態を確認
    if(confirmChangeProject() == false) { return; }

    static QString memoriedPath = "/";
    QString filePath = QFileDialog::getOpenFileName(this, tr("Import FABOOLSoftware Project"),
                       memoriedPath, "projectFile(*" + FILE_SUFFIX_PROJECT_BROWSER + ")");
    if(filePath == "") { ret = ERR_PROJ_LOAD_CANCEL; }
    else {
        int pathIndx = filePath.lastIndexOf("/");
        memoriedPath = filePath.left(pathIndx);

// 今持っているデータの削除
        clearCurrentProject();

// プロジェクトのロードと表示更新
        ret = mDMInstance->loadBrowserProject(filePath);
        applyProject(ret);
    }
}

void MainDialog::onClickPauseProcess(bool isPaused)     // 運転開始
{
    mFaboolMachine->pause(isPaused);
}

void MainDialog::onClickResetProcess()     // 運転リセット
{
    mFaboolMachine->stop();
    QTimer::singleShot(1000, this, SLOT(onClickReturnOrigin()));

    if (mProgressDialog->isModal() == true) {
        mProgressDialog->hide();
        mProgressDialog->setModal(false);
    }
}

void MainDialog::onFinishProcess()
{
    if (mProgressDialog->isModal() == true) {
        INFO_STRING("Finish Sending Gcode.");

        mProgressDialog->hide();
        mProgressDialog->setModal(false);
    }
}

void MainDialog::onStopProcessBySafetyWarning()
{
    WARNING_STRING("ProcessWarning");
    onClickResetProcess();
    showSafetyWarningDialog();
}

void MainDialog::showSafetyWarningDialog()
{
    QMessageBox::warning(
        this,
        tr("Safety Warning"),
        tr("Please check the status bar"));
}

void MainDialog::onClickReturnOrigin()     // 原点復帰
{
    QPointF laserPos = -(mMaintenanceView->laserPos());
    mMaintenanceView->movelaser(laserPos);
    mFaboolMachine->setParameter(createParameterGCode());
    mFaboolMachine->goToOrigin();
}

void MainDialog::onJogControl(QPointF pos)
{
    QString gcode = QString("\nG91\nS0\nG0X%1Y%2\nS0\nG90\n").arg(pos.x()).arg(pos.y()) ;
    QList<QString> gcodes ;
    gcodes.append(gcode);
    mFaboolMachine->maintenance(gcode);
}


void MainDialog::onMoveDirectly(QPointF pos)
{
    QString gcode = QString("\nG90\nS0\nG0X%1Y%2\nS0\n").arg(pos.x()).arg(pos.y()) ;
    mFaboolMachine->maintenance(gcode);
}

void MainDialog::onClickCheckProcessRange()// 加工範囲の確認
{
    applyGroupModify();

// SizeCheck
    if (checkProcessAreaOfImages() != SUCCESS) {
        return;
    }

    QRectF processRect = mDMInstance->processRange();
    QStringList gcodes;
    gcodes.append(QString("\n~\n"));
    gcodes.append(QString("\nG90\nS0\nG0X%1Y%2\nS0\n").arg(processRect.topLeft().x()).arg(processRect.topLeft().y())) ;
    gcodes.append(QString("\nG90\nS0\nG0X%1Y%2\nS0\n").arg(processRect.topRight().x()).arg(processRect.topRight().y())) ;
    gcodes.append(QString("\nG90\nS0\nG0X%1Y%2\nS0\n").arg(processRect.bottomRight().x()).arg(processRect.bottomRight().y())) ;
    gcodes.append(QString("\nG90\nS0\nG0X%1Y%2\nS0\n").arg(processRect.bottomLeft().x()).arg(processRect.bottomLeft().y())) ;
    gcodes.append(QString("\nG90\nS0\nG0X%1Y%2\nS0\n").arg(processRect.topLeft().x()).arg(processRect.topLeft().y())) ;

    mFaboolMachine->maintenance(gcodes);
}

QStringList MainDialog::createParameterGCode()
{
    SysParam* sysPara = SysParam::getInstance();
    quint16 driverPow = sysPara->getDriverPower();
    quint16 seekSpeed = sysPara->getOffMachiningSpeed();
    INFO_STRING(QString("Set driver power %1").arg(driverPow));
    INFO_STRING(QString("Set seek speed %1").arg(seekSpeed));
    QStringList gcodes;
    gcodes << QString("\nM96X400Y%1\n").arg(driverPow) << QString("\nG0F%1\n").arg(seekSpeed);

    return gcodes;
}

void MainDialog::sendParaChange()
{
    QStringList gcodes = createParameterGCode();
    mFaboolMachine->setParameter(gcodes);

    wrapperEstimationTime();
}

//　設定画面
void MainDialog::onClickSettingMenu()      // 設定ダイアログの展開
{
    int retBtn = mSettingDialog->exec();
    if(retBtn == QDialog::Accepted) {
        mScene->destructGrid();
        mScene->setMovableRange(SysParam::getInstance()->getWorkArea());
        mScene->createGrid();
        mView->setSceneAndRange(mScene);
        mMaintenanceView->setSceneAndRange(mScene);
    }
}

// このアプリについて画面
void MainDialog::onClickAbout()
{
    AboutDialog versionInfoDialog(this);
    connect(&versionInfoDialog, SIGNAL(versionUp()), this, SLOT(onVersionUp()));
    versionInfoDialog.exec();
}

// アップデート可能メッセージの自動表示
void MainDialog::autoDisplayUpdateMessage()
{
    if(SysParam::getInstance()->getNotifyUpgrade() == false)
    { return; }

    VersionInfo versionInfo;
    if(versionInfo.fetchLatestVersion() == false) {
        return;
    }
    if(versionInfo.isLatestVersion() == false) {
        AboutDialog versionInfoDialog(this);
        connect(&versionInfoDialog, SIGNAL(versionUp()), this, SLOT(onVersionUp()));
        versionInfoDialog.show();
        versionInfoDialog.onUpdateCheckButtonPushed();
    }
}

// リリースノートへジャンプ
void MainDialog::onClickReleaseNote()
{
    QUrl url("https://www.smartdiys.com/manual/fabool-desktop-releasenotes/");
    QDesktopServices::openUrl(url);
}

//　メンテ画面切り替え
void MainDialog::onConfirmPositionClose()  // メンテナンスダイアログの展開
{
    mScene->unselect();
    ui->ParamAndMaintenance->setCurrentWidget(ui->ParamWidget);
    mMaintenanceView->laserHide();

    mMaintenanceView->hide();
    mView->show();
    mView->resize(ui->GraphicsViewArea->size());
    mView->fitInViewRect();

    ui->ToolWidget->setEnabled(true);
    ui->UtilityFrame->setEnabled(true);
    ui->ConfirmPosition->setEnabled(true);
    ui->StartProcessing->setEnabled(true);
}

//　メンテ画面切り替え
void MainDialog::onConfirmPositionOpen()  // メンテナンスダイアログの展開
{
    mScene->unselect();   //　メンテ画面に移動したときは今の選択状態をリセットしたい
    ui->ParamAndMaintenance->setCurrentWidget(mMaintenanceWidget);
    mMaintenanceView->laserHide();

    mView->hide();
    mMaintenanceView->show();
    mMaintenanceView->resize(ui->GraphicsViewArea->size());
    mMaintenanceView->fitInViewRect();

    ui->ToolWidget->setDisabled(true);
    ui->UtilityFrame->setDisabled(true);
    ui->ConfirmPosition->setDisabled(true);
    ui->StartProcessing->setDisabled(true);
}

void MainDialog::onClickProcess()
{

    applyGroupModify();

    if (checkProcessAreaOfImages() != SUCCESS) {
        return;
    }

    if (mFaboolMachine->getState().isSafety() == false) {
        showSafetyWarningDialog();
        return;
    }

    int btnRet = QMessageBox::warning(this, tr("Warning"),
                                      tr("Please don't be leave from machine during processing.\nWould you start processing?"),
                                      QMessageBox::Yes | QMessageBox::Cancel);
    if (btnRet == QMessageBox::Cancel) {
        return;
    }
    if (mProgressDialog != NULL) {
        mProgressDialog->setModal(true);
        mProgressDialog->show();
        processStart();
    }
}

//　フォーラムボタン押下時処理
void MainDialog::openForum()
{
    QUrl url("https://forum.smartdiys.com/");
    QDesktopServices::openUrl(url);
}

//　マニュアルボタン押下時処理
void MainDialog::openManual()
{
    QUrl url("https://www.smartdiys.com/manual/fabool_desktop/");
    QDesktopServices::openUrl(url);
}

void MainDialog::recalculateEstimationTime()
{
    if(mProcessTimeCalculator->isCalculating() == false) {
        mProcessTimeCalculator->setCalculating(true);
        ui->NeedTime->setText(tr("Calculating Required Times...  "));
        mProcessTimeCalculator->calculate(createProcessGCode().join("\n"));
    } else {
        mProcessTimeCalculator->setUpdateRequest(true);
    }
}

void MainDialog::wrapperEstimationTime()
{
    QtConcurrent::run(this, &MainDialog::recalculateEstimationTime);
}

void MainDialog::updateEstimationTime(QTime processTime)
{
    QTime zeroTimeforCompare = QTime(0, 0, 0);
    QTime ceiledProcessTime;

    if (processTime == zeroTimeforCompare) {
        ceiledProcessTime = processTime;
    } else {
        ceiledProcessTime = processTime.addSecs(10);
    }

    int hour   = ceiledProcessTime.hour();
    int minute = ceiledProcessTime.minute();
    int second = qFloor(ceiledProcessTime.second() * 0.1) * 10;

    if (hour >= 1) {
        ui->NeedTime->setText(tr("Required Times :  ") + QString::number(hour) + tr(" hour") + QString::number(minute) + tr(" min"));
    } else if (minute >= 10) {
        ui->NeedTime->setText(tr("Required Times :  ") + QString::number(minute) + tr(" min"));
    } else {
        ui->NeedTime->setText(tr("Required Times :  ") + QString::number(minute) + tr(" min ") + QString::number(second) + tr(" sec"));
    }
}

//　グリッド表示切替
void MainDialog::onClickGridOn(bool offGrid)
{
    if(offGrid == true) { mScene->hideGrid(); }
    else                { mScene->showGrid(); }
}

//　パラメータエリアの更新
//　パラメータの追加等で呼ばれる
void MainDialog::onUpdateParam()
{
    QList<ParamWidget* > widList = ui->ParamDisplayArea->findChildren<ParamWidget* >();
    QMap<QString, ParamWidget*> widMap;
    foreach (ParamWidget* wid, widList) {
        widMap.insert(wid->itemName(), wid);
    }
    QStringList ImageList = mDMInstance->imageNameList();

    foreach(QString itemName, ImageList) {
        if(!widMap.keys().contains(itemName)) {
            ItemInfo* info = mDMInstance->getImageWithFormalName(itemName);
            if(info == NULL) { continue; }
            ParamWidget* newParam = new ParamWidget(info);
            ui->scrollAreaWidgetContents->addWidget(newParam);
        } else {
            ParamWidget* wid = widMap.take(itemName);
            wid->updateParameter();
        }
    }

    foreach (QString widName, widMap.keys()) {
        delete widMap.value(widName);
    }
}

//　Graphicsの表示内容の更新
//　プロジェクトをロードしたときに呼ばれる
void MainDialog::onUpdateGraphics()
{
    foreach (ItemInfo* item, mDMInstance->ImageItem()) {
        item->setItemToScene(mScene);
    }
}

void MainDialog::onChangeOrder(int prev, int curr)
{
    mDMInstance->changeOrder(prev, curr);
}

void MainDialog::onMoveUp()
{
    if(mMaintenanceView->movelaser(QPointF(0, -mLaserMoveStep)) == true)
    { onJogControl(QPointF(0, -mLaserMoveStep)); }
}

void MainDialog::onMoveDown()
{
    if(mMaintenanceView->movelaser(QPointF(0, mLaserMoveStep)) == true)
    { onJogControl(QPointF(0, mLaserMoveStep)); }
}

void MainDialog::onMoveRight()
{
    if(mMaintenanceView->movelaser(QPointF(mLaserMoveStep, 0)) == true)
    { onJogControl(QPointF(mLaserMoveStep, 0)); }
}

void MainDialog::onMoveLeft()
{
    if(mMaintenanceView->movelaser(QPointF(-mLaserMoveStep, 0)) == true)
    { onJogControl(QPointF(-mLaserMoveStep, 0)); }
}

void MainDialog::openLibrary()
{
    if(mLibraryDialog->isHidden())
    { mLibraryDialog->show(); }
}

void MainDialog::openMaterial()
{
    if(mMaterialDialog->isHidden())
    { mMaterialDialog->show(); }
}

void MainDialog::selectLibrary()
{
    mLibraryDialog->hide();

    QModelIndex index = mLibraryDialog->selectedIndex();
    QVariant libData = mLibraryModel->data(index, Qt::UserRole);

    mScene->clearSelection();

    if(libData.isValid() == true) {
        ListedDataCore* data = libData.value<ListedDataCore*>();
        EN_LOAD_STATE retLoad = EN_LOAD_UNLOADED;
        ItemInfo* item = mDMInstance->loadImage((LibraryListedData*)data, &retLoad);
        if(retLoad == EN_LOAD_SUCCESS) {
            item->setItemToScene(mScene);
            onUpdateParam();
        }
    }
}

void MainDialog::selectMaterial()
{
    mMaterialDialog->hide();

    int retBtn = QMessageBox::question(this, tr("Confirmation"), tr("Parameter will overwrite.\nContinue?"), QMessageBox::Yes | QMessageBox::No);
    if(retBtn == QMessageBox::No)
    { return; }

    QModelIndex index = mMaterialDialog->selectedIndex();
    QVariant libData = mMaterialModel->data(index, Qt::UserRole);

    if(libData.isValid() == true) {
        ListedDataCore* data = libData.value<ListedDataCore*>();
        mDMInstance->setMaterialData((MaterialListedData*)data);
        onUpdateParam();
    }
}

//　リサイズイベント
//　ウィンドウサイズが変更されたら自動で走る
void MainDialog::resizeEvent(QResizeEvent* e)
{
    QMainWindow::resizeEvent(e);
    mView->resize(ui->GraphicsViewArea->size());
    mView->fitInViewRect();
}

bool MainDialog::eventFilter(QObject *watched, QEvent *event)
{
    if(event->type() == QEvent::GraphicsSceneMouseRelease) {
        wrapperEstimationTime();
    }
    if(event->type() != QEvent::KeyPress) {
        return QMainWindow::eventFilter(watched, event);
    }

    QKeyEvent* e = (QKeyEvent*)event;
    int key = e->key();
    int mod = e->modifiers();

    switch(key) {
    case Qt::Key_Delete:
    case Qt::Key_Backspace:
        if(mod == Qt::SHIFT) {
            mCmdMgr->deleteAllCommand();
        } else {
            mCmdMgr->deleteCommand();
        }
        return false;
    case Qt::Key_S:
        if(mod == (Qt::CTRL + Qt::SHIFT)) {
            onClickSaveAs();
        } else if (mod == Qt::CTRL) {
            onClickSave();
        }
        return false;
    case Qt::Key_A:
        if(mod == Qt::CTRL) {
            mScene->allSelect();
        }
        return false;
    case Qt::Key_O:
        if(mod == Qt::CTRL) {
            onClickLoad();
        }
        return false;
    case Qt::Key_N:
        if(mod == Qt::CTRL) {
            onClickNewProject();
        }
        return false;
    case Qt::Key_Comma:
        if(mod == Qt::CTRL) {
            onClickSettingMenu();
        }
        return false;
    }
    return QMainWindow::eventFilter(watched, event);
}

void MainDialog::mouseReleaseEvent(QMouseEvent *event)
{
    wrapperEstimationTime();
    QMainWindow::mouseReleaseEvent(event);
}

QStringList MainDialog::createProcessGCode()
{
    mDMInstance->createGcode();
    QStringList graphicGCode   = mDMInstance->getGcode();
    QStringList parameterGCode = createParameterGCode();

    QStringList gcode;
    gcode.append(parameterGCode);
    gcode.append(graphicGCode);


    return gcode;
}

void MainDialog::processStart()
{
    QStringList gcode = createProcessGCode();

    mProcessStart = QDateTime::currentDateTime();
    mFaboolMachine->setGCode(gcode);
    mFaboolMachine->start();
}

//　クローズ時に変更が残っている場合は保存を促す
void MainDialog::closeEvent(QCloseEvent* e)
{
    INFO_STRING("MainDialog Finalize.");
    static bool isAlreadyClosed = false;
    if(isAlreadyClosed == true) {
        e->accept();
        INFO_STRING("Finalized.");
        return;
    }

    if(mDMInstance->isSaved() == false) {
        int ret = QMessageBox::question(this, tr("close process"),
                                        tr("Opened Project has not saved.\nDo you want to save and exit?"),
                                        QMessageBox::Yes | QMessageBox::No | QMessageBox::Cancel );

        if(ret == QMessageBox::Cancel) {
            e->ignore();
            return;
        } else if(ret == QMessageBox::Yes) {
            int retSave = onClickSave();
            if(retSave != SUCCESS) {
                e->ignore();
                return;
            }
        }
    }

    projectFinal();
    serialPortFinal();
    commandFinal();
    graphicsAreaFinal();

    isAlreadyClosed = true;
    e->accept();
    INFO_STRING("Finalized.");
}

int MainDialog::showErrMes(QtMsgType type, QString mes)
{
    switch (type) {
    case QtInfoMsg:
        return QMessageBox::information(this, tr("INFO"), mes);
    case QtWarningMsg:
        return QMessageBox::warning(this, tr("WARNING"), mes);
    case QtCriticalMsg:
        return QMessageBox::critical(this, tr("CRITICAL"), mes);
    default:
        return QMessageBox::Ok;
    }
}

void MainDialog::showErrStrings(const QMap<QString, QString>& detail)
{
    ErrMesDialog errDialog(tr("Notification"), this);
    errDialog.setStrings(detail);
    errDialog.exec();
}

void MainDialog::onUpdateMachineState()
{
    if (mFaboolMachine->getState().isChillerOff()) {
        mChillerOffStatus->show();
    } else {
        mChillerOffStatus->hide();
    }

    if (mFaboolMachine->getState().isDoorOpen()) {
        mDoorOpenedStatus->show();
    } else {
        mDoorOpenedStatus->hide();
    }

    if (mFaboolMachine->getState().isLimitHit()) {
        mLimitHitOnStatus->show();
        ui->StartProcessing->setDisabled(true);
    } else {
        mLimitHitOnStatus->hide();
        ui->StartProcessing->setDisabled(false);
    }

    if(mFaboolMachine->getState().isConnect()) {
        mPortStatus->setCurrentWidget(mConnectedStatus);
    } else {
        mPortStatus->setCurrentWidget(mDisconnectedStatus);
        mDoorOpenedStatus->hide();
        mChillerOffStatus->hide();
    }
}

void MainDialog::onVersionUp()
{
    int retSave = onClickSave();
    if(retSave != SUCCESS) {
        int retBtn = QMessageBox::question(this, tr("SoftwareVersionUp"),
                                           tr("Would you like to continue upgrading without saving project?"),
                                           QMessageBox::Yes | QMessageBox::Cancel);
        if(retBtn == QMessageBox::Cancel) {
            return ;
        }
    }

//    QUrl mainteURL("file:///" + mSysPara->getAppPath() + "/maintenancetool" + QString(APP_SUFFIX) + QString(" --updater"), QUrl::TolerantMode);
//    bool ret = QDesktopServices::openUrl(mainteURL);
    SysParam* sysPara = SysParam::getInstance();
    QString maintenancePath = sysPara->getAppPath() + "/maintenancetool" + QString(APP_SUFFIX) ;
    QStringList args;
    args << "--updater" << "--script ";
    if(QSysInfo::productType().indexOf("MacOS", 0, Qt::CaseInsensitive) != -1 ||
            QSysInfo::productType().indexOf("osx", 0, Qt::CaseInsensitive) != -1  ) {
        maintenancePath += "/Contents/MacOS/maintenancetool";
        args << sysPara->getAppPath() + "/auto_update.qs" ;
    } else {
        args << "auto_update.qs" ;
    }
    bool ret = QProcess::startDetached(maintenancePath, args);
    if(ret == false) {
        QMessageBox::information(this, tr("info"), tr("Failed to start maintenanceTool."));
        return;
    }

    close();
}

void MainDialog::importFiles(QStringList fileList)
{
    foreach (QString file, fileList) {
        QFileInfo info(file);
        if(info.suffix().indexOf(QRegExp("(fds|fsd)", Qt::CaseInsensitive)) != -1) {
            if(confirmChangeProject() == false) {
                return;
            }
            clearCurrentProject();
            int ret = SUCCESS;
            if(info.suffix() == "fds") { ret = mDMInstance->loadProject(file); }
            else                       { ret = mDMInstance->loadBrowserProject(file); }
            applyProject(ret);
            return ;
        }
    }
    mCmdMgr->importCommnad(fileList);
}
