#include "materialListedData.h"

MaterialListedData::MaterialListedData(const QString& name, const QString& tag, qreal thicks):
    ListedDataCore(name, tag)
{
    mThick = thicks;
}

MaterialListedData::MaterialListedData():
    ListedDataCore()
{

}

void MaterialListedData::setData(const QJsonObject& json)
{
    QJsonObject paramObj = json[JSON_TAG_PARAMETER].toObject();

    QJsonObject engrave  = paramObj[JSON_TAG_USE_TYPE_ENGRAVE].toObject();
    mEngrave.setHeadSpeed(engrave[JSON_TAG_PATH_SPEED].toInt());
    mEngrave.setLaserPower(engrave[JSON_TAG_PATH_LASER].toInt());
    mEngrave.setProcessTimes(engrave[JSON_TAG_PATH_TIMES].toInt());

    QJsonObject cut = paramObj[JSON_TAG_USE_TYPE_CUT].toObject();
    mCut.setHeadSpeed(cut[JSON_TAG_PATH_SPEED].toInt());
    mCut.setLaserPower(cut[JSON_TAG_PATH_LASER].toInt());
    mCut.setProcessTimes(cut[JSON_TAG_PATH_TIMES].toInt());
}

ParamData MaterialListedData::param(PARAM_USE_TYPE type)
{
    if(type == PARAM_USE_TYPE_ENGRAVE) {
        return mEngrave;
    } else {
        return mCut;
    }
}
