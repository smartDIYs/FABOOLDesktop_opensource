#ifndef GROUPINFO_H
#define GROUPINFO_H

#include <QtCore>
#include <QtGui>
#include <QGraphicsItem>

#include "common/common.h"
#include "graphics/itemInfo.h"
#include "graphics/graphicsFlexibleFrame.h"

class GroupInfo
{
public:
    GroupInfo(QList<GraphicsFlexibleFrame*> list);

    void set();
    void apply();
    void apply(GraphicsFlexibleFrame* srcFrame, GraphicsFlexibleFrame* distFrame);

    void show();
    void hide();
    bool isShown();

    void pastedCountUp() { mPastedCount++; }
    void pastedCountDown() { mPastedCount--; }
    int  pastedCount() { return mPastedCount; }

    GraphicsFlexibleFrame* groupFrame() { return mGroupFrame; }
    QGraphicsItemGroup*    centerItem() { return mCenterItem; }
    QList<GraphicsFlexibleFrame* > factorFrameList() { return mFactorFrameList; }
    QMap<QGraphicsItem*, QPointF> centerMAp() { return mCenterMap; }

private:
    GraphicsFlexibleFrame*          mGroupFrame;
    QGraphicsItemGroup*             mCenterItem;
    QList<GraphicsFlexibleFrame* >  mFactorFrameList;           //　グループ化した元のアイテムを記憶しておく
    QMap<QGraphicsItem*, QPointF>   mCenterMap;   //　グループ化解除時に各アイテムの中心位置を再計算するため、グループ化時点での個々の中心位置を記憶しておく
    QMap< GraphicsFlexibleFrame*, QPair<QRectF, qreal> >  mStocksPrev;  // グループ化前の個々の情報
    int                             mPastedCount;
};

#endif // GROUPINFO_H
