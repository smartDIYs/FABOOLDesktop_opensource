#include <QtWidgets>

#include "maintenanceView.h"
#include "mainScene.h"

MaintenanceView::MaintenanceView(QGraphicsScene* scene, QWidget *parent):
    QGraphicsView(parent)
{
    setSceneAndRange(scene);
}

//　Sceneの設定
void MaintenanceView::setSceneAndRange(QGraphicsScene* scene)
{
    setScene(scene);
    mViewRect = scene->sceneRect();
    setSceneRect(mViewRect);
    setBackgroundBrush(QBrush(Qt::lightGray));

//　レーザー光がわりのアイテム
    mLaserPointer = scene->addEllipse(0, 0, 4, 4, QPen(Qt::blue), QBrush(Qt::blue));
    mLaserPointer->setVisible(false);
    mLaserPointer->setZValue(10);
}

//　フィット
void MaintenanceView::fitInViewRect()
{
    fitInView(mViewRect, Qt::KeepAspectRatio);
}

//　マウスプレス
//　設定された範囲内だけでレーザー光の位置を指定する
void MaintenanceView::mousePressEvent(QMouseEvent* e)
{
    QPointF scenePos = mapToScene( e->pos() );
    if(mViewRect.contains(scenePos) == false) {
        if(scenePos.x() < 0) { scenePos.setX(0); }
        if(scenePos.x() > mViewRect.width()) { scenePos.setX(mViewRect.width()); }
        if(scenePos.y() < 0) { scenePos.setY(0); }
        if(scenePos.y() > mViewRect.height()) { scenePos.setY(mViewRect.height()); }
    }

    if(mLaserPointer->isVisible() == false) { mLaserPointer->setVisible(true); }

    mLaserPointer->setPos(scenePos.x() - 2, scenePos.y() - 2);

    emit sendPos(scenePos);
}

//　マウスムーブ
//　Sceneへイベントを送らないように、あえて空のまま
void MaintenanceView::mouseMoveEvent(QMouseEvent* e)
{
    return ;
}

//　マウスリリース
//　Sceneへイベントを送らないように、あえて空のまま
void MaintenanceView::mouseReleaseEvent(QMouseEvent* e)
{
    return;
}

//　レーザー光の位置を返す
QPointF MaintenanceView::laserPos()
{
    QPointF LaserCenter = mLaserPointer->rect().center();
    return mLaserPointer->mapToScene(LaserCenter);
}

//　レーザー光の位置を調整
bool MaintenanceView::movelaser(QPointF diff)
{
    if(mLaserPointer->isVisible() == false) { return false; }
    QPointF newPos = QPointF(laserPos() + diff);
    if(mViewRect.contains(newPos) == false) {
        if(newPos.x() < 0) { diff.setX(0); }
        if(newPos.x() > mViewRect.width()) { diff.setX(mViewRect.width()); }
        if(newPos.y() < 0) { diff.setY(0); }
        if(newPos.y() > mViewRect.height()) { diff.setY(mViewRect.height()); }
    }

    mLaserPointer->moveBy(diff.x(), diff.y());
    return true;
}
