#ifndef PARAMROW_H
#define PARAMROW_H

// Paramの1行をまとめたクラス
// パスが追加されるとこのクラスが生成されてParamWidgetに追加される

#include <QObject>
#include <QtWidgets>
#include <QTableWidgetItem>
#include <QCheckBox>
#include <QSpinBox>
#include <QPushButton>
#include <QHBoxLayout>

#include "common/common.h"
#include "graphics/paramData.h"

//　パスパラメータのパスごとのEditUI

class ParamRow : public QObject
{
    Q_OBJECT
    QTTEST_OBJECT
private:
    class comboBox : public QComboBox
    {
    public:
        comboBox(bool isRaster, QWidget* parent = 0);
        ~comboBox() {;}

        void setType(PARAM_USE_TYPE type);
        PARAM_USE_TYPE currentType();
    };

    class lineColor : public QWidget
    {
    public:
        lineColor(QWidget* parent = 0);
        void setColor(QColor);
    };

public:
    explicit ParamRow(QObject* parent = 0);
    ParamRow(bool isRaster, ParamData* param, QObject* parnet = 0);
    ~ParamRow();

    QWidget* getWidget(qint16 colNum);
    ParamData* getParam()const { return mPara; }

    int      setBackParam();
    void     updateParam();

public slots:
    void    changed();

private:
    QSpinBox*      mSpeedEdit;
    QSpinBox*      mLaserPower;
    QSpinBox*      mTimesEdit;
    comboBox*      mUseTypeBox;
    lineColor*     mLineColor;

    ParamData*     mPara;

};

//#include "paramRow.moc"
#endif // PARAMROW_H
