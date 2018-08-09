#ifndef VERSIONINFODIALOG_H
#define VERSIONINFODIALOG_H

#include <QDialog>
#include <QJsonObject>
#include "common/versionInfo.h"

namespace Ui
{
class VersionInfoDialog;
}

class AboutDialog : public QDialog
{
    Q_OBJECT

public:
    explicit AboutDialog(QWidget *parent = 0);
    ~AboutDialog();

public slots:
    void onCloseButtonPushed();
    void onUpdateCheckButtonPushed();

signals:
    void versionUp();

private:
    Ui::VersionInfoDialog *ui;
    VersionInfo* versionInfo;
    void setCurrentVersionInfo();
};

#endif // VERSIONINFODIALOG_H
