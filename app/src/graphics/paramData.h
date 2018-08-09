#ifndef QPARAMDATA_H
#define QPARAMDATA_H

// 画像ごとに持つパス情報
// Original クラス

#include <QObject>
#include <QMap>
#include <QColor>

#include "common/common.h"

enum PARAM_USE_TYPE {
    PARAM_USE_TYPE_ENGRAVE = 0,
    PARAM_USE_TYPE_CUT
};

class ParamData
{
    QTTEST_OBJECT
public:
    //QParamData();
    ParamData(QColor color = QColor(Qt::black), qint16 headSpeed = 3000, qint16 laserPower = 100, PARAM_USE_TYPE type = PARAM_USE_TYPE_ENGRAVE, qint16 times = 1);
    ParamData(ParamData* copy);
    ~ParamData() {;}

    void setHeadSpeed   (qint16 headSpeed)     { mHeadSpeed    = headSpeed;}
    void setLaserPower  (qint16 laserPower)    { mLaserPower   = laserPower;}
    void setColor       (QColor color)         { mColor        = color; }
    void setType        (PARAM_USE_TYPE type)  { mUseType      = type; }
    void setProcessTimes(qint16 processTimes)  { mProcessTimes = processTimes;}

    qint16 headSpeed() const     { return mHeadSpeed; }
    qint16 laserPower() const    { return mLaserPower; }
    qint16 processTimes() const  { return mProcessTimes; }
    QColor color()          { return mColor; }
    PARAM_USE_TYPE useType() const { return mUseType; }

    void   readData(const QJsonObject &json);
    void   writeDown(QJsonObject &json) const;

private:
    qint16   mHeadSpeed;
    qint16   mLaserPower;
    QColor   mColor;
    PARAM_USE_TYPE mUseType;
    qint16   mProcessTimes;

};
Q_DECLARE_METATYPE(ParamData)
#endif // QPARAMDATA_H
