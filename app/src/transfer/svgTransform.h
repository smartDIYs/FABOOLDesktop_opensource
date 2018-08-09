//---------------------------------------------------------------------------
// SVG座標変換クラス
//---------------------------------------------------------------------------
#ifndef SVGTRANSFORM_H
#define SVGTRANSFORM_H
//---------------------------------------------------------------------------
#include <QtCore>
#include "common/common.h"
#include "unitTransfer.h"
//---------------------------------------------------------------------------
#define DF_SVG_TRANS_ITEM   6
//---------------------------------------------------------------------------
class SvgTransform
{
    QTTEST_OBJECT

public:
    SvgTransform();
    SvgTransform(const SvgTransform &src);
    SvgTransform(const SvgTransform &src1, const SvgTransform &src2);
    ~SvgTransform();
    SvgTransform &operator=(const SvgTransform &src);
    void initMenber();
    bool setTransform(const QString &strTransform, EN_SVG_UNIT enUnit, qreal dDpi);

    void setTranslate(qreal d1);
    void setTranslate(qreal d1, qreal d2);
    void setRotate(qreal d1, qreal d2, qreal d3);
    void setRotate(qreal d1);
    void setScale(qreal d1);
    void setScale(qreal d1, qreal d2);
    void setMatrix(qreal d1, qreal d2, qreal d3, qreal d4, qreal d5, qreal d6);
    void setSkewX(qreal d1);
    void setSkewY(qreal d1);

    QPointF applyTransform(const QPointF &p);
    QString toString();
    qreal getScale();

    void readData(const QJsonObject &json);
    void writeDown(QJsonObject &json) const;

protected:
    void deleteMenber();
    SvgTransform &copyMenber(const SvgTransform &src);
    void getTransformValue(const QString &strFormKind, const QStringList &params, EN_SVG_UNIT enUnit, qreal dDpi);
    void multTransform(qreal dAddTrans[]);

private:
    qreal           m_dTransForm[DF_SVG_TRANS_ITEM];

};
Q_DECLARE_METATYPE(SvgTransform)
//---------------------------------------------------------------------------
#endif // SVGTRANSFORM_H
