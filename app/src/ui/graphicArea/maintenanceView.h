#ifndef MAINTENANCEVIEW_H
#define MAINTENANCEVIEW_H

#include <QObject>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "common/common.h"
#include "mainScene.h"

//　メンテナンス用のView
//　Graphicを操作したくないのでMouseイベントをOverrideしてSceneへイベントを伝達しない

class MaintenanceView : public QGraphicsView
{
    Q_OBJECT
    QTTEST_OBJECT
public:
    MaintenanceView(QGraphicsScene* scene, QWidget *parent = 0);
    ~MaintenanceView() {;}

    void    fitInViewRect();
    void    setSceneAndRange(QGraphicsScene* scene);

    void    mousePressEvent(QMouseEvent* e);
    void    mouseMoveEvent(QMouseEvent* e);
    void    mouseReleaseEvent(QMouseEvent* e);

    QPointF laserPos();
    void    laserHide() { mLaserPointer->setVisible(false); }

public slots:
    bool    movelaser(QPointF diff);
    void    moveToOrigin() { mLaserPointer->setPos(-2, -2); }

signals:
    void    sendPos(QPointF pos);

private:
    QRectF  mViewRect;

    QGraphicsEllipseItem* mLaserPointer;

};

#endif // MAINTENANCEVIEW_H
