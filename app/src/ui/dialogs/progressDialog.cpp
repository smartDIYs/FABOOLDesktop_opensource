#include "common/common.h"
#include "progressDialog.h"
#include "ui_progressDialog.h"

#include <QKeyEvent>

ProgressDialog::ProgressDialog(QWidget *parent) :
    QDialog(parent, (parent->windowFlags() | Qt::CustomizeWindowHint) & ~Qt::WindowCloseButtonHint & ~Qt::WindowMinMaxButtonsHint /*& ~Qt::WindowTitleHint*/),
    ui(new Ui::ProgressDialog)
{
    ui->setupUi(this);
    setWindowTitle("Progress");

    connect(ui->ProcessButton,  SIGNAL(toggled(bool)),     this,  SIGNAL(onProcess(bool)));
    connect(ui->ResetButton,    SIGNAL(clicked()),         this,  SIGNAL(onReset()));
}

ProgressDialog::~ProgressDialog()
{
    delete ui;
}

void ProgressDialog::showEvent(QShowEvent* e)
{
    ui->ProcessButton->show();
    ui->ResetButton->show();
    ui->progressBar->reset();
    ui->ProcessButton->setChecked(false);
    QDialog::showEvent(e);
}

void ProgressDialog::hideEvent(QHideEvent* e)
{
    ui->ProcessButton->hide();
    ui->ResetButton->hide();
    QDialog::hideEvent(e);
}

void ProgressDialog::updateProgress(int progress)
{
    ui->progressBar->setValue(progress);
    update();
}
