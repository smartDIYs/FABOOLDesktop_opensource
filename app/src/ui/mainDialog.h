#ifndef MAINDIALOG_H
#define MAINDIALOG_H

// UI全体
// main.cppにてこのMainWindowがキックされる

#include <QMainWindow>
#include <QMenu>
#include <QMimeData>

#include "common/common.h"
#include "common/dataManager.h"
#include "common/sysParam.h"
#include "ui/graphicArea/mainView.h"
#include "ui/graphicArea/maintenanceView.h"
#include "ui/graphicArea/mainScene.h"
#include "command/commandManager.h"
#include "tracer/pythonApp.h"

#include "ui/dialogs/machineSettingDialog.h"
#include "ui/dialogs/progressDialog.h"
#include "ui/listView/selectListDialog.h"
#include "ui/listView/libraryListModel.h"
#include "ui/listView/materialListModel.h"
#include "ui/widgets/itemShapeWidget.h"
#include "ui/widgets/maintenanceWidget.h"
#include "machine/faboolMachine.h"
#include "transfer/processTime.h"
#include "machine/util/debugPanel.h"

namespace Ui
{
class MainDialog;
}

class CommandManager;

class MainDialog : public QMainWindow
{
    Q_OBJECT
    QTTEST_OBJECT

public:
    explicit MainDialog(QWidget *parent = 0);
    ~MainDialog();

public slots:
    void onClickCopy();
    int  onClickSave();             // Projectファイルの保存
    int  onClickSaveAs();           // Projectファイルの命名保存
    void onClickLoad();             // Projectファイルの展開
    void onClickNewProject();       // Projectファイルの新規作成
    void onClickImportProject();    // FABOOLSoftwareのプロジェクトのインポート
    void onClickPauseProcess(bool); //
    void onClickResetProcess();     // 運転リセット
    void onClickReturnOrigin();     //　原点復帰コードの送信
    void onJogControl(QPointF);     //　差分移動コードの送信
    void onMoveDirectly(QPointF);   //　位置指定移動コードの送信
    void onClickCheckProcessRange();// 加工範囲の確認
    void onClickSettingMenu();      // 設定ダイアログの展開
    void onConfirmPositionClose();  // 位置確認終了
    void onConfirmPositionOpen();   // 位置確認開始
    void onClickAbout();            // このアプリについての表示
    void onClickReleaseNote();      // リリースノートへ

    void recalculateEstimationTime();   // 所要時間の推定
    void wrapperEstimationTime();       // 所要時間の推定
    void updateEstimationTime(QTime);

    void onClickProcess();               // 加工実行ダイアログを出して実行する
    void onFinishProcess();              // 加工終了（現時点ではGcodeをすべて送信し終えた後）に通知が来た後の処理
    void onStopProcessBySafetyWarning(); // 安全警告による強制停止
    void showSafetyWarningDialog();      // 安全警告のダイアログを表示

    void onClickGridOn(bool offGrid);   // グリッド表示切り替え

    void closeEvent(QCloseEvent* e);

    void onUpdateParam();           //　パラメータの更新
    void onUpdateGraphics();        // グラフィックの更新
    void onChangeOrder(int prev, int curr);

    void openManual();              // マニュアル　外部サイトへの遷移
    void openForum();               //　フォーラム　外部サイトへの遷移

    void onMoveUp();
    void onMoveDown();
    void onMoveRight();
    void onMoveLeft();

    void openLibrary();
    void selectLibrary();

    void openMaterial();
    void selectMaterial();

    QStringList createParameterGCode();
    void sendParaChange();
    void showErrStrings(const QMap<QString, QString>& detail);

    void onUpdateMachineState();

    void autoDisplayUpdateMessage();
    void onVersionUp();

private slots:
    void importFiles(QStringList );

    void materialsUpdate();

signals:
    void sendGcode(const QStringList&);        // GcodeをCommDirectorに送る
    void homeComing();              //　原点復帰コード送信
    void confirmRange();            //　範囲確認コードの送信

protected:
    void resizeEvent(QResizeEvent* e);
    bool eventFilter(QObject *watched, QEvent *event);

private:
    void  mainUIInit();
    void  systemInit();    // system情報の初期化
    void  progressInit();
    void  commandInit();             //
    void  commandFinal();

    void graphicsAreaInit();        // GraphicsView、Sceneなどの準備
    void graphicsAreaFinal();

    void maintenanceAreaInit();     // Maintenunce準備
    void maintenanceAreaFinal();

    void projectMenuInit();
    void optionAreaInit();
    void deleteMenuInit();
    void connectionInit();          // 各種Connectを宣言

    void serialPortInit();          // シリアル通信初期化
    void serialPortFinal();

    void projectInit();
    void projectFinal();
    void clearCurrentProject();
    void applyProject(int loadRet);
    bool confirmChangeProject();

    void statusbarInit();

    int  libraryInit();
    void libraryFinal();
    int  createLibraryModel();
    int  materialsInit();
    void materialsFinal();
    int  createMaterialModel();

    void updateGeometry();          // UIの位置構成の更新
    int  checkProcessAreaOfImages();
    void applyGroupModify();

    QStringList createProcessGCode();
    void processStart();

    int  showErrMes(QtMsgType, QString);

    bool eventDragEnter(QEvent* e);
    bool eventDrop(QEvent* e);
    void mouseReleaseEvent(QMouseEvent *event) override;
private:
    Ui::MainDialog*    ui;
    MainView*          mView;
    ItemShapeWidget*   mShapeController;
    MaintenanceWidget* mMaintenanceWidget;
    MaintenanceView*   mMaintenanceView;
    MainScene*         mScene;
    QWidget*           mStatus;
    QStackedWidget*    mPortStatus;
    QWidget*           mSafetyStatus;
    QHBoxLayout*       mSafetyStatusLayout;
    QLabel*            mConnectedStatus;
    QLabel*            mDisconnectedStatus;
    QLabel*            mDoorOpenedStatus;
    QLabel*            mChillerOffStatus;
    QLabel*            mLimitHitOnStatus;

    DataManager*       mDMInstance;

    CommandManager*    mCmdMgr;
    QThread*           mSerialThread;
    QDateTime          mProcessStart;

    FaboolMachine*         mFaboolMachine;
    ProcessTimeCalculator* mProcessTimeCalculator;

    QMenu*             mOptionMenu;
    QMenu*             mSaveProjectMenu;
    QMenu*             mLoadProjectMenu;
    QMenu*             mDeleteMenu;

    MachineSettingDialog* mSettingDialog;        // 設定ダイアログ 機種やIPアドレスなどを管理
    ProgressDialog*       mProgressDialog;
    SelectListDialog*     mLibraryDialog;
    LibraryListModel*     mLibraryModel;
    SelectListDialog*     mMaterialDialog;
    MaterialListModel*    mMaterialModel;

    QString    mAccessToken;

    int        mLaserMoveStep;

    DebugPanel* mDebugPanel;
};

#endif // QMAIN_H
