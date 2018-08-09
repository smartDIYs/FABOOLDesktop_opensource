//---------------------------------------------------------------------------
#include <QString>
#include <QtTest>
#include <QCoreApplication>
#include <QApplication>
//---------------------------------------------------------------------------
class FaboolTest;
#define QTTEST_OBJECT friend class FaboolTest;
//---------------------------------------------------------------------------
#include "common/common.h"
#include "common/sysParam.h"
#include "tracer/bigQuery.h"
#include "tracer/pythonApp.h"
#include "transfer/svgPath.h"
#include "transfer/svgParser.h"
#include "transfer/imgParser.h"

Q_DECLARE_METATYPE(QtMsgType)
//---------------------------------------------------------------------------
class FaboolTest : public QObject
{
    Q_OBJECT

public:
    FaboolTest();

private Q_SLOTS:
    void initTestCase();
    void cleanupTestCase();

    void testSysParam_data();
    void testSysParam();

    void testBigQuery_data();
    void testBigQuery();

    void testPythonApp_data();
    void testPythonApp();

    void testSvgColor_data();
    void testSvgColor();

    void testSvgTransform_data();
    void testSvgTransform();

    void testSvgPathEle_data();
    void testSvgPathEle();

    void testSvgPath_data();
    void testSvgPath();

    void testSvgNode_data();
    void testSvgNode();

    void testSvgNodeBBox_data();
    void testSvgNodeBBox();

    void testSvgParserBase_data();
    void testSvgParserBase();

    void testSvgParserViewBox_data();
    void testSvgParserViewBox();

    void testSvgParserColor_data();
    void testSvgParserColor();

    void testSvgParserStr_data();
    void testSvgParserStr();

    void testSvgParserSubPoints_data();
    void testSvgParserSubPoints();

    void testSvgParserSeparationColor_data();
    void testSvgParserSeparationColor();

    void testSvgParserGCode_data();
    void testSvgParserGCode();

    void testImgPaser_data();
    void testImgPaser();

    void testImgPaserGCode_data();
    void testImgPaserGCode();

private:
    bool makeFile(QString strFilePath, QString strOut);
    bool qrealEqual(qreal d1, qreal d2);
    bool qrealEqual2(qreal d1, qreal d2);
    PythonApp*   m_pPythonApp;
};
//---------------------------------------------------------------------------
FaboolTest::FaboolTest()
{
}
//---------------------------------------------------------------------------
void FaboolTest::initTestCase()
{
    m_pPythonApp = new PythonApp();
    QVERIFY2(m_pPythonApp->startUpApp() == SUCCESS, "外部プログラム起動失敗");
}
//---------------------------------------------------------------------------
void FaboolTest::cleanupTestCase()
{
    QVERIFY2(m_pPythonApp->exitApp() == SUCCESS, "外部プログラム終了失敗");
    delete m_pPythonApp;
}
//---------------------------------------------------------------------------
void FaboolTest::testSysParam_data()
{
    QTest::addColumn<QSize>("sWorkArea");
    QTest::addColumn<quint16>("uiDriverPower");
    QTest::addColumn<QtMsgType>("enLogLevelSelector");
    QTest::addColumn<QString>("strUserName");
    QTest::addColumn<QString>("strMail");

    QTest::newRow("input1")<< QSize(400, 300) << quint16(800) << QtCriticalMsg << QString("有井佳也") << QString("yoshiya.arii@smartdiys.com");
    QTest::newRow("input2")<< QSize(3000, 3000) << quint16(3000) << QtDebugMsg << QString("Yoshiya Arii") << QString("yoshiya2010@gmail.com");
}
//---------------------------------------------------------------------------
void FaboolTest::testSysParam()
{
    QFETCH(QSize, sWorkArea);
    QFETCH(quint16, uiDriverPower);
    QFETCH(QtMsgType, enLogLevelSelector);
    QFETCH(QString, strUserName);
    QFETCH(QString, strMail);

    SysParam* pSysParam = new SysParam();

    QString strFilePath = pSysParam->getSysFilePath();

    // システムファイルが存在する場合は削除する
    if (QFile::exists(strFilePath)) {
        QVERIFY2(QFile::remove(strFilePath), "ファイル削除失敗");
    }
    // 初期化
    pSysParam->init();

    QVERIFY2(pSysParam->getMacAddress() != "", "MACアドレス取得失敗");
    QVERIFY2(pSysParam->getMacAddress() != "0.0.0.0", "IPアドレス取得失敗");
    QVERIFY2(pSysParam->getHostName() != "", "ホスト名取得失敗");
    QVERIFY2(pSysParam->getCpuType() != "", "CPUタイプ取得失敗");
    QVERIFY2(pSysParam->getOsName() != "", "OS名取得失敗");
    QVERIFY2(pSysParam->getOsVer() != "", "OSバージョン取得失敗");

    // システムファイルから読み込む
    QVERIFY2(pSysParam->load() == ERR_SETTING_FILE_OPEN, "ファイル読み込み失敗");

    // UUID確認
    QUuid u = pSysParam->getUUID();
    QVERIFY2(!u.isNull(), "UUID作成失敗");

    // データー設定
    pSysParam->setWorkArea(sWorkArea);
    pSysParam->setDriverPower(uiDriverPower);
    pSysParam->setLogLevelSelector(enLogLevelSelector);
    pSysParam->setUserName(strUserName);
    pSysParam->setMail(strMail);

    // システムファイルへ書き込み
    QVERIFY2(pSysParam->save() == SUCCESS, "ファイル書き込み失敗");

    // ファイル存在確認
    QVERIFY2(QFile::exists(strFilePath), "ファイル作成失敗");

    // データー初期化
    pSysParam->init();

    // システムファイルから読み込む
    QVERIFY2(pSysParam->load() == SUCCESS, "ファイル読み込み失敗");

    // 値確認
    QCOMPARE(sWorkArea, pSysParam->getWorkArea());
    QCOMPARE(uiDriverPower, pSysParam->getDriverPower());
    QCOMPARE(enLogLevelSelector, pSysParam->getLogLevelSelector());
    QCOMPARE(strUserName, pSysParam->getUserName());
    QCOMPARE(strMail, pSysParam->getMail());
    QCOMPARE(u, pSysParam->getUUID());

    delete pSysParam;
}
////---------------------------------------------------------------------------
//void FaboolTest::testLogger_data()
//{
//}
////---------------------------------------------------------------------------
//void FaboolTest::testLogger()
//{
//    Logger* pLogger = new Logger();
//    QString strFolderPath = pLogger->getLogFolderPath();

//    //-------------------------------------------------------------
//    // 古いLogファイル削除確認
//    //-------------------------------------------------------------
//    // 全logファイル削除
//    QStringList filters;
//    filters << "*.log";
//    QDir dir = QDir(strFolderPath);
//    dir.setNameFilters(filters);
//    dir.setFilter(QDir::Files | QDir::NoSymLinks);
//    QFileInfoList infoList = dir.entryInfoList();
//    foreach (QFileInfo fileInfo, infoList) {
//        QFile::remove(fileInfo.filePath());
//    }
//    // ファイル作成
//    QString strFile1Path = strFolderPath + QDate::currentDate().toString("yyyyMMdd") + ".log";
//    QString strFile2Path = strFolderPath + QDate::currentDate().addDays(-6).toString("yyyyMMdd") + ".log";
//    QString strFile3Path = strFolderPath + QDate::currentDate().addDays(-7).toString("yyyyMMdd") + ".log";
//    QString strFile4Path = strFolderPath + QDate::currentDate().addMonths(-1).toString("yyyyMMdd") + ".log";
//    QString strFile5Path = strFolderPath + QDate::currentDate().addMonths(-4).toString("yyyyMMdd") + ".log";
//    QVERIFY2(makeFile(strFile1Path, "dummy\n"), "ファイル1作成失敗");
//    QVERIFY2(makeFile(strFile2Path, "dummy\n"), "ファイル2作成失敗");
//    QVERIFY2(makeFile(strFile3Path, "dummy\n"), "ファイル3作成失敗");
//    QVERIFY2(makeFile(strFile4Path, "dummy\n"), "ファイル4作成失敗");
//    QVERIFY2(makeFile(strFile5Path, "dummy\n"), "ファイル5作成失敗");

//    // 古いファイル削除
//    pLogger->deleteLogFile(strFolderPath);

//    // 削除確認
//    QVERIFY2(QFile::exists(strFile1Path), "削除1失敗");
//    QVERIFY2(QFile::exists(strFile2Path), "削除2失敗");
//    QVERIFY2(QFile::exists(strFile3Path) == false, "削除3失敗");
//    QVERIFY2(QFile::exists(strFile4Path) == false, "削除4失敗");
//    QVERIFY2(QFile::exists(strFile5Path) == false, "削除5失敗");

//    //-------------------------------------------------------------
//    // Logファイル出力確認
//    //-------------------------------------------------------------
//    QString strLogFilePath = strFolderPath + QDate::currentDate().toString("yyyyMMdd") + ".log";
//    // logファイル削除
//    QFile::remove(strLogFilePath);
//    // log出力セレクタ設定
//    pLogger->setLogSelector(EN_LOG_LEVEL_DEBUG);
//    // log出力
//    QVERIFY2(pLogger->write(__FILE__, __FUNCTION__, __LINE__, EN_LOG_LEVEL_INFO, "log messege") == SUCCESS, "log出力1失敗");
//    // logファイル確認
//    QVERIFY2(QFile::exists(strLogFilePath), "log出力2失敗");
//    //-------------------------------------------------------------
//    // logファイル削除
//    QFile::remove(strLogFilePath);
//    // log出力セレクタ設定
//    pLogger->setLogSelector(EN_LOG_LEVEL_FATAL);
//    // log出力
//    QVERIFY2(pLogger->write(__FILE__, __FUNCTION__, __LINE__, EN_LOG_LEVEL_INFO, "log messege") == SUCCESS, "log出力3失敗");
//    // logファイル確認
//    QVERIFY2(QFile::exists(strLogFilePath) == false, "log出力4失敗");
//    //-------------------------------------------------------------

//    delete pLogger;
//}
//---------------------------------------------------------------------------
bool FaboolTest::makeFile(QString strFilePath, QString strOut)
{
    QFile* pFile = new QFile();

    pFile->setFileName(strFilePath);
    if (pFile->open(QIODevice::WriteOnly | QIODevice::Text) == false) {
        delete pFile;
        return false;
    }
    QTextStream out(pFile);
    out.setCodec("UTF-8");
    out << strOut;

    pFile->close();
    delete pFile;

    return true;
}
//---------------------------------------------------------------------------
void FaboolTest::testBigQuery_data()
{
}
//---------------------------------------------------------------------------
void FaboolTest::testBigQuery()
{
    BigQuery* pBigQuery = new BigQuery();
    QString strAccessToken;

    //---------------------------------------------------------------------------
    // JSONファイル確認
    QString strUserFilePath;
    QString strProcessFilePath;
    pBigQuery->getJsonFilePath(&strUserFilePath, &strProcessFilePath);
    // JSONファイル削除
    if (QFile::exists(strUserFilePath)) {
        QFile::remove(strUserFilePath);
    }
    if (QFile::exists(strProcessFilePath)) {
        QFile::remove(strProcessFilePath);
    }
    // JSON設定
    QJsonObject     jsUser;
    jsUser.insert("uuid", "test");
    jsUser.insert("name", "XXXXXX");
    jsUser.insert("mail_address", "XXX@gmail.com");
    jsUser.insert("mac_address", "FF:FF:FF:FF:FF:FF");
    jsUser.insert("cpu_type", "i386");
    jsUser.insert("os_name", "windows");
    jsUser.insert("os_ver", "10");

    QJsonObject     jsProcess;
    QJsonArray      jsArrProcess;
    jsProcess.insert("uuid", "test");
    jsProcess.insert("use_machine", "fabool");
    jsProcess.insert("ip_address", "127.0.0.1");
    jsProcess.insert("process_start_date_time", "2015-11-11T11:11:11");
    jsProcess.insert("process_total_time", "11:11:11");
    jsProcess.insert("process_material_type", "wood");
    jsProcess.insert("process_material_thick", "450");
    jsProcess.insert("process_area_width", "300");
    jsProcess.insert("process_area_height", "200");
    jsProcess.insert("process_data_size", "123");
    jsProcess.insert("process_method", "1,0,1,1");
    jsProcess.insert("process_fille_extension", "svg,dxf");
    QJsonObject rowProcess;
    rowProcess.insert("json", jsProcess);
    jsArrProcess.append(rowProcess);

    // メンバ変数に設定
    pBigQuery->m_jsUserBk = jsUser;
    pBigQuery->m_jsArrProcessBk = jsArrProcess;

    // 書き込み
    QVERIFY2(pBigQuery->saveJson(), "JSONファイル書き込み1失敗");
    QVERIFY2(QFile::exists(strUserFilePath), "JSONファイル書き込み2失敗");
    QVERIFY2(QFile::exists(strProcessFilePath), "JSONファイル書き込み3失敗");

    // メンバ変数クリア
    pBigQuery->m_jsUserBk = QJsonObject();
    pBigQuery->m_jsArrProcessBk = QJsonArray();

    // 読み込み
    QVERIFY2(pBigQuery->loadJson(), "JSONファイル読み込み1失敗");
    QVERIFY2(pBigQuery->m_jsUserBk["uuid"].toString() == "test", "JSONファイル読み込み2失敗");
    QJsonObject retRow = pBigQuery->m_jsArrProcessBk[0].toObject();
    QJsonObject retJson = retRow["json"].toObject();
    QVERIFY2(retJson["uuid"].toString() == "test", "JSONファイル読み込み3失敗");

    //---------------------------------------------------------------------------
    // アクセストークン取得
    QBENCHMARK {
        QVERIFY2(m_pPythonApp->getAccessToken(&strAccessToken) == SUCCESS, "アクセストークン取得失敗");
    }

    // テーブル追加
    ST_BIGQ_PROCESS stBigqProc;

    stBigqProc.uuid = QUuid::createUuid();
    stBigqProc.use_machine = "fabool";
    stBigqProc.ip_address = "127.0.0.1";
    stBigqProc.process_start_date_time = QDateTime::currentDateTime();
    stBigqProc.process_total_time = QTime::currentTime();
    stBigqProc.process_material_type = "wood";
    stBigqProc.process_material_thick = 34;
    stBigqProc.process_area_width = 300;
    stBigqProc.process_area_height = 200;
    stBigqProc.process_data_size = 123;
    stBigqProc.process_method = "0,0,0,0,0,0,1,0";
    stBigqProc.process_file_extension = "png,svg,svg,jpeg";

    QBENCHMARK {
        QVERIFY2(pBigQuery->sendProcessTable(strAccessToken, stBigqProc) == SUCCESS, "テーブル追加失敗1");
    }
    QVERIFY2(pBigQuery->m_jsUserBk.size() == 0, "テーブル追加失敗2");
    QVERIFY2(pBigQuery->m_jsArrProcessBk.count() == 0, "テーブル追加失敗3");
    //---------------------------------------------------------------------------

    delete pBigQuery;
}
//---------------------------------------------------------------------------
void FaboolTest::testPythonApp_data()
{
}
//---------------------------------------------------------------------------
void FaboolTest::testPythonApp()
{
    QFileInfo fInfo(QCoreApplication::applicationFilePath());
    QString strFolderPath;
    QString strFilePath;

    strFolderPath = fInfo.absolutePath() + "/tmp/";
    strFilePath = fInfo.absolutePath() + "/tmp.zip";

    if (QDir().exists(strFolderPath) == true) {
        // 全ファイル削除
        QStringList filters;
        filters << "*.*";
        QDir dir = QDir(strFolderPath);
        dir.setNameFilters(filters);
        dir.setFilter(QDir::Files | QDir::NoSymLinks);
        QFileInfoList infoList = dir.entryInfoList();
        foreach (QFileInfo fileInfo, infoList) {
            QFile::remove(fileInfo.filePath());
        }
    }
    else {
        // フォルダ作成
        QDir().mkdir(strFolderPath);
    }

    // ファイル作成
    QVERIFY2(makeFile(strFolderPath + "xxx.txt", "dummy\n"), "ファイル1作成失敗");
    QVERIFY2(makeFile(strFolderPath + "yyy.txt", "dummy\n"), "ファイル1作成失敗");
    QVERIFY2(makeFile(strFolderPath + "zzz.txt", "dummy\n"), "ファイル1作成失敗");

    // 圧縮ファイルがある場合は削除
    if (QFile::exists(strFilePath) == true) {
         QFile::remove(strFilePath);
    }
    // 圧縮実行
    QBENCHMARK {
        QVERIFY2(m_pPythonApp->zipDir(strFolderPath, strFilePath) == SUCCESS, "圧縮ファイル作成1失敗");
    }
    QVERIFY2(QFile::exists(strFilePath), "圧縮ファイル作成2失敗");

}
//---------------------------------------------------------------------------
void FaboolTest::testSvgColor_data()
{
    QTest::addColumn<QString>("colorData");
    QTest::addColumn<QString>("colorResult");

    QTest::newRow("input1")<< QString("none") << QString("none");
    QTest::newRow("input2")<< QString("xsdqfq") << QString("none");
    QTest::newRow("input3")<< QString("#456") << QString("#445566");
    QTest::newRow("input4")<< QString("#123456") << QString("#123456");
    QTest::newRow("input5")<< QString("rgb(125,126,127)") << QString("#7d7e7f");
    QTest::newRow("input6")<< QString("rgb(50%,60%,70%)") << QString("#7f98b2");
    QTest::newRow("input7")<< QString("rgba(115,126,137,50)") << QString("#737e89");
    QTest::newRow("input8")<< QString("rgba(12%,13%,14%,30)") << QString("#1e2123");
    QTest::newRow("input9")<< QString("darkgreen") << QString("#006400");
    QTest::newRow("input10")<< QString("lightblue") << QString("#add8e6");

}
//---------------------------------------------------------------------------
void FaboolTest::testSvgColor()
{
    QFETCH(QString, colorData);
    QFETCH(QString, colorResult);

    SvgColor    svgColor;
    svgColor.setColor(colorData);
    SvgColor    svgColorCopy;
    svgColorCopy = svgColor;

    // 値確認
    QCOMPARE(colorResult, svgColor.toString());
    QCOMPARE(colorResult, svgColorCopy.toString());
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgTransform_data()
{
    QList<qreal>    dTransForm;
    QTest::addColumn<QString>("strSubject");
    QTest::addColumn<QList<qreal>>("dTransForm");

    dTransForm.clear();
    dTransForm.append(0.43239536);
    dTransForm.append(0.22065049);
    dTransForm.append(-0.22065049);
    dTransForm.append(0.43239536);
    dTransForm.append(-454.46003);
    dTransForm.append(404.8582);
    QTest::newRow("input1")<< QString("matrix(0.43239536,0.22065049,-0.22065049,0.43239536,-454.46003,404.8582)") << dTransForm;

    dTransForm.clear();
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    dTransForm.append(1.0);
    dTransForm.append(-35.134816);
    dTransForm.append(-34.164228);
    QTest::newRow("input2")<< QString("translate(-35.134816,-34.164228)") << dTransForm;

    dTransForm.clear();
    dTransForm.append(0.009390379136024609);
    dTransForm.append(-0.9999559094178512);
    dTransForm.append(0.9999559094178512);
    dTransForm.append(0.009390379136024609);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    QTest::newRow("input3")<< QString("rotate(-89.461963)") << dTransForm;

    dTransForm.clear();
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(153.0);
    QTest::newRow("input4")<< QString("translate(0,153)") << dTransForm;

    dTransForm.clear();
    dTransForm.append(0.26458333);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    dTransForm.append(0.26458333);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    QTest::newRow("input5")<< QString("scale(0.26458333)") << dTransForm;

    dTransForm.clear();
    dTransForm.append(0.00726432020461593);
    dTransForm.append(-0.7735576818876333);
    dTransForm.append(0.7735576818876333);
    dTransForm.append(0.00726432020461593);
    dTransForm.append(101.03664932804725);
    dTransForm.append(209.0281746988181);
    QTest::newRow("input6")<< QString("matrix(0.77359179,0,0,0.77359179,9.8623292,234.3459) translate(-35.134816,-34.164228) rotate(-89.461963) translate(0,153)") << dTransForm;
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgTransform()
{
    QFETCH(QString, strSubject);
    QFETCH(QList<qreal>, dTransForm);

    SvgTransform    svgTransform;

    QVERIFY2(svgTransform.setTransform(strSubject, EN_SVG_UNIT_PX, 90.0), "トランスフォーム失敗");

    // 値確認
    QCOMPARE(svgTransform.m_dTransForm[0], dTransForm[0]);
    QCOMPARE(svgTransform.m_dTransForm[1], dTransForm[1]);
    QCOMPARE(svgTransform.m_dTransForm[2], dTransForm[2]);
    QCOMPARE(svgTransform.m_dTransForm[3], dTransForm[3]);
    QCOMPARE(svgTransform.m_dTransForm[4], dTransForm[4]);
    QCOMPARE(svgTransform.m_dTransForm[5], dTransForm[5]);
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgNode_data()
{
    ST_SVG_ELE_COM  stCom;

    QTest::addColumn<QString>("strSvg");
    QTest::addColumn<ST_SVG_ELE_COM>("stCom");

    stCom.fill.setColor("none");
    stCom.stroke.setColor("#000000");
    stCom.transform.initMenber();
    stCom.transform.setTransform("matrix(1,0,0,1,0,0)", EN_SVG_UNIT_PX, 90.0);
    stCom.display = true;
    QTest::newRow("input1")<< QString("circle.svg") << stCom;

    stCom.fill.setColor("none");
    stCom.stroke.setColor("#670000");
    stCom.transform.initMenber();
    stCom.transform.setTransform("matrix(1,0,0,1,0,0)", EN_SVG_UNIT_MM, 90.0);
    stCom.display = true;
    QTest::newRow("input2")<< QString("ellipse.svg") << stCom;

    stCom.fill.setColor("none");
    stCom.stroke.setColor("#000000");
    stCom.transform.initMenber();
    stCom.transform.setTransform("matrix(1,0,0,1,0,0)", EN_SVG_UNIT_PX, 72.0);
    stCom.display = true;
    QTest::newRow("input3")<< QString("line.svg") << stCom;

    stCom.fill.setColor("none");
    stCom.stroke.setColor("#ff0000");
    stCom.transform.initMenber();
    stCom.transform.setTransform("matrix(0.50455436,0,0,0.62419095,40.270464,15.703969)", EN_SVG_UNIT_PX, 90.0);
    stCom.display = true;
    QTest::newRow("input4")<< QString("path.svg") << stCom;

    stCom.fill.setColor("none");
    stCom.stroke.setColor("#000000");
    stCom.transform.initMenber();
    stCom.transform.setTransform("matrix(1,0,0,1,0,0)", EN_SVG_UNIT_PX, 72.0);
    stCom.display = true;
    QTest::newRow("input5")<< QString("polygon.svg") << stCom;

    stCom.fill.setColor("none");
    stCom.stroke.setColor("#9f0000");
    stCom.transform.initMenber();
    stCom.transform.setTransform("matrix(0.00899999,0,0,0.00899999,-73.45145,-67.742179)", EN_SVG_UNIT_PX, 90.0);
    stCom.display = true;
    QTest::newRow("input6")<< QString("polyline.svg") << stCom;

    stCom.fill.setColor("none");
    stCom.stroke.setColor("#00ff00");
    stCom.transform.initMenber();
    stCom.transform.setTransform("matrix(1.0,0,0,1.0,0,-308.267716535433)", EN_SVG_UNIT_PX, 90.0);
    stCom.display = true;
    QTest::newRow("input7")<< QString("rect.svg") << stCom;
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgNode()
{
    QFETCH(QString, strSvg);
    QFETCH(ST_SVG_ELE_COM, stCom);

    QFileInfo fInfo(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strSvg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    SvgParser   svgPaser;
    QVERIFY(svgPaser.loadFile(strFilePath));

    // 要素数確認
    QVERIFY(svgPaser.getPathCount() != 0);
    // 要素
    SvgPath svgPath = svgPaser.m_listPath.at(0);
    // fill
    QCOMPARE(svgPath.m_stCom.fill.color.name(), stCom.fill.color.name());
    // stroke
    QCOMPARE(svgPath.m_stCom.stroke.color.name(), stCom.stroke.color.name());
    // transform
    for (int i = 0; i < DF_SVG_TRANS_ITEM; ++i) {
        QVERIFY(qrealEqual(svgPath.m_stCom.transform.m_dTransForm[i], stCom.transform.m_dTransForm[i]));
    }
    // display
    QCOMPARE(svgPath.m_stCom.display, stCom.display);
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgNodeBBox_data()
{
    ST_SVG_BBOX stBBox;
    QTest::addColumn<QString>("strSvg");
    QTest::addColumn<ST_SVG_BBOX>("stBBox");

    stBBox.p1 = QPointF(50.0, 50.0);
    stBBox.p2 = QPointF(150.0, 150.0);
    QTest::newRow("input1")<< QString("bbox1.svg") << stBBox;

    stBBox.p1 = QPointF(9.449396133422852, 15.596715927124023);
    stBBox.p2 = QPointF(42.33333778381348, 47.15774154663086);
    QTest::newRow("input2")<< QString("bbox2.svg") << stBBox;

    stBBox.p1 = QPointF(501.7049865722656, 224.41200256347656);
    stBBox.p2 = QPointF(536.176025390625, 264.0589904785156);
    QTest::newRow("input3")<< QString("bbox3.svg") << stBBox;

    stBBox.p1 = QPointF(17.142860412597656, 21.192352294921875);
    stBBox.p2 = QPointF(371.4735794067383, 375.5230712890625);
    QTest::newRow("input4")<< QString("bbox4.svg") << stBBox;

    stBBox.p1 = QPointF(82.85713958740234, 78.07649230957031);
    stBBox.p2 = QPointF(425.7142868041992, 355.21937561035156);
    QTest::newRow("input5")<< QString("bbox5.svg") << stBBox;

    stBBox.p1 = QPointF(3.691349983215332, 6.646795272827148);
    stBBox.p2 = QPointF(7.130992889404297, 10.08643627166748);
    QTest::newRow("input6")<< QString("bbox6.svg") << stBBox;

    stBBox.p1 = QPointF(86.6989974975586, 110.0);
    stBBox.p2 = QPointF(222.95198822021484, 228.0);
    QTest::newRow("input7")<< QString("bbox7.svg") << stBBox;
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgNodeBBox()
{
    ST_SVG_BBOX stPathBBox;

    QFETCH(QString, strSvg);
    QFETCH(ST_SVG_BBOX, stBBox);

    QFileInfo fInfo(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strSvg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    SvgParser   svgPaser;
    QVERIFY(svgPaser.loadFile(strFilePath));

    // 要素数確認
    QVERIFY(svgPaser.getPathCount() != 0);
    // 要素
    SvgPath svgPath = svgPaser.m_listPath.at(0);
    QVERIFY(svgPath.getBBox(&stPathBBox));

    QVERIFY(qrealEqual2(stPathBBox.p1.x(), stBBox.p1.x()));
    QVERIFY(qrealEqual2(stPathBBox.p1.y(), stBBox.p1.y()));
    QVERIFY(qrealEqual2(stPathBBox.p2.x(), stBBox.p2.x()));
    QVERIFY(qrealEqual2(stPathBBox.p2.y(), stBBox.p2.y()));
}
//---------------------------------------------------------------------------
// 円弧→3次曲線変換確認
void FaboolTest::testSvgPathEle_data()
{
    ST_SVG_PATH_BEZIER stBezire;
    QList<ST_SVG_PATH_BEZIER>   listBezire;
    QTest::addColumn<QPointF>("p1");
    QTest::addColumn<QPointF>("p2");
    QTest::addColumn<QPointF>("r");
    QTest::addColumn<qreal>("xrot");
    QTest::addColumn<bool>("large");
    QTest::addColumn<bool>("sweep");
    QTest::addColumn<QList<ST_SVG_PATH_BEZIER>>("listBezire");

    listBezire.clear();
    stBezire.p1 = QPointF(7.1309928, 8.3666165);
    stBezire.p2 = QPointF(7.130991916344784, 9.316447192600197);
    stBezire.p3 = QPointF(6.361001492600479, 10.086436497056543);
    stBezire.p4 = QPointF(5.4111708, 10.086436);
    listBezire.append(stBezire);
    QTest::newRow("input1")<< QPointF(7.1309928, 8.3666165) << QPointF(5.4111708, 10.086436) << QPointF(1.7198211, 1.7198211) << 0.0 << false << true << listBezire;

    listBezire.clear();
    stBezire.p1 = QPointF(5.4111708, 6.6467965);
    stBezire.p2 = QPointF(6.36100168786246, 6.646796002943572);
    stBezire.p3 = QPointF(7.1309921924869535, 7.416785612137602);
    stBezire.p4 = QPointF(7.130992799999999, 8.3666165);
    listBezire.append(stBezire);
    QTest::newRow("input2")<< QPointF(5.4111708, 6.6467965) << QPointF(7.1309928, 8.3666165) << QPointF(1.7198211, 1.7198211) << 0.0 << false << true << listBezire;
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgPathEle()
{
    QFETCH(QPointF, p1);
    QFETCH(QPointF, p2);
    QFETCH(QPointF, r);
    QFETCH(qreal, xrot);
    QFETCH(bool, large);
    QFETCH(bool, sweep);
    QFETCH(QList<ST_SVG_PATH_BEZIER>, listBezire);

    SvgPathEle  stPathEle;

    QVERIFY(stPathEle.setArc(EN_SVG_PATH_ELE_A, p1, p2, r, xrot, large, sweep));
    QCOMPARE(stPathEle.m_pstArc->listBezire.length(), listBezire.length());
    for (int i = 0; i < listBezire.length(); ++i) {
        QCOMPARE(stPathEle.m_pstArc->listBezire[i].p1, listBezire[i].p1);
        QCOMPARE(stPathEle.m_pstArc->listBezire[i].p2, listBezire[i].p2);
        QCOMPARE(stPathEle.m_pstArc->listBezire[i].p3, listBezire[i].p3);
        QCOMPARE(stPathEle.m_pstArc->listBezire[i].p4, listBezire[i].p4);
    }
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgPath_data()
{
    SvgPathEle*         pstPathEle;
    QList<SvgPathEle>   listPathEle;
    QTest::addColumn<QString>("strD");
    QTest::addColumn<QList<SvgPathEle>>("listPathEle");

    listPathEle.clear();
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_M, QPointF(7.1309928, 8.3666165));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setArc(EN_SVG_PATH_ELE_A, QPointF(7.1309928, 8.3666165), QPointF(5.4111708, 10.086436), QPointF(1.7198211, 1.7198211), 0.0, false, true);
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setArc(EN_SVG_PATH_ELE_A, QPointF(5.4111708, 10.086436), QPointF(3.6913498, 8.3666165), QPointF(1.7198211, 1.7198211), 0.0, false, true);
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setArc(EN_SVG_PATH_ELE_A, QPointF(3.6913498, 8.3666165), QPointF(5.4111708, 6.6467965), QPointF(1.7198211, 1.7198211), 0.0, false, true);
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setArc(EN_SVG_PATH_ELE_A, QPointF(5.4111708, 6.6467965), QPointF(7.1309928, 8.3666165), QPointF(1.7198211, 1.7198211), 0.0, false, true);
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_Z, QPointF(7.1309928, 8.3666165));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    QTest::newRow("input1")<< QString("m 7.1309928,8.3666165 a 1.7198211,1.7198211 0 0 1 -1.719822,1.7198195 1.7198211,1.7198211 0 0 1 -1.719821,-1.7198195 1.7198211,1.7198211 0 0 1 1.719821,-1.71982 1.7198211,1.7198211 0 0 1 1.719822,1.71982 z") << listPathEle;

    listPathEle.clear();
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_M, QPointF(62.19, 76.094));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setBezier(EN_SVG_PATH_ELE_C, QPointF(62.19, 76.094), QPointF(62.946,76.944), QPointF(61.074,77.207), QPointF(60.99,78.014));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setBezier(EN_SVG_PATH_ELE_C, QPointF(60.99,78.014), QPointF(60.292, 77.222), QPointF(62.15, 76.943), QPointF(62.19, 76.094));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_Z, QPointF(62.19, 76.094));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    QTest::newRow("input2")<< QString("M62.19,76.094c0.756,0.85-1.116,1.113-1.2,1.92C60.292,77.222,62.15,76.943,62.19,76.094z") << listPathEle;

    listPathEle.clear();
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_M, QPointF(256.111, 413.773));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setBezier(EN_SVG_PATH_ELE_C, QPointF(256.111, 413.773), QPointF(254.968, 414.114), QPointF(253.584, 412.624), QPointF(252.751, 411.853));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setBezier(EN_SVG_PATH_ELE_C, QPointF(252.751, 411.853), QPointF(252.036, 411.937), QPointF(252.139, 412.839), QPointF(251.071, 412.573));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setBezier(EN_SVG_PATH_ELE_C, QPointF(251.071, 412.573), QPointF(252.407, 409.535), QPointF(254.311, 413.14), QPointF(256.111, 413.773));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_Z, QPointF(256.111, 413.773));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    QTest::newRow("input3")<< QString("M256.111,413.773c-1.143,0.341-2.527-1.149-3.36-1.92c-0.715,0.084-0.612,0.986-1.68,0.72C252.407,409.535,254.311,413.14,256.111,413.773z") << listPathEle;

    listPathEle.clear();
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_M, QPointF(118.5906, 386.733));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(118.5906, 386.733), QPointF(122.0281, 386.733), QPointF(124.5281, 384.233));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(124.5281, 384.233), QPointF(127.3406, 381.4205), QPointF(127.3406, 377.358));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(127.3406, 377.358), QPointF(127.3406, 373.2955), QPointF(125.1531, 371.108));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(125.1531, 371.108), QPointF(122.3406, 368.2955), QPointF(119.5281, 368.2955));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_H, QPointF(109.2156, 368.2955));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_V, QPointF(109.2156, 386.733));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_Z, QPointF(118.5906, 386.733));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_M, QPointF(108.9031, 362.358));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(108.9031, 362.358), QPointF(116.0906, 362.358), QPointF(119.8406, 366.108));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(119.8406, 366.108), QPointF(124.5281, 370.7955), QPointF(124.5281, 377.358));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(124.5281, 377.358), QPointF(124.5281, 383.2955), QPointF(121.0906, 386.733));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(121.0906, 386.733), QPointF(118.2781, 389.5455), QPointF(116.4031, 390.1705));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(116.4031, 390.1705), QPointF(119.8406, 392.0455), QPointF(122.0281, 394.233));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(122.0281, 394.233), QPointF(126.0906, 398.2955), QPointF(126.0906, 405.483));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(126.0906, 405.483), QPointF(126.0906, 413.2955), QPointF(122.0281, 417.358));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(122.0281, 417.358), QPointF(116.0906, 423.2955), QPointF(108.2781, 423.2955));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_H, QPointF(92.6531, 423.2955));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_V, QPointF(92.6531, 362.358));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_Z, QPointF(108.9031, 362.358));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_M, QPointF(83.5906, 392.6705));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_V, QPointF(83.5906, 417.358));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_H, QPointF(90.7781, 417.358));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(90.7781, 417.358), QPointF(96.0906, 417.358), QPointF(99.8406, 413.608));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(99.8406, 413.608), QPointF(102.9656, 410.483), QPointF(102.9656, 405.483));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(102.9656, 405.483), QPointF(102.9656, 399.858), QPointF(99.5281, 396.4205));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setQuadratic(EN_SVG_PATH_ELE_Q, QPointF(99.5281, 396.4205), QPointF(95.7781, 392.6705), QPointF(92.0281, 392.6705));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    pstPathEle = new SvgPathEle();
    pstPathEle->setPoint(EN_SVG_PATH_ELE_Z, QPointF(83.5906, 392.6705));
    listPathEle.append(*pstPathEle);
    delete pstPathEle;
    // -------------------
    QTest::newRow("input4")<< QString("m 118.5906,386.733 q 3.4375,0 5.9375,-2.5 2.8125,-2.8125 2.8125,-6.875 0,-4.0625 -2.1875,-6.25 -2.8125,-2.8125 -5.625,-2.8125 h -10.3125 v 18.4375 z m -0.3125,-24.375 q 7.1875,0 10.9375,3.75 4.6875,4.6875 4.6875,11.25 0,5.9375 -3.4375,9.375 -2.8125,2.8125 -4.6875,3.4375 3.4375,1.875 5.625,4.0625 4.0625,4.0625 4.0625,11.25 0,7.8125 -4.0625,11.875 -5.9375,5.9375 -13.75,5.9375 h -15.625 V 362.358 Z m -9.0625,30.3125 v 24.6875 h 7.1875 q 5.3125,0 9.0625,-3.75 3.125,-3.125 3.125,-8.125 0,-5.625 -3.4375,-9.0625 -3.75,-3.75 -7.5,-3.75 z") << listPathEle;
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgPath()
{
    QFETCH(QString, strD);
    QFETCH(QList<SvgPathEle>, listPathEle);

    SvgPath svgPath;
    QVERIFY(svgPath.loadPath(strD, EN_SVG_UNIT_PX, 90.0));

    QCOMPARE(svgPath.m_listPathEle.length(), listPathEle.length());

    for (int i = 0; i < svgPath.m_listPathEle.length(); ++i) {
        QCOMPARE(svgPath.m_listPathEle[i].m_enPathEle, listPathEle[i].m_enPathEle);
        switch (svgPath.m_listPathEle[i].m_enPathEle) {
        case EN_SVG_PATH_ELE_M:
        case EN_SVG_PATH_ELE_L:
        case EN_SVG_PATH_ELE_H:
        case EN_SVG_PATH_ELE_V:
        case EN_SVG_PATH_ELE_Z:
            QCOMPARE(svgPath.m_listPathEle[i].m_pstPoint->p1, listPathEle[i].m_pstPoint->p1);
            break;
        case EN_SVG_PATH_ELE_Q:
        case EN_SVG_PATH_ELE_T:
            QCOMPARE(svgPath.m_listPathEle[i].m_pstQuadratic->p1, listPathEle[i].m_pstQuadratic->p1);
            QCOMPARE(svgPath.m_listPathEle[i].m_pstQuadratic->p2, listPathEle[i].m_pstQuadratic->p2);
            QCOMPARE(svgPath.m_listPathEle[i].m_pstQuadratic->p3, listPathEle[i].m_pstQuadratic->p3);
            break;
        case EN_SVG_PATH_ELE_C:
        case EN_SVG_PATH_ELE_S:
            QCOMPARE(svgPath.m_listPathEle[i].m_pstBezier->p1, listPathEle[i].m_pstBezier->p1);
            QCOMPARE(svgPath.m_listPathEle[i].m_pstBezier->p2, listPathEle[i].m_pstBezier->p2);
            QCOMPARE(svgPath.m_listPathEle[i].m_pstBezier->p3, listPathEle[i].m_pstBezier->p3);
            QCOMPARE(svgPath.m_listPathEle[i].m_pstBezier->p4, listPathEle[i].m_pstBezier->p4);
            break;
        case EN_SVG_PATH_ELE_A:
            QCOMPARE(svgPath.m_listPathEle[i].m_pstArc->listBezire.length(), listPathEle[i].m_pstArc->listBezire.length());
            for (int j = 0; j < svgPath.m_listPathEle[i].m_pstArc->listBezire.length(); ++j) {
                QCOMPARE(svgPath.m_listPathEle[i].m_pstArc->listBezire[j].p1, listPathEle[i].m_pstArc->listBezire[j].p1);
                QCOMPARE(svgPath.m_listPathEle[i].m_pstArc->listBezire[j].p2, listPathEle[i].m_pstArc->listBezire[j].p2);
                QCOMPARE(svgPath.m_listPathEle[i].m_pstArc->listBezire[j].p3, listPathEle[i].m_pstArc->listBezire[j].p3);
                QCOMPARE(svgPath.m_listPathEle[i].m_pstArc->listBezire[j].p4, listPathEle[i].m_pstArc->listBezire[j].p4);
            }
            break;
        default:
            QVERIFY(false);
            break;
        }
    }
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserBase_data()
{
    QTest::addColumn<QString>("strSvg");
    QTest::addColumn<EN_SVG_SOFT>("enSoft");
    QTest::addColumn<qreal>("dDpi");
    QTest::addColumn<qreal>("dWidth");
    QTest::addColumn<qreal>("dHeight");
    QTest::addColumn<EN_SVG_UNIT>("uiUnit");

    QTest::newRow("input1")<< QString("circle.svg") << EN_SVG_SOFT_INKSCAPE << qreal(90.0) << qreal(744.09448819) << qreal(1052.3622047) << EN_SVG_UNIT_PX;
    QTest::newRow("input2")<< QString("ellipse.svg") << EN_SVG_SOFT_INKSCAPE << qreal(90.0) << qreal(744.09448819) << qreal(1052.3622047) << EN_SVG_UNIT_MM;
    QTest::newRow("input3")<< QString("line.svg") << EN_SVG_SOFT_ILLUSTRATOR << qreal(72.0) << qreal(841.89) << qreal(595.28) << EN_SVG_UNIT_PX;
    QTest::newRow("input4")<< QString("path.svg") << EN_SVG_SOFT_INKSCAPE << qreal(90.0) << qreal(744.09448819) << qreal(1052.3622047) << EN_SVG_UNIT_PX;
    QTest::newRow("input5")<< QString("polygon.svg") << EN_SVG_SOFT_ILLUSTRATOR << qreal(72.0) << qreal(595.279) << qreal(841.89) << EN_SVG_UNIT_PX;
    QTest::newRow("input6")<< QString("polyline.svg") << EN_SVG_SOFT_INKSCAPE << qreal(90.0) << qreal(744.09448819) << qreal(1052.3622047) << EN_SVG_UNIT_PX;
    QTest::newRow("input7")<< QString("rect.svg") << EN_SVG_SOFT_INKSCAPE << qreal(90.0) << qreal(1052.3622047) << qreal(744.09448819) << EN_SVG_UNIT_PX;
    QTest::newRow("input8")<< QString("Visio.svg") << EN_SVG_SOFT_VISIO << qreal(72.0) << qreal(841.8888) << qreal(595.27584) << EN_SVG_UNIT_PX;
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserBase()
{
    QFETCH(QString, strSvg);
    QFETCH(EN_SVG_SOFT, enSoft);
    QFETCH(qreal, dDpi);
    QFETCH(qreal, dWidth);
    QFETCH(qreal, dHeight);
    QFETCH(EN_SVG_UNIT, uiUnit);

    QFileInfo fInfo(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strSvg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    SvgParser   svgPaser;
    QVERIFY(svgPaser.loadFile(strFilePath));

    // 使用ソフト
    QCOMPARE(svgPaser.m_enSoft, enSoft);
    // DPI
    QCOMPARE(svgPaser.m_dDpi, dDpi);
    // 幅
    QVERIFY(qrealEqual(svgPaser.m_dWidth, dWidth));
    // 高さ
    QVERIFY(qrealEqual(svgPaser.m_dHeight, dHeight));
    // 単位
    QCOMPARE(svgPaser.m_enUnit, uiUnit);
    // 要素数確認
    QVERIFY(svgPaser.getPathCount() != 0);
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserViewBox_data()
{
    ST_SVG_VIEW_BOX stViewBox;
    QList<qreal>    dTransForm;
    QTest::addColumn<QString>("strSvg");
    QTest::addColumn<ST_SVG_VIEW_BOX>("stViewBox");
    QTest::addColumn<QList<qreal>>("dTransForm");

    stViewBox.x = 0.0;
    stViewBox.y = 0.0;
    stViewBox.width = 744.09448819;
    stViewBox.height = 1052.3622047;
    dTransForm.clear();
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    QTest::newRow("input1")<< QString("circle.svg") << stViewBox << dTransForm;

    stViewBox.x = 0.0;
    stViewBox.y = 0.0;
    stViewBox.width = 744.09448819;
    stViewBox.height = 1052.3622047;
    dTransForm.clear();
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    QTest::newRow("input2")<< QString("ellipse.svg") << stViewBox << dTransForm;

    stViewBox.x = 0.0;
    stViewBox.y = 0.0;
    stViewBox.width = 841.89;
    stViewBox.height = 595.28;
    dTransForm.clear();
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    QTest::newRow("input3")<< QString("line.svg") << stViewBox << dTransForm;

    stViewBox.x = 0.0;
    stViewBox.y = 0.0;
    stViewBox.width = 100.0;
    stViewBox.height = 100.0;
    dTransForm.clear();
    dTransForm.append(2.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    dTransForm.append(2.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    QTest::newRow("input4")<< QString("viewbox1.svg") << stViewBox << dTransForm;

    stViewBox.x = 100.0;
    stViewBox.y = 0.0;
    stViewBox.width = 100.0;
    stViewBox.height = 100.0;
    dTransForm.clear();
    dTransForm.append(2.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    dTransForm.append(2.0);
    dTransForm.append(-100.0);
    dTransForm.append(0.0);
    QTest::newRow("input5")<< QString("viewbox2.svg") << stViewBox << dTransForm;

    stViewBox.x = -50.0;
    stViewBox.y = 0.0;
    stViewBox.width = 200.0;
    stViewBox.height = 200.0;
    dTransForm.clear();
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    dTransForm.append(1.0);
    dTransForm.append(50.0);
    dTransForm.append(0.0);
    QTest::newRow("input6")<< QString("viewbox3.svg") << stViewBox << dTransForm;

    stViewBox.x = 0.0;
    stViewBox.y = 0.0;
    stViewBox.width = 100.0;
    stViewBox.height = 200.0;
    dTransForm.clear();
    dTransForm.append(2.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    dTransForm.append(1.0);
    dTransForm.append(0.0);
    dTransForm.append(0.0);
    QTest::newRow("input7")<< QString("viewbox4.svg") << stViewBox << dTransForm;
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserViewBox()
{
    QFETCH(QString, strSvg);
    QFETCH(ST_SVG_VIEW_BOX, stViewBox);
    QFETCH(QList<qreal>, dTransForm);

    QFileInfo fInfo(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strSvg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    SvgParser   svgPaser;
    QVERIFY(svgPaser.loadFile(strFilePath));

    // viewBox確認
    QVERIFY(qrealEqual(svgPaser.m_stViewBox.x, stViewBox.x));
    QVERIFY(qrealEqual(svgPaser.m_stViewBox.y, stViewBox.y));
    QVERIFY(qrealEqual(svgPaser.m_stViewBox.width, stViewBox.width));
    QVERIFY(qrealEqual(svgPaser.m_stViewBox.height, stViewBox.height));

    // transform確認
    for (int i = 0; i < DF_SVG_TRANS_ITEM; ++i) {
        QVERIFY(qrealEqual(svgPaser.m_stEleCom.transform.m_dTransForm[i], dTransForm[i]));
    }
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserColor_data()
{
    QColor color;
    QList<QColor> listColor;

    QTest::addColumn<QString>("strSvg");
    QTest::addColumn<QList<QColor>>("listColor");

    color.setRgb(0, 255, 0);
    listColor.append(color);
    color.setRgb(102, 144, 0);
    listColor.append(color);
    color.setRgb(255, 144, 0);
    listColor.append(color);
    color.setRgb(0, 0, 0);
    listColor.append(color);

    QTest::newRow("input1")<< QString("color.svg") << QList<QColor>(listColor);
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserColor()
{
    QFETCH(QString, strSvg);
    QFETCH(QList<QColor>, listColor);

    QFileInfo fInfo(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strSvg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    SvgParser   svgPaser;
    QVERIFY(svgPaser.loadFile(strFilePath));

    // 登録色数確認
    QCOMPARE(svgPaser.m_listColor.length(), listColor.length());

    // 色配列の確認
    for (int i = 0; i < svgPaser.m_listColor.length(); ++i) {
        QCOMPARE(svgPaser.m_listColor.at(i).name(), listColor.at(i).name());
    }
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserStr_data()
{
    QTest::addColumn<QString>("strSvg");

    QTest::newRow("input1")<< QString("bbox1.svg");
    QTest::newRow("input2")<< QString("bbox2.svg");
    QTest::newRow("input3")<< QString("bbox3.svg");
    QTest::newRow("input4")<< QString("bbox4.svg");
    QTest::newRow("input5")<< QString("bbox5.svg");
    QTest::newRow("input6")<< QString("bbox6.svg");
    QTest::newRow("input7")<< QString("bbox7.svg");
    QTest::newRow("input8")<< QString("circle.svg");
    QTest::newRow("input9")<< QString("color.svg");
    QTest::newRow("input10")<< QString("ellipse.svg");
    QTest::newRow("input11")<< QString("line.svg");
    QTest::newRow("input12")<< QString("path.svg");
    QTest::newRow("input13")<< QString("path2.svg");
    QTest::newRow("input13")<< QString("path3.svg");
    QTest::newRow("input14")<< QString("polygon.svg");
    QTest::newRow("input15")<< QString("polyline.svg");
    QTest::newRow("input16")<< QString("rect.svg");
    QTest::newRow("input17")<< QString("viewbox1.svg");
    QTest::newRow("input18")<< QString("viewbox2.svg");
    QTest::newRow("input19")<< QString("viewbox3.svg");
    QTest::newRow("input20")<< QString("viewbox4.svg");
    QTest::newRow("input21")<< QString("Visio.svg");
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserStr()
{
    QFETCH(QString, strSvg);

    QFileInfo fInfo(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strSvg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    SvgParser   svgPaser;
    QVERIFY(svgPaser.loadFile(strFilePath));


    QFileInfo fResultInfo(QCoreApplication::applicationDirPath() + "/result/");
    QString strResultFilePath = fResultInfo.absolutePath() + "/" + strSvg;

    if (QDir().exists(fResultInfo.absolutePath()) == false) {
        // フォルダ作成
        QDir().mkdir(fResultInfo.absolutePath());
    }

    QByteArray  bySvg = svgPaser.toByte();
    makeFile(strResultFilePath, bySvg);
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserSubPoints_data()
{
    QList<int>  listPointCnt;
    QList<QList<int>>  listCnt;

    QTest::addColumn<QString>("strSvg");
    QTest::addColumn<QList<QList<int>>>("listCnt");

    listCnt.clear();

    listPointCnt.clear();
    listPointCnt.append(5);
    listCnt.append(listPointCnt);

    listPointCnt.clear();
    listPointCnt.append(11);
    listCnt.append(listPointCnt);

    listPointCnt.clear();
    listPointCnt.append(5);
    listCnt.append(listPointCnt);

    listPointCnt.clear();
    listPointCnt.append(42);
    listCnt.append(listPointCnt);

    listPointCnt.clear();
    listPointCnt.append(42);
    listCnt.append(listPointCnt);

    listPointCnt.clear();
    listPointCnt.append(5);
    listCnt.append(listPointCnt);
    QTest::newRow("input1")<< QString("color.svg") << listCnt;


    listCnt.clear();

    listPointCnt.clear();
    listPointCnt.append(6);
    listPointCnt.append(7);
    listPointCnt.append(6);
    listPointCnt.append(8);
    listCnt.append(listPointCnt);

    listPointCnt.clear();
    listPointCnt.append(5);
    listCnt.append(listPointCnt);

    listPointCnt.clear();
    listPointCnt.append(14);
    listPointCnt.append(9);
    listPointCnt.append(4);
    listPointCnt.append(20);
    listPointCnt.append(59);
    listPointCnt.append(28);
    listPointCnt.append(22);
    listPointCnt.append(5);
    listPointCnt.append(60);
    listPointCnt.append(20);
    listPointCnt.append(39);
    listPointCnt.append(34);
    listPointCnt.append(46);
    listPointCnt.append(93);
    listCnt.append(listPointCnt);

    QTest::newRow("input2")<< QString("path4.svg") << listCnt;
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserSubPoints()
{
    QFETCH(QString, strSvg);
    QFETCH(QList<QList<int>>, listCnt);

    QFileInfo fInfo(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strSvg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    SvgParser   svgPaser;
    QVERIFY(svgPaser.loadFile(strFilePath));

    QCOMPARE(svgPaser.m_listPath.length(), listCnt.length());

    SvgTransform transform;
    transform.initMenber();
    QSubPointList* pList = new QSubPointList[svgPaser.m_listPath.length()];
    QBENCHMARK {
        svgPaser.getSubPoints(pList, transform);
    }

    for (int i = 0; i < svgPaser.m_listPath.length(); ++i) {
        QCOMPARE(pList[i].length(), listCnt[i].length());
        for (int j = 0; j < pList[i].length(); ++j) {
            QCOMPARE(pList[i][j].length(), listCnt[i][j]);
        }
    }
    delete[] pList;
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserSeparationColor_data()
{
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserSeparationColor()
{
    QString strSvg = QString("color.svg");

    QMap<QString, QList<int>> count;
    QList<int>  pointCnt;
    count.clear();

    pointCnt.clear();
    pointCnt.append(5);
    count["#000000"] = pointCnt;

    pointCnt.clear();
    pointCnt.append(5);
    pointCnt.append(11);
    count["#00ff00"] = pointCnt;

    pointCnt.clear();
    pointCnt.append(5);
    pointCnt.append(42);
    count["#669000"] = pointCnt;

    pointCnt.clear();
    pointCnt.append(42);
    count["#ff9000"] = pointCnt;

    QFileInfo fInfo(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strSvg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    SvgParser   svgPaser;
    QVERIFY(svgPaser.loadFile(strFilePath));

    SvgTransform transform;
    transform.initMenber();
    QSubPointList* pList = new QSubPointList[svgPaser.m_listPath.length()];
    svgPaser.getSubPoints(pList, transform);

    QMap<QString, QSubPointList> boundarys;
    svgPaser.separationColor(boundarys, pList);
    delete[] pList;

    QCOMPARE(boundarys.size(), count.size());

    QList<QString> key = count.keys();

    for (int i = 0; i < key.length(); ++i) {
        QCOMPARE(boundarys[key[i]].length(), count[key[i]].length());
        for (int j = 0; j < count[key[i]].length(); ++j) {
            QCOMPARE(boundarys[key[i]][j].length(), count[key[i]][j]);
        }
    }
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserGCode_data()
{
}
//---------------------------------------------------------------------------
void FaboolTest::testSvgParserGCode()
{
    QString strSvg = QString("color.svg");

    QFileInfo fInfo(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strSvg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    SvgParser   svgPaser;
    QVERIFY(svgPaser.loadFile(strFilePath));

    QList<ParamData*>   pathParam;
    QColor color;
    ParamData* Param;

    color.setNamedColor("#00ff00");
    Param = new ParamData(color);
    Param->setHeadSpeed(700);
    Param->setLaserPower(25);
    Param->setType(PARAM_USE_TYPE_ENGRAVE);
    Param->setProcessTimes(3);
    pathParam.append(Param);

    Param = new ParamData();
    Param->setHeadSpeed(700);
    color.setNamedColor("#669000");
    Param->setColor(color);
    Param->setLaserPower(25);
    Param->setType(PARAM_USE_TYPE_ENGRAVE);
    Param->setProcessTimes(3);
    pathParam.append(new ParamData(color));

    Param = new ParamData();
    Param->setHeadSpeed(700);
    color.setNamedColor("#ff9000");
    Param->setColor(color);
    Param->setLaserPower(25);
    Param->setType(PARAM_USE_TYPE_ENGRAVE);
    Param->setProcessTimes(3);
    pathParam.append(new ParamData(color));

    Param = new ParamData();
    Param->setHeadSpeed(700);
    color.setNamedColor("#000000");
    Param->setColor(color);
    Param->setLaserPower(25);
    Param->setType(PARAM_USE_TYPE_ENGRAVE);
    Param->setProcessTimes(3);
    pathParam.append(new ParamData(color));

    QStringList listGcode = svgPaser.toGcode(1.0, 0.0, 0.0, 1.0, 0.0, 0.0, pathParam);

    foreach(ParamData* para, pathParam) {
        delete para;
    }

//    for (int i = 0; i < listGcode.length(); ++i) {
//        qDebug() << listGcode[i];
//    }
}
//---------------------------------------------------------------------------
void FaboolTest::testImgPaser_data()
{
    QTest::addColumn<QString>("strImg");

    QTest::newRow("input1")<< QString("10x10.png");
    QTest::newRow("input2")<< QString("100x100.png");
    QTest::newRow("input3")<< QString("1000x1000.png");
    QTest::newRow("input4")<< QString("2000x2000.png");
    QTest::newRow("input5")<< QString("9999x9999.png");
    QTest::newRow("input6")<< QString("png.png");
    QTest::newRow("input7")<< QString("jpg.jpg");
    QTest::newRow("input8")<< QString("bmp.bmp");
    QTest::newRow("input9")<< QString("gif.gif");
    QTest::newRow("input10")<< QString("color-pattern.jpg");
}
//---------------------------------------------------------------------------
void FaboolTest::testImgPaser()
{
    QFETCH(QString, strImg);

    QFileInfo fInfo;
    QFileInfo fFile;

    fInfo.setFile(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strImg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    ImgParser   imgPaser;
    QVERIFY(imgPaser.loadFile(strFilePath));

    fInfo.setFile(QCoreApplication::applicationDirPath() + "/result/");
    fFile.setFile(strImg);
    QString strResultFilePath = fInfo.absolutePath() + "/" + fFile.baseName() + ".svg";

    if (QDir().exists(fInfo.absolutePath()) == false) {
        // フォルダ作成
        QDir().mkdir(fInfo.absolutePath());
    }

    QByteArray  bySvg = imgPaser.toByte();
    makeFile(strResultFilePath, bySvg);
}
//---------------------------------------------------------------------------
void FaboolTest::testImgPaserGCode_data()
{
    QTest::addColumn<QString>("strImg");

    QTest::newRow("input1")<< QString("color-pattern.jpg");
}
//---------------------------------------------------------------------------
void FaboolTest::testImgPaserGCode()
{
    QFETCH(QString, strImg);

    QFileInfo fInfo;

    fInfo.setFile(QCoreApplication::applicationDirPath() + "/../../test/data/");
    QString strFilePath = fInfo.absolutePath() + "/" + strImg;

    // ファイル存在確認
    QVERIFY(QFile::exists(strFilePath));

    // ファイル読み込み
    ImgParser   imgPaser;
    QVERIFY(imgPaser.loadFile(strFilePath));

    QList<ParamData*>   pathParam;
    QColor color;
    ParamData* Param;

    color.setNamedColor("#00ff00");
    Param = new ParamData(color);
    Param->setHeadSpeed(700);
    Param->setLaserPower(25);
    Param->setType(PARAM_USE_TYPE_ENGRAVE);
    Param->setProcessTimes(1);
    pathParam.append(Param);

    QStringList listGcode = imgPaser.toGcode(1.0, 0.0, 0.0, 1.0, 0.0, 0.0, pathParam);

    foreach(ParamData* para, pathParam) {
        delete para;
    }

//    for (int i = 0; i < listGcode.length(); ++i) {
//        qDebug() << listGcode[i];
//    }
}
//---------------------------------------------------------------------------
bool FaboolTest::qrealEqual(qreal d1, qreal d2)
{
    if (qFabs(d1 - d2) > 1.192092896e-07) {
        qDebug() << QString::number(d1);
        qDebug() << QString::number(d2);
        return false;;
    }

    return true;
}
//---------------------------------------------------------------------------
bool FaboolTest::qrealEqual2(qreal d1, qreal d2)
{
    if (qFabs(d1 - d2) > 1.192092896e-03) {
        qDebug() << QString::number(d1);
        qDebug() << QString::number(d2);
        return false;;
    }

    return true;
}
//---------------------------------------------------------------------------
QTEST_MAIN(FaboolTest)

#include "tst_fabooltest.moc"
//---------------------------------------------------------------------------
