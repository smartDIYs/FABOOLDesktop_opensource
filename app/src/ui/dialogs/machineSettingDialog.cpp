#include "machineSettingDialog.h"
#include "ui_machineSettingDialog.h"
#include "firstRegistrationDialog.h"
#include "../3rd/quazip/include/JlCompress.h"
#include "common/dataManager.h"

#include <QMessageBox>
#include <QFileDialog>

MachineSettingDialog::MachineSettingDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::MachineSettingDialog)
{
    SysParam* sysPara = SysParam::getInstance();
    ui->setupUi(this);

    mSystemInfo = sysPara;
    mSelectedLsrType = MACHINE_TYPE_NONE;
    updateUI();

    connect(ui->SaveButton,     SIGNAL(clicked()), this, SLOT(onSave()));
    connect(ui->CancelButton,   SIGNAL(clicked()), this, SLOT(onCancel()));
    connect(ui->SendLogButton,  SIGNAL(clicked()), this, SLOT(onZipLogs()));
    connect(ui->LaserType,      SIGNAL(currentIndexChanged(int)), this, SLOT(onChangeLaserType(int)));
}

MachineSettingDialog::~MachineSettingDialog()
{
    delete ui;
}

int MachineSettingDialog::updateUI()
{
    int ret = SUCCESS;
    qint16     lsrType     = mSystemInfo->getLaserType();
    qint16     driverPower = mSystemInfo->getDriverPower();
    QSize      workArea    = mSystemInfo->getWorkArea();
    QtMsgType  logLevel    = mSystemInfo->getLogLevelSelector();
    bool       isOverWrite = mSystemInfo->isOverWritePower();
    quint16    offSpeed    = mSystemInfo->getOffMachiningSpeed();
    quint16    dealType    = mSystemInfo->getRasterDealType();
    bool       effectStrokeWid = mSystemInfo->getEffectiveStrokeWid();
    qreal      strokeWid   = mSystemInfo->getStrokeWidth();
    bool       notifyUp    = mSystemInfo->getNotifyUpgrade();
    bool       effectFill  = mSystemInfo->getEffectiveFill();
    qreal      fillPitch = mSystemInfo->getFillPitch();

    if(driverPower < SYS_PARA_DRIVER_POWER_MIN || driverPower > SYS_PARA_DRIVER_POWER_MAX) {
        INFO_STRING(ERR_SETTING_DRIVER_POW_INVALID_STR);
        ret = ERR_SETTING_DRIVER_POW_INVALID;
        driverPower = SYS_PARA_DRIVER_POWER_DEFAULT_LOW;
    }
    if(workArea.width() < SYS_PARA_WORK_AREA_WIDTH_MIN || workArea.width() > SYS_PARA_WORK_AREA_WIDTH_MAX) {
        INFO_STRING(ERR_SETTING_WORK_AREA_INVALID_STR);
        ret = ERR_SETTING_WORK_AREA_INVALID;
        workArea.setWidth(SYS_PARA_WORK_AREA_WIDTH_MIN);
    }
    if(workArea.height() < SYS_PARA_WORK_AREA_HEIGHT_MIN  || workArea.height() > SYS_PARA_WORK_AREA_HEIGHT_MAX) {
        INFO_STRING(ERR_SETTING_WORK_AREA_INVALID_STR);
        ret = ERR_SETTING_WORK_AREA_INVALID;
        workArea.setHeight(SYS_PARA_WORK_AREA_HEIGHT_MIN);
    }

    ui->LaserType->setCurrentIndex(lsrType);
    ui->DriverPowerEdit->setValue(driverPower);
    ui->OverWirteCheckBox->setChecked(isOverWrite);
    ui->WidthEdit->setValue(workArea.width());
    ui->HeightEdit->setValue(workArea.height());
    ui->LogLevelSelector->setCurrentIndex((int)(logLevel));
    ui->SpeedControlEdit->setValue(offSpeed);
    ui->RasterDealingComboBox->setCurrentIndex((int) dealType);
    ui->StrokeWidthCheckBox->setChecked(effectStrokeWid);
    ui->StrokeWidSelect->setValue(strokeWid);
    ui->NotifyUpgrade->setChecked(notifyUp);
    ui->FillmentCheckBox->setChecked(effectFill);
    ui->FillPitchSelect->setValue(fillPitch);

    mSelectedLsrType = lsrType;
    return ret;
}

int MachineSettingDialog::updateParam()
{
    int ret = SUCCESS;

    quint16    lsrType     = ui->LaserType->currentIndex();
    qint16     driverPower = ui->DriverPowerEdit->value();
    qreal      workWidth   = ui->WidthEdit->value();
    qreal      workHeight  = ui->HeightEdit->value();
    QtMsgType  logLevel    = (QtMsgType)(ui->LogLevelSelector->currentIndex());
    bool       isOverWrite = ui->OverWirteCheckBox->isChecked();
    quint16    offSpeed    = ui->SpeedControlEdit->value();
    int        dealType    = ui->RasterDealingComboBox->currentIndex();
    bool       effectStrokeWid    = ui->StrokeWidthCheckBox->isChecked();
    qreal      strokeWid   = ui->StrokeWidSelect->value();
    bool       notifyUp    = ui->NotifyUpgrade->isChecked();
    bool       effectFill  = ui->FillmentCheckBox->isChecked();
    qreal      fillPitch = ui->FillPitchSelect->value();

    if(driverPower < SYS_PARA_DRIVER_POWER_MIN || driverPower > SYS_PARA_DRIVER_POWER_MAX) {
        INFO_STRING(ERR_SETTING_DRIVER_POW_INVALID_STR);
        ret = ERR_SETTING_DRIVER_POW_INVALID;
        driverPower = 800;
    }
    if(workWidth < SYS_PARA_WORK_AREA_WIDTH_MIN || workWidth > SYS_PARA_WORK_AREA_WIDTH_MAX) {
        INFO_STRING(ERR_SETTING_WORK_AREA_INVALID_STR);
        ret = ERR_SETTING_WORK_AREA_INVALID;
        workWidth = SYS_PARA_WORK_AREA_WIDTH_MIN;
    }
    if(workHeight < SYS_PARA_WORK_AREA_HEIGHT_MIN  || workHeight > SYS_PARA_WORK_AREA_HEIGHT_MAX) {
        INFO_STRING(ERR_SETTING_WORK_AREA_INVALID_STR);
        ret = ERR_SETTING_WORK_AREA_INVALID;
        workHeight = SYS_PARA_WORK_AREA_HEIGHT_MIN;
    }

    mSystemInfo->setLaserType(lsrType);
    mSystemInfo->setDriverPower(driverPower);
    mSystemInfo->setOverWritePower(isOverWrite);
    QSize size = QSize(workWidth, workHeight);
    mSystemInfo->setWorkArea(size);
    mSystemInfo->setLogLevelSelector(logLevel);
    mSystemInfo->setOffMachiningSpeed(offSpeed);
    mSystemInfo->setRasterDealType((quint16)dealType);
    mSystemInfo->setEffectiveStrokeWid(effectStrokeWid);
    mSystemInfo->setStrokeWidth(strokeWid);
    mSystemInfo->setNotifyUpgrade(notifyUp);
    mSystemInfo->setEffectiveFill(effectFill);
    mSystemInfo->setFillPitch(fillPitch);

    if(mSelectedLsrType != lsrType)
    { ret = MODIFY_LASER_TYPE_CHANGED; }

    return ret;
}

void MachineSettingDialog::onSave()
{
    int ret = updateParam();
    if(ret != SUCCESS && ret != MODIFY_LASER_TYPE_CHANGED) {
        QString text;
        switch(ret) {
        case ERR_SETTING_DRIVER_POW_INVALID:
            text = tr("Driver power is invalid.");
            break;
        case ERR_SETTING_WORK_AREA_INVALID:
            text = tr("Work area is invalid.");
            break;
        default :
            text = tr("There is unknown Error.");
            break;
        }
        QMessageBox::information(this, tr("INFO"), text + tr("\nNeed confirmtion your settings."));
        return;
    }
    mSystemInfo->save();
    emit systemSaved();
    accept();
}

void MachineSettingDialog::onCancel()
{
    close();
}

void MachineSettingDialog::onZipLogs()
{
    static QString memoriedPath = "/";
    QString distPath = QFileDialog::getExistingDirectory(this, tr("distrbution Path"), memoriedPath);
    if(distPath == "") {
        return;
    }
    memoriedPath = distPath;

    QString sysFilePath = mSystemInfo->getLogPath() + "/systemInfo";
    QFile sysInfoFile(sysFilePath);
    QString uuid = mSystemInfo->getUUID().toString();
    if(sysInfoFile.open(QIODevice::WriteOnly) == true) {
        QTextStream out(&sysInfoFile);
        out << "AcountName: " + uuid + "\n";
        out << "ProductType: " + mSystemInfo->getOsName() + "\n";
        out << "ProductVersion: " + mSystemInfo->getOsVer() + "\n";
        out << "CPUArchitecture: " + mSystemInfo->getCpuType() + "\n";
        out << "LaserModel: " + mSystemInfo->getLaserTypeString() + "\n";
        out << "Area W: " + QString::number(mSystemInfo->getWorkArea().width()) + "\n";
        out << "Area H: " + QString::number(mSystemInfo->getWorkArea().height()) + "\n";
        if(mSystemInfo->isOverWritePower() == true)
        { out << "Driver power:" + QString::number(mSystemInfo->getDriverPower()) + "\n"; }
        out << "Work speed: " + QString::number(mSystemInfo->getOffMachiningSpeed()) + "\n";
        out << "Raster dealing: " + QString::number(mSystemInfo->getRasterDealType()) + "\n";
        if(mSystemInfo->getEffectiveStrokeWid() == true)
        { out << "Effective Stroke width: " + QString::number(mSystemInfo->getStrokeWidth()) + "\n" ; }
        out << DataManager::getInstance()->getProjectData().toBase64() + "\n";

        sysInfoFile.close();
    }

    QString target = mSystemInfo->getLogPath();
    distPath += "/" + uuid + "_" + QDate::currentDate().toString("yyyyMMdd") + ".zip";
    JlCompress::compressDir(distPath, target);

    sysInfoFile.remove();
}

void MachineSettingDialog::onChangeLaserType(int index)
{
    switch(index) {
    case MACHINE_TYPE_CO2: {
            ui->WidthEdit->setValue(SYS_PARA_WORK_AREA_WIDTH_CO2);
            ui->HeightEdit->setValue(SYS_PARA_WORK_AREA_HEIGHT_CO2);
            ui->DriverPowerEdit->setValue(SYS_PARA_DRIVER_POWER_DEFAULT_HIGH);
            break;
        }
    case MACHINE_TYPE_DS: {
            ui->WidthEdit->setValue(SYS_PARA_WORK_AREA_WIDTH_DS);
            ui->HeightEdit->setValue(SYS_PARA_WORK_AREA_HEIGHT_DS);
            ui->DriverPowerEdit->setValue(SYS_PARA_DRIVER_POWER_DEFAULT_HIGH);
            break;
        }
    case MACHINE_TYPE_MINI_1_6W:
    case MACHINE_TYPE_MINI_3_5W:
    default: {
            ui->WidthEdit->setValue(SYS_PARA_WORK_AREA_WIDTH_MIN);
            ui->HeightEdit->setValue(SYS_PARA_WORK_AREA_HEIGHT_MIN);
            ui->DriverPowerEdit->setValue(SYS_PARA_DRIVER_POWER_DEFAULT_LOW);
            break;
        }
    }
}

void MachineSettingDialog::showEvent(QShowEvent* e)
{
    QDialog::showEvent(e);

    int ret = updateUI();
    if(ret != SUCCESS) {
        QString text;
        switch(ret) {
        case ERR_SETTING_DRIVER_POW_INVALID:
            text = tr("Driver power is invalid.");
            break;
        case ERR_SETTING_WORK_AREA_INVALID:
            text = tr("Work area is invalid.");
            break;
        default :
            text = tr("There is unknown Error.");
            break;
        }
        QMessageBox::information(this, tr("INFO"), text + ERR_SETTING_CONFIRMATION_STR);
    }
}
