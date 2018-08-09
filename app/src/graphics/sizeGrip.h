#ifndef QGRIPPER_H
#define QGRIPPER_H

#include "common/common.h"
#include "graphicsFlexibleFrame.h"

// 拡大縮小＆回転用のグリッパークラス
// グリッパーの位置が移動するとFlexibleFrameが持つサイズ変更矩形が変化し、
// 変更矩形の形に合うようにトランスフォームマトリックスを更新・適用する

enum gripPos {
    Top         = 0x1,
    Right       = 0x2,
    Bottom      = 0x4,
    Left        = 0x8,
    TopLeft     = Top | Left,
    TopRight    = Top | Right,
    BottomLeft  = Bottom | Left,
    BottomRight = Bottom | Right,
    Rotate      = 0x0
};

class SizeGrip : public QGraphicsRectItem
{
    QTTEST_OBJECT
public:
// Qt override
    SizeGrip(int handlePos, GraphicsFlexibleFrame* parent);
    enum { Type = UserType + 11 };
    int type() const
    {
        return Type;
    }

    void mousePressEvent(QGraphicsSceneMouseEvent* e);      // 回転時はアンカーの位置を更新する
    void mouseMoveEvent (QGraphicsSceneMouseEvent* e);      // グリッパーがつかまれたまま移動するとその都度スケーリングや回転量が更新され、画像アイテムに適用される

// original
    int  handlingPosition() const { return mHandlingPos; }   // 自身のポジション番号を返す

private:
    int                     mHandlingPos;   // 自身のポジション番号
    GraphicsFlexibleFrame*  mParent;        // 親
};
#endif // QGRIPPERHANDLE_H
