#include <QGraphicsView>
#include <QTimeLine>
#include <QWheelEvent>
#include <QScrollBar>

#include "mainView.h"
#include "mainScene.h"

MainView::MainView(QWidget *parent):
    QGraphicsView(parent)
{
    setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);  //スクロールバーは必要になった時だけ表示する
    setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);//同上

    setDragMode(QGraphicsView::RubberBandDrag);                 //範囲選択モード
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);  //ビューの更新方法の設定
    setBackgroundBrush(QBrush(Qt::lightGray));                       //背景色
}

//　Sceneと範囲の設定
void MainView::setSceneAndRange(MainScene* scene)
{
    if(scene == NULL) { return; }

    setScene(scene);
    mMovableRange = mViewRect = scene->movableRenge()->rect();
    setSceneRect(mMovableRange);
}

//　ズームイン処理
//　見える範囲を設定してその範囲内でズームインする
void MainView::zoomIn()
{
    qreal wid = mViewRect.width()  * 0.8;
    qreal hgt = mViewRect.height() * 0.8;
    if(wid < MAIN_VIEW_ZOOM_MIN) { wid = mViewRect.width(); }  //　100以下のサイズにはズーム不可
    if(hgt < MAIN_VIEW_ZOOM_MIN) { hgt = mViewRect.height(); }
    mViewRect = {0, 0, wid, hgt};
    fitInView(mViewRect, Qt::KeepAspectRatio);

}

//　ズームアウト処理
//　見える範囲を設定してその範囲内でズームアウトする
void MainView::zoomOut()
{
    qreal wid = mViewRect.width()  * 1.2;
    qreal hgt = mViewRect.height() * 1.2;
    if(wid > mMovableRange.width())  { wid = mViewRect.width(); }
    if(hgt > mMovableRange.height()) { hgt = mViewRect.height(); }
    mViewRect = {0, 0, wid, hgt};
    fitInView(mViewRect, Qt::KeepAspectRatio);
}

//　加工範囲にViewをフィットさせる
void MainView::actualSize()
{
    fitInView(mMovableRange, Qt::KeepAspectRatio);
    mViewRect = {0, 0, mMovableRange.width(), mMovableRange.height()};
}

//　今のズーム状態にViewをフィットさせる
void MainView::fitInViewRect()
{
    fitInView(mViewRect, Qt::KeepAspectRatio);
}

//　ホイールのロールでズームインとズームアウトを行う
void MainView::wheelEvent(QWheelEvent* e)  // ViewScaling
{
    int numDegrees = e->delta() / 8;
    qreal factor = - ((qreal)numDegrees * (0.8 / 180));

    qreal wid = mViewRect.width() * (1 + factor);
    qreal hgt = mViewRect.height() * (1 + factor);

    if(wid > mMovableRange.width()  || wid < MAIN_VIEW_ZOOM_MIN ||
            hgt > mMovableRange.height() || hgt < MAIN_VIEW_ZOOM_MIN ) {
        wid = mViewRect.width();
        hgt = mViewRect.height();
        factor = 0;
    }
    mViewRect = {0, 0, wid, hgt};

    fitInView(mViewRect, Qt::KeepAspectRatio);
}
