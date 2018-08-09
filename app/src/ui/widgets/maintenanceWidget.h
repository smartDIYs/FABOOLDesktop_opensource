#ifndef MAINTENANCEWIDGET_H
#define MAINTENANCEWIDGET_H

#include <QtWidgets>
#include <QtCore>
#include <QtGui>

#include "common/common.h"
#include "ui/graphicArea/maintenanceView.h"

namespace Ui
{
class MaintenanceWidget;
}

class MaintenanceWidget : public QWidget
{
    Q_OBJECT
    QTTEST_OBJECT
public:
    explicit MaintenanceWidget(QWidget *parent = 0);
    ~MaintenanceWidget();

    void connection();

signals:
    void close();

    void MoveUp();
    void MoveRight();
    void MoveDown();
    void MoveLeft();

private:
    Ui::MaintenanceWidget*  ui;

};

#endif // MAINTENANCEWIDGET_H
