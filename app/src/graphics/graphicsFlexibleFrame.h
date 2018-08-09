#ifndef SIZEGRIPPER_H
#define SIZEGRIPPER_H

// 移動、拡大縮小、回転用の入れ物
// 本クラスインスタンス化時に、Scene上に描画したいアイテムを渡すことで、そのアイテムの拡大縮小を行えるようになる
// Scene上の操作はこのクラスを介して行うようになるので、SelectedItemｓで拾うのはこコイツである点に注意

#include <QGraphicsRectItem>

#include "common/common.h"
#include "ui/graphicArea/mainScene.h"


class ItemInfo;

class GraphicsFlexibleFrame : public QGraphicsRectItem
{
    QTTEST_OBJECT

public:
    GraphicsFlexibleFrame(QGraphicsItem* target = 0);
    virtual ~GraphicsFlexibleFrame();

    enum { Type = UserType + 10 };
    int type() const { return Type; }

    virtual void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget);

protected:
    virtual QVariant itemChange(GraphicsItemChange change, const QVariant &value);  // FlexibleFrameが選択状態にあるとき、Gripperを表示する

public:
    void    setTopLeft(const QPointF& pos);        // サイズ変更用矩形の更新
    void    setTop(qreal y);
    void    setTopRight(const QPointF& pos);
    void    setRight(qreal x);
    void    setBottomRight(const QPointF& pos);
    void    setBottom(qreal y);
    void    setBottomLeft(const QPointF& pos);
    void    setLeft(qreal x);

    void    setAnchorCenter();                              // 回転用のアンカーの更新
    void    updateAngle(QPointF lastPos, QPointF curPos);   // 回転量の更新
    void    updateScale();                                  // スケール量の更新

    void    updateMatrix();                                 // 回転、スケーリング行列の更新と適用
    void    updateRectFit();                                // マウスを離したときにTargetアイテムの位置にサイズ変更用矩形の位置をフィッティングする
    void    applyParser(ParserCore* parser);

    QPointF transformItemCoordinate(QPointF pos);           // Gripの移動が起きたときに、回転量を考慮して新たに移動量を計算する
    QPointF rectPos(int pos);                               // サイズ変更用矩形のそれぞれのポジションの座標をもらう関数

    void    setToScene(QGraphicsScene* scene);              // シーン上へ自身を追加 TargetとGripは子アイテムなので同時に追加される
    void    removeFromScene(QGraphicsScene* scene);         // シーン上から自身を削除

    QPointF rectPos()               { return mCurRect.topLeft(); }
    QPointF rectCenter()            { return mCurRect.center(); }
    qreal   rectRotate()            { return mCurAngle; }
    qreal   rectScaleX()            { return mScaleW; }
    qreal   rectScaleY()            { return mScaleH; }
    qreal   rectWidth()             { return mCurRect.width(); }
    qreal   rectHeight()            { return mCurRect.height(); }
    void    setRectPosX(qreal x)    { mCurRect.moveLeft(x);}
    void    setRectPosY(qreal y)    { mCurRect.moveTop(y); }
    void    setRectPos(QPointF pos) { mCurRect.moveTopLeft(pos); }
    void    setRectRotate(qreal r)  { mCurAngle = r; }
    void    setRectWidth(qreal w)   { mCurRect.setWidth(w); }
    void    setRectHeight(qreal h)  { mCurRect.setHeight(h); }

    QRectF  currentRect() {return mCurRect;}
    QRectF  previousRect() {return mPreRect;}
    qreal   previousAngle() {return mPreAngle; }
    void    setCurrentRect(QRectF rect) { mCurRect = rect; updateScale(); }
    QRectF  defRect() const {return mDefRect;}

    void    actualFit();
    void    setParentInfo(ItemInfo* info) { mParentInfo = info; }
    ItemInfo* parentInfo() { return mParentInfo; }

    void    setCenteredItem(QGraphicsItem* target);
    int     centeredItemType()    { return mCenteredItem->type(); }
    QGraphicsItem* centeredItem() { return mCenteredItem; }

    QPointF calcNewCenter(QPointF point);
    void    applyGroupModify(QPointF newCenter, qreal groupedScaleX, qreal groupedScaleY, qreal groupedAngle);

    void    passCurRect();
    void    passCurAngle();

    qreal   transformParam0(); //
    qreal   transformParam1(); //  [11  12  13]
    qreal   transformParam2(); //
    qreal   transformParam3(); //  [21  22  23]
    qreal   transformParam4(); //
    qreal   transformParam5(); //  [31  32  33]

    void    hideRotateGrip();
    bool    isRaster() {return mIsRaster;}

//original
private:
    void    updateHandleItemPositions();// Gripのアライメント調整 初回しか呼ばない
    void    resizeRectWithinLimit(qreal min = 0.01, qreal max = 10000000);  // スケーリング時の消失対策

    QGraphicsItem*       mCenteredItem; // メインになるアイテム  ItemGroupに追加するけど、一応アクセスしやすいようにしておく
    QRectF               mDefRect;      // デフォルトのサイズを持った矩形
    QRectF               mCurRect;      // サイズ更新用矩形
    QRectF               mPreRect;
    qreal                mScaleW;       // Width方向のスケール量
    qreal                mScaleH;       // Height方向のスケール量
    qreal                mCurAngle;     // 回転量 積算型
    qreal                mPreAngle;
    QPointF              mAnchorCenter; // 回転時の基準点

    QTransform           mScaleMatrix;  // スケーリング用トランスフォームマトリックス
    QTransform           mRotateMatrix; // 回転用トランスフォームマトリックス

    ItemInfo*            mParentInfo;

    QPointF              mDiffOrigin;   // グループ化したときの原点からTopLeftまでの距離　グループ解放時の個々のセンター位置計算に使う

    bool                 mIsRaster;  //　ラスタ画像の回転回避用

};
#endif // SIZEGRIPPER_H
