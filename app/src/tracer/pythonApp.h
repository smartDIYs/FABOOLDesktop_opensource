//---------------------------------------------------------------------------
// 外部プログラム(Python)管理
//---------------------------------------------------------------------------
#ifndef PYTHONAPP_H
#define PYTHONAPP_H
//---------------------------------------------------------------------------
#include <QtCore>
#include <QtNetwork>
#include "common/common.h"
//---------------------------------------------------------------------------
class PythonApp
{
    QTTEST_OBJECT

public:
    PythonApp();
    ~PythonApp();
    quint16 startUpApp();
    quint16 exitApp();
    quint16 getAccessToken(QString* pstrAccessToken);
    quint16 zipDir(const QString &strDirPath, const QString &strZipFilePath);


protected:
    bool getAPI(const QString &strAPI, QString* pstrRet);
    bool postAPI(const QString &strAPI, const QJsonObject &json, QString* pstrRet);


private:
    QProcess* m_pProc;
    QNetworkAccessManager*  m_pNetworkAccessManager;

};
Q_DECLARE_METATYPE(PythonApp)
//---------------------------------------------------------------------------
#endif // PYTHONAPP_H
