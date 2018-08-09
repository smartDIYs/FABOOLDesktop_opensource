
#include <QGraphicsSceneMouseEvent>

#include "sizeGrip.h"
#include "graphicsFlexibleFrame.h"

SizeGrip::SizeGrip(int handlePos, GraphicsFlexibleFrame* parent):
    QGraphicsRectItem(-5, -5, 10, 10, parent),
    mParent(parent)
{
    mHandlingPos = handlePos;
    setFlags(ItemIsMovable | ItemIsSelectable | ItemIgnoresTransformations);  // ItemIgnoresTransformationsを付加すると、Viewの拡大縮小のえいきょうをうけなくなる

    if(mHandlingPos == Rotate)
    { setBrush(QBrush(Qt::yellow)); }
}

void SizeGrip::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    if(handlingPosition() == Rotate)
    { mParent->setAnchorCenter(); }
}

void SizeGrip::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    QPointF curPos ;
    QPointF prePos ;

    QPointF diff = mParent->transformItemCoordinate(e->scenePos() - e->lastScenePos());
    prePos = mParent->rectPos(handlingPosition()) ;
    curPos = prePos + diff;


    qreal ratioWPH      = mParent->rectWidth() / mParent->rectHeight();//mRectの W/H　の比率


    switch(handlingPosition()) {
    case TopLeft:
        diff = QPointF( diff.y() * ratioWPH, diff.y() );
        curPos = prePos + diff;
        mParent->setTopLeft(curPos);
        break;
    case Top:
        mParent->setTop(curPos.y());
        break;
    case TopRight:
        diff = QPointF( -diff.y() * ratioWPH, diff.y() );
        curPos = prePos + diff;
        mParent->setTopRight(curPos);
        break;
    case Right:
        mParent->setRight(curPos.x());
        break;
    case BottomRight:
        diff = QPointF( diff.y() * ratioWPH, diff.y() );
        curPos = prePos + diff;
        mParent->setBottomRight(curPos);
        break;
    case Bottom:
        mParent->setBottom(curPos.y());
        break;
    case BottomLeft:
        diff = QPointF( -diff.y() * ratioWPH, diff.y() );
        curPos = prePos + diff;
        mParent->setBottomLeft(curPos);
        break;
    case Left:
        mParent->setLeft(curPos.x());
        break;
    case Rotate:
        mParent->updateAngle(prePos, curPos);
        return;
    }
    mParent->updateScale();
}// Gripperが動くとFlexibleFrameのｍRectが変化し、その形状に合わせるようにTransform行列を作る

