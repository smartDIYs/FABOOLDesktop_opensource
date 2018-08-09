#ifndef FABOOLWEBAPI_H
#define FABOOLWEBAPI_H

#include <QObject>
#include <QtCore>
#include <QUrl>
#include <QNetworkAccessManager>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QDesktopServices>
#include <QJsonDocument>
#include <QJsonObject>


class FaboolAPIResponse : public QObject{
public:
    FaboolAPIResponse(QNetworkReply *reply, QObject *parent = 0);
    bool hasReply(){return mHasReply;}
    QJsonObject getData(){return mData;}
private:
    bool mHasReply;
    QJsonObject mData;
};


class FaboolAPI : public QObject
{
    Q_OBJECT
public:
    explicit FaboolAPI(QObject *parent = 0);
    static FaboolAPIResponse *sendRequest(QUrlQuery query);
    static FaboolAPIResponse *getLatestFaboolDesktopVersionInfo();

    static QUrl getApiUrl(int version = 1);

signals:

public slots:

private:
};

#endif // FABOOLWEBAPI_H
