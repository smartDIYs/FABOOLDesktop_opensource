#include "libraryListModel.h"
#include "ui/listView/libraryListedData.h"

LibraryListModel::LibraryListModel(QObject* parent):
    ListModelCore(parent)
{

}

void LibraryListModel::setData(const QJsonObject &json)
{
    QJsonArray imgArray = json[JSON_TAG_IMAGE].toArray();
    QString localeTag = "";
    QLocale::Language useLang = QLocale::system().language();
    if(useLang == QLocale::Japanese)
    { localeTag = "_ja";}
    else if(useLang == QLocale::English)
    { localeTag = "_en"; }

    QString imgName = JSON_TAG_IMG_NAME + localeTag;
    QString attrTag = JSON_TAG_ATTRIBUTION + localeTag;
    foreach (QJsonValue imgVal, imgArray) {
        QJsonObject imgObj = imgVal.toObject();
        QString name = imgObj[imgName].toString();
        QString tag  = imgObj[attrTag].toString();
        LibraryListedData* listedData = new LibraryListedData(name, tag);
        listedData->setData(imgObj);
        mData.append(listedData);
        if(mTagList.contains(tag) == false) { mTagList.append(tag); }
    }
}
