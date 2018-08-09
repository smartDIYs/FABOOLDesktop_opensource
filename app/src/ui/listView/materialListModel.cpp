#include "materialListModel.h"
#include "materialListedData.h"

#include <QIcon>

MaterialListModel::MaterialListModel(QObject* parent):
    ListModelCore(parent)
{

}

void MaterialListModel::setData(const QJsonObject &json)
{
    QJsonArray matArray = json[JSON_TAG_PROCESS_PARAM].toArray();
    QString localeTag = "";
    QLocale::Language useLang = QLocale::system().language();
    if(useLang == QLocale::Japanese)
    { localeTag = "_ja";}
    else if(useLang == QLocale::English)
    { localeTag = "_en"; }

    QString mtrName = JSON_TAG_MATERIAL_NAME + localeTag;
    QString attrTag = JSON_TAG_ATTRIBUTION + localeTag;
    foreach (QJsonValue matVal, matArray) {
        QJsonObject imgObj = matVal.toObject();
        QString name = imgObj[mtrName].toString();
        QString tag  = imgObj[attrTag].toString();
        qreal thick = imgObj[JSON_TAG_THICK].toDouble();
        MaterialListedData* listedData = new MaterialListedData(name, tag, thick);
        listedData->setData(imgObj);
        mData.append(listedData);
        if(mTagList.contains(tag) == false) { mTagList.append(tag); }
    }
}
