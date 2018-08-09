//---------------------------------------------------------------------------
// SVG定義共通ヘッダー
//---------------------------------------------------------------------------
#ifndef SVGCOMMON_H
#define SVGCOMMON_H
//---------------------------------------------------------------------------
#include "unitTransfer.h"
#include "svgColor.h"
#include "svgTransform.h"

//---------------------------------------------------------------------------
#define DF_SVG_TO_GCODE_TOLERANCE   0.1
//---------------------------------------------------------------------------
// SVG解析・整形定義
//---------------------------------------------------------------------------
// SVG作成ソフト
enum EN_SVG_SOFT {
    EN_SVG_SOFT_OTHER = 0,
    EN_SVG_SOFT_INKSCAPE,
    EN_SVG_SOFT_ILLUSTRATOR,
    EN_SVG_SOFT_INTAGLIO,
    EN_SVG_SOFT_COREDRAW,
    EN_SVG_SOFT_QT,
    EN_SVG_SOFT_VISIO,
    EN_SVG_SOFT_DXF2SVG,
    EN_SVG_IMPORTED,

    EN_SVG_SOFT_MAX
};
Q_DECLARE_METATYPE(EN_SVG_SOFT)
// viewBox構造体
struct ST_SVG_VIEW_BOX {
    qreal   x;
    qreal   y;
    qreal   width;
    qreal   height;
};
Q_DECLARE_METATYPE(ST_SVG_VIEW_BOX)
// Bounding Box構造体
struct ST_SVG_BBOX {
    QPointF p1;
    QPointF p2;
};
Q_DECLARE_METATYPE(ST_SVG_BBOX)
// SVG要素共通設定
struct ST_SVG_ELE_COM {
    SvgColor fill;
    SvgColor stroke;
    qreal strokeWidth;
    QString dashArray;
    SvgTransform  transform;
    bool        display;
    QMap<QString, QString> style;
};
Q_DECLARE_METATYPE(ST_SVG_ELE_COM)

//---------------------------------------------------------------------------
// SVG PATH管理定義
//---------------------------------------------------------------------------
// 絶対値、相対値
enum EN_SVG_PATH_VALUE {
    // 絶対値
    EN_SVG_PATH_VALUE_A = 0,
    // 相対値
    EN_SVG_PATH_VALUE_R,

    EN_SVG_PATH_VALUE_MAX
};
Q_DECLARE_METATYPE(EN_SVG_PATH_VALUE)

//---------------------------------------------------------------------------
// SVG PATH要素管理定義
//---------------------------------------------------------------------------
// PATH要素種別
enum EN_SVG_PATH_ELE {
    EN_SVG_PATH_ELE_NONE = 0,
    // 初期位置、位置のスキップ
    EN_SVG_PATH_ELE_M,
    // 直線
    EN_SVG_PATH_ELE_L,
    // 水平線
    EN_SVG_PATH_ELE_H,
    // 垂直線
    EN_SVG_PATH_ELE_V,
    // 2次ベジェ曲線
    EN_SVG_PATH_ELE_Q,
    // 2次ベジェ曲線(制御点省略)
    EN_SVG_PATH_ELE_T,
    // 3次ベジェ曲線
    EN_SVG_PATH_ELE_C,
    // 3次ベジェ曲線(制御点省略)
    EN_SVG_PATH_ELE_S,
    // 円弧
    EN_SVG_PATH_ELE_A,
    // 直線を引いて線を閉じる
    EN_SVG_PATH_ELE_Z,

    EN_SVG_PATH_ELE_MAX
};
Q_DECLARE_METATYPE(EN_SVG_PATH_ELE)
// 直線
struct ST_SVG_PATH_POINT {
    QPointF         p1;
};
Q_DECLARE_METATYPE(ST_SVG_PATH_POINT)
// 2次曲線
struct ST_SVG_PATH_QUADRATIC {
    QPointF         p1;
    QPointF         p2;
    QPointF         p3;
};
Q_DECLARE_METATYPE(ST_SVG_PATH_QUADRATIC)
// 3次曲線
struct ST_SVG_PATH_BEZIER {
    QPointF         p1;
    QPointF         p2;
    QPointF         p3;
    QPointF         p4;
};
Q_DECLARE_METATYPE(ST_SVG_PATH_BEZIER)
// 円弧
struct ST_SVG_PATH_ARC {
    QList<ST_SVG_PATH_BEZIER>   listBezire;
};
Q_DECLARE_METATYPE(ST_SVG_PATH_ARC)

//---------------------------------------------------------------------------
typedef QList<QPointF> QPointList;
typedef QList<QPointList> QSubPointList;
//---------------------------------------------------------------------------

//---------------------------------------------------------------------------
#endif // SVGCOMMON_H
