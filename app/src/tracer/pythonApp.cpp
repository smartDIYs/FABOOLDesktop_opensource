//---------------------------------------------------------------------------
#include "pythonApp.h"
//---------------------------------------------------------------------------
// 定数
#define DF_EXT_NETWORK_ADDRESS      "http://localhost:15555/"
#define DF_EXT_PY_APP               "python_app"
//---------------------------------------------------------------------------
PythonApp::PythonApp()
{
    // メンバ変数初期化
    m_pProc = NULL;
    m_pProc = new QProcess();
    m_pNetworkAccessManager = NULL;
    m_pNetworkAccessManager = new QNetworkAccessManager();
}
//---------------------------------------------------------------------------
PythonApp::~PythonApp()
{
    if (m_pNetworkAccessManager != NULL) {
        delete m_pNetworkAccessManager;
        m_pNetworkAccessManager = NULL;
    }
    if (m_pProc != NULL) {
        delete m_pProc;
        m_pProc = NULL;
    }
}
//---------------------------------------------------------------------------
quint16 PythonApp::startUpApp()
{
    QFileInfo fInfo(QCoreApplication::applicationFilePath());
    QString strExtPath = fInfo.absolutePath() + "/" + DF_EXT_PY_APP;
    // Pythonプログラム起動
    m_pProc->start(strExtPath);
    // 起動確認
    if (m_pProc->waitForStarted(1000) == false) {
        return ERR_PYTHONAPP_START_UP;
    }

    // 起動確認
    QString strRet;
    if (getAPI("check_start_up", &strRet) == false) {
        return ERR_PYTHONAPP_START_UP;
    }
    if (strRet != "__OK__") {
        return ERR_PYTHONAPP_START_UP;
    }

    return SUCCESS;
}
//---------------------------------------------------------------------------
quint16 PythonApp::exitApp()
{
    // 終了
    m_pNetworkAccessManager->get(QNetworkRequest(QUrl(DF_EXT_NETWORK_ADDRESS + QString("app_exit"))));
    QThread::sleep(1);

    // プログラムが終了していなかった場合Killする
    if (m_pProc->state() == QProcess::Running) {
        m_pProc->kill();
        if (m_pProc->waitForFinished(1000) == false) {
            return ERR_PYTHONAPP_EXIT;
        }
    }

    return SUCCESS;
}
//---------------------------------------------------------------------------
quint16 PythonApp::getAccessToken(QString* pstrAccessToken)
{
    if (getAPI("get_access_token", pstrAccessToken) == false) {
        return ERR_PYTHONAPP_ACCESS_TOKEN;
    }
    if (*pstrAccessToken == "") {
        return ERR_PYTHONAPP_ACCESS_TOKEN;
    }

    return SUCCESS;
}
//---------------------------------------------------------------------------
quint16 PythonApp::zipDir(const QString &strDirPath, const QString &strZipFilePath)
{
    QString strRet;
    QJsonObject json;
    json.insert("DirPath", strDirPath);
    json.insert("ZipFilePath", strZipFilePath);

    if (postAPI("zip_dir", json, &strRet) == false) {
        return ERR_PYTHONAPP_ZIP_DIR;
    }
    if (strRet != "__OK__") {
        return ERR_PYTHONAPP_ZIP_DIR;
    }

    return SUCCESS;
}
//---------------------------------------------------------------------------
bool PythonApp::getAPI(const QString &strAPI, QString* pstrRet)
{
    QEventLoop eventLoop;
    QTimer timer;
    QNetworkReply* pReply;

    // GET送信実行
    pReply = m_pNetworkAccessManager->get(QNetworkRequest(QUrl(DF_EXT_NETWORK_ADDRESS + strAPI)));
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    QObject::connect(pReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    timer.start(10000);  // 10秒でタイムアウト
    eventLoop.exec();
    if (timer.isActive()) {
        timer.stop();
        if (pReply->error() == QNetworkReply::NoError) {
            *pstrRet = pReply->readAll();
            return true;
        }
    } else {
        // タイムアウト
        QObject::disconnect(pReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        pReply->abort();
    }

    return false;
}
//---------------------------------------------------------------------------
bool PythonApp::postAPI(const QString &strAPI, const QJsonObject &json, QString* pstrRet)
{
    QNetworkRequest request;
    QEventLoop eventLoop;
    QTimer timer;
    QNetworkReply* pReply;

    // url設定
    request.setUrl(QUrl(DF_EXT_NETWORK_ADDRESS + strAPI));
    // ヘッダー設定
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    // POST送信実行
    pReply = m_pNetworkAccessManager->post(request, QJsonDocument(json).toJson());
    timer.setSingleShot(true);
    QObject::connect(&timer, SIGNAL(timeout()), &eventLoop, SLOT(quit()));
    QObject::connect(pReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
    timer.start(10000);  // 10秒でタイムアウト
    eventLoop.exec();
    if (timer.isActive()) {
        timer.stop();
        if (pReply->error() == QNetworkReply::NoError) {
            *pstrRet = pReply->readAll();
            return true;
        }
    } else {
        // タイムアウト
        QObject::disconnect(pReply, SIGNAL(finished()), &eventLoop, SLOT(quit()));
        pReply->abort();
    }

    return false;
}
//---------------------------------------------------------------------------
