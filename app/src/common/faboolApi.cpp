#include "faboolApi.h"

// FaboolAPIResponse
FaboolAPIResponse::FaboolAPIResponse(QNetworkReply *reply, QObject *parent) : QObject(parent)
{
    QString responseString = (QString)reply->readAll();
    qDebug() << responseString;
    if(responseString != ""){
        mHasReply = true;
        mData = QJsonObject(QJsonDocument::fromJson(responseString.toUtf8()).object());
    } else {
        mHasReply = false;
    }
}

// FaboolAPI
FaboolAPI::FaboolAPI(QObject *parent) : QObject(parent)
{
}

QUrl FaboolAPI::getApiUrl(int version)
{
    if(version != 1) {
        return QUrl();
    }
    return QUrl("https://api.smartdiys.com/fabool/v1/");
}

FaboolAPIResponse* FaboolAPI::sendRequest(QUrlQuery query)
{

    QNetworkAccessManager *manager = new QNetworkAccessManager();

    QUrl url = FaboolAPI::getApiUrl(1);

    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/x-www-form-urlencoded");

    QEventLoop eventLoop;
    connect(manager, SIGNAL(finished(QNetworkReply*)),
            &eventLoop, SLOT(quit()));
    QNetworkReply *reply = manager->post(request, query.toString(QUrl::FullyEncoded).toUtf8());
    eventLoop.exec();

    FaboolAPIResponse* response = new FaboolAPIResponse(reply);
    return response;
}

FaboolAPIResponse* FaboolAPI::getLatestFaboolDesktopVersionInfo(){

    QUrlQuery query;
    query.addQueryItem("request", "getLatestFaboolVersionInfo");
    query.addQueryItem("appname", "fabooldesktop");
    return FaboolAPI::sendRequest(query);
}

