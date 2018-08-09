//---------------------------------------------------------------------------
// システム設定
// アプリケーション全体の設定を扱う
//---------------------------------------------------------------------------
#ifndef SYSPARAM_H
#define SYSPARAM_H
//---------------------------------------------------------------------------
#include <QtCore>
#include <QtNetwork>
#include "common.h"
#include "dataManager.h"
//---------------------------------------------------------------------------
// 設定情報構造体
struct ST_SYS_SETTING_INFO {
    //　laserType
    quint16         uiLaserType;
    // 加工範囲
    QSize           sWorkArea;
    // ドライバパワー
    quint16         uiDriverPower;
    // ドライバパワー上書き
    bool            bOverWritePow;
    // ログ出力レベルセレクター
    QtMsgType       enLogLevelSelector;
    // 最終使用プロジェクト名
    QString         stProjectPath;
};

struct ST_SYS_OTHER_INFO {
    // 加工外移動速度
    quint16         uiOffMachiningSpeed;
    // ラスタ画像の処理方法
    quint16         uiRasterDealType;
    // SVG strokeWidthをGraphic上で反映するか
    bool            bEffectiveStorke;
    //
    qreal           dStrokeWidth;
    // アップデート検知を通知するかどうか
    bool            bNotifyUpgrade;
    // SVG Fillを有効にするか
    bool            bEffectiveFill;
    //　Fill密度
    qreal           dFillPitch;
};

Q_DECLARE_METATYPE(ST_SYS_SETTING_INFO)
//---------------------------------------------------------------------------
// ユーザー情報構造体
struct ST_SYS_USER_INFO {
    // UUID
    QUuid   uUId;
};
Q_DECLARE_METATYPE(ST_SYS_USER_INFO)
//---------------------------------------------------------------------------
// PC情報構造体
struct ST_SYS_PC_INFO {
    // MACアドレス
    QString strMacAddress;
    // グローバルIPアドレス
    QString strIpAddress;
    // ホスト名
    QString strHostName;
    // CPUタイプ
    QString strCpuType;
    // OS名
    QString strOsName;
    // OSバージョン
    QString strOsVer;
};
Q_DECLARE_METATYPE(ST_SYS_PC_INFO)
//---------------------------------------------------------------------------
class SysParam
{
    QTTEST_OBJECT

public:
    static SysParam* getInstance()
    {
        static SysParam* instance = NULL;
        if(instance == NULL) { instance = new SysParam(); }
        return instance;
    }

    void init();

    quint16 save();
    quint16 load();

    bool exist();

    void setWorkArea(QSize sWorkArea);
    QSize getWorkArea();

    void setDriverPower(quint16 uiDriverPower);
    quint16 getDriverPower();

    void setOverWritePower(bool isOverWritePow);
    bool isOverWritePower();

    void setLogLevelSelector(QtMsgType enLogLevelSelector);
    QtMsgType getLogLevelSelector();
    QtMsgType* getLogSelectorPointer();

    void setLaserType(quint16 type);
    quint16 getLaserType();

    void setCurrentProject(QString name);
    QString getCurrentProject();

    void setOffMachiningSpeed(quint16  uiSpeed);
    quint16 getOffMachiningSpeed();
    void setRasterDealType(quint16  enDealType);
    quint16  getRasterDealType();
    void setEffectiveStrokeWid(bool bEffective);
    bool getEffectiveStrokeWid();
    void setStrokeWidth(qreal dStrokeWid);
    qreal getStrokeWidth();
    void setNotifyUpgrade(bool bNotify);
    bool getNotifyUpgrade();
    void setEffectiveFill(bool bEffective);
    bool getEffectiveFill();
    void setFillPitch(qreal pitch);
    qreal getFillPitch();

    QUuid getUUID();
    QString getMacAddress();
    QString getIpAddress();
    QString getHostName();
    QString getCpuType();
    QString getOsName();
    QString getOsVer();

    QString getLaserTypeString();
    QString getAppPath();
    QString getLogPath();

private:
    SysParam();
    ~SysParam();

protected:
    QString getSysFilePath();

private:
    // 設定情報
    ST_SYS_SETTING_INFO stSettingInfo;
    // ユーザー情報
    ST_SYS_USER_INFO    stUserInfo;
    // PC情報
    ST_SYS_PC_INFO      stPcInfo;
    //  その他の設定
    ST_SYS_OTHER_INFO   stOtherInfo;

};
//Q_DECLARE_METATYPE(SysParam)
//---------------------------------------------------------------------------
#endif // SYSPARAM_H
