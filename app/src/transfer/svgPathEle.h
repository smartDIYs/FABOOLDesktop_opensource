//---------------------------------------------------------------------------
// SVG PATH要素管理
//---------------------------------------------------------------------------
#ifndef SVGPATHELE_H
#define SVGPATHELE_H
//---------------------------------------------------------------------------
#include <QtCore>
#include "common/common.h"
#include "svgCommon.h"

class SvgPathEle
{
    QTTEST_OBJECT

public:
    SvgPathEle();
    SvgPathEle(const SvgPathEle &src);
    ~SvgPathEle();
    SvgPathEle &operator=(const SvgPathEle &src);
    EN_SVG_PATH_ELE getPathEle();
    QPointF getPoint();
    QPointList getPxPointList();
    void setPoint(EN_SVG_PATH_ELE enEle, const QPointF &p1);
    void setQuadratic(EN_SVG_PATH_ELE enEle, const QPointF &p1, const QPointF &p2, const QPointF &p3);
    void setBezier(EN_SVG_PATH_ELE enEle, const QPointF &p1, const QPointF &p2, const QPointF &p3, const QPointF &p4);
    bool setArc(EN_SVG_PATH_ELE enEle, const QPointF &p1, const QPointF &p2, const QPointF &r, const qreal &xrot, bool large, bool sweep);
    QString getEleStr();
    QString getEleStr(SvgTransform form);
    bool getBBox(ST_SVG_BBOX* pstBBox);
    void getQuadraticBezier(QPointList* pListPoint, SvgTransform* pTransform, qreal dDpi, qreal tolerance2);
    void getCubicBezier(QPointList* pListPoint, SvgTransform* pTransform, qreal dDpi, qreal tolerance2);
    void getArc(QPointList* pListPoint, SvgTransform* pTransform, qreal dDpi, qreal tolerance2);
    void getQuadraticBezier(QPointList* pListPoint, SvgTransform* innerform, SvgTransform* outerform, qreal dDpi, qreal tolerance2, QPointF offset);
    void getCubicBezier(QPointList* pListPoint, SvgTransform* innerform, SvgTransform* outerform, qreal dDpi, qreal tolerance2, QPointF offset);
    void getArc(QPointList* pListPoint, SvgTransform* innerform, SvgTransform* outerform, qreal dDpi, qreal tolerance2, QPointF offset);

    void   readData(const QJsonObject &json);
    void   writeDown(QJsonObject &json) const;
    void   readDataPoint(const QJsonObject &json);
    void   writeDownPoint(QJsonObject &json) const;
    void   readDataQuad(const QJsonArray &json);
    void   writeDownQuad(QJsonArray &json) const;
    void   readDataBezier(const QJsonArray &json, ST_SVG_PATH_BEZIER* bezier);
    void   writeDownBezier(QJsonArray &json, const ST_SVG_PATH_BEZIER& bezier) const;
    void   readDataArc(const QJsonArray &json);
    void   writeDownArc(QJsonArray &json) const;

    void   applyViewBox(QPointF initPos, QPointF addDist, qreal scaleW, qreal scaleH);

protected:
    void initMenber();
    void deleteMenber();
    SvgPathEle &copyMenber(const SvgPathEle &src);
    QPointF rotatePoint(const qreal &originX, const qreal &originY, const qreal &x, const qreal &y, const qreal &radiansX, const qreal &radiansY);

    void getBBoxQuadraticCurve(ST_SVG_BBOX* pstBBox, const QPointF &p1, const QPointF &p2, const QPointF &p3);
    void getBBoxBezierCurve(ST_SVG_BBOX* pstBBox, const QPointF &p1, const QPointF &p2, const QPointF &p3, const QPointF &p4);
    void setBBoxPointX(ST_SVG_BBOX* pstBBox, const qreal &x);
    void setBBoxPointY(ST_SVG_BBOX* pstBBox, const qreal &y);
    qreal calcF(const qreal &t, const qreal &p0, const qreal &p1, const qreal &p2, const qreal &p3);
    void addQuadraticBezier(QPointList* pListPoint, SvgTransform* pTransform, QPointF p1, QPointF p2, QPointF p3, int iLevel, qreal dDpi, qreal tolerance2);
    void addCubicBezier(QPointList* pListPoint, SvgTransform* pTransform, QPointF p1, QPointF p2, QPointF p3, QPointF p4, int iLevel, qreal dDpi, qreal tolerance2);
    void addQuadraticBezier(QPointList* pListPoint, SvgTransform* innerform, SvgTransform* outerform, QPointF p1, QPointF p2, QPointF p3, int iLevel, qreal dDpi, qreal tolerance2, QPointF offset);
    void addCubicBezier(QPointList* pListPoint, SvgTransform* innerform, SvgTransform* outerform, QPointF p1, QPointF p2, QPointF p3, QPointF p4, int iLevel, qreal dDpi, qreal tolerance2, QPointF offset);

    void   applyViewBoxQuad(ST_SVG_PATH_QUADRATIC* quad, QPointF initPos, QPointF addDist, qreal scaleW, qreal scaleH);
    void   applyViewBoxBezier(ST_SVG_PATH_BEZIER* bezier, QPointF initPos, QPointF addDist, qreal scaleW, qreal scaleH);
    void   applyViewBoxArc(ST_SVG_PATH_ARC* arc, QPointF initPos, QPointF addDist, qreal scaleW, qreal scaleH);

private:
    // Path要素種類
    EN_SVG_PATH_ELE         m_enPathEle;
    // ポイント
    ST_SVG_PATH_POINT*      m_pstPoint;
    // 2次曲線
    ST_SVG_PATH_QUADRATIC*  m_pstQuadratic;
    // 3次曲線
    ST_SVG_PATH_BEZIER*     m_pstBezier;
    // 円弧
    ST_SVG_PATH_ARC*        m_pstArc;
};
Q_DECLARE_METATYPE(SvgPathEle)
//---------------------------------------------------------------------------
#endif // SVGPATHELE_H
