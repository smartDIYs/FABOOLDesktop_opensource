//---------------------------------------------------------------------------
#include "svgPathEle.h"
//---------------------------------------------------------------------------
SvgPathEle::SvgPathEle()
{
    initMenber();
}
//---------------------------------------------------------------------------
SvgPathEle::SvgPathEle(const SvgPathEle &src)
{
    initMenber();
    copyMenber(src);
}
//---------------------------------------------------------------------------
SvgPathEle::~SvgPathEle()
{
    deleteMenber();
}
//---------------------------------------------------------------------------
SvgPathEle &SvgPathEle::operator=(const SvgPathEle &src)
{
    deleteMenber();

    return copyMenber(src);
}
//---------------------------------------------------------------------------
void SvgPathEle::initMenber()
{
    // 初期化
    m_enPathEle = EN_SVG_PATH_ELE_NONE;
    m_pstPoint = NULL;
    m_pstQuadratic = NULL;
    m_pstBezier = NULL;
    m_pstArc = NULL;
}
//---------------------------------------------------------------------------
void SvgPathEle::deleteMenber()
{
    // 個別要素メモリ解放
    if (m_pstPoint != NULL) {
        delete m_pstPoint;
        m_pstPoint = NULL;
    }
    if (m_pstQuadratic != NULL) {
        delete m_pstQuadratic;
        m_pstQuadratic = NULL;
    }
    if (m_pstBezier != NULL) {
        delete m_pstBezier;
        m_pstBezier = NULL;
    }
    if (m_pstArc != NULL) {
        delete m_pstArc;
        m_pstArc = NULL;
    }
}
//---------------------------------------------------------------------------
SvgPathEle &SvgPathEle::copyMenber(const SvgPathEle &src)
{
    // 属性コピー
    m_enPathEle = src.m_enPathEle;
    // 要素ごとの設定コピー
    if (src.m_pstPoint != NULL) {
        m_pstPoint = new ST_SVG_PATH_POINT;
        *m_pstPoint = *(src.m_pstPoint);
    }
    if (src.m_pstQuadratic != NULL) {
        m_pstQuadratic = new ST_SVG_PATH_QUADRATIC;
        *m_pstQuadratic = *(src.m_pstQuadratic);
    }
    if (src.m_pstBezier != NULL) {
        m_pstBezier = new ST_SVG_PATH_BEZIER;
        *m_pstBezier = *(src.m_pstBezier);
    }
    if (src.m_pstArc != NULL) {
        m_pstArc = new ST_SVG_PATH_ARC;
        *m_pstArc = *(src.m_pstArc);
    }

    return *this;
}
//---------------------------------------------------------------------------
EN_SVG_PATH_ELE SvgPathEle::getPathEle()
{
    return m_enPathEle;
}
//---------------------------------------------------------------------------
QPointF SvgPathEle::getPoint()
{
    switch (m_enPathEle) {
    case EN_SVG_PATH_ELE_M:
    case EN_SVG_PATH_ELE_L:
    case EN_SVG_PATH_ELE_H:
    case EN_SVG_PATH_ELE_V:
    case EN_SVG_PATH_ELE_Z:
        return m_pstPoint->p1;
    default:
        return QPointF(0.0, 0.0);
        break;
    }
}
//---------------------------------------------------------------------------
QPointList SvgPathEle::getPxPointList()
{
    //㎜換算せずにサブポイント化
    QPointList retPoints;

    switch (m_enPathEle) {
    case EN_SVG_PATH_ELE_NONE:
    case EN_SVG_PATH_ELE_M:
    case EN_SVG_PATH_ELE_L:
    case EN_SVG_PATH_ELE_H:
    case EN_SVG_PATH_ELE_V:
    case EN_SVG_PATH_ELE_Z: {
            retPoints.append(m_pstPoint->p1);
        }
        break;
    case EN_SVG_PATH_ELE_Q:
    case EN_SVG_PATH_ELE_T: {
            SvgTransform unitTrans;
            getQuadraticBezier(&retPoints, &unitTrans, &unitTrans, 25.4, 0.01, QPointF());
        }
        break;
    case EN_SVG_PATH_ELE_C:
    case EN_SVG_PATH_ELE_S: {
            SvgTransform unitTrans;
            getCubicBezier(&retPoints, &unitTrans, &unitTrans, 25.4, 0.01, QPointF());
        }
        break;
    case EN_SVG_PATH_ELE_A: {
            SvgTransform unitTrans;
            getArc(&retPoints, &unitTrans, &unitTrans, 25.4, 0.01, QPointF());
        }
        break;
    default:
        break;
    }

    return retPoints;
}
//---------------------------------------------------------------------------
void SvgPathEle::setPoint(EN_SVG_PATH_ELE enEle, const QPointF &p1)
{
    m_enPathEle = enEle;

    if (m_pstPoint == NULL) {
        m_pstPoint = new ST_SVG_PATH_POINT;
    }
    m_pstPoint->p1 = p1;
}
//---------------------------------------------------------------------------
void SvgPathEle::setQuadratic(EN_SVG_PATH_ELE enEle, const QPointF &p1, const QPointF &p2, const QPointF &p3)
{
    m_enPathEle = enEle;

    if (m_pstQuadratic == NULL) {
        m_pstQuadratic = new ST_SVG_PATH_QUADRATIC;
    }
    m_pstQuadratic->p1 = p1;
    m_pstQuadratic->p2 = p2;
    m_pstQuadratic->p3 = p3;
}
//---------------------------------------------------------------------------
void SvgPathEle::setBezier(EN_SVG_PATH_ELE enEle, const QPointF &p1, const QPointF &p2, const QPointF &p3, const QPointF &p4)
{
    m_enPathEle = enEle;

    if (m_pstBezier == NULL) {
        m_pstBezier = new ST_SVG_PATH_BEZIER;
    }
    m_pstBezier->p1 = p1;
    m_pstBezier->p2 = p2;
    m_pstBezier->p3 = p3;
    m_pstBezier->p4 = p4;
}
//---------------------------------------------------------------------------
bool SvgPathEle::setArc(EN_SVG_PATH_ELE enEle, const QPointF &p1, const QPointF &p2, const QPointF &r, const qreal &xrot, bool large, bool sweep)
{
    m_enPathEle = enEle;

    if (m_pstArc == NULL) {
        m_pstArc = new ST_SVG_PATH_ARC;
    }
    m_pstArc->listBezire.clear();

    // 円弧を3次曲線に変換
    qreal angleRads = qDegreesToRadians(xrot);
    QPointF midPointDistance = QPointF((p1.x() - p2.x()) * 0.5, (p1.y() - p2.y()) * 0.5);
    qreal rx = r.x();
    qreal ry = r.y();

    qreal squareRx = rx * rx;
    qreal squareRy = ry * ry;
    qreal squareX = midPointDistance.x() * midPointDistance.x();
    qreal squareY = midPointDistance.y() * midPointDistance.y();
    qreal radiiScale = squareX / squareRx + squareY / squareRy;
    if (radiiScale > 1.0) {
        qreal sqrtRadiiScale = qSqrt(radiiScale);
        rx *= sqrtRadiiScale;
        ry *= sqrtRadiiScale;
    }
    qreal rxd = qPow(rx * qCos(angleRads), 2) + qPow(ry * qSin(angleRads), 2) ;
    qreal ryd = qPow(rx * qSin(angleRads), 2) + qPow(ry * qCos(angleRads), 2) ;
    rxd = qSqrt(rxd);
    ryd = qSqrt(ryd);
    QPointF point1 = QPointF(p1.x() / rxd, p1.y() / ryd);
    QPointF point2 = QPointF(p2.x() / rxd, p2.y() / ryd);
    QPointF delta = QPointF(point2.x() - point1.x(), point2.y() - point1.y());
    qreal d = delta.x() * delta.x() + delta.y() * delta.y();
    qreal scaleFactorSquared = qMax(1.0 / d - 0.25, 0.0);
    qreal scaleFactor = qSqrt(scaleFactorSquared);
    if (sweep == large) {
        scaleFactor = -scaleFactor;
    }
    delta.setX(delta.x() * scaleFactor);
    delta.setY(delta.y() * scaleFactor);
    QPointF centerPoint = QPointF((point1.x() + point2.x()) * 0.5, (point1.y() + point2.y()) * 0.5);
    centerPoint.setX(centerPoint.x() - delta.y());
    centerPoint.setY(centerPoint.y() + delta.x());
    qreal theta1 = qAtan2(point1.y() - centerPoint.y(), point1.x() - centerPoint.x());
    qreal theta2 = qAtan2(point2.y() - centerPoint.y(), point2.x() - centerPoint.x());
    qreal thetaArc = theta2 - theta1;
    if (thetaArc < 0.0 && sweep == true) {
        thetaArc += 2.0 * M_PI;
    } else if (thetaArc > 0.0 && sweep == false) {
        thetaArc -= 2.0 * M_PI;
    }

    QPointF start = p1;
    int segments =  qCeil(qFabs(thetaArc / ((M_PI / 2.0) + 0.001)));
    for (int i = 0; i < segments; ++i) {
        qreal startTheta = theta1 + i * thetaArc / segments;
        qreal endTheta = theta1 + (i + 1) * thetaArc / segments;
        qreal t = (8.0 / 6.0) * qTan(0.25 * (endTheta - startTheta));
        if (!std::isfinite(t)) {
            return false;
        }
        qreal sinStartTheta = qSin(startTheta);
        qreal cosStartTheta = qCos(startTheta);
        qreal sinEndTheta = qSin(endTheta);
        qreal cosEndTheta = qCos(endTheta);
        point1 = QPointF(cosStartTheta - t * sinStartTheta, sinStartTheta + t * cosStartTheta);
        point1.setX(point1.x() + centerPoint.x());
        point1.setY(point1.y() + centerPoint.y());
        QPointF targetPoint = QPointF(cosEndTheta, sinEndTheta);
        targetPoint.setX(targetPoint.x() + centerPoint.x());
        targetPoint.setY(targetPoint.y() + centerPoint.y());
        point2 = targetPoint;
        point2.setX(point2.x() + (t * sinEndTheta));
        point2.setY(point2.y() + (-t * cosEndTheta));
        // rotate and scale
        point1.setX(point1.x() * rxd);
        point1.setY(point1.y() * ryd);
        point2.setX(point2.x() * rxd);
        point2.setY(point2.y() * ryd);

        targetPoint.setX(targetPoint.x() * rxd);
        targetPoint.setY(targetPoint.y() * ryd);

        ST_SVG_PATH_BEZIER   stBezier;
        stBezier.p1 = start;
        stBezier.p2 = point1;
        stBezier.p3 = point2;
        stBezier.p4 = targetPoint;
        m_pstArc->listBezire.append(stBezier);
        start = targetPoint;
    }

    return true;
}
//---------------------------------------------------------------------------
QPointF SvgPathEle::rotatePoint(const qreal &originX, const qreal &originY, const qreal &x, const qreal &y, const qreal &radiansX, const qreal &radiansY)
{
    QPointF v = QPointF(x - originX, y - originY);
    qreal vx = (v.x() * qCos(radiansX)) - (v.y() * qSin(radiansX));
    qreal vy = (v.x() * qSin(radiansY)) + (v.y() * qCos(radiansY));

    return QPointF(vx + originX, vy + originY);
}
//---------------------------------------------------------------------------
QString SvgPathEle::getEleStr()
{
    QString strEleStr;

    switch (m_enPathEle) {
    case EN_SVG_PATH_ELE_M:
        strEleStr = QString("M %1,%2").arg(m_pstPoint->p1.x()).arg(m_pstPoint->p1.y());
        break;
    case EN_SVG_PATH_ELE_Z:
        strEleStr = "Z";
        break;
    case EN_SVG_PATH_ELE_L:
        strEleStr = QString("L %1,%2").arg(m_pstPoint->p1.x()).arg(m_pstPoint->p1.y());
        break;
    case EN_SVG_PATH_ELE_H:
        strEleStr = QString("H %1").arg(m_pstPoint->p1.x());
        break;
    case EN_SVG_PATH_ELE_V:
        strEleStr = QString("V %1").arg(m_pstPoint->p1.y());
        break;
    case EN_SVG_PATH_ELE_C:
        strEleStr = QString("C %1,%2 %3,%4 %5,%6").arg(m_pstBezier->p2.x()).arg(m_pstBezier->p2.y()).arg(m_pstBezier->p3.x()).arg(m_pstBezier->p3.y()).arg(m_pstBezier->p4.x()).arg(m_pstBezier->p4.y());
        break;
    case EN_SVG_PATH_ELE_S:
        strEleStr = QString("S %1,%2 %3,%4").arg(m_pstBezier->p3.x()).arg(m_pstBezier->p3.y()).arg(m_pstBezier->p4.x()).arg(m_pstBezier->p4.y());
        break;
    case EN_SVG_PATH_ELE_Q:
        strEleStr = QString("Q %1,%2 %3,%4").arg(m_pstQuadratic->p2.x()).arg(m_pstQuadratic->p2.y()).arg(m_pstQuadratic->p3.x()).arg(m_pstQuadratic->p3.y());
        break;
    case EN_SVG_PATH_ELE_T:
        strEleStr = QString("T %1,%2").arg(m_pstQuadratic->p3.x()).arg(m_pstQuadratic->p3.y());
        break;
    case EN_SVG_PATH_ELE_A:
        strEleStr = "";
        for (int i = 0; i < m_pstArc->listBezire.length(); ++i) {
            strEleStr += QString("C %1,%2 %3,%4 %5,%6").arg(m_pstArc->listBezire[i].p2.x()).arg(m_pstArc->listBezire[i].p2.y()).arg(m_pstArc->listBezire[i].p3.x()).arg(m_pstArc->listBezire[i].p3.y()).arg(m_pstArc->listBezire[i].p4.x()).arg(m_pstArc->listBezire[i].p4.y());
            if (i != m_pstArc->listBezire.length() - 1) {
                strEleStr +=  " ";
            }
        }
        break;
    default:
        break;
    }

    return strEleStr;
}
//---------------------------------------------------------------------------
QString SvgPathEle::getEleStr(SvgTransform form)
{
    QString strEleStr;
    QPointF ps;
    QPointF ps2 ;
    QPointF ps3 ;
    QPointF ps4 ;

    switch (m_enPathEle) {
    case EN_SVG_PATH_ELE_M:
        ps = form.applyTransform(m_pstPoint->p1);
        strEleStr = QString("M %1,%2").arg(ps.x()).arg(ps.y());
        break;
    case EN_SVG_PATH_ELE_Z:
        strEleStr = "Z";
        break;
    case EN_SVG_PATH_ELE_L:
        ps = form.applyTransform(m_pstPoint->p1);
        strEleStr = QString("L %1,%2").arg(ps.x()).arg(ps.y());
        break;
    case EN_SVG_PATH_ELE_H:
        ps = form.applyTransform(m_pstPoint->p1);
        strEleStr = QString("H %1").arg(ps.x());
        break;
    case EN_SVG_PATH_ELE_V:
        ps = form.applyTransform(m_pstPoint->p1);
        strEleStr = QString("V %1").arg(ps.y());
        break;
    case EN_SVG_PATH_ELE_C:
        ps2 = form.applyTransform(m_pstBezier->p2);
        ps3 = form.applyTransform(m_pstBezier->p3);
        ps4 = form.applyTransform(m_pstBezier->p4);
        strEleStr = QString("C %1,%2 %3,%4 %5,%6").arg(ps2.x()).arg(ps2.y()).arg(ps3.x()).arg(ps3.y()).arg(ps4.x()).arg(ps4.y());
        break;
    case EN_SVG_PATH_ELE_S:
        ps3 = form.applyTransform(m_pstBezier->p3);
        ps4 = form.applyTransform(m_pstBezier->p4);
        strEleStr = QString("S %1,%2 %3,%4").arg(ps3.x()).arg(ps3.y()).arg(ps4.x()).arg(ps4.y());
        break;
    case EN_SVG_PATH_ELE_Q:
        ps2 = form.applyTransform(m_pstQuadratic->p2);
        ps3 = form.applyTransform(m_pstQuadratic->p3);
        strEleStr = QString("Q %1,%2 %3,%4").arg(ps2.x()).arg(ps2.y()).arg(ps3.x()).arg(ps3.y());
        break;
    case EN_SVG_PATH_ELE_T:
        ps3 = form.applyTransform(m_pstQuadratic->p3);
        strEleStr = QString("T %1,%2").arg(ps3.x()).arg(ps3.y());
        break;
    case EN_SVG_PATH_ELE_A:
        strEleStr = "";
        for (int i = 0; i < m_pstArc->listBezire.length(); ++i) {
            ps2 = form.applyTransform(m_pstArc->listBezire[i].p2);
            ps3 = form.applyTransform(m_pstArc->listBezire[i].p3);
            ps4 = form.applyTransform(m_pstArc->listBezire[i].p4);
            strEleStr += QString("C %1,%2 %3,%4 %5,%6").arg(ps2.x()).arg(ps2.y()).arg(ps3.x()).arg(ps3.y()).arg(ps4.x()).arg(ps4.y());
            if (i != m_pstArc->listBezire.length() - 1) {
                strEleStr +=  " ";
            }
        }
        break;
    default:
        break;
    }

    return strEleStr;
}
//---------------------------------------------------------------------------
bool SvgPathEle::getBBox(ST_SVG_BBOX* pstBBox)
{
    switch (m_enPathEle) {
    case EN_SVG_PATH_ELE_M:
    case EN_SVG_PATH_ELE_L:
    case EN_SVG_PATH_ELE_H:
    case EN_SVG_PATH_ELE_V:
    case EN_SVG_PATH_ELE_Z:
        pstBBox->p1.setX(m_pstPoint->p1.x());
        pstBBox->p1.setY(m_pstPoint->p1.y());
        pstBBox->p2.setX(m_pstPoint->p1.x());
        pstBBox->p2.setY(m_pstPoint->p1.y());
        break;
    case EN_SVG_PATH_ELE_C:
    case EN_SVG_PATH_ELE_S:
        getBBoxBezierCurve(pstBBox, m_pstBezier->p1, m_pstBezier->p2, m_pstBezier->p3, m_pstBezier->p4);
        break;
    case EN_SVG_PATH_ELE_Q:
    case EN_SVG_PATH_ELE_T:
        getBBoxQuadraticCurve(pstBBox, m_pstQuadratic->p1, m_pstQuadratic->p2, m_pstQuadratic->p3);
        break;
    case EN_SVG_PATH_ELE_A:
        getBBoxBezierCurve(pstBBox, m_pstArc->listBezire[0].p1, m_pstArc->listBezire[0].p2, m_pstArc->listBezire[0].p3, m_pstArc->listBezire[0].p4);
        for (int i = 1; i < m_pstArc->listBezire.length(); ++i) {
            ST_SVG_BBOX stBBox;
            getBBoxBezierCurve(&stBBox, m_pstArc->listBezire[i].p1, m_pstArc->listBezire[i].p2, m_pstArc->listBezire[i].p3, m_pstArc->listBezire[i].p4);
            if (pstBBox->p1.x() > stBBox.p1.x()) {
                pstBBox->p1.setX(stBBox.p1.x());
            }
            if (pstBBox->p1.y() > stBBox.p1.y()) {
                pstBBox->p1.setY(stBBox.p1.y());
            }
            if (pstBBox->p2.x() < stBBox.p2.x()) {
                pstBBox->p2.setX(stBBox.p2.x());
            }
            if (pstBBox->p2.y() < stBBox.p2.y()) {
                pstBBox->p2.setY(stBBox.p2.y());
            }
        }
        break;
    default:
        return false;;
    }

    return true;
}
//---------------------------------------------------------------------------
void SvgPathEle::getBBoxQuadraticCurve(ST_SVG_BBOX* pstBBox, const QPointF &p1, const QPointF &p2, const QPointF &p3)
{
    QPointF cp1;
    QPointF cp2;

    cp1.setX(p1.x() + 2.0 / 3.0 * (p2.x() - p1.x()));
    cp1.setY(p1.y() + 2.0 / 3.0 * (p2.y() - p1.y()));
    cp2.setX(cp1.x() + 1.0 / 3.0 * (p3.x() - p1.x()));
    cp2.setY(cp1.y() + 1.0 / 3.0 * (p3.y() - p1.y()));

    getBBoxBezierCurve(pstBBox, p1, cp1, cp2, p2);
}
//---------------------------------------------------------------------------
void SvgPathEle::getBBoxBezierCurve(ST_SVG_BBOX* pstBBox, const QPointF &p1, const QPointF &p2, const QPointF &p3, const QPointF &p4)
{
    pstBBox->p1.setX(p1.x());
    pstBBox->p1.setY(p1.y());
    pstBBox->p2.setX(p1.x());
    pstBBox->p2.setY(p1.y());

    setBBoxPointX(pstBBox, p4.x());
    setBBoxPointY(pstBBox, p4.y());

    qreal   dP0[] = {p1.x(), p1.y()};
    qreal   dP1[] = {p2.x(), p2.y()};
    qreal   dP2[] = {p3.x(), p3.y()};
    qreal   dP3[] = {p4.x(), p4.y()};
    for (int i = 0; i <= 1; ++i) {
        qreal b = 6.0 * dP0[i] - 12.0 * dP1[i] + 6.0 * dP2[i];
        qreal a = -3.0 * dP0[i] + 9.0 * dP1[i] - 9.0 * dP2[i] + 3.0 * dP3[i];
        qreal c = 3.0 * dP1[i] - 3.0 * dP0[i];
        if (a == 0.0) {
            if (b == 0.0) {
                continue;
            }
            qreal t = -c / b;
            if (0.0 < t && t < 1.0) {
                if (i == 0) {
                    setBBoxPointX(pstBBox, calcF(t, dP0[i], dP1[i], dP2[i], dP3[i]));
                }
                if (i == 1) {
                    setBBoxPointY(pstBBox, calcF(t, dP0[i], dP1[i], dP2[i], dP3[i]));
                }
            }
            continue;
        }

        qreal b2ac = qPow(b, 2.0) - 4.0 * c * a;
        if (b2ac < 0.0) {
            continue;
        }
        qreal t1 = (-b + qSqrt(b2ac)) / (2.0 * a);
        if (0.0 < t1 && t1 < 1.0) {
            if (i == 0) {
                setBBoxPointX(pstBBox, calcF(t1, dP0[i], dP1[i], dP2[i], dP3[i]));
            }
            if (i == 1) {
                setBBoxPointY(pstBBox, calcF(t1, dP0[i], dP1[i], dP2[i], dP3[i]));
            }
        }
        qreal t2 = (-b - qSqrt(b2ac)) / (2.0 * a);
        if (0.0 < t2 && t2 < 1.0) {
            if (i == 0) {
                setBBoxPointX(pstBBox, calcF(t2, dP0[i], dP1[i], dP2[i], dP3[i]));
            }
            if (i == 1) {
                setBBoxPointY(pstBBox, calcF(t2, dP0[i], dP1[i], dP2[i], dP3[i]));
            }
        }
    }
}
//---------------------------------------------------------------------------
void SvgPathEle::setBBoxPointX(ST_SVG_BBOX* pstBBox, const qreal &x)
{
    if (pstBBox->p1.x() > x) {
        pstBBox->p1.setX(x);
    }
    if (pstBBox->p2.x() < x) {
        pstBBox->p2.setX(x);
    }
}
//---------------------------------------------------------------------------
void SvgPathEle::setBBoxPointY(ST_SVG_BBOX* pstBBox, const qreal &y)
{
    if (pstBBox->p1.y() > y) {
        pstBBox->p1.setY(y);
    }
    if (pstBBox->p2.y() < y) {
        pstBBox->p2.setY(y);
    }
}
//---------------------------------------------------------------------------
qreal SvgPathEle::calcF(const qreal &t, const qreal &p0, const qreal &p1, const qreal &p2, const qreal &p3)
{
    return qPow(1.0 - t, 3.0) * p0 + 3.0 * qPow(1.0 - t, 2.0) * t * p1 + 3.0 * (1.0 - t) * qPow(t, 2.0) * p2 + qPow(t, 3.0) * p3;
}
//---------------------------------------------------------------------------
void SvgPathEle::getQuadraticBezier(QPointList* pListPoint, SvgTransform* pTransform, qreal dDpi, qreal tolerance2)
{
    if (m_enPathEle != EN_SVG_PATH_ELE_Q && m_enPathEle != EN_SVG_PATH_ELE_T) {
        return;
    }
    QPointF tp;
    QPointF p = UnitTransfer::SvgPixelPointto(m_pstQuadratic->p1, EN_SVG_UNIT_MM, dDpi);
    tp = pTransform->applyTransform(p);
    pListPoint->append(tp);

    addQuadraticBezier(pListPoint, pTransform, m_pstQuadratic->p1, m_pstQuadratic->p2, m_pstQuadratic->p3, 0, dDpi, tolerance2);

    p = UnitTransfer::SvgPixelPointto(m_pstQuadratic->p3, EN_SVG_UNIT_MM, dDpi);
    tp = pTransform->applyTransform(p);
    pListPoint->append(tp);

}
//---------------------------------------------------------------------------
void SvgPathEle::getQuadraticBezier(QPointList* pListPoint, SvgTransform* innerform, SvgTransform* outerform, qreal dDpi, qreal tolerance2, QPointF offset)
{
    if (m_enPathEle != EN_SVG_PATH_ELE_Q && m_enPathEle != EN_SVG_PATH_ELE_T) {
        return;
    }
    QPointF tp;
    QPointF p = innerform->applyTransform(m_pstQuadratic->p1) - offset;
    p = UnitTransfer::SvgPixelPointto(p, EN_SVG_UNIT_MM, dDpi);
    tp = outerform->applyTransform(p);
    pListPoint->append(tp);

    addQuadraticBezier(pListPoint, innerform, outerform, m_pstQuadratic->p1, m_pstQuadratic->p2, m_pstQuadratic->p3, 0, dDpi, tolerance2, offset);

    p = innerform->applyTransform(m_pstQuadratic->p3) - offset;
    p = UnitTransfer::SvgPixelPointto(p, EN_SVG_UNIT_MM, dDpi);
    tp = outerform->applyTransform(p);
    pListPoint->append(tp);
}
//---------------------------------------------------------------------------
void SvgPathEle::addQuadraticBezier(QPointList* pListPoint, SvgTransform* pTransform, QPointF p1, QPointF p2, QPointF p3, int iLevel, qreal dDpi, qreal tolerance2)
{
    if (iLevel > 18) {
        return;
    }
    QPointF tp;
    QPointF p;

    qreal x12   = (p1.x() + p2.x()) / 2.0;
    qreal y12   = (p1.y() + p2.y()) / 2.0;
    qreal x23   = (p2.x() + p3.x()) / 2.0;
    qreal y23   = (p2.y() + p3.y()) / 2.0;
    qreal x123  = (x12 + x23) / 2.0;
    qreal y123  = (y12 + y23) / 2.0;

    qreal dx = p3.x() - p1.x();
    qreal dy = p3.y() - p1.y();
    qreal d = qAbs(((p2.x() - p3.x()) * dy - (p2.y() - p3.y()) * dx));

    if (d * d <= 5.0 * tolerance2 * (dx * dx + dy * dy)) {
        p = UnitTransfer::SvgPixelPointto(QPointF(x123, y123), EN_SVG_UNIT_MM, dDpi);
        tp = pTransform->applyTransform(p);
        pListPoint->append(tp);
        return;
    }

    addQuadraticBezier(pListPoint, pTransform, p1, QPointF(x12, y12), QPointF(x123, y123), iLevel + 1, dDpi, tolerance2);
    addQuadraticBezier(pListPoint, pTransform, QPointF(x123, y123), QPointF(x23, y23), p3, iLevel + 1, dDpi, tolerance2);
}
//---------------------------------------------------------------------------
void SvgPathEle::addQuadraticBezier(QPointList* pListPoint, SvgTransform* innerForm, SvgTransform* outerForm, QPointF p1, QPointF p2, QPointF p3, int iLevel, qreal dDpi, qreal tolerance2, QPointF offset)
{
    if (iLevel > 18) {
        return;
    }
    QPointF tp;
    QPointF p;

    qreal x12   = (p1.x() + p2.x()) / 2.0;
    qreal y12   = (p1.y() + p2.y()) / 2.0;
    qreal x23   = (p2.x() + p3.x()) / 2.0;
    qreal y23   = (p2.y() + p3.y()) / 2.0;
    qreal x123  = (x12 + x23) / 2.0;
    qreal y123  = (y12 + y23) / 2.0;

    qreal dx = p3.x() - p1.x();
    qreal dy = p3.y() - p1.y();
    qreal d = qAbs(((p2.x() - p3.x()) * dy - (p2.y() - p3.y()) * dx));

    if (d * d <= 5.0 * tolerance2 * (dx * dx + dy * dy)) {
        p = innerForm->applyTransform(QPointF(x123, y123)) - offset;
        p = UnitTransfer::SvgPixelPointto(p, EN_SVG_UNIT_MM, dDpi);
        tp = outerForm->applyTransform(p);
        pListPoint->append(tp);
        return;
    }

    addQuadraticBezier(pListPoint, innerForm, outerForm, p1, QPointF(x12, y12), QPointF(x123, y123), iLevel + 1, dDpi, tolerance2, offset);
    addQuadraticBezier(pListPoint, innerForm, outerForm, QPointF(x123, y123), QPointF(x23, y23), p3, iLevel + 1, dDpi, tolerance2, offset);
}
//---------------------------------------------------------------------------
void SvgPathEle::getCubicBezier(QPointList* pListPoint, SvgTransform* pTransform, qreal dDpi, qreal tolerance2)
{
    if (m_enPathEle != EN_SVG_PATH_ELE_C && m_enPathEle != EN_SVG_PATH_ELE_S) {
        return;
    }
    QPointF tp;
    QPointF p;
    p = UnitTransfer::SvgPixelPointto(m_pstBezier->p1, EN_SVG_UNIT_MM, dDpi);
    tp = pTransform->applyTransform(p);
    pListPoint->append(tp);

    addCubicBezier(pListPoint, pTransform, m_pstBezier->p1, m_pstBezier->p2, m_pstBezier->p3, m_pstBezier->p4, 0, dDpi, tolerance2);

    p = UnitTransfer::SvgPixelPointto(m_pstBezier->p4, EN_SVG_UNIT_MM, dDpi);
    tp = pTransform->applyTransform(p);
    pListPoint->append(tp);
}
//---------------------------------------------------------------------------
void SvgPathEle::getCubicBezier(QPointList* pListPoint, SvgTransform* innerForm, SvgTransform* outerForm, qreal dDpi, qreal tolerance2, QPointF offset)
{
    if (m_enPathEle != EN_SVG_PATH_ELE_C && m_enPathEle != EN_SVG_PATH_ELE_S) {
        return;
    }
    QPointF tp;
    QPointF p;
    p = innerForm->applyTransform(m_pstBezier->p1) - offset;
    p = UnitTransfer::SvgPixelPointto(p, EN_SVG_UNIT_MM, dDpi);
    tp = outerForm->applyTransform(p);
    pListPoint->append(tp);

    addCubicBezier(pListPoint, innerForm, outerForm, m_pstBezier->p1, m_pstBezier->p2, m_pstBezier->p3, m_pstBezier->p4, 0, dDpi, tolerance2, offset);

    p = innerForm->applyTransform(m_pstBezier->p4) - offset;
    p = UnitTransfer::SvgPixelPointto(p, EN_SVG_UNIT_MM, dDpi);
    tp = outerForm->applyTransform(p);
    pListPoint->append(tp);
}
//---------------------------------------------------------------------------
void SvgPathEle::addCubicBezier(QPointList* pListPoint, SvgTransform* pTransform, QPointF p1, QPointF p2, QPointF p3, QPointF p4, int iLevel, qreal dDpi, qreal tolerance2)
{
    if (iLevel > 18) {
        return;
    }
    QPointF tp;
    QPointF p;

    qreal x12   = (p1.x() + p2.x()) / 2.0;
    qreal y12   = (p1.y() + p2.y()) / 2.0;
    qreal x23   = (p2.x() + p3.x()) / 2.0;
    qreal y23   = (p2.y() + p3.y()) / 2.0;
    qreal x34   = (p3.x() + p4.x()) / 2.0;
    qreal y34   = (p3.y() + p4.y()) / 2.0;
    qreal x123  = (x12 + x23) / 2.0;
    qreal y123  = (y12 + y23) / 2.0;
    qreal x234  = (x23 + x34) / 2.0;
    qreal y234  = (y23 + y34) / 2.0;
    qreal x1234 = (x123 + x234) / 2.0;
    qreal y1234 = (y123 + y234) / 2.0;

    qreal dx = p4.x() - p1.x();
    qreal dy = p4.y() - p1.y();

    qreal d2 = qAbs(((p2.x() - p4.x()) * dy - (p2.y() - p4.y()) * dx));
    qreal d3 = qAbs(((p3.x() - p4.x()) * dy - (p3.y() - p4.y()) * dx));

    if (qPow(d2 + d3, 2) < 5.0 * tolerance2 * (dx * dx + dy * dy)) {
        p = UnitTransfer::SvgPixelPointto(QPointF(x1234, y1234), EN_SVG_UNIT_MM, dDpi);
        tp = pTransform->applyTransform(p);
        pListPoint->append(tp);
        return;
    }

    addCubicBezier(pListPoint, pTransform, p1, QPointF(x12, y12), QPointF(x123, y123), QPointF(x1234, y1234), iLevel + 1, dDpi, tolerance2);
    addCubicBezier(pListPoint, pTransform, QPointF(x1234, y1234), QPointF(x234, y234), QPointF(x34, y34), p4, iLevel + 1, dDpi, tolerance2);
}
//---------------------------------------------------------------------------
void SvgPathEle::addCubicBezier(QPointList* pListPoint, SvgTransform* innerForm, SvgTransform* outerForm, QPointF p1, QPointF p2, QPointF p3, QPointF p4, int iLevel, qreal dDpi, qreal tolerance2, QPointF offset)
{
    if (iLevel > 18) {
        return;
    }
    QPointF tp;
    QPointF p;

    qreal x12   = (p1.x() + p2.x()) / 2.0;
    qreal y12   = (p1.y() + p2.y()) / 2.0;
    qreal x23   = (p2.x() + p3.x()) / 2.0;
    qreal y23   = (p2.y() + p3.y()) / 2.0;
    qreal x34   = (p3.x() + p4.x()) / 2.0;
    qreal y34   = (p3.y() + p4.y()) / 2.0;
    qreal x123  = (x12 + x23) / 2.0;
    qreal y123  = (y12 + y23) / 2.0;
    qreal x234  = (x23 + x34) / 2.0;
    qreal y234  = (y23 + y34) / 2.0;
    qreal x1234 = (x123 + x234) / 2.0;
    qreal y1234 = (y123 + y234) / 2.0;

    qreal dx = p4.x() - p1.x();
    qreal dy = p4.y() - p1.y();

    qreal d2 = qAbs(((p2.x() - p4.x()) * dy - (p2.y() - p4.y()) * dx));
    qreal d3 = qAbs(((p3.x() - p4.x()) * dy - (p3.y() - p4.y()) * dx));

    if (qPow(d2 + d3, 2) < 5.0 * tolerance2 * (dx * dx + dy * dy)) {
        p = innerForm->applyTransform(QPointF(x1234, y1234)) - offset;
        p = UnitTransfer::SvgPixelPointto(p, EN_SVG_UNIT_MM, dDpi);
        tp = outerForm->applyTransform(p);
        pListPoint->append(tp);
        return;
    }

    addCubicBezier(pListPoint, innerForm, outerForm, p1, QPointF(x12, y12), QPointF(x123, y123), QPointF(x1234, y1234), iLevel + 1, dDpi, tolerance2, offset);
    addCubicBezier(pListPoint, innerForm, outerForm, QPointF(x1234, y1234), QPointF(x234, y234), QPointF(x34, y34), p4, iLevel + 1, dDpi, tolerance2, offset);
}
//---------------------------------------------------------------------------
void SvgPathEle::getArc(QPointList* pListPoint, SvgTransform* pTransform, qreal dDpi, qreal tolerance2)
{
    if (m_enPathEle != EN_SVG_PATH_ELE_A) {
        return;
    }
    QPointF tp;
    QPointF p;

    for (int i = 0; i < m_pstArc->listBezire.length(); ++i) {
        p = UnitTransfer::SvgPixelPointto(m_pstArc->listBezire[i].p1, EN_SVG_UNIT_MM, dDpi);
        tp = pTransform->applyTransform(p);
        pListPoint->append(tp);

        addCubicBezier(pListPoint, pTransform, m_pstArc->listBezire[i].p1, m_pstArc->listBezire[i].p2, m_pstArc->listBezire[i].p3, m_pstArc->listBezire[i].p4, 0, dDpi, tolerance2);

        p = UnitTransfer::SvgPixelPointto(m_pstArc->listBezire[i].p4, EN_SVG_UNIT_MM, dDpi);
        tp = pTransform->applyTransform(p);
        pListPoint->append(tp);
    }
}
//---------------------------------------------------------------------------
void SvgPathEle::getArc(QPointList* pListPoint, SvgTransform* innerForm, SvgTransform* outerForm, qreal dDpi, qreal tolerance2, QPointF offset)
{
    if (m_enPathEle != EN_SVG_PATH_ELE_A) {
        return;
    }
    QPointF tp;
    QPointF p;

    for (int i = 0; i < m_pstArc->listBezire.length(); ++i) {
        p = innerForm->applyTransform(m_pstArc->listBezire[i].p1) - offset;
        p = UnitTransfer::SvgPixelPointto(p, EN_SVG_UNIT_MM, dDpi);
        tp = outerForm->applyTransform(p);
        pListPoint->append(tp);

        addCubicBezier(pListPoint, innerForm, outerForm, m_pstArc->listBezire[i].p1, m_pstArc->listBezire[i].p2, m_pstArc->listBezire[i].p3, m_pstArc->listBezire[i].p4, 0, dDpi, tolerance2, offset);

        p = innerForm->applyTransform(m_pstArc->listBezire[i].p4) - offset;
        p = UnitTransfer::SvgPixelPointto(p, EN_SVG_UNIT_MM, dDpi);
        tp = outerForm->applyTransform(p);
        pListPoint->append(tp);
    }
}
//---------------------------------------------------------------------------
void  SvgPathEle::readData(const QJsonObject &json)
{
    m_enPathEle = (EN_SVG_PATH_ELE)(json[JSON_TAG_SVG_PATH_ELEM_TYPE].toInt());

    switch (m_enPathEle) {
    case EN_SVG_PATH_ELE_NONE:
    case EN_SVG_PATH_ELE_M:
    case EN_SVG_PATH_ELE_L:
    case EN_SVG_PATH_ELE_H:
    case EN_SVG_PATH_ELE_V:
    case EN_SVG_PATH_ELE_Z: {
            m_pstPoint = new ST_SVG_PATH_POINT;
            QJsonObject  point = json[JSON_TAG_SVG_PATH_ELEM_POINT].toObject();
            readDataPoint(point);
        }
        break;
    case EN_SVG_PATH_ELE_Q:
    case EN_SVG_PATH_ELE_T: {
            m_pstQuadratic = new ST_SVG_PATH_QUADRATIC;
            QJsonArray  quadArray =  json[JSON_TAG_SVG_PATH_ELEM_QUAD].toArray();
            readDataQuad(quadArray);
        }
        break;
    case EN_SVG_PATH_ELE_C:
    case EN_SVG_PATH_ELE_S: {
            m_pstBezier = new ST_SVG_PATH_BEZIER;
            QJsonArray  beziArray = json[JSON_TAG_SVG_PATH_ELEM_QUAD].toArray();
            readDataBezier(beziArray, m_pstBezier);
        }
        break;
    case EN_SVG_PATH_ELE_A: {
            m_pstArc = new ST_SVG_PATH_ARC;
            QJsonArray arcArray = json[JSON_TAG_SVG_PATH_ELEM_ARC].toArray();
            readDataArc(arcArray);
        }
        break;
    default:
        break;
    }
}
//---------------------------------------------------------------------------
void  SvgPathEle::writeDown(QJsonObject &json) const
{
    json[JSON_TAG_SVG_PATH_ELEM_TYPE] = m_enPathEle;

    switch (m_enPathEle) {
    case EN_SVG_PATH_ELE_NONE:
    case EN_SVG_PATH_ELE_M:
    case EN_SVG_PATH_ELE_L:
    case EN_SVG_PATH_ELE_H:
    case EN_SVG_PATH_ELE_V:
    case EN_SVG_PATH_ELE_Z: {
            QJsonObject  point;
            writeDownPoint(point);
            json[JSON_TAG_SVG_PATH_ELEM_POINT] = point;
        }
        break;
    case EN_SVG_PATH_ELE_Q:
    case EN_SVG_PATH_ELE_T: {
            QJsonArray  quadArray;
            writeDownQuad(quadArray);
            json[JSON_TAG_SVG_PATH_ELEM_QUAD] = quadArray;
        }
        break;
    case EN_SVG_PATH_ELE_C:
    case EN_SVG_PATH_ELE_S: {
            QJsonArray  beziArray;
            writeDownBezier(beziArray, *m_pstBezier);
            json[JSON_TAG_SVG_PATH_ELEM_QUAD] = beziArray;
        }
        break;
    case EN_SVG_PATH_ELE_A: {
            QJsonArray arcArray;
            writeDownArc(arcArray);
            json[JSON_TAG_SVG_PATH_ELEM_ARC] = arcArray;
        }
        break;
    default:
        break;
    }
}
//---------------------------------------------------------------------------
void  SvgPathEle::readDataPoint(const QJsonObject &json)
{
    m_pstPoint->p1.setX(json[JSON_TAG_SVG_POINT_X].toDouble());
    m_pstPoint->p1.setY(json[JSON_TAG_SVG_POINT_Y].toDouble());
}
//---------------------------------------------------------------------------
void  SvgPathEle::writeDownPoint(QJsonObject &json) const
{
    json[JSON_TAG_SVG_POINT_X] = m_pstPoint->p1.x();
    json[JSON_TAG_SVG_POINT_Y] = m_pstPoint->p1.y();
}
//---------------------------------------------------------------------------
void  SvgPathEle::readDataQuad(const QJsonArray &json)
{
    foreach (QJsonValue quadVal, json) {
        QJsonObject quad = quadVal.toObject();
        switch(quad[JSON_TAG_SVG_POINT_NUM].toInt()) {
        case 1:
            m_pstQuadratic->p1.setX(quad[JSON_TAG_SVG_POINT_X].toDouble());
            m_pstQuadratic->p1.setY(quad[JSON_TAG_SVG_POINT_Y].toDouble());
            break;
        case 2:
            m_pstQuadratic->p2.setX(quad[JSON_TAG_SVG_POINT_X].toDouble());
            m_pstQuadratic->p2.setY(quad[JSON_TAG_SVG_POINT_Y].toDouble());
            break;
        case 3:
            m_pstQuadratic->p3.setX(quad[JSON_TAG_SVG_POINT_X].toDouble());
            m_pstQuadratic->p3.setY(quad[JSON_TAG_SVG_POINT_Y].toDouble());
            break;
        }
    }
}
//---------------------------------------------------------------------------
void  SvgPathEle::writeDownQuad(QJsonArray &json) const
{
    QJsonObject quad1;
    QJsonObject quad2;
    QJsonObject quad3;
    quad1[JSON_TAG_SVG_POINT_NUM] = 1;
    quad1[JSON_TAG_SVG_POINT_X] = m_pstQuadratic->p1.x();
    quad1[JSON_TAG_SVG_POINT_Y] = m_pstQuadratic->p1.y();
    quad2[JSON_TAG_SVG_POINT_NUM] = 2;
    quad2[JSON_TAG_SVG_POINT_X] = m_pstQuadratic->p2.x();
    quad2[JSON_TAG_SVG_POINT_Y] = m_pstQuadratic->p2.y();
    quad3[JSON_TAG_SVG_POINT_NUM] = 3;
    quad3[JSON_TAG_SVG_POINT_X] = m_pstQuadratic->p3.x();
    quad3[JSON_TAG_SVG_POINT_Y] = m_pstQuadratic->p3.y();
    json.append(quad1);
    json.append(quad2);
    json.append(quad3);
}
//---------------------------------------------------------------------------
void  SvgPathEle::readDataBezier(const QJsonArray &json, ST_SVG_PATH_BEZIER* bezier)
{
    foreach (QJsonValue bezieVal, json) {
        QJsonObject bezieObj = bezieVal.toObject();
        switch(bezieObj[JSON_TAG_SVG_POINT_NUM].toInt()) {
        case 1:
            bezier->p1.setX(bezieObj[JSON_TAG_SVG_POINT_X].toDouble());
            bezier->p1.setY(bezieObj[JSON_TAG_SVG_POINT_Y].toDouble());
            break;
        case 2:
            bezier->p2.setX(bezieObj[JSON_TAG_SVG_POINT_X].toDouble());
            bezier->p2.setY(bezieObj[JSON_TAG_SVG_POINT_Y].toDouble());
            break;
        case 3:
            bezier->p3.setX(bezieObj[JSON_TAG_SVG_POINT_X].toDouble());
            bezier->p3.setY(bezieObj[JSON_TAG_SVG_POINT_Y].toDouble());
            break;
        case 4:
            bezier->p4.setX(bezieObj[JSON_TAG_SVG_POINT_X].toDouble());
            bezier->p4.setY(bezieObj[JSON_TAG_SVG_POINT_Y].toDouble());
            break;
        }
    }
}
//---------------------------------------------------------------------------
void  SvgPathEle::writeDownBezier(QJsonArray &json, const ST_SVG_PATH_BEZIER& bezier) const
{
    QJsonObject bezi1;
    QJsonObject bezi2;
    QJsonObject bezi3;
    QJsonObject bezi4;
    bezi1[JSON_TAG_SVG_POINT_NUM] = 1;
    bezi1[JSON_TAG_SVG_POINT_X] = bezier.p1.x();
    bezi1[JSON_TAG_SVG_POINT_Y] = bezier.p1.y();
    bezi2[JSON_TAG_SVG_POINT_NUM] = 2;
    bezi2[JSON_TAG_SVG_POINT_X] = bezier.p2.x();
    bezi2[JSON_TAG_SVG_POINT_Y] = bezier.p2.y();
    bezi3[JSON_TAG_SVG_POINT_NUM] = 3;
    bezi3[JSON_TAG_SVG_POINT_X] = bezier.p3.x();
    bezi3[JSON_TAG_SVG_POINT_Y] = bezier.p3.y();
    bezi4[JSON_TAG_SVG_POINT_NUM] = 4;
    bezi4[JSON_TAG_SVG_POINT_X] = bezier.p4.x();
    bezi4[JSON_TAG_SVG_POINT_Y] = bezier.p4.y();
    json.append(bezi1);
    json.append(bezi2);
    json.append(bezi3);
    json.append(bezi4);
}
//---------------------------------------------------------------------------
void  SvgPathEle::readDataArc(const QJsonArray &json)
{
    foreach (QJsonValue bezieArray, json) {
        ST_SVG_PATH_BEZIER bezie;
        readDataBezier(bezieArray.toArray(), &bezie);
        m_pstArc->listBezire.append(bezie);
    }
}
//---------------------------------------------------------------------------
void  SvgPathEle::writeDownArc(QJsonArray &json) const
{
    foreach (ST_SVG_PATH_BEZIER bezie, m_pstArc->listBezire) {
        QJsonArray  beziArray;
        writeDownBezier(beziArray, bezie);
        json.append(beziArray);
    }
}
//---------------------------------------------------------------------------
//Useはサイズを指定可能で、サイズに合わせてID指定先を縮小拡大する
void SvgPathEle::applyViewBox(QPointF initialPos, QPointF addDiff, qreal scaleW, qreal scaleH)
{
    switch (m_enPathEle) {
    case EN_SVG_PATH_ELE_NONE:
    case EN_SVG_PATH_ELE_M:
    case EN_SVG_PATH_ELE_L:
    case EN_SVG_PATH_ELE_H:
    case EN_SVG_PATH_ELE_V:
    case EN_SVG_PATH_ELE_Z: {
            m_pstPoint->p1 = m_pstPoint->p1 - initialPos;
            m_pstPoint->p1 = QPointF(m_pstPoint->p1.x() * scaleW, m_pstPoint->p1.y() * scaleH);
            m_pstPoint->p1 = m_pstPoint->p1 + initialPos + addDiff;
        }
        break;
    case EN_SVG_PATH_ELE_Q:
    case EN_SVG_PATH_ELE_T: {
            applyViewBoxQuad(m_pstQuadratic, initialPos, addDiff, scaleW, scaleH);
        }
        break;
    case EN_SVG_PATH_ELE_C:
    case EN_SVG_PATH_ELE_S: {
            applyViewBoxBezier(m_pstBezier, initialPos, addDiff, scaleW, scaleH);
        }
        break;
    case EN_SVG_PATH_ELE_A: {
            applyViewBoxArc(m_pstArc, initialPos, addDiff, scaleW, scaleH);
        }
        break;
    default:
        break;
    }
}
//---------------------------------------------------------------------------
void SvgPathEle::applyViewBoxQuad(ST_SVG_PATH_QUADRATIC* quad, QPointF initialPos, QPointF addDiff, qreal scaleW, qreal scaleH)
{
    quad->p1 = quad->p1 - initialPos;
    quad->p1 = QPointF(quad->p1.x() * scaleW, quad->p1.y() * scaleH);
    quad->p1 = quad->p1 + initialPos + addDiff;

    quad->p2 = quad->p2 - initialPos;
    quad->p2 = QPointF(quad->p2.x() * scaleW, quad->p2.y() * scaleH);
    quad->p2 = quad->p2 + initialPos + addDiff;

    quad->p3 = quad->p3 - initialPos;
    quad->p3 = QPointF(quad->p3.x() * scaleW, quad->p3.y() * scaleH);
    quad->p3 = quad->p3 + initialPos + addDiff;
}
//---------------------------------------------------------------------------
void SvgPathEle::applyViewBoxBezier(ST_SVG_PATH_BEZIER* bezier, QPointF initialPos, QPointF addDiff, qreal scaleW, qreal scaleH)
{
    bezier->p1 = bezier->p1 - initialPos;
    bezier->p1 = QPointF(bezier->p1.x() * scaleW, bezier->p1.y() * scaleH);
    bezier->p1 = bezier->p1 + initialPos + addDiff;

    bezier->p2 = bezier->p2 - initialPos;
    bezier->p2 = QPointF(bezier->p2.x() * scaleW, bezier->p2.y() * scaleH);
    bezier->p2 = bezier->p2 + initialPos + addDiff;

    bezier->p3 = bezier->p3 - initialPos;
    bezier->p3 = QPointF(bezier->p3.x() * scaleW, bezier->p3.y() * scaleH);
    bezier->p3 = bezier->p3 + initialPos + addDiff;

    bezier->p4 = bezier->p4 - initialPos;
    bezier->p4 = QPointF(bezier->p4.x() * scaleW, bezier->p4.y() * scaleH);
    bezier->p4 = bezier->p4 + initialPos + addDiff;
}
//---------------------------------------------------------------------------
void SvgPathEle::applyViewBoxArc(ST_SVG_PATH_ARC* arc, QPointF initialPos, QPointF addDiff, qreal scaleW, qreal scaleH)
{
    QList<ST_SVG_PATH_BEZIER> retList;
    foreach (ST_SVG_PATH_BEZIER bezier, arc->listBezire) {
        applyViewBoxBezier(&bezier, initialPos, addDiff, scaleW, scaleH);
        retList.append(bezier);
    }
    arc->listBezire.clear();
    arc->listBezire << retList;
}
//---------------------------------------------------------------------------
