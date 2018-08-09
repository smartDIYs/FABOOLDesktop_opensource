#include "errMesDialog.h"
#include "ui_errMesDialog.h"

ErrMesDialog::ErrMesDialog(const QString& errDiscription, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::ErrMesDialog)
{
    ui->setupUi(this);
    connect(ui->OkButton, SIGNAL(clicked()), this, SLOT(onOk()));

    QStringList header;
    header << tr("image") << tr("reason");
    ui->tableWidget->setHorizontalHeaderLabels(header);
}

ErrMesDialog::~ErrMesDialog()
{
    delete ui;
}

void ErrMesDialog::setStrings(QMap<QString, QString> errMes)
{
    foreach (QString str, errMes.keys()) {
        int rowC = ui->tableWidget->rowCount();
        ui->tableWidget->insertRow(ui->tableWidget->rowCount());
        QTableWidgetItem* item = new QTableWidgetItem(str);
        ui->tableWidget->setItem(rowC, 0, item);
        item = new QTableWidgetItem(errMes.value(str));
        ui->tableWidget->setItem(rowC, 1, item);
    }
}

void ErrMesDialog::onOk()
{
    accept();
}
