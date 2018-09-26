//---------------------------------------------------------------------------
#include "svgTransform.h"
//---------------------------------------------------------------------------
#define DEG_TO_RAD  M_PI / 180.0
#define RAD_TO_DEG  180.0 / M_PI
//---------------------------------------------------------------------------
SvgTransform::SvgTransform()
{
    initMenber();
}
//---------------------------------------------------------------------------
SvgTransform::SvgTransform(const SvgTransform &src)
{
    initMenber();
    copyMenber(src);
}
//---------------------------------------------------------------------------
SvgTransform::SvgTransform(const SvgTransform &src1, const SvgTransform &src2)
{
    m_dTransForm[0] = src1.m_dTransForm[0] * src2.m_dTransForm[0] + src1.m_dTransForm[2] * src2.m_dTransForm[1];
    m_dTransForm[1] = src1.m_dTransForm[1] * src2.m_dTransForm[0] + src1.m_dTransForm[3] * src2.m_dTransForm[1];
    m_dTransForm[2] = src1.m_dTransForm[0] * src2.m_dTransForm[2] + src1.m_dTransForm[2] * src2.m_dTransForm[3];
    m_dTransForm[3] = src1.m_dTransForm[1] * src2.m_dTransForm[2] + src1.m_dTransForm[3] * src2.m_dTransForm[3];
    m_dTransForm[4] = src1.m_dTransForm[0] * src2.m_dTransForm[4] + src1.m_dTransForm[2] * src2.m_dTransForm[5] + src1.m_dTransForm[4];
    m_dTransForm[5] = src1.m_dTransForm[1] * src2.m_dTransForm[4] + src1.m_dTransForm[3] * src2.m_dTransForm[5] + src1.m_dTransForm[5];
}
//---------------------------------------------------------------------------
SvgTransform::~SvgTransform()
{
    deleteMenber();
}
//---------------------------------------------------------------------------
SvgTransform &SvgTransform::operator=(const SvgTransform &src)
{
    deleteMenber();

    return copyMenber(src);
}
//---------------------------------------------------------------------------
void SvgTransform::initMenber()
{
    m_dTransForm[0] = 1.0;
    m_dTransForm[1] = 0.0;
    m_dTransForm[2] = 0.0;
    m_dTransForm[3] = 1.0;
    m_dTransForm[4] = 0.0;
    m_dTransForm[5] = 0.0;
}
//---------------------------------------------------------------------------
void SvgTransform::deleteMenber()
{
}
//---------------------------------------------------------------------------
SvgTransform &SvgTransform::copyMenber(const SvgTransform &src)
{
    for (int i = 0; i < DF_SVG_TRANS_ITEM; ++i) {
        m_dTransForm[i] = src.m_dTransForm[i];
    }

    return *this;
}
//---------------------------------------------------------------------------
bool SvgTransform::setTransform(const QString &strTransform, EN_SVG_UNIT enUnit, qreal dDpi)
{
    if (strTransform.trimmed() == "") {
        return true;
    }

    QRegularExpression re("([a-z]+\\s*\\([^\\)]*\\))");
    QRegularExpressionMatchIterator i = re.globalMatch(strTransform);

    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QStringList list = match.captured(1).split('(');
        QString strFormKind = list.at(0).trimmed();
        QStringList params = list.at(1).mid(0, list.at(1).length() - 1).split(QRegExp(",|\\s"), QString::SkipEmptyParts);
        getTransformValue(strFormKind, params, enUnit, dDpi);
    }

    return true;
}
//---------------------------------------------------------------------------
void SvgTransform::getTransformValue(const QString &strFormKind, const QStringList &params, EN_SVG_UNIT enUnit, qreal dDpi)
{
    int iParaCnt = params.length();
    qreal dValue[DF_SVG_TRANS_ITEM];

    // 数値がない場合エラー
    if (iParaCnt == 0) {
        return;
    }
    // 数値に変換
    for (int i = 0; i < iParaCnt; i++) {
        bool ok;
        dValue[i] = params.at(i).trimmed().toDouble(&ok);
        if (ok == false) {
            return;
        }
    }
    // 要素ごとに設定
    if (strFormKind == "translate") {
        if (iParaCnt == 1) {
            setTranslate(UnitTransfer::toSvgPixel(dValue[0], enUnit, dDpi));
        } else if (iParaCnt == 2) {
            setTranslate(UnitTransfer::toSvgPixel(dValue[0], enUnit, dDpi), UnitTransfer::toSvgPixel(dValue[1], enUnit, dDpi));
        }
    } else if (strFormKind == "rotate") {
        if (iParaCnt == 3) {
            setRotate(dValue[0], dValue[1], dValue[2]);
        } else if (iParaCnt == 1) {
            setRotate(dValue[0]);
        }
    } else if (strFormKind == "scale") {
        if (iParaCnt == 1) {
            setScale(dValue[0]);
        } else if (iParaCnt == 2) {
            setScale(dValue[0], dValue[1]);
        }
    } else if (strFormKind == "matrix") {
        if (iParaCnt == 6) {
            setMatrix(dValue[0], dValue[1], dValue[2], dValue[3], UnitTransfer::toSvgPixel(dValue[4], enUnit, dDpi), UnitTransfer::toSvgPixel(dValue[5], enUnit, dDpi));
        }
    } else if (strFormKind == "skewX") {
        if (iParaCnt == 1) {
            setSkewX(dValue[0]);
        }
    } else if (strFormKind == "skewY") {
        if (iParaCnt == 1) {
            setSkewY(dValue[0]);
        }
    }
}
//---------------------------------------------------------------------------
void SvgTransform::setTranslate(qreal d1)
{
    qreal dAddTrans[DF_SVG_TRANS_ITEM];

    dAddTrans[0] = 1.0;
    dAddTrans[1] = 0.0;
    dAddTrans[2] = 0.0;
    dAddTrans[3] = 1.0;
    dAddTrans[4] = d1;
    dAddTrans[5] = d1;
    multTransform(dAddTrans);
}
//---------------------------------------------------------------------------
void SvgTransform::setTranslate(qreal d1, qreal d2)
{
    qreal dAddTrans[DF_SVG_TRANS_ITEM];

    dAddTrans[0] = 1.0;
    dAddTrans[1] = 0.0;
    dAddTrans[2] = 0.0;
    dAddTrans[3] = 1.0;
    dAddTrans[4] = d1;
    dAddTrans[5] = d2;
    multTransform(dAddTrans);
}
//---------------------------------------------------------------------------
void SvgTransform::setRotate(qreal d1, qreal d2, qreal d3)
{
    qreal dAddTrans[DF_SVG_TRANS_ITEM];
    qreal   dAngle = d1 * DEG_TO_RAD;

    dAddTrans[0] = 1.0;
    dAddTrans[1] = 0.0;
    dAddTrans[2] = 0.0;
    dAddTrans[3] = 1.0;
    dAddTrans[4] = d2;
    dAddTrans[5] = d3;
    multTransform(dAddTrans);

    dAddTrans[0] = qCos(dAngle);
    dAddTrans[1] = qSin(dAngle);
    dAddTrans[2] = qSin(dAngle) * -1;
    dAddTrans[3] = qCos(dAngle);
    dAddTrans[4] = 0.0;
    dAddTrans[5] = 0.0;
    multTransform(dAddTrans);

    dAddTrans[0] = 1.0;
    dAddTrans[1] = 0.0;
    dAddTrans[2] = 0.0;
    dAddTrans[3] = 1.0;
    dAddTrans[4] = d2 * -1;
    dAddTrans[5] = d2 * -1;
    multTransform(dAddTrans);
}
//---------------------------------------------------------------------------
void SvgTransform::setRotate(qreal d1)
{
    qreal dAddTrans[DF_SVG_TRANS_ITEM];
    qreal   dAngle = d1 * DEG_TO_RAD;

    dAddTrans[0] = qCos(dAngle);
    dAddTrans[1] = qSin(dAngle);
    dAddTrans[2] = qSin(dAngle) * -1;
    dAddTrans[3] = qCos(dAngle);
    dAddTrans[4] = 0.0;
    dAddTrans[5] = 0.0;
    multTransform(dAddTrans);
}
//---------------------------------------------------------------------------
void SvgTransform::setScale(qreal d1)
{
    qreal dAddTrans[DF_SVG_TRANS_ITEM];

    dAddTrans[0] = d1;
    dAddTrans[1] = 0.0;
    dAddTrans[2] = 0.0;
    dAddTrans[3] = d1;
    dAddTrans[4] = 0.0;
    dAddTrans[5] = 0.0;
    multTransform(dAddTrans);
}
//---------------------------------------------------------------------------
void SvgTransform::setScale(qreal d1, qreal d2)
{
    qreal dAddTrans[DF_SVG_TRANS_ITEM];

    dAddTrans[0] = d1;
    dAddTrans[1] = 0.0;
    dAddTrans[2] = 0.0;
    dAddTrans[3] = d2;
    dAddTrans[4] = 0.0;
    dAddTrans[5] = 0.0;
    multTransform(dAddTrans);
}
//---------------------------------------------------------------------------
void SvgTransform::setMatrix(qreal d1, qreal d2, qreal d3, qreal d4, qreal d5, qreal d6)
{
    qreal dAddTrans[DF_SVG_TRANS_ITEM];

    dAddTrans[0] = d1;
    dAddTrans[1] = d2;
    dAddTrans[2] = d3;
    dAddTrans[3] = d4;
    dAddTrans[4] = d5;
    dAddTrans[5] = d6;
    multTransform(dAddTrans);
}
//---------------------------------------------------------------------------
void SvgTransform::setSkewX(qreal d1)
{
    qreal dAddTrans[DF_SVG_TRANS_ITEM];

    qreal   dAngle = d1 * DEG_TO_RAD;
    dAddTrans[0] = 1.0;
    dAddTrans[1] = 0.0;
    dAddTrans[2] = qTan(dAngle);
    dAddTrans[3] = 1.0;
    dAddTrans[4] = 0.0;
    dAddTrans[5] = 0.0;
    multTransform(dAddTrans);
}
//---------------------------------------------------------------------------
void SvgTransform::setSkewY(qreal d1)
{
    qreal dAddTrans[DF_SVG_TRANS_ITEM];

    qreal   dAngle = d1 * DEG_TO_RAD;
    dAddTrans[0] = 1.0;
    dAddTrans[1] = qTan(dAngle);
    dAddTrans[2] = 0.0;
    dAddTrans[3] = 1.0;
    dAddTrans[4] = 0.0;
    dAddTrans[5] = 0.0;
    multTransform(dAddTrans);
}
//---------------------------------------------------------------------------
void SvgTransform::multTransform(qreal dAddTrans[])
{
    qreal   dBkTrans[DF_SVG_TRANS_ITEM];

    dBkTrans[0] = m_dTransForm[0];
    dBkTrans[1] = m_dTransForm[1];
    dBkTrans[2] = m_dTransForm[2];
    dBkTrans[3] = m_dTransForm[3];
    dBkTrans[4] = m_dTransForm[4];
    dBkTrans[5] = m_dTransForm[5];

    m_dTransForm[0] = dBkTrans[0] * dAddTrans[0] + dBkTrans[2] * dAddTrans[1];
    m_dTransForm[1] = dBkTrans[1] * dAddTrans[0] + dBkTrans[3] * dAddTrans[1];
    m_dTransForm[2] = dBkTrans[0] * dAddTrans[2] + dBkTrans[2] * dAddTrans[3];
    m_dTransForm[3] = dBkTrans[1] * dAddTrans[2] + dBkTrans[3] * dAddTrans[3];
    m_dTransForm[4] = dBkTrans[0] * dAddTrans[4] + dBkTrans[2] * dAddTrans[5] + dBkTrans[4];
    m_dTransForm[5] = dBkTrans[1] * dAddTrans[4] + dBkTrans[3] * dAddTrans[5] + dBkTrans[5];

}
//---------------------------------------------------------------------------
QPointF SvgTransform::applyTransform(const QPointF &p)
{
    return QPointF(m_dTransForm[0] * p.x() + m_dTransForm[2] * p.y() + m_dTransForm[4],
                   m_dTransForm[1] * p.x() + m_dTransForm[3] * p.y() + m_dTransForm[5]);
}
//---------------------------------------------------------------------------
QString SvgTransform::toString()
{
    return QString("matrix(%1,%2,%3,%4,%5,%6)").arg(m_dTransForm[0]).arg(m_dTransForm[1]).arg(m_dTransForm[2]).arg(m_dTransForm[3]).arg(m_dTransForm[4]).arg(m_dTransForm[5]);
}
//---------------------------------------------------------------------------
qreal SvgTransform::getScale()
{
    qreal sx = qSqrt(m_dTransForm[0] * m_dTransForm[0] + m_dTransForm[1] * m_dTransForm[1]);
    qreal sy = qSqrt(m_dTransForm[2] * m_dTransForm[2] + m_dTransForm[3] * m_dTransForm[3]);

    if (sx > sy) {
        return sx;
    } else {
        return sy;
    }
}
//---------------------------------------------------------------------------
void SvgTransform::readData(const QJsonObject &json)
{
    m_dTransForm[0] = json[JSON_TAG_SVG_TRANSFORM_0].toDouble();
    m_dTransForm[1] = json[JSON_TAG_SVG_TRANSFORM_1].toDouble();
    m_dTransForm[2] = json[JSON_TAG_SVG_TRANSFORM_2].toDouble();
    m_dTransForm[3] = json[JSON_TAG_SVG_TRANSFORM_3].toDouble();
    m_dTransForm[4] = json[JSON_TAG_SVG_TRANSFORM_4].toDouble();
    m_dTransForm[5] = json[JSON_TAG_SVG_TRANSFORM_5].toDouble();
}
//---------------------------------------------------------------------------
void SvgTransform::writeDown(QJsonObject &json) const
{
    json[JSON_TAG_SVG_TRANSFORM_0] = m_dTransForm[0];
    json[JSON_TAG_SVG_TRANSFORM_1] = m_dTransForm[1];
    json[JSON_TAG_SVG_TRANSFORM_2] = m_dTransForm[2];
    json[JSON_TAG_SVG_TRANSFORM_3] = m_dTransForm[3];
    json[JSON_TAG_SVG_TRANSFORM_4] = m_dTransForm[4];
    json[JSON_TAG_SVG_TRANSFORM_5] = m_dTransForm[5];
}
//---------------------------------------------------------------------------
