#ifndef QMACHINESETTINGDIALOG_H
#define QMACHINESETTINGDIALOG_H

// SettingManagerが持ってる情報をUI化
// 社長担当箇所

#include <QDialog>
#include "common/common.h"
#include "common/sysParam.h"

namespace Ui
{
class MachineSettingDialog;
}

class MachineSettingDialog : public QDialog
{
    Q_OBJECT
    QTTEST_OBJECT
public:
    explicit MachineSettingDialog( QWidget *parent = 0);
    ~MachineSettingDialog();

    int updateUI();
    int updateParam();

private slots:
    void onSave();
    void onCancel();
    void onZipLogs();
    void onChangeLaserType(int);

protected:
    void showEvent(QShowEvent* e);

signals:
    void systemSaved();

private:
    Ui::MachineSettingDialog *ui;

    SysParam* mSystemInfo;

    int mSelectedLsrType;
};

#endif // QMACHINESETTINGDIALOG_H
