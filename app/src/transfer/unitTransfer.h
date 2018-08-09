//---------------------------------------------------------------------------
// 単位変換
//---------------------------------------------------------------------------
#ifndef UNITTRANSFER_H
#define UNITTRANSFER_H
//---------------------------------------------------------------------------
#include <QtCore>
//---------------------------------------------------------------------------
// SVG単位
enum EN_SVG_UNIT {
    EN_SVG_UNIT_NON = 0,
    EN_SVG_UNIT_PX,
    EN_SVG_UNIT_PT,
    EN_SVG_UNIT_PC,
    EN_SVG_UNIT_CM,
    EN_SVG_UNIT_MM,
    EN_SVG_UNIT_IN,

    EN_SVG_UNIT_MAX
};
Q_DECLARE_METATYPE(EN_SVG_UNIT)
//---------------------------------------------------------------------------
class UnitTransfer
{
public:
    static qreal toSvgPixel(qreal dValue, EN_SVG_UNIT enUnit, qreal dDpi);
    static QPointF toSvgPixelPoint(const QPointF &p, EN_SVG_UNIT enUnit, qreal dDpi);
    static qreal SvgPixelto(qreal dValue, EN_SVG_UNIT enUnit, qreal dDpi);
    static QPointF SvgPixelPointto(const QPointF &p, EN_SVG_UNIT enUnit, qreal dDpi);
};
//---------------------------------------------------------------------------
#endif // UNITTRANSFER_H
