#include "aboutDialog.h"
#include "ui_aboutDialog.h"
#include "common/faboolApi.h"
#include <QMessageBox>

AboutDialog::AboutDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::VersionInfoDialog)
{
    ui->setupUi(this);
    connect(ui->CloseButton,     SIGNAL(clicked()), this, SLOT(onCloseButtonPushed()));
    connect(ui->CheckForUpdates, SIGNAL(clicked()), this, SLOT(onUpdateCheckButtonPushed()));

    versionInfo = new VersionInfo();
    setCurrentVersionInfo();
}

AboutDialog::~AboutDialog()
{
    delete ui;
}

void AboutDialog::setCurrentVersionInfo()
{
    ui->currentVersionInfo->setText("version: " + versionInfo->getCurrentVersionString());
}

void AboutDialog::onUpdateCheckButtonPushed()
{
    QString message = "";
    if(versionInfo->fetchLatestVersion() == false) {
        message = tr("There was an error checking for updates.");
    } else if (versionInfo->isLatestVersion()) {
        message = tr("You have the latest version.\n");
    } else {
        message = tr("You can update to the latest version.\n"
                     "Are you sure you want to exit the software?\n");
        QMessageBox question(this);
        question.setWindowTitle(tr("SoftwareVersionUp"));
        question.setTextFormat(Qt::RichText);
        question.setText(message + "<a href='https://www.smartdiys.com/manual/fabool-desktop-releasenotes/'>release notes</a>" );
        question.setStandardButtons(QMessageBox::Yes | QMessageBox::Cancel);
        int retBtn = question.exec();
        if(retBtn == QMessageBox::Yes) {
            emit versionUp();
        }
        accept();
        return;
    }

    QMessageBox msgBox;
    msgBox.setText(message);
    msgBox.exec();
}

void AboutDialog::onCloseButtonPushed()
{
    accept();
}


