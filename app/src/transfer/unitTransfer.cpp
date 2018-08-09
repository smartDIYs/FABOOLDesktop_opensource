//---------------------------------------------------------------------------
#include "unitTransfer.h"
//---------------------------------------------------------------------------
// SVG指定単位をPXに変換
qreal UnitTransfer::toSvgPixel(qreal dValue, EN_SVG_UNIT enUnit, qreal dDpi)
{
    switch (enUnit) {
    case EN_SVG_UNIT_PT:
        return dValue * (1.0 / 72.0 * dDpi);
    case EN_SVG_UNIT_PC:
        return dValue * (1.0 / 6.0 * dDpi);
    case EN_SVG_UNIT_CM:
        return dValue * (dDpi / 2.54);
    case EN_SVG_UNIT_MM:
        return dValue * (dDpi / 25.4);
    case EN_SVG_UNIT_IN:
        return dValue * dDpi;
    default:
        return dValue;
    }
}
//---------------------------------------------------------------------------
QPointF UnitTransfer::toSvgPixelPoint(const QPointF &p, EN_SVG_UNIT enUnit, qreal dDpi)
{
    qreal d;

    switch (enUnit) {
    case EN_SVG_UNIT_PT:
        d = 1.0 / 72.0 * dDpi;
        break;
    case EN_SVG_UNIT_PC:
        d = 1.0 / 6.0 * dDpi;
        break;
    case EN_SVG_UNIT_CM:
        d = dDpi / 2.54;
        break;
    case EN_SVG_UNIT_MM:
        d = dDpi / 25.4;
        break;
    case EN_SVG_UNIT_IN:
        d = dDpi;
        break;
    default:
        d = 1.0;
        break;
    }

    return QPointF(p.x() * d, p.y() * d);
}
//---------------------------------------------------------------------------
// PXからSVG指定単位に変換
qreal UnitTransfer::SvgPixelto(qreal dValue, EN_SVG_UNIT enUnit, qreal dDpi)
{
    switch (enUnit) {
    case EN_SVG_UNIT_PT:
        return dValue / (1.0 / 72.0 * dDpi);
    case EN_SVG_UNIT_PC:
        return dValue / (1.0 / 6.0 * dDpi);
    case EN_SVG_UNIT_CM:
        return dValue / (dDpi / 2.54);
    case EN_SVG_UNIT_MM:
        return dValue / (dDpi / 25.4);
    case EN_SVG_UNIT_IN:
        return dValue / dDpi;
    default:
        return dValue;
    }
}
//---------------------------------------------------------------------------
QPointF UnitTransfer::SvgPixelPointto(const QPointF &p, EN_SVG_UNIT enUnit, qreal dDpi)
{
    qreal d;

    switch (enUnit) {
    case EN_SVG_UNIT_PT:
        d = 1.0 / 72.0 * dDpi;
        break;
    case EN_SVG_UNIT_PC:
        d = 1.0 / 6.0 * dDpi;
        break;
    case EN_SVG_UNIT_CM:
        d = dDpi / 2.54;
        break;
    case EN_SVG_UNIT_MM:
        d = dDpi / 25.4;
        break;
    case EN_SVG_UNIT_IN:
        d = dDpi;
        break;
    default:
        d = 1.0;
        break;
    }

    return QPointF(p.x() / d, p.y() / d);
}
//---------------------------------------------------------------------------
