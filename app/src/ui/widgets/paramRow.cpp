#include <QObject>
#include <QTableWidgetItem>
#include <QCheckBox>

#include "common/common.h"
#include "common/dataManager.h"
#include "paramRow.h"
#include "graphics/paramData.h"

ParamRow::comboBox::comboBox(bool isRaster, QWidget* parent):
    QComboBox(parent)
{
    addItem(QString(tr("Engrave")));

    if(isRaster == false)
    { addItem(QString(tr("Cut"))); }
}

void ParamRow::comboBox::setType(PARAM_USE_TYPE type)
{
    if(type == PARAM_USE_TYPE_CUT) { setCurrentText(QString(tr("Cut"))); }
    else                           { setCurrentText(QString(tr("Engrave"))); }
}

PARAM_USE_TYPE ParamRow::comboBox::currentType()
{
    if(currentText() == QString(tr("Cut"))) { return PARAM_USE_TYPE_CUT; }
    else                           { return PARAM_USE_TYPE_ENGRAVE; }
}

ParamRow::lineColor::lineColor(QWidget* parent):
    QWidget(parent)
{

}

void ParamRow::lineColor::setColor(QColor color)
{
    setStyleSheet("margin-left:50%; margin-right:50%; background-color:" + color.name());
}

ParamRow::ParamRow(QObject* parent):
    QObject(parent)
{
    mPara       = NULL;
    mLineColor  = NULL;
    mSpeedEdit  = NULL;
    mLaserPower = NULL;
    mUseTypeBox = NULL;
    mTimesEdit  = NULL;
}

//　初期化
//　パス情報をもらってUIを作る
ParamRow::ParamRow(bool isRaster, ParamData* param, QObject* parent):
    QObject(parent)
{
    mPara = param;

// color
    QColor color = param->color();
    mLineColor = new lineColor();
    mLineColor->setColor(color);
    mLineColor->setObjectName("PathColor");
    mLineColor->setCursor(Qt::PointingHandCursor);

    QString css = "QSpinBox\n{\n"
                  "background-color: transparent;\n"
                  "border: 1px solid transparent;\n"
                  "}\n"
                  "QSpinBox::up-arrow\n{\n"
                  "image: url(:/uiIcon/src/ui/resource/icon/up_arrow.png);\n"
                  "width:7px; height:7px;\n\n"
                  "}\n"
                  "QSpinBox::down-arrow\n{\n"
                  "image: url(:/uiIcon/src/ui/resource/icon/down_arrow.png);\n"
                  "}\n"
                  "QSpinBox::down-arrow:disabled\n{\n"
                  "image: url(:/uiIcon/src/ui/resource/icon/down_arrow_disabled.png);\n"
                  "}\n"
                  "QSpinBox::up-arrow:disabled\n{\n"
                  "image: url(:/uiIcon/src/ui/resource/icon/up_arrow_disabled.png);\n"
                  "}";
//　速度
    mSpeedEdit = new QSpinBox();
    mSpeedEdit->setStyleSheet(css);
    mSpeedEdit->setSingleStep(50);
    mSpeedEdit->setRange(PATH_PARAMETER_SPEED_MIN, PATH_PARAMETER_SPEED_MAX);
    mSpeedEdit->setAlignment(Qt::AlignRight);
    mSpeedEdit->setFrame(false);
    mSpeedEdit->setValue(param->headSpeed());
    connect(mSpeedEdit, SIGNAL(valueChanged(int)), this, SLOT(changed()));

//　レーザー強度
    mLaserPower = new QLineEdit();
    QIntValidator* laserValidator = new QIntValidator(PATH_PARAMETER_LASER_MIN, PATH_PARAMETER_LASER_MAX, mLaserPower);
    mLaserPower->setValidator(laserValidator);
    mLaserPower->setText(QString::number(param->laserPower()));
    mLaserPower->setAlignment(Qt::AlignRight);
    mLaserPower->setFrame(false);
    connect(mLaserPower, SIGNAL(textChanged(QString)), this, SLOT(changed()));

//　切断・刻印
    mUseTypeBox = new comboBox(isRaster);
    mUseTypeBox->setType(param->useType());
    mUseTypeBox->setFrame(false);
    connect(mUseTypeBox, SIGNAL(currentIndexChanged(int)), this, SLOT(changed()));

//　走査回数
    mTimesEdit  = new QLineEdit();
    QIntValidator* timeValidator = new QIntValidator(PATH_PARAMETER_PROCESS_MIN, PATH_PARAMETER_PROCESS_MAX, mTimesEdit);
    mTimesEdit->setValidator(timeValidator);
    mTimesEdit->setText(QString::number(param->processTimes()));
    mTimesEdit->setAlignment(Qt::AlignRight);
    mTimesEdit->setFrame(false);
    connect(mTimesEdit, SIGNAL(textChanged(QString)), this, SLOT(changed()));
}

ParamRow::~ParamRow()
{
// TableWidget　が　SetCellWidgetしたWidgetの破棄を行ってくれるのでここではやらないようにした
//    delete mLineColor;
//    delete mSpeedEdit;
//    delete mLaserPower;
//    delete mUseTypeBox;
//    delete mTimesEdit;
}

//　TableのCol番号に従って対応するWidgetを返す
QWidget* ParamRow::getWidget(qint16 colNum)
{
    QWidget* retWid = NULL;

    switch (colNum) {
    case PARAM_TABLE_HEADER_COL_COLOR:
        retWid = mLineColor;
        break;
    case PARAM_TABLE_HEADER_COL_HEAD_SPEED:
        retWid = mSpeedEdit;
        break;
    case PARAM_TABLE_HEADER_COL_LASER_POWER:
        retWid = mLaserPower;
        break;
    case PARAM_TABLE_HEADER_COL_USE_TYPE:
        retWid = mUseTypeBox;
        break;
    case PARAM_TABLE_HEADER_COL_PROCESS_TIME:
        retWid = mTimesEdit;
        break;
    default:
        break;
    }

    return retWid;
}

//　プロジェクト保存時に表示中のパス情報をプロジェクトに反映する
int ParamRow::setBackParam()
{
    int speed = mSpeedEdit->text().toInt();
    int laser = mLaserPower->text().toInt();
    int times = mTimesEdit->text().toInt();

    mPara->setHeadSpeed(speed);
    mPara->setLaserPower(laser);
    mPara->setProcessTimes(times);
    mPara->setType(mUseTypeBox->currentType());

    return SUCCESS;
}

void ParamRow::updateParam()
{
    mSpeedEdit->setValue(mPara->headSpeed());
    mLaserPower->setText(QString::number(mPara->laserPower()));
    mTimesEdit->setText(QString::number(mPara->processTimes()));
    mUseTypeBox->setCurrentIndex((PARAM_USE_TYPE)(mPara->useType()));
}

void ParamRow::changed()
{
    setBackParam();
    DataManager::getInstance()->modified();
}
