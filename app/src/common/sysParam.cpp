//---------------------------------------------------------------------------
#include "sysParam.h"
//---------------------------------------------------------------------------
SysParam::SysParam()
{
    // アプリケーション情報設定
    QCoreApplication::setApplicationName(APP_NAME);
    QCoreApplication::setApplicationVersion(QString().sprintf("%d.%d.%d", APP_VERSION_MAJOR, APP_VERSION_MINOR, APP_VERSION_BUILD));

    // 初期化
    init();
}
//---------------------------------------------------------------------------
SysParam::~SysParam()
{
    // システムファイル書き込み
    save();
}
//---------------------------------------------------------------------------
// 初期化
void SysParam::init()
{
// Logシステムの設定前に呼ばれるのでここではログ書き出しできない
    stSettingInfo.uiLaserType = MACHINE_TYPE_NONE;
    stSettingInfo.sWorkArea = QSize(0, 0);
    stSettingInfo.uiDriverPower = 0;
    stSettingInfo.bOverWritePow = false;
    stSettingInfo.enLogLevelSelector = QtWarningMsg;
    stOtherInfo.uiOffMachiningSpeed = 0;
    stOtherInfo.uiRasterDealType = 0;
    stOtherInfo.bEffectiveStorke = true;
    stOtherInfo.dStrokeWidth = 1;
    stOtherInfo.bNotifyUpgrade = true;
    stOtherInfo.bEffectiveFill = true;
    stOtherInfo.dFillPitch = 0.4;

    stUserInfo.uUId = QUuid();

    // MACアドレス取得
    stPcInfo.strMacAddress = "";
    foreach (const QNetworkInterface &netInterface, QNetworkInterface::allInterfaces()) {
        QNetworkInterface::InterfaceFlags flags = netInterface.flags();
        if ((bool)(flags & QNetworkInterface::IsRunning) && !(bool)(flags & QNetworkInterface::IsLoopBack)) {
            foreach (const QNetworkAddressEntry &address, netInterface.addressEntries()) {
                if(address.ip().protocol() == QAbstractSocket::IPv4Protocol) {
                    stPcInfo.strMacAddress = netInterface.hardwareAddress();
                    break;
                }
            }
            if (stPcInfo.strMacAddress != "") {
                break;
            }
        }
    }
    // グローバルIPアドレス取得
    stPcInfo.strIpAddress = "0.0.0.0";
    if (stPcInfo.strMacAddress != "") {
        // MACアドレスが取得できた場合のみグローバルIPを取得
        QNetworkAccessManager* pNetManager = new QNetworkAccessManager();
        QEventLoop eventLoop;
        QTimer timer;
        QNetworkReply* pReply;

        pReply = pNetManager->get(QNetworkRequest(QUrl("http://ipcheck.ieserver.net/")));
        timer.setSingleShot(true);
        QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
        QObject::connect(pReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        timer.start(3000);  // 3秒でタイムアウト
        eventLoop.exec();
        if (timer.isActive()) {
            timer.stop();
            if (pReply->error() == QNetworkReply::NoError) {
                stPcInfo.strIpAddress = QString::fromUtf8(pReply->readAll()).trimmed();
            }
        } else {
            // タイムアウト
            QObject::disconnect(pReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
            pReply->abort();
        }
        delete pNetManager;
    }

    // OS情報取得
    QSysInfo sysInfo;

    stPcInfo.strHostName = sysInfo.machineHostName();
    stPcInfo.strCpuType = sysInfo.currentCpuArchitecture();
    stPcInfo.strOsName = sysInfo.productType();
    stPcInfo.strOsVer = sysInfo.productVersion();
}
//---------------------------------------------------------------------------
// ファイルパス取得
QString SysParam::getAppPath()
{
    QFileInfo fInfo(QCoreApplication::applicationFilePath());
    QString strAppPath;

    strAppPath = fInfo.absolutePath();
    if(stPcInfo.strOsName.indexOf("macOS", 0, Qt::CaseInsensitive) != -1
            || stPcInfo.strOsName.indexOf("osx", 0, Qt::CaseInsensitive) != -1) {
        strAppPath.chop(QString("%1%2").arg(APP_NAME).arg(".app/Contents/MacOS/").length());
    }

    return strAppPath;
}
//---------------------------------------------------------------------------
// logパス取得
QString SysParam::getLogPath()
{
    QFileInfo fInfo(QCoreApplication::applicationFilePath());
    QString strLogPath = fInfo.absolutePath() + "/log";

    return strLogPath;
}
//---------------------------------------------------------------------------
// システム設定ファイルパス取得
QString SysParam::getSysFilePath()
{
    QFileInfo fInfo(QCoreApplication::applicationFilePath());
    QString strFilePath;

    strFilePath = fInfo.absolutePath() + "/" + fInfo.completeBaseName() + ".ini";

    return strFilePath;
}
//---------------------------------------------------------------------------
// システムファイルへ保存
quint16 SysParam::save()
{
    DEBUG_STRING("System setting save.");

    QSettings settings(getSysFilePath(), QSettings::IniFormat);

    settings.beginGroup("SETTING_INFO");
    settings.setValue("LaserType", stSettingInfo.uiLaserType);
    settings.setValue("WorkArea", stSettingInfo.sWorkArea);
    settings.setValue("DriverPower", stSettingInfo.uiDriverPower);
    settings.setValue("OverWritePower", stSettingInfo.bOverWritePow);
    settings.setValue("LogLevelSelector", (quint8)stSettingInfo.enLogLevelSelector);
    settings.setValue("CurrentProject", stSettingInfo.stProjectPath);
    settings.setValue("OffMachiningSpeed", stOtherInfo.uiOffMachiningSpeed);
    settings.setValue("RasterDealType", stOtherInfo.uiRasterDealType);
    settings.setValue("EffectiveStrokeWidth", stOtherInfo.bEffectiveStorke);
    settings.setValue("StrokeWidth", stOtherInfo.dStrokeWidth);
    settings.setValue("NotifyUpgrade", stOtherInfo.bNotifyUpgrade);
    settings.setValue("EffectiveFill", stOtherInfo.bEffectiveFill);
    settings.setValue("FillPitch", stOtherInfo.dFillPitch);
    settings.endGroup();

    settings.beginGroup("USER_INFO");
    settings.setValue("UUID", stUserInfo.uUId);
    settings.endGroup();

    DEBUG_STRING("System setting save done.");

    return SUCCESS;
}
//---------------------------------------------------------------------------
// システムファイルから読み込み
quint16 SysParam::load()
{
    DEBUG_STRING("System setting load.");
    quint16 ret = SUCCESS;
    QString path = getSysFilePath();
    if(QFileInfo(path).exists() == false) { ret = ERR_SETTING_FILE_OPEN; INFO_STRING(ERR_SETTING_FILE_OPEN_STR); }

    QSettings settings(path, QSettings::IniFormat);
    QString defaultProject = this->getAppPath() + "/HelloWorld.fds";
    if(QFileInfo(defaultProject).exists() == false) { defaultProject = ""; }

    settings.beginGroup("SETTING_INFO");
    stSettingInfo.uiLaserType = settings.value("LaserType", MACHINE_TYPE_MINI_1_6W).toUInt();
    stSettingInfo.sWorkArea = settings.value("WorkArea", QSize(300, 230)).toSize();
    stSettingInfo.uiDriverPower = settings.value("DriverPower", 800).toUInt();
    stSettingInfo.bOverWritePow = settings.value("OverWritePower", false).toBool();
    stSettingInfo.enLogLevelSelector = (QtMsgType)settings.value("LogLevelSelector", (quint8)QtWarningMsg).toUInt();
    stSettingInfo.stProjectPath = settings.value("CurrentProject", defaultProject).toString();
    stOtherInfo.uiOffMachiningSpeed = settings.value("OffMachiningSpeed", 5500).toUInt();
    stOtherInfo.uiRasterDealType = settings.value("RasterDealType", SYS_PARA_RASTER_DEAL_F_S_METHOD).toUInt();
    stOtherInfo.bEffectiveStorke = settings.value("EffectiveStrokeWidth", true).toBool();
    stOtherInfo.dStrokeWidth = settings.value("StrokeWidth", 1).toDouble();
    stOtherInfo.bNotifyUpgrade = settings.value("NotifyUpgrade", true).toBool();
    stOtherInfo.bEffectiveFill = settings.value("EffectiveFill", true).toBool();
    stOtherInfo.dFillPitch = settings.value("FillPitch", 0.4).toDouble();
    settings.endGroup();

    settings.beginGroup("USER_INFO");
    stUserInfo.uUId = settings.value("UUID", QUuid::createUuid()).toString();
    settings.endGroup();

    DEBUG_STRING("System setting load done.");

    return ret;
}
//---------------------------------------------------------------------------
bool SysParam::exist()
{
    QString path = getSysFilePath();
    return QFileInfo(path).exists();
}
//---------------------------------------------------------------------------
// 加工範囲設定
void SysParam::setWorkArea(QSize sWorkArea)
{
    QSize limitedSize = sWorkArea;
    if(limitedSize.width() > SYS_PARA_WORK_AREA_WIDTH_MAX) { limitedSize.setWidth(SYS_PARA_WORK_AREA_WIDTH_MAX); }
    else if(limitedSize.width() < SYS_PARA_WORK_AREA_WIDTH_MIN) { limitedSize.setWidth(SYS_PARA_WORK_AREA_WIDTH_MIN); }
    if(limitedSize.height() > SYS_PARA_WORK_AREA_HEIGHT_MAX) { limitedSize.setHeight(SYS_PARA_WORK_AREA_HEIGHT_MAX); }
    else if(limitedSize.height() < SYS_PARA_WORK_AREA_HEIGHT_MIN) { limitedSize.setHeight(SYS_PARA_WORK_AREA_HEIGHT_MIN); }
    stSettingInfo.sWorkArea = limitedSize;
}
//---------------------------------------------------------------------------
// 加工範囲取得
QSize SysParam::getWorkArea()
{
    return stSettingInfo.sWorkArea;
}
//---------------------------------------------------------------------------
// ドライバパワー設定
void SysParam::setDriverPower(quint16 uiDriverPower)
{
    quint16 checkPower = uiDriverPower;
    if(uiDriverPower > SYS_PARA_DRIVER_POWER_MAX)
    { checkPower = SYS_PARA_DRIVER_POWER_MAX; }
    else if(uiDriverPower < SYS_PARA_DRIVER_POWER_MIN)
    { checkPower = SYS_PARA_DRIVER_POWER_MIN; }

    stSettingInfo.uiDriverPower = checkPower;
}
//---------------------------------------------------------------------------
// ドライバパワー取得
quint16 SysParam::getDriverPower()
{
    quint16 driverPower = SYS_PARA_DRIVER_POWER_DEFAULT_LOW;
    if(stSettingInfo.uiLaserType == MACHINE_TYPE_CO2 ||
            stSettingInfo.uiLaserType == MACHINE_TYPE_DS ) {
        driverPower = SYS_PARA_DRIVER_POWER_DEFAULT_HIGH;
    }
    if(isOverWritePower() == true) {
        driverPower = stSettingInfo.uiDriverPower;
    }
    return driverPower;
}
//---------------------------------------------------------------------------
// ドライバパワー上書き設定
void SysParam::setOverWritePower(bool isOverWritePow)
{
    stSettingInfo.bOverWritePow = isOverWritePow;
}
//---------------------------------------------------------------------------
// ドライバパワー上書き設定取得
bool SysParam::isOverWritePower()
{
    return stSettingInfo.bOverWritePow;
}
//---------------------------------------------------------------------------
// ログ出力レベルセレクター設定
void SysParam::setLogLevelSelector(QtMsgType enLogLevelSelector)
{
    stSettingInfo.enLogLevelSelector = enLogLevelSelector;
}
//---------------------------------------------------------------------------
// ログ出力レベルセレクター取得
QtMsgType SysParam::getLogLevelSelector()
{
    return stSettingInfo.enLogLevelSelector;
}
//---------------------------------------------------------------------------
// ログ出力レベルセレクター取得
QtMsgType* SysParam::getLogSelectorPointer()
{
    return &stSettingInfo.enLogLevelSelector;
}
//---------------------------------------------------------------------------
// レーザータイプ設定
void SysParam::setLaserType(quint16 uiLaserType)
{
    stSettingInfo.uiLaserType = uiLaserType;
    switch(stSettingInfo.uiLaserType) {
    case MACHINE_TYPE_CO2: {
            stSettingInfo.sWorkArea.setWidth(SYS_PARA_WORK_AREA_WIDTH_CO2);
            stSettingInfo.sWorkArea.setHeight(SYS_PARA_WORK_AREA_HEIGHT_CO2);
            break;
        }
    case MACHINE_TYPE_DS: {
            stSettingInfo.sWorkArea.setWidth(SYS_PARA_WORK_AREA_WIDTH_DS);
            stSettingInfo.sWorkArea.setHeight(SYS_PARA_WORK_AREA_HEIGHT_DS);
            break;
        }
    case MACHINE_TYPE_MINI_1_6W:
    case MACHINE_TYPE_MINI_3_5W:
    default: {
            stSettingInfo.sWorkArea.setWidth(SYS_PARA_WORK_AREA_WIDTH_MIN);
            stSettingInfo.sWorkArea.setHeight(SYS_PARA_WORK_AREA_HEIGHT_MIN);
            break;
        }
    }
}
//---------------------------------------------------------------------------
// メールアドレス取得
quint16 SysParam::getLaserType()
{
    return stSettingInfo.uiLaserType;
}
//---------------------------------------------------------------------------
// 最後に使用したプロジェクト設定
void SysParam::setCurrentProject(QString strProjcetPath)
{
    stSettingInfo.stProjectPath = strProjcetPath;
}
//---------------------------------------------------------------------------
// 最後に使用したプロジェクトの取得
QString SysParam::getCurrentProject()
{
    return stSettingInfo.stProjectPath;
}
//---------------------------------------------------------------------------
// 加工外でのヘッド移動速度設定
void SysParam::setOffMachiningSpeed(quint16  uiSpeed)
{
    stOtherInfo.uiOffMachiningSpeed = uiSpeed;
}
//---------------------------------------------------------------------------
// 加工外でのヘッド移動速度取得
quint16 SysParam::getOffMachiningSpeed()
{
    return stOtherInfo.uiOffMachiningSpeed;
}
//---------------------------------------------------------------------------
// ラスタ画像の処理タイプ設定
void SysParam::setRasterDealType(quint16  enDealType)
{
    stOtherInfo.uiRasterDealType = enDealType;
}
//---------------------------------------------------------------------------
// ラスタ画像の処理タイプ取得
quint16 SysParam::getRasterDealType()
{
    return stOtherInfo.uiRasterDealType;
}
//---------------------------------------------------------------------------
//
void SysParam::setEffectiveStrokeWid(bool bEffective)
{
    stOtherInfo.bEffectiveStorke = bEffective;
}
//---------------------------------------------------------------------------
//
bool SysParam::getEffectiveStrokeWid()
{
    return stOtherInfo.bEffectiveStorke;
}
//---------------------------------------------------------------------------
//
void SysParam::setStrokeWidth(qreal dStrokeWid)
{
    stOtherInfo.dStrokeWidth = dStrokeWid;
}
//---------------------------------------------------------------------------
//
qreal SysParam::getStrokeWidth()
{
    return stOtherInfo.dStrokeWidth;
}
//---------------------------------------------------------------------------
// アップデート検知通知設定設定
void SysParam::setNotifyUpgrade(bool bNofity)
{
    stOtherInfo.bNotifyUpgrade = bNofity;
}
//---------------------------------------------------------------------------
// アップデート検知通知設定取得
bool SysParam::getNotifyUpgrade()
{
    return stOtherInfo.bNotifyUpgrade;
}
//---------------------------------------------------------------------------
void SysParam::setEffectiveFill(bool bEffective)
{
    stOtherInfo.bEffectiveFill = bEffective;
}
//---------------------------------------------------------------------------
bool SysParam::getEffectiveFill()
{
    return stOtherInfo.bEffectiveFill;
}
//---------------------------------------------------------------------------
void SysParam::setFillPitch(qreal pitch)
{
    stOtherInfo.dFillPitch = pitch;
}
//---------------------------------------------------------------------------
qreal SysParam::getFillPitch()
{
    return stOtherInfo.dFillPitch;
}
//---------------------------------------------------------------------------
// UUID取得
QUuid SysParam::getUUID()
{
    return stUserInfo.uUId;
}
//---------------------------------------------------------------------------
// MACアドレス取得
QString SysParam::getMacAddress()
{
    return stPcInfo.strMacAddress;
}
//---------------------------------------------------------------------------
// グローバルIPアドレス取得
QString SysParam::getIpAddress()
{
    return stPcInfo.strIpAddress;
}
//---------------------------------------------------------------------------
// ホスト名取得カーソル
QString SysParam::getHostName()
{
    return stPcInfo.strHostName;
}
//---------------------------------------------------------------------------
// CPUタイプ取得
QString SysParam::getCpuType()
{
    return stPcInfo.strCpuType;
}
//---------------------------------------------------------------------------
// OS名取得
QString SysParam::getOsName()
{
    return stPcInfo.strOsName;
}
//---------------------------------------------------------------------------
// OSバージョン取得
QString SysParam::getOsVer()
{
    return stPcInfo.strOsVer;
}
//---------------------------------------------------------------------------
//　出力タイプ取得
//　後々マシンからとってこれるようにしたい
QString SysParam::getLaserTypeString()
{
    quint16 lsrType = stSettingInfo.uiLaserType;
    switch(lsrType) {
    case MACHINE_TYPE_NONE:
    case MACHINE_TYPE_MINI_1_6W:
        return JSON_TAG_LASER_TYPE_1_6W;
    case MACHINE_TYPE_MINI_3_5W:
        return JSON_TAG_LASER_TYPE_3_5W;
    case MACHINE_TYPE_CO2:
        return JSON_TAG_LASER_TYPE_CO2;
    case MACHINE_TYPE_DS:
        return JSON_TAG_LASER_TYPE_DS;
    }

    return QString();
}
//---------------------------------------------------------------------------
