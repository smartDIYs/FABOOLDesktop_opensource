
#include <QObject>
#include <QColor>

#include "common/common.h"
#include "paramData.h"

ParamData::ParamData(QColor color, qint16 headSpeed, qint16 laserPower, PARAM_USE_TYPE type, qint16 processTimes)
{
    mHeadSpeed  = headSpeed;
    mLaserPower = laserPower;
    mColor      = color;
    mUseType    = type;
    mProcessTimes = processTimes;
}

ParamData::ParamData(ParamData *copy)
{
    this->mHeadSpeed    = copy->mHeadSpeed;
    this->mLaserPower   = copy->mLaserPower;
    this->mColor        = copy->mColor;
    this->mUseType      = copy->mUseType;
    this->mProcessTimes = copy->mProcessTimes;
}

void ParamData::readData(const QJsonObject &json)
{
    mHeadSpeed    = json[JSON_TAG_PATH_SPEED].toDouble();
    mLaserPower   = json[JSON_TAG_PATH_LASER].toDouble();
    mProcessTimes = json[JSON_TAG_PATH_TIMES].toInt();
    mColor        = json[JSON_TAG_PATH_COLOR].toString();
    mUseType      = (PARAM_USE_TYPE)(json[JSON_TAG_PATH_USE_TYPE].toInt());
}

void ParamData::writeDown(QJsonObject &json) const
{
    json[JSON_TAG_PATH_SPEED] = mHeadSpeed;
    json[JSON_TAG_PATH_LASER] = mLaserPower;
    json[JSON_TAG_PATH_TIMES] = mProcessTimes;
    json[JSON_TAG_PATH_COLOR] = mColor.name();
    json[JSON_TAG_PATH_USE_TYPE] = mUseType;
}
