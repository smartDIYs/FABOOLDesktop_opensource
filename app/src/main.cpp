
#include <QApplication>
#include <QTranslator>
#include <QSize>
#include <QResizeEvent>

#include "common/singleApplication.h"
#include "ui/mainDialog.h"
#include "common/common.h"
#include "common/sysParam.h"
#include "common/faboolApi.h"
#include "ui/dialogs/firstRegistrationDialog.h"

QString getLogFolderPath()
{
    QFileInfo fInfo(QCoreApplication::applicationFilePath());
    return fInfo.absolutePath() + "/log/";
}

void deleteLogFile(QString strFolderPath)
{
    // 削除しないファイル一覧
    QDate date1Date = QDate::currentDate();
    QDate date2Date = QDate::currentDate().addDays(-1);
    QDate date3Date = QDate::currentDate().addDays(-2);
    QDate date4Date = QDate::currentDate().addDays(-3);
    QDate date5Date = QDate::currentDate().addDays(-4);
    QDate date6Date = QDate::currentDate().addDays(-5);
    QDate date7Date = QDate::currentDate().addDays(-6);

    QStringList filtersNotDel;
    filtersNotDel << date1Date.toString("yyyyMMdd") + ".log";
    filtersNotDel << date2Date.toString("yyyyMMdd") + ".log";
    filtersNotDel << date3Date.toString("yyyyMMdd") + ".log";
    filtersNotDel << date4Date.toString("yyyyMMdd") + ".log";
    filtersNotDel << date5Date.toString("yyyyMMdd") + ".log";
    filtersNotDel << date6Date.toString("yyyyMMdd") + ".log";
    filtersNotDel << date7Date.toString("yyyyMMdd") + ".log";
    QDir dirNotDel = QDir(strFolderPath);
    dirNotDel.setNameFilters(filtersNotDel);
    dirNotDel.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList infoListNotDel = dirNotDel.entryInfoList();

    // 全てのlogファイル一覧
    QStringList filtersAll;
    filtersAll << "*.log";
    QDir dirAll = QDir(strFolderPath);
    dirAll.setNameFilters(filtersAll);
    dirAll.setFilter(QDir::Files | QDir::NoSymLinks);
    QFileInfoList infoListAll = dirAll.entryInfoList();

    // 削除
    foreach (QFileInfo fileInfoAll, infoListAll) {
        bool bDelFlg = true;
        foreach (QFileInfo fileInfoNotDel, infoListNotDel) {
            if (fileInfoAll.filePath() == fileInfoNotDel.filePath()) {
                bDelFlg = false;
                break;
            }
        }
        if (bDelFlg == true) {
            QFile::remove(fileInfoAll.filePath());
        }
    }
}

void loggerExtender(QtMsgType type, const QMessageLogContext &context, const QString & msg)
{
    SysParam* sysPara = SysParam::getInstance();
// LogSelector 切り替え
// Qtのログのレベルは　 debug:0  warning:1 　critical(error):2  fatal:3  info:4　　になっている点に注意
    QtMsgType selector = *(sysPara->getLogSelectorPointer());
    if(selector != QtDebugMsg) {
        if(type == QtDebugMsg) { return; }
        if(selector != QtInfoMsg ) {
            if(type == QtInfoMsg) { return; }
            if(selector != QtWarningMsg) {
                if(type == QtWarningMsg) { return; }
                if(selector != QtCriticalMsg) {
                    if(type == QtCriticalMsg) { return; }
                }
            }
        }
    }

    static QString path = getLogFolderPath();
    static bool dirExist = false;

// Log出力フォルダ確認
    if(dirExist == false)
        if (QDir().exists(path) == false) {
            QDir().mkdir(path);
        }
    dirExist = true;

// 古いログファイルの削除
    static bool isDeleteOldLog = false;
    if(isDeleteOldLog == false) {
        deleteLogFile(path);
    }
    isDeleteOldLog = true;

// textの書き出し
    QString txt;
    switch (type) {
    case QtDebugMsg:
        txt = QString("%1 [DEBUG] %2 Line:%3  %4\n").arg(QTime::currentTime().toString("HH:mm:ss:zzz")).arg(context.function).arg(context.line).arg(msg);
        break;
    case QtInfoMsg:
        txt = QString("%1 [INFO] %2 Line:%3  %4\n").arg(QTime::currentTime().toString("HH:mm:ss:zzz")).arg(context.function).arg(context.line).arg(msg);
        break;
    case QtWarningMsg:
        txt = QString("%1 [WARN] %2 Line:%3  %4\n").arg(QTime::currentTime().toString("HH:mm:ss:zzz")).arg(context.function).arg(context.line).arg(msg);
        break;
    case QtCriticalMsg:
        txt = QString("%1 [ERR] %2 Line:%3  %4\n").arg(QTime::currentTime().toString("HH:mm:ss:zzz")).arg(context.function).arg(context.line).arg(msg);
        break;
    case QtFatalMsg:
        txt = QString("%1 [FATAL] %2 Line:%3  %4\n").arg(QTime::currentTime().toString("HH:mm:ss:zzz")).arg(context.function).arg(context.line).arg(msg);
        break;
    default:
        txt = QString("%1 [WARN] %2 Line:%3  %4\n").arg(QTime::currentTime().toString("HH:mm:ss:zzz")).arg(context.function).arg(context.line).arg(msg);
        break;
    }

    static QString filePath = path + QDate::currentDate().toString("yyyyMMdd") + ".log";
    static QFile mLogFile(filePath);

// open Closeはコストがかかるので開きっぱなしにしておく
    static QMutex mutex;
    QMutexLocker locker(&mutex);

    if(mLogFile.isOpen() == false)
    { mLogFile.open(QIODevice::WriteOnly | QIODevice::Append); }

    QTextStream out(&mLogFile);
    out << txt ;
}

int initializeSettings(SysParam* instance)
{
    DEBUG_STRING("Setting initialize in.");
    if(instance->load() != SUCCESS) {

        FirstRegistrationDialog* firstDialog = new FirstRegistrationDialog(NULL);

        int pushed = firstDialog->exec();
        if(pushed == QDialog::Rejected) {
            INFO_STRING("First registration exit.");
            return ERR_SETTING_FILE_OPEN;
        }
        INFO_STRING("First registration done.");

        quint16 lsrType     = firstDialog->laserType();
        instance->setLaserType(lsrType);

    }
    INFO_STRING("Setting initialize done.");
    return SUCCESS;
}

//void  finalizeSettings(SysParam* instance)
//{
//    delete instance;
//}

bool checkDate()
{
    bool isLimit = false;
    QDate date = QDate::currentDate();
    int year = date.year();
    int month = date.month();
    int day = date.day();

    if(year > 2018 || month > 2 || day > 15)
    { isLimit = true; }

    return isLimit;
}

int main(int argc, char *argv[])
{
    QApplication application(argc, argv);

//　翻訳ファイルの適用
    QTranslator translator;
    QString langFileName;
    QString langFileDefault;
    QString appPath = QApplication::applicationDirPath();
    langFileName  = appPath + "/translations/" + APP_NAME_TRANSLATE;
    langFileDefault = langFileName + "en";
    langFileName += QLocale::system().name();
    if(translator.load(langFileName)) {
        application.installTranslator(&translator);
    } else {
        if(translator.load(langFileDefault))
        {application.installTranslator(&translator);}
    }

// システムパラメータ＆ログシステムの設定
    SysParam* sysPara = SysParam::getInstance();
#ifndef QT_DEBUG
    qInstallMessageHandler(loggerExtender);
#endif
    if(initializeSettings(sysPara) != SUCCESS)
    { return -1; }

    MainDialog w;
    w.show();

// 起動時のリサイズイベントを発生させてGraphicArea等のサイズ調整を行う
    QSize *sz = new QSize( 640, 480 );
    QApplication::sendEvent( &w, new QResizeEvent( *sz, w.size() ));

    INFO_STRING("Application in.");
    int ret = application.exec();
    INFO_STRING("Application exit.");

    sysPara->save();
//    finalizeSettings(sysPara);

    return ret;
}
