#include "maintenanceWidget.h"
#include "ui_maintenanceWidget.h"

MaintenanceWidget::MaintenanceWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::MaintenanceWidget)
{
    ui->setupUi(this);

    connection();
}

MaintenanceWidget::~MaintenanceWidget()
{
    delete ui;
}

void MaintenanceWidget::connection()
{
    connect(ui->CloseButton,            SIGNAL(clicked()),  this,   SIGNAL(close()));

    connect(ui->MoveUpButton,           SIGNAL(pressed()),  this,   SIGNAL(MoveUp()) );
    connect(ui->MoveRightButton,        SIGNAL(pressed()),  this,   SIGNAL(MoveRight()) );
    connect(ui->MoveDownButton,         SIGNAL(pressed()),  this,   SIGNAL(MoveDown()) );
    connect(ui->MoveLeftButton,         SIGNAL(pressed()),  this,   SIGNAL(MoveLeft()) );
}
