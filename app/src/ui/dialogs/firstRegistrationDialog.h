#ifndef FIRSTREGISTRATIONDIALOG_H
#define FIRSTREGISTRATIONDIALOG_H

#include <QDialog>
#include "common/common.h"

// Iniファイルがない時　＝　初回起動時　にユーザー登録を行うためのダイアログ
//　Iniファイルがないのは上の処理で判定してくれるので、このクラスではユーザー情報を入力、保持するだけのクラス
//

namespace Ui
{
class FirstRegistrationDialog;
}

class FirstRegistrationDialog : public QDialog
{
    Q_OBJECT
    QTTEST_OBJECT
public:
    explicit FirstRegistrationDialog(QWidget *parent = 0);
    ~FirstRegistrationDialog();
    int     laserType();

public slots:
    void startButtonPushed();
    void cancelButtonPushed();

private:
    Ui::FirstRegistrationDialog *ui;
};

#endif // FIRSTREGISTRATIONDIALOG_H
