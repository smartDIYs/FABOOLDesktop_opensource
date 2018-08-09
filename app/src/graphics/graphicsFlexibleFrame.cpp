#include <QtCore>
#include <QtGui>
#include <QGraphicsSceneEvent>
#include <QMessageBox>
#include <QObject>

#include "graphicsFlexibleFrame.h"
#include "vectorImageItem.h"
#include "itemInfo.h"
#include "sizeGrip.h"

GraphicsFlexibleFrame::GraphicsFlexibleFrame(QGraphicsItem *target):
    QGraphicsRectItem(NULL),
    mCenteredItem(NULL)
{
    this->setFlags(ItemIsMovable | ItemIsSelectable);   // FlexibleFrameを移動可能かつ選択可能にする

//　各SizeGripperの初期化
//　作っておけばあとはGripper内部で処理してくれるのでリスト化しなくてもいい
    new SizeGrip(TopLeft,     this);
    new SizeGrip(Top,         this);
    new SizeGrip(TopRight,    this);
    new SizeGrip(Right,       this);
    new SizeGrip(BottomRight, this);
    new SizeGrip(Bottom,      this);
    new SizeGrip(BottomLeft,  this);
    new SizeGrip(Left,        this);
    new SizeGrip(Rotate,      this);

    mParentInfo   = NULL;
    mRotateMatrix = QTransform();
    mScaleMatrix  = QTransform();
    mCurAngle   = 0;            //
    mPreAngle   = 0;
    mScaleH     = 1.0;          //
    mScaleW     = 1.0;          //
    mIsRaster   = false;

    setCenteredItem(target);
    foreach (QGraphicsItem* item, this->childItems()) {
        item->hide();
    }
    if(mCenteredItem != NULL) { mCenteredItem->show(); }

}

GraphicsFlexibleFrame::~GraphicsFlexibleFrame()
{
    // 親子関係を結んだインスタンスは、基本的に親の破棄に伴って子も自動的に破棄されるのでここには何も記述しない
}

//　選択時以外は枠線を表示しないための処理
void GraphicsFlexibleFrame::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    if(isSelected()) {
        QGraphicsRectItem::paint(painter, option, widget);
    }
}

#define IMPL_SET_FN(TYPE, POS)                       \
    void GraphicsFlexibleFrame::set ## POS (TYPE v)  \
    {   mPreRect = mCurRect;                         \
        mCurRect.set ## POS (v);                     \
        resizeRectWithinLimit();                     \
    }

IMPL_SET_FN(qreal, Top)
IMPL_SET_FN(qreal, Right)
IMPL_SET_FN(qreal, Bottom)
IMPL_SET_FN(qreal, Left)
IMPL_SET_FN(const QPointF&, TopLeft)
IMPL_SET_FN(const QPointF&, TopRight)
IMPL_SET_FN(const QPointF&, BottomRight)
IMPL_SET_FN(const QPointF&, BottomLeft)

//　回転時の中心を再設定
void GraphicsFlexibleFrame::setAnchorCenter()
{
    mAnchorCenter = mCurRect.center();
}

// 回転の更新
//　回転gripperの以前のポイントと現在のポイント、それぞれ回転中心を結んだ2直線のなす角を積算する
void GraphicsFlexibleFrame::updateAngle(QPointF lastPos, QPointF updatePos)
{
    QPointF center  = mCurRect.center();

    QLineF centerToGrip(center, lastPos);
    QLineF centerToMouse(center, updatePos);

    mPreAngle = mCurAngle;
    mCurAngle += centerToMouse.unitVector().angleTo(centerToGrip.unitVector());
    if(mCurAngle >= 180) { mCurAngle -= 360; }
}

// 縦横の拡大率の更新
//　Gripperの引っ張りによって変化したｍRectと最初のサイズを比較して比率を出す
void GraphicsFlexibleFrame::updateScale()
{
    mScaleW = (mCurRect.width()  / mDefRect.width()) ;
    mScaleH = (mCurRect.height() / mDefRect.height());
}

// 変換行列の計算 移動・回転・スケーリングはすべて行列で行う
void GraphicsFlexibleFrame::updateMatrix()
{
    mRotateMatrix.reset();
    mRotateMatrix.translate(mAnchorCenter.x(), mAnchorCenter.y());
    mRotateMatrix.rotate(mCurAngle);
    mRotateMatrix.translate(-mAnchorCenter.x(), -mAnchorCenter.y());

    mScaleMatrix.reset();
    mScaleMatrix.setMatrix(mScaleMatrix.m11()*mScaleW, mScaleMatrix.m12(), mScaleMatrix.m13(),
                           mScaleMatrix.m21(), mScaleMatrix.m22()*mScaleH, mScaleMatrix.m23(),
                           mScaleMatrix.m31() + mCurRect.topLeft().x(), mScaleMatrix.m32() + mCurRect.topLeft().y(), mScaleMatrix.m33());

    this->resetTransform();
    this->setTransform(mScaleMatrix * mRotateMatrix);
}

//　実際に移動・スケーリング・回転を行いたいグラフィックアイテムを設定する
void GraphicsFlexibleFrame::setCenteredItem(QGraphicsItem* target)
{
    if(target) {
        mCenteredItem = target;
        target->setParentItem(this);
        target->setFlag(ItemStacksBehindParent, true);    // 通常だと子に設定したアイテムがグラフィック上の一番上にくるが、ドラッグイベントを行うにあたって都合が悪いので、親の後ろに移す

        switch(target->type()) {
        case VectorImageItem::Type:
            mCurRect = target->sceneBoundingRect();
            mCurRect.setSize(dynamic_cast<VectorImageItem *>(target)->getSizeInMM());
            if(mCurRect.width() < SYS_PARA_GRAPHIC_MIN_SIZE) {
                mCurRect.setWidth(SYS_PARA_GRAPHIC_MIN_SIZE);
            }
            if(mCurRect.height() < SYS_PARA_GRAPHIC_MIN_SIZE) {
                mCurRect.setHeight(SYS_PARA_GRAPHIC_MIN_SIZE);
            }

            mPreRect = mCurRect;

            mDefRect = {0, 0, mCurRect.width(), mCurRect.height()};
            mCurAngle   = 0;
            break;
        case QGraphicsItemGroup::Type:
            mDiffOrigin = target->boundingRect().topLeft();
            mPreRect = mCurRect    = target->boundingRect();
            mDefRect = {0, 0, mCurRect.width(), mCurRect.height()};
            target->moveBy(-mCurRect.x(), -mCurRect.y());
            mCurAngle   = 0;
            break;
        }

        this->setRect(mDefRect);

        updateHandleItemPositions();
        updateMatrix();
    } else {
        mCenteredItem = NULL;
    }
}

//　SizeGripperの位置を所定の位置へ配置
void GraphicsFlexibleFrame::updateHandleItemPositions()
{
    foreach (QGraphicsItem* item, this->childItems()) {
        if(item->type() == SizeGrip::Type) {
            switch (((SizeGrip*)item)->handlingPosition()) {
            case TopLeft:
                item->setPos(mDefRect.topLeft());
                break;
            case Top:
                item->setPos(mDefRect.left() + mDefRect.width() / 2 - 1,
                             mDefRect.top());
                break;
            case TopRight:
                item->setPos(mDefRect.topRight());
                break;
            case Right:
                item->setPos(mDefRect.right(),
                             mDefRect.top() + mDefRect.height() / 2 - 1);
                break;
            case BottomRight:
                item->setPos(mDefRect.bottomRight());
                break;
            case Bottom:
                item->setPos(mDefRect.left() + mDefRect.width() / 2 - 1,
                             mDefRect.bottom());
                break;
            case BottomLeft:
                item->setPos(mDefRect.bottomLeft());
                break;
            case Left:
                item->setPos(mDefRect.left(),
                             mDefRect.top() + mDefRect.height() / 2 - 1);
                break;
            case Rotate:
                item->setPos(mDefRect.left() + mDefRect.width() / 2 - 1,
                             mDefRect.top() - mDefRect.height() * 0.1);
                break;
            }
        }
    }
}

// 最小・最大スケールを超えない様に調整（最小実装済み・最大未実装）
void GraphicsFlexibleFrame::resizeRectWithinLimit(qreal min, qreal max)
{
    if(mCurRect.width() < min || mCurRect.height() < min) {
        qreal rescale;
        if(mCurRect.width() < mCurRect.height()) {
            rescale = min / mCurRect.width();
        } else {
            rescale = min / mCurRect.height();
        }
        mCurRect.setWidth(mCurRect.width() * rescale);
        mCurRect.setHeight(mCurRect.height() * rescale);
    }
}

// FlexibleFrameの選択状態によってGripperの表示非表示を切り替える
QVariant GraphicsFlexibleFrame::itemChange(GraphicsItemChange change, const QVariant &value)
{
    if(change == ItemSelectedChange) {
        if(!this->isSelected()) {
            foreach (QGraphicsItem* item, this->childItems()) {
                item->show();
            }
        } else {
            foreach (QGraphicsItem* item, this->childItems()) {
                item->hide();
            }
            if(mCenteredItem != NULL) { mCenteredItem->show(); }
        }
    }
    return value;
}

//　画像アイテムをSceneへ表示
void GraphicsFlexibleFrame::setToScene(QGraphicsScene* scene)
{
    scene->addItem(this);
    setAnchorCenter();
    updateScale();
    updateMatrix();

    setSelected(true);
}

//　画像アイテムをSceneから削除
//　Sceneから削除してもデータは消えない
void GraphicsFlexibleFrame::removeFromScene(QGraphicsScene* scene)
{
    scene->removeItem(this);
}

//　ｍCurRectの中心と画像自体の中心をそろえる
//　回転してスケールするとCurRectとScene上の画像の中心がずれてしまう
void GraphicsFlexibleFrame::updateRectFit()
{
    QPointF updatePos = this->sceneBoundingRect().center();//mapToScene(this->rect().center());
    updatePos = {updatePos.x() - mCurRect.width() / 2, updatePos.y() - mCurRect.height() / 2};
    mCurRect.moveTo(updatePos);
}

void GraphicsFlexibleFrame::applyParser(ParserCore* parser)
{
    if(mCenteredItem == NULL) { return; }

    ((VectorImageItem*)mCenteredItem)->applyParser(parser);
}

//　Scene上のxyの動きを画像座標系のxyへ変換
// 回転Gripperを動かしたとき、その差分は画像の回転状態を考慮しないので、その対応
QPointF GraphicsFlexibleFrame::transformItemCoordinate(QPointF pos)
{
    return QTransform().rotate(-mCurAngle).map(pos);
}

// ｍRectの代表8点の位置を返す
//　Transformですべての変換を行うので、ｍRectのポイントの位置＝Scene上の位置といえる
QPointF GraphicsFlexibleFrame::rectPos(int pos)
{
    qreal centerX = mCurRect.center().x();
    qreal centerY = mCurRect.center().y();
    switch (pos) {
    case TopLeft:
        return  mCurRect.topLeft();
    case Top:
        return  QPointF(centerX, mCurRect.top());
    case TopRight:
        return  mCurRect.topRight();
    case Right:
        return  QPointF(mCurRect.right(), centerY);
    case BottomRight:
        return  mCurRect.bottomRight();
    case Bottom:
        return  QPointF(centerX, mCurRect.bottom());
    case BottomLeft:
        return  mCurRect.bottomLeft();
    case Left:
        return  QPointF(mCurRect.left(), centerY);
    case Rotate:
        return  QPointF(centerX, mCurRect.top() - 20);
    }
    return mCurRect.topLeft();
}

// 等倍化処理
//　センターの位置が変わるのでそれも考慮
void GraphicsFlexibleFrame::actualFit()
{
    mCurRect.setWidth(mDefRect.width());
    mCurRect.setHeight(mDefRect.height());
    setAnchorCenter();
    updateScale();
}

//グループ化解除時にグループの変更を各アイテムに適用する
void  GraphicsFlexibleFrame::applyGroupModify(QPointF newCenter, qreal groupedScaleX, qreal groupedScaleY, qreal groupedAngle)
{
    mCurAngle += groupedAngle;

    mCurRect.setWidth(mCurRect.width()*groupedScaleX);
    mCurRect.setHeight(mCurRect.height()*groupedScaleY);

    QPointF newTopLeft = {newCenter.x() - mCurRect.width() / 2, newCenter.y() - mCurRect.height() / 2};
    mCurRect.moveTo(newTopLeft);

    setAnchorCenter();
    updateScale();
}

//　グループ解放時に個々のアイテムの新しい中心位置を求める計算
//　これがないと個々のアイテムの復帰位置がわからない
QPointF GraphicsFlexibleFrame::calcNewCenter(QPointF point)
{
    QPointF groupedCenter = point - mDiffOrigin;
    QPointF newCenter = (mScaleMatrix * mRotateMatrix).map(groupedCenter);

    return newCenter;
}

//　操作直前の情報の更新
void GraphicsFlexibleFrame::passCurRect()
{
    mPreRect = mCurRect;
}

//　操作直前の情報の更新
void GraphicsFlexibleFrame::passCurAngle()
{
    mPreAngle = mCurAngle;
}

qreal GraphicsFlexibleFrame::transformParam0()
{
    return transform().m11();
}

qreal GraphicsFlexibleFrame::transformParam1()
{
    return transform().m12();
}

qreal GraphicsFlexibleFrame::transformParam2()
{
    return transform().m21();
}

qreal GraphicsFlexibleFrame::transformParam3()
{
    return transform().m22();
}

qreal GraphicsFlexibleFrame::transformParam4()
{
    return transform().m31();
}

qreal GraphicsFlexibleFrame::transformParam5()
{
    return transform().m32();
}

void GraphicsFlexibleFrame::hideRotateGrip()
{
    mIsRaster = true;
    foreach (QGraphicsItem* item, childItems()) {
        if(item->type() == UserType + 11) { //SizeGrip
            if(((SizeGrip*)item)->handlingPosition() == Rotate) {
                item->setParentItem(NULL);
                return;
            }
        }
    }
}
