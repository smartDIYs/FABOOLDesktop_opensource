
#include <QMessageBox>

#include "common/common.h"
#include "firstRegistrationDialog.h"
#include "ui_firstRegistrationDialog.h"
#include "common/faboolApi.h"

FirstRegistrationDialog::FirstRegistrationDialog(QWidget *parent) :
    QDialog(parent, (Qt::CustomizeWindowHint)),
    ui(new Ui::FirstRegistrationDialog)
{
    ui->setupUi(this);
    connect(ui->StartButton,    SIGNAL(clicked()), this, SLOT(startButtonPushed()));
    connect(ui->CancelButton,   SIGNAL(clicked()), this, SLOT(cancelButtonPushed()));
}

FirstRegistrationDialog::~FirstRegistrationDialog()
{
    delete ui;
}

int FirstRegistrationDialog::laserType()
{
    return ui->MachineSelect->currentIndex();
}

void FirstRegistrationDialog::startButtonPushed()
{
    ui->MessageLabel->setText("");

    if(ui->MachineSelect->currentIndex() == 0) {
        ui->MessageLabel->setText(tr("Please select your machine."));
        return;
    } else {
        accept();
    }
}

void FirstRegistrationDialog::cancelButtonPushed()
{
    reject();
}
