//---------------------------------------------------------------------------
#include "svgPath.h"
#include "svgParser.h"
#include "common/sortablePointF.h"
#include "common/sysParam.h"

#include <float.h>

#include <QPainterPath>
#include <QTransform>
//---------------------------------------------------------------------------
SvgPath::SvgPath()
{
    initMenber();
}
//---------------------------------------------------------------------------
SvgPath::SvgPath(const SvgPath &src)
{
    initMenber();
    copyMenber(src);
}
//---------------------------------------------------------------------------
SvgPath::~SvgPath()
{
    deleteMenber();
}
//---------------------------------------------------------------------------
SvgPath &SvgPath::operator=(const SvgPath &src)
{
    deleteMenber();

    return copyMenber(src);
}
//---------------------------------------------------------------------------
void SvgPath::initMenber()
{
    m_stCom.fill.setColor("none");
    m_stCom.stroke.setColor("none");
    m_stCom.transform.initMenber();
    m_stCom.display = true;
    m_stCom.strokeWidth = 1;
    m_listPathEle.clear();
}
//---------------------------------------------------------------------------
void SvgPath::deleteMenber()
{
}
//---------------------------------------------------------------------------
SvgPath &SvgPath::copyMenber(const SvgPath &src)
{
    m_stCom = src.m_stCom;
    m_listPathEle = src.m_listPathEle;
    m_content = src.m_content;
    m_childPath << src.m_childPath;

    return *this;
}
//---------------------------------------------------------------------------
bool SvgPath::setNode(const QDomElement &element, ST_SVG_ELE_COM stEleCom, EN_SVG_UNIT enUnit, qreal dDpi)
{
    bool ok;
    QString strBuf;
    QString strTag = element.tagName();

    // 共通属性コピー
    m_stCom = stEleCom;
    if (m_stCom.stroke.getColorObj().isValid() == false) {
        m_stCom.stroke.setColor("black");
    }

    // 要素種別設定
    if (strTag == "path") {
        if (element.hasAttribute("d") == false) {
            DEBUG_STRING("Path has NOT attribute \"d\".");
            return false;
        }
        if (loadPath(element.attribute("d").trimmed(), enUnit, dDpi) == false) {
            DEBUG_STRING("Fail to load path attribute \"d\".");
            return false;
        }
    } else if (strTag == "circle") {
        if (element.hasAttribute("r") == false) {
            DEBUG_STRING("Circle has NOT attribute \"r\".");
            return false;
        }
        // 属性取得
        qreal cx = 0.0;
        qreal cy = 0.0;
        qreal r = 0.0;
        cx = element.attribute("cx").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Circle cx is invalid."); cx = 0.0; }
        cy = element.attribute("cy").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Circle cy is invalid"); cy = 0.0; }
        r = element.attribute("r").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Circle r is invalid"); return false; }

        // path文字列作成
        strBuf = QString("M %1,%2 ").arg(cx - r).arg(cy);
        strBuf += QString("A %1,%2 0 0 0 %3,%4 ").arg(r).arg(r).arg(cx).arg(cy + r);
        strBuf += QString("A %1,%2 0 0 0 %3,%4 ").arg(r).arg(r).arg(cx + r).arg(cy);
        strBuf += QString("A %1,%2 0 0 0 %3,%4 ").arg(r).arg(r).arg(cx).arg(cy - r);
        strBuf += QString("A %1,%2 0 0 0 %3,%4 ").arg(r).arg(r).arg(cx - r).arg(cy);
        strBuf += "z";
        // path要素分解
        if (loadPath(strBuf, enUnit, dDpi) == false) {
            DEBUG_STRING("Fail to load path circle.");
            return false;
        }
    } else if (strTag == "ellipse") {
        if (element.hasAttribute("rx") == false || element.hasAttribute("ry") == false) {
            DEBUG_STRING("Ellipse has NOT attribute \"rx\" or \"ry\".");
            return false;
        }
        // 属性取得
        qreal cx = 0.0;
        qreal cy = 0.0;
        qreal rx = 0.0;
        qreal ry = 0.0;
        cx = element.attribute("cx").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Ellipse cx is invalid."); cx = 0.0; }
        cy = element.attribute("cy").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Ellipse cy is invalid."); cy = 0.0; }
        rx = element.attribute("rx").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Ellipse rx is invalid."); return false; }
        ry = element.attribute("ry").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Ellipse ry is invalid."); return false; }

        // path文字列作成
        strBuf = QString("M %1,%2 ").arg(cx - rx).arg(cy);
        strBuf += QString("A %1,%2 0 0 0 %3,%4 ").arg(rx).arg(ry).arg(cx).arg(cy + ry);
        strBuf += QString("A %1,%2 0 0 0 %3,%4 ").arg(rx).arg(ry).arg(cx + rx).arg(cy);
        strBuf += QString("A %1,%2 0 0 0 %3,%4 ").arg(rx).arg(ry).arg(cx).arg(cy - ry);
        strBuf += QString("A %1,%2 0 0 0 %3,%4 ").arg(rx).arg(ry).arg(cx - rx).arg(cy);
        strBuf += "z";
        // path要素分解
        if (loadPath(strBuf, enUnit, dDpi) == false) {
            DEBUG_STRING("Fail to load path ellipse.");
            return false;
        }
    } else if (strTag == "rect") {
        if (element.hasAttribute("width") == false || element.hasAttribute("height") == false) {
            DEBUG_STRING("Rect has NOT attribute \"width\" or \"height\".");
            return false;
        }
        // 属性取得
        qreal x = 0.0;
        qreal y = 0.0;
        qreal width = 0.0;
        qreal height = 0.0;
        qreal rx = -1.0;
        qreal ry = -1.0;
        x = element.attribute("x").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Rect x is invalid."); x = 0.0; }
        y = element.attribute("y").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Rect y is invalid."); y = 0.0; }
        width = element.attribute("width").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Rect width is invalid."); return false; }
        height = element.attribute("height").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("Rect height is invalid."); return false; }
        if (element.hasAttribute("rx") == true) {
            rx = element.attribute("rx").trimmed().toDouble();
        }
        if (element.hasAttribute("ry") == true) {
            ry = element.attribute("ry").trimmed().toDouble();
        }

        // path文字列作成
        if (rx < 0.0 || ry < 0.0) {
            strBuf = QString("M %1,%2 ").arg(x).arg(y);
            strBuf += QString("H %1 ").arg(x + width);
            strBuf += QString("V %1 ").arg(y + height);
            strBuf += QString("H %1 ").arg(x);
            strBuf += "z";
        } else {
            qreal r = x + width;
            qreal b = y + height;

            strBuf = QString("M %1,%2 ").arg(x + rx).arg(y);
            strBuf += QString("L %1,%2 ").arg(r - rx).arg(y);
            strBuf += QString("Q %1,%2 %3,%4 ").arg(r).arg(y).arg(r).arg(y + ry);
            strBuf += QString("L %1,%2 ").arg(r).arg(b - ry);
            strBuf += QString("Q %1,%2 %3,%4 ").arg(r).arg(b).arg(r - rx).arg(b);
            strBuf += QString("L %1,%2 ").arg(x + rx).arg(b);
            strBuf += QString("Q %1,%2 %3,%4 ").arg(x).arg(b).arg(x).arg(b - ry);
            strBuf += QString("L %1,%2 ").arg(x).arg(y + ry);
            strBuf += QString("Q %1,%2 %3,%4 ").arg(x).arg(y).arg(x + rx).arg(y);
            strBuf += "z";
        }
        // path要素分解
        if (loadPath(strBuf, enUnit, dDpi) == false) {
            DEBUG_STRING("Fail to load path rect.");
            return false;
        }
    } else if (strTag == "line") {
        // 属性取得
        qreal x1 = 0.0;
        qreal y1 = 0.0;
        qreal x2 = 0.0;
        qreal y2 = 0.0;
        x1 = element.attribute("x1").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("line x1 is invalid."); x1 = 0.0; }
        y1 = element.attribute("y1").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("line y1 is invalid."); y1 = 0.0; }
        x2 = element.attribute("x2").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("line x2 is invalid."); x2 = 0.0; }
        y2 = element.attribute("y2").trimmed().toDouble(&ok);
        if (ok == false) { DEBUG_STRING("line y2 is invalid."); y2 = 0.0; }

        // path文字列作成
        strBuf = QString("M %1,%2 L %3,%4").arg(x1).arg(y1).arg(x2).arg(y2);
        // path要素分解
        if (loadPath(strBuf, enUnit, dDpi) == false) {
            DEBUG_STRING("Fail to load path line.");
            return false;
        }
    } else if (strTag == "polyline") {
        if (element.hasAttribute("points") == false) {
            DEBUG_STRING("Polyline has NOT attribute \"points\".");
            return false;
        }
        // 属性取得
        QList<QPointF> points;
        strBuf = element.attribute("points").trimmed();
        if (getPoints(strBuf, &points, EN_SVG_UNIT_NON, dDpi) == false) {
            DEBUG_STRING("Fail to get point polyline.");
            return false;
        }

        // path文字列作成
        strBuf = "M ";
        for (int i = 0; i < points.length(); ++i) {
            strBuf += QString("%1,%2 ").arg(points[i].x()).arg(points[i].y());
        }
        // path要素分解
        if (loadPath(strBuf, enUnit, dDpi) == false) {
            DEBUG_STRING("Fail to load path polyline.");
            return false;
        }
    } else if (strTag == "polygon") {
        if (element.hasAttribute("points") == false) {
            DEBUG_STRING("Polygon has NOT attribute \"points\".");
            return false;
        }
        // 属性取得
        QList<QPointF> points;
        strBuf = element.attribute("points").trimmed();
        if (getPoints(strBuf, &points, EN_SVG_UNIT_NON, dDpi) == false) {
            DEBUG_STRING("Fail to get point polygon.");
            return false;
        }

        // path文字列作成
        strBuf = "M ";
        for (int i = 0; i < points.length(); ++i) {
            strBuf += QString("%1,%2 ").arg(points[i].x()).arg(points[i].y());
        }
        strBuf += "z";
        // path要素分解
        if (loadPath(strBuf, enUnit, dDpi) == false) {
            DEBUG_STRING("Fail to load path polygon.");
            return false;
        }
    } else if(strTag == "use") {
        //Useの場合、参照先が存在しない場合は無効なデータとなる
        if (element.hasAttribute("href") == false) {
            DEBUG_STRING("use has NOT attribute \"xlink:href\".");
            return false;
        }
    }

    //自身のIDを拾えるようにするため、Domを覚えておくようにする
    m_content = element;

    return true;
}
//---------------------------------------------------------------------------
bool SvgPath::getPoints(const QString &strPoints, QList<QPointF>* pPoints, EN_SVG_UNIT enUnit, qreal dDpi)
{
    QStringList listPoints;

    // ポイントクリア
    pPoints->clear();

    listPoints = strPoints.split(QRegExp("[ \,\n\t]"), QString::SkipEmptyParts);
    if((listPoints.count() % 2) != 0) {
        DEBUG_STRING("XY count is weird.");
        return false;
    }

    for (int i = 0; i < listPoints.length(); i = i + 2) {
        QPointF point;
        qreal   d;
        bool ok;
        // X
        d = UnitTransfer::toSvgPixel(listPoints.at(i).trimmed().toDouble(&ok), enUnit, dDpi);
        if (ok == false) {
            DEBUG_STRING("X value is invalid.");
            return false;
        }
        point.setX(d);
        // Y
        d = UnitTransfer::toSvgPixel(listPoints.at(i + 1).trimmed().toDouble(&ok), enUnit, dDpi);
        if (ok == false) {
            DEBUG_STRING("Y value is invalid.");
            return false;
        }
        point.setY(d);
        // リストに追加
        pPoints->append(point);
    }

    if (pPoints->length() == 0) {
        DEBUG_STRING("Point Length is 0.");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool SvgPath::loadPath(const QString  &strPath, EN_SVG_UNIT enUnit, qreal dDpi)
{
    QStringList listPathStr;

    if (strPath == "") {
        DEBUG_STRING("Path string is empty.");
        return false;
    }

    m_listPathEle.clear();

    // Pathを分解
    pathSplit(&listPathStr, strPath);

    if (listPathStr.length() <= 2) {
        DEBUG_STRING("Path string is short.");
        return false;
    }

    EN_SVG_PATH_ELE enPrevEle = EN_SVG_PATH_ELE_NONE;
    qreal   dX = 0.0;
    qreal   dY = 0.0;
    qreal   dValue = 0.0;
    qreal   xPrevCp = 0.0;
    qreal   yPrevCp = 0.0;
    int     iMPos = 0;

    // 分解したPathを要素リストに格納
    while (listPathStr.length() > 0) {
        SvgPathEle stPathEle;
        EN_SVG_PATH_ELE enEle;
        EN_SVG_PATH_VALUE enValue;

        // 要素取得
        if (pathGetNextCmd(&listPathStr, &enEle, &enValue) == false) {
            DEBUG_STRING("Cannot get next path.");
            return false;
        }
        // 要素ごとに設定
        switch (enEle) {
        case EN_SVG_PATH_ELE_M:
            if (enValue == EN_SVG_PATH_VALUE_A) {
                if (pathGetNextNum(&listPathStr, &dX, enUnit, dDpi) == false) { DEBUG_STRING("Cannot get next path element M value A."); return false; }
                if (pathGetNextNum(&listPathStr, &dY, enUnit, dDpi) == false) { DEBUG_STRING("Cannot get next path element M value A."); return false; }
                stPathEle.setPoint(enEle, QPointF(dX, dY));
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
                // M位置を保存
                iMPos = m_listPathEle.length() - 1;
                while (pathNextIsNum(&listPathStr) == true) {
                    // 続く場合はLコマンド
                    if (pathGetNextNum(&listPathStr, &dX, enUnit, dDpi) == false) { DEBUG_STRING("Cannot get next path element M value A next."); return false; }
                    if (pathGetNextNum(&listPathStr, &dY, enUnit, dDpi) == false) { DEBUG_STRING("Cannot get next path element M value A next."); return false; }
                    stPathEle.setPoint(EN_SVG_PATH_ELE_L, QPointF(dX, dY));
                    // リストに追加
                    m_listPathEle.append(stPathEle);
                    enPrevEle = stPathEle.getPathEle();
                }
            } else {
                if (enPrevEle == EN_SVG_PATH_ELE_NONE) {
                    if (pathGetNextNum(&listPathStr, &dX, enUnit, dDpi) == false) { DEBUG_STRING("Cannot get next path element M prevElement NONE."); return false; }
                    if (pathGetNextNum(&listPathStr, &dY, enUnit, dDpi) == false) { DEBUG_STRING("Cannot get next path element M prevElement NONE."); return false; }
                } else {
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING("Cannot get next path element M prevElement Other NONE."); return false; }
                    dX += dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING("Cannot get next path element M prevElement Other NONE."); return false; }
                    dY += dValue;
                }
                stPathEle.setPoint(enEle, QPointF(dX, dY));
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
                // M位置を保存
                iMPos = m_listPathEle.length() - 1;
                while (pathNextIsNum(&listPathStr) == true) {
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING("Cannot get next path element M prevElement Other NONE next."); return false; }
                    dX += dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING("Cannot get next path element M prevElement Other NONE next."); return false; }
                    dY += dValue;
                    stPathEle.setPoint(EN_SVG_PATH_ELE_L, QPointF(dX, dY));
                    // リストに追加
                    m_listPathEle.append(stPathEle);
                    enPrevEle = stPathEle.getPathEle();
                }
            }
            break;
        case EN_SVG_PATH_ELE_Z:
            if (m_listPathEle.length() > 0) {
                stPathEle.setPoint(enEle, m_listPathEle[iMPos].getPoint());
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
                dX = stPathEle.getPoint().x();
                dY = stPathEle.getPoint().y();
            }
            break;
        case EN_SVG_PATH_ELE_L:
            while (pathNextIsNum(&listPathStr) == true) {
                if (enValue == EN_SVG_PATH_VALUE_A) {
                    if (pathGetNextNum(&listPathStr, &dX, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dY, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                } else {
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX += dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY += dValue;
                }
                stPathEle.setPoint(enEle, QPointF(dX, dY));
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
            }
            break;
        case EN_SVG_PATH_ELE_H:
            while (pathNextIsNum(&listPathStr) == true) {
                if (enValue == EN_SVG_PATH_VALUE_A) {
                    if (pathGetNextNum(&listPathStr, &dX, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                } else {
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX += dValue;
                }
                stPathEle.setPoint(enEle, QPointF(dX, dY));
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
            }
            break;
        case EN_SVG_PATH_ELE_V:
            while (pathNextIsNum(&listPathStr) == true) {
                if (enValue == EN_SVG_PATH_VALUE_A) {
                    if (pathGetNextNum(&listPathStr, &dY, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                } else {
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY += dValue;
                }
                stPathEle.setPoint(enEle, QPointF(dX, dY));
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
            }
            break;
        case EN_SVG_PATH_ELE_C:
            while (pathNextIsNum(&listPathStr) == true) {
                qreal   dX2;
                qreal   dY2;
                qreal   dX3;
                qreal   dY3;
                qreal   dX4;
                qreal   dY4;

                if (enValue == EN_SVG_PATH_VALUE_A) {
                    if (pathGetNextNum(&listPathStr, &dX2, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dY2, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dX3, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dY3, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dX4, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dY4, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                } else {
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX2 = dX + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY2 = dY + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX3 = dX + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY3 = dY + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX4 = dX + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY4 = dY + dValue;
                }
                stPathEle.setBezier(enEle, QPointF(dX, dY), QPointF(dX2, dY2), QPointF(dX3, dY3), QPointF(dX4, dY4));
                dX = dX4;
                dY = dY4;
                xPrevCp = dX3;
                yPrevCp = dY3;
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
            }
            break;
        case EN_SVG_PATH_ELE_S:
            while (pathNextIsNum(&listPathStr) == true) {
                qreal   dX2;
                qreal   dY2;
                qreal   dX3;
                qreal   dY3;
                qreal   dX4;
                qreal   dY4;

                if (enPrevEle == EN_SVG_PATH_ELE_C || enPrevEle == EN_SVG_PATH_ELE_S ) {
                    dX2 = dX - (xPrevCp - dX);
                    dY2 = dY - (yPrevCp - dY);
                } else {
                    dX2 = dX;
                    dY2 = dY;
                }
                if (enValue == EN_SVG_PATH_VALUE_A) {
                    if (pathGetNextNum(&listPathStr, &dX3, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dY3, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dX4, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dY4, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                } else {
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX3 = dX + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY3 = dY + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX4 = dX + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY4 = dY + dValue;
                }
                stPathEle.setBezier(enEle, QPointF(dX, dY), QPointF(dX2, dY2), QPointF(dX3, dY3), QPointF(dX4, dY4));
                dX = dX4;
                dY = dY4;
                xPrevCp = dX3;
                yPrevCp = dY3;
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
            }
            break;
        case EN_SVG_PATH_ELE_Q:
            while (pathNextIsNum(&listPathStr) == true) {
                qreal   dX2;
                qreal   dY2;
                qreal   dX3;
                qreal   dY3;

                if (enValue == EN_SVG_PATH_VALUE_A) {
                    if (pathGetNextNum(&listPathStr, &dX2, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dY2, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dX3, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dY3, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                } else {
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX2 = dX + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY2 = dY + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX3 = dX + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY3 = dY + dValue;
                }
                stPathEle.setQuadratic(enEle, QPointF(dX, dY), QPointF(dX2, dY2), QPointF(dX3, dY3));
                dX = dX3;
                dY = dY3;
                xPrevCp = dX2;
                yPrevCp = dY2;
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
            }
            break;
        case EN_SVG_PATH_ELE_T:
            while (pathNextIsNum(&listPathStr) == true) {
                qreal   dX2;
                qreal   dY2;
                qreal   dX3;
                qreal   dY3;

                if (enPrevEle == EN_SVG_PATH_ELE_Q || enPrevEle == EN_SVG_PATH_ELE_T ) {
                    dX2 = dX - (xPrevCp - dX);
                    dY2 = dY - (yPrevCp - dY);
                } else {
                    dX2 = dX;
                    dY2 = dY;
                }
                if (enValue == EN_SVG_PATH_VALUE_A) {
                    if (pathGetNextNum(&listPathStr, &dX3, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dY3, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                } else {
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX3 = dX + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY3 = dY + dValue;
                }
                stPathEle.setQuadratic(enEle, QPointF(dX, dY), QPointF(dX2, dY2), QPointF(dX3, dY3));
                dX = dX3;
                dY = dY3;
                xPrevCp = dX2;
                yPrevCp = dY2;
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
            }
            break;
        case EN_SVG_PATH_ELE_A:
            while (pathNextIsNum(&listPathStr) == true) {
                qreal   rx;
                qreal   ry;
                qreal   xrot;
                qreal   large;
                qreal   sweep;
                qreal   dX2;
                qreal   dY2;

                if (pathGetNextNum(&listPathStr, &rx, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                if (pathGetNextNum(&listPathStr, &ry, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                if (pathGetNextNum(&listPathStr, &xrot, enUnit, dDpi, false) == false) { DEBUG_STRING(""); return false; }
                if (pathGetNextNum(&listPathStr, &large, enUnit, dDpi, false) == false) { DEBUG_STRING(""); return false; }
                if (pathGetNextNum(&listPathStr, &sweep, enUnit, dDpi, false) == false) { DEBUG_STRING(""); return false; }
                if (enValue == EN_SVG_PATH_VALUE_A) {
                    if (pathGetNextNum(&listPathStr, &dX2, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    if (pathGetNextNum(&listPathStr, &dY2, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                } else {
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dX2 = dX + dValue;
                    if (pathGetNextNum(&listPathStr, &dValue, enUnit, dDpi) == false) { DEBUG_STRING(""); return false; }
                    dY2 = dY + dValue;
                }
                if (dX == dX2 && dY == dY2) {
                    continue;
                }
                if (rx == 0.0 || ry == 0.0) {
                    continue;
                }
                if (rx < 0.0) {
                    rx *= -1.0;
                }
                if (ry < 0.0) {
                    ry *= -1.0;
                }
                if (stPathEle.setArc(enEle, QPointF(dX, dY), QPointF(dX2, dY2), QPointF(rx, ry), xrot, large == 0.0 ? false : true, sweep == 0.0 ? false : true) == false) {
                    continue;
                }
                dX = dX2;
                dY = dY2;
                // リストに追加
                m_listPathEle.append(stPathEle);
                enPrevEle = stPathEle.getPathEle();
            }
            break;
        default:
            DEBUG_STRING("");
            return false;
        }
    }

    return true;
}
//---------------------------------------------------------------------------
void SvgPath::pathSplit(QStringList* pListPathStr, const QString  &strPath)
{
    // D要素分解
    QRegularExpression re("([A-Za-z]|-?[0-9]*\\.?[0-9]*(?:e-?[0-9]*)?)");
    QRegularExpressionMatchIterator i = re.globalMatch(strPath);

    // 配列に格納
    pListPathStr->clear();
    while (i.hasNext()) {
        QRegularExpressionMatch match = i.next();
        QString content = match.captured(1);
        if(content == "") { continue; }
        if(content.startsWith("-.") == 1) {
            content.replace("-.", "-0.");
        }
        if(content.startsWith(".") == 1) {
            content = "0" + content;
        }
        pListPathStr->append(content);
    }
}
//---------------------------------------------------------------------------
bool SvgPath::pathNextIsNum(QStringList* pListPathStr)
{
    bool ok;

    if (pListPathStr->length() <= 0) {
        return false;
    }

    pListPathStr->at(0).toDouble(&ok);

    return ok;
}
//---------------------------------------------------------------------------
bool SvgPath::pathGetNextCmd(QStringList* pListPathStr, EN_SVG_PATH_ELE* penEle, EN_SVG_PATH_VALUE* penValue)
{
    QString strEle;

    if (pListPathStr->length() <= 0) {
        DEBUG_STRING("");
        return false;
    }

    strEle = pListPathStr->at(0);
    pListPathStr->removeFirst();

    if (strEle == "M") {
        *penEle = EN_SVG_PATH_ELE_M;
        *penValue = EN_SVG_PATH_VALUE_A;
    } else if (strEle == "m") {
        *penEle = EN_SVG_PATH_ELE_M;
        *penValue = EN_SVG_PATH_VALUE_R;
    } else if (strEle == "Z") {
        *penEle = EN_SVG_PATH_ELE_Z;
        *penValue = EN_SVG_PATH_VALUE_A;
    } else if (strEle == "z") {
        *penEle = EN_SVG_PATH_ELE_Z;
        *penValue = EN_SVG_PATH_VALUE_R;
    } else if (strEle == "L") {
        *penEle = EN_SVG_PATH_ELE_L;
        *penValue = EN_SVG_PATH_VALUE_A;
    } else if (strEle == "l") {
        *penEle = EN_SVG_PATH_ELE_L;
        *penValue = EN_SVG_PATH_VALUE_R;
    } else if (strEle == "H") {
        *penEle = EN_SVG_PATH_ELE_H;
        *penValue = EN_SVG_PATH_VALUE_A;
    } else if (strEle == "h") {
        *penEle = EN_SVG_PATH_ELE_H;
        *penValue = EN_SVG_PATH_VALUE_R;
    } else if (strEle == "V") {
        *penEle = EN_SVG_PATH_ELE_V;
        *penValue = EN_SVG_PATH_VALUE_A;
    } else if (strEle == "v") {
        *penEle = EN_SVG_PATH_ELE_V;
        *penValue = EN_SVG_PATH_VALUE_R;
    } else if (strEle == "C") {
        *penEle = EN_SVG_PATH_ELE_C;
        *penValue = EN_SVG_PATH_VALUE_A;
    } else if (strEle == "c") {
        *penEle = EN_SVG_PATH_ELE_C;
        *penValue = EN_SVG_PATH_VALUE_R;
    } else if (strEle == "S") {
        *penEle = EN_SVG_PATH_ELE_S;
        *penValue = EN_SVG_PATH_VALUE_A;
    } else if (strEle == "s") {
        *penEle = EN_SVG_PATH_ELE_S;
        *penValue = EN_SVG_PATH_VALUE_R;
    } else if (strEle == "Q") {
        *penEle = EN_SVG_PATH_ELE_Q;
        *penValue = EN_SVG_PATH_VALUE_A;
    } else if (strEle == "q") {
        *penEle = EN_SVG_PATH_ELE_Q;
        *penValue = EN_SVG_PATH_VALUE_R;
    } else if (strEle == "T") {
        *penEle = EN_SVG_PATH_ELE_T;
        *penValue = EN_SVG_PATH_VALUE_A;
    } else if (strEle == "t") {
        *penEle = EN_SVG_PATH_ELE_T;
        *penValue = EN_SVG_PATH_VALUE_R;
    } else if (strEle == "A") {
        *penEle = EN_SVG_PATH_ELE_A;
        *penValue = EN_SVG_PATH_VALUE_A;
    } else if (strEle == "a") {
        *penEle = EN_SVG_PATH_ELE_A;
        *penValue = EN_SVG_PATH_VALUE_R;
    } else {
        DEBUG_STRING("");
        return false;
    }

    return true;
}
//---------------------------------------------------------------------------
bool SvgPath::pathGetNextNum(QStringList* pListPathStr, qreal* pdNum, EN_SVG_UNIT enUnit, qreal dDpi, bool bTransfer)
{
    bool ok;

    if (pListPathStr->length() <= 0) {
        DEBUG_STRING("");
        return false;
    }

    if (bTransfer == true) {
        *pdNum = UnitTransfer::toSvgPixel(pListPathStr->at(0).toDouble(&ok), enUnit, dDpi);
    } else {
        *pdNum = pListPathStr->at(0).toDouble(&ok);
    }
    if(ok == false) {
        DEBUG_STRING(pListPathStr->at(0));
    }
    pListPathStr->removeFirst();

    return ok;
}
//---------------------------------------------------------------------------
// ストロークの色を取得
QColor SvgPath::getStrokeColor()
{
    return m_stCom.stroke.getColorObj();
}
//---------------------------------------------------------------------------
// ストロークの色を取得
QColor SvgPath::getFillColor()
{
    return m_stCom.fill.getColorObj();
}
//---------------------------------------------------------------------------
QDomElement SvgPath::getNodeElement(QDomDocument* pDoc)
{
    QDomElement node = pDoc->createElement("path");

    // 共通属性
    node.setAttribute("fill", m_stCom.fill.toString());
    node.setAttribute("stroke", m_stCom.stroke.toString());
    node.setAttribute("stroke-width", QString::number(m_stCom.strokeWidth));
    node.setAttribute("stroke-dasharray", m_stCom.dashArray);
    node.setAttribute("transform", m_stCom.transform.toString());
    node.setAttribute("vector-effect", "non-scaling-stroke");
    node.setAttribute("d", getPathStr());
    QString display = m_stCom.display ? "true" : "false";
    node.setAttribute("display", display);

    return node;
}
//---------------------------------------------------------------------------
QString SvgPath::getPathStr()
{
    QString strPathStr = "";

    for (int i = 0; i < m_listPathEle.length(); ++i) {
        strPathStr += m_listPathEle[i].getEleStr();
        if (i != m_listPathEle.length() - 1) {
            strPathStr +=  " ";
        }
    }

    return strPathStr;
}
//---------------------------------------------------------------------------
// BOXサイズ取得
bool SvgPath::getBBox(ST_SVG_BBOX* pstBBox)
{
    ST_SVG_BBOX stMaxBBox;
    ST_SVG_BBOX stBBox;

    if (m_listPathEle.length() == 0) {
        DEBUG_STRING("");
        return false;
    }

    stMaxBBox.p1 = QPointF(FLT_MAX, FLT_MAX);
    stMaxBBox.p2 = QPointF(FLT_MIN, FLT_MIN);

    for (int i = 0; i < m_listPathEle.length(); ++i) {
        if (m_listPathEle[i].getBBox(&stBBox) == false) {
            DEBUG_STRING("");
            return false;
        }
        if (stMaxBBox.p1.x() > stBBox.p1.x()) {
            stMaxBBox.p1.setX(stBBox.p1.x());
        }
        if (stMaxBBox.p1.y() > stBBox.p1.y()) {
            stMaxBBox.p1.setY(stBBox.p1.y());
        }
        if (stMaxBBox.p2.x() < stBBox.p2.x()) {
            stMaxBBox.p2.setX(stBBox.p2.x());
        }
        if (stMaxBBox.p2.y() < stBBox.p2.y()) {
            stMaxBBox.p2.setY(stBBox.p2.y());
        }
    }

    // トランスフォーム適用
    QPointF tpl, tpr, btl, btr;
    tpl = m_stCom.transform.applyTransform(stMaxBBox.p1);
    btr = m_stCom.transform.applyTransform(stMaxBBox.p2);
    tpr = m_stCom.transform.applyTransform(QPointF(stMaxBBox.p2.x(), stMaxBBox.p1.y()));
    btl = m_stCom.transform.applyTransform(QPointF(stMaxBBox.p1.x(), stMaxBBox.p2.y()));

    QRectF overall(tpl, btr);
    overall = overall.united(QRectF(tpr, btl));

    pstBBox->p1 = overall.topLeft();
    pstBBox->p2 = overall.bottomRight();

    return true;
}
//---------------------------------------------------------------------------
void SvgPath::getSubPoint(QSubPointList* pSubPointList, const SvgTransform &transform, qreal dDpi, qreal tolerance2, QPointF offset)
{
    QPointList  listPoint;
    SvgTransform    newTransform(transform);
    qreal totalMaxScale = newTransform.getScale();
    QPointF p;
    QPointF tp;

    if (totalMaxScale != 0) {
        tolerance2 /= qPow(totalMaxScale, 2);
    }

    pSubPointList->clear();
    listPoint.clear();
    for (int i = 0; i < m_listPathEle.length(); ++i) {
        switch ( m_listPathEle[i].getPathEle()) {
        case EN_SVG_PATH_ELE_M:
            if (listPoint.length() > 0) {
                pSubPointList->append(listPoint);
                listPoint.clear();
            }
            p = m_stCom.transform.applyTransform( m_listPathEle[i].getPoint() ) - offset;
            p = UnitTransfer::SvgPixelPointto(p, EN_SVG_UNIT_MM, dDpi);
            tp = newTransform.applyTransform(p);
            listPoint.append(tp);
            break;
        case EN_SVG_PATH_ELE_Z:
            if (listPoint.length() > 0) {
                listPoint.append(listPoint[0]);
                pSubPointList->append(listPoint);
                listPoint.clear();
            }
            break;
        case EN_SVG_PATH_ELE_L:
        case EN_SVG_PATH_ELE_H:
        case EN_SVG_PATH_ELE_V:
            p = m_stCom.transform.applyTransform(m_listPathEle[i].getPoint() ) - offset;
            p = UnitTransfer::SvgPixelPointto(p, EN_SVG_UNIT_MM, dDpi);
            tp = newTransform.applyTransform(p);
            listPoint.append(tp);
            break;
        case EN_SVG_PATH_ELE_Q:
        case EN_SVG_PATH_ELE_T:
            m_listPathEle[i].getQuadraticBezier(&listPoint, &(m_stCom.transform), &newTransform, dDpi, tolerance2, offset);
            break;
        case EN_SVG_PATH_ELE_C:
        case EN_SVG_PATH_ELE_S:
            m_listPathEle[i].getCubicBezier(&listPoint, &(m_stCom.transform), &newTransform, dDpi, tolerance2, offset);
            break;
        case EN_SVG_PATH_ELE_A:
            m_listPathEle[i].getArc(&listPoint, &(m_stCom.transform), &newTransform, dDpi, tolerance2, offset);
            break;
        default:
            break;
        }
    }
    if (listPoint.length() > 0) {
        pSubPointList->append(listPoint);
        listPoint.clear();
    }
}
//---------------------------------------------------------------------------
void  SvgPath::setStrokeWid(qreal wid)
{
    m_stCom.strokeWidth = wid;
}
//---------------------------------------------------------------------------
void  SvgPath::readData(const QJsonObject &json)
{
    QJsonObject  cmn             = json[JSON_TAG_SVG_COMMON_SETTING].toObject();
    QJsonObject  cmn_fill        = cmn[JSON_TAG_SVG_COMMON_FILL].toObject();
    QJsonObject  cmn_stroke      = cmn[JSON_TAG_SVG_COMMON_STROKE].toObject();
    QString      cmn_stroke_dash = cmn[JSON_TAG_SVG_COMMON_STROKE_DASH].toString();
    QJsonObject  cmn_transform   = cmn[JSON_TAG_SVG_COMMON_TRANSFORM].toObject();
    qreal        strokeW         = cmn[JSON_TAG_SVG_COMMON_STROKE_WIDTH].toDouble();
    if(strokeW <= 0) { strokeW = 1.0; }
    m_stCom.fill.readData(cmn_fill);
    m_stCom.stroke.readData(cmn_stroke);
    m_stCom.strokeWidth = strokeW;
    m_stCom.dashArray = cmn_stroke_dash;
    m_stCom.transform.readData(cmn_transform);
    m_stCom.display = cmn[JSON_TAG_SVG_COMMON_DISPLAY].toBool();

    QJsonArray pathEleArray = json[JSON_TAG_SVG_PATH_ELEM_LIST].toArray();
    foreach (QJsonValue elemVal, pathEleArray) {
        SvgPathEle elem;
        QJsonObject elemObj = elemVal.toObject();
        elem.readData(elemObj);
        m_listPathEle.append(elem);
    }
}
//---------------------------------------------------------------------------
void  SvgPath::writeDown(QJsonObject &json) const
{
    QJsonObject  cmn;
    QJsonObject  cmn_fill;
    QJsonObject  cmn_stroke;
    QJsonObject  cmn_transform;
    m_stCom.fill.writeDown(cmn_fill);
    m_stCom.stroke.writeDown(cmn_stroke);
    m_stCom.transform.writeDown(cmn_transform);
    cmn[JSON_TAG_SVG_COMMON_FILL]       = cmn_fill;
    cmn[JSON_TAG_SVG_COMMON_STROKE]     = cmn_stroke;
    cmn[JSON_TAG_SVG_COMMON_STROKE_WIDTH] = m_stCom.strokeWidth;
    cmn[JSON_TAG_SVG_COMMON_STROKE_DASH] = m_stCom.dashArray;
    cmn[JSON_TAG_SVG_COMMON_TRANSFORM]  = cmn_transform;
    cmn[JSON_TAG_SVG_COMMON_DISPLAY]    = m_stCom.display;
    json[JSON_TAG_SVG_COMMON_SETTING]   = cmn;

    QJsonArray pathEleArray;
    foreach (SvgPathEle elem, m_listPathEle) {
        QJsonObject elemObj;
        elem.writeDown(elemObj);
        pathEleArray.append(elemObj);
    }
    json[JSON_TAG_SVG_PATH_ELEM_LIST] = pathEleArray;

}
//---------------------------------------------------------------------------
void SvgPath::setAttribute(const QJsonObject& common)
{
    QJsonObject  cmn_fill       = common[JSON_TAG_SVG_COMMON_FILL].toObject();
    QJsonObject  cmn_stroke     = common[JSON_TAG_SVG_COMMON_STROKE].toObject();
    qreal        cmn_strokeW    = common[JSON_TAG_SVG_COMMON_STROKE_WIDTH].toDouble();
    QString      cmn_stroke_dash = common[JSON_TAG_SVG_COMMON_STROKE_DASH].toString();
    QJsonObject  cmn_transform  = common[JSON_TAG_SVG_COMMON_TRANSFORM].toObject();
    if(cmn_strokeW <= 0) { cmn_strokeW = 1.0; }
    m_stCom.fill.readData(cmn_fill);
    m_stCom.stroke.readData(cmn_stroke);
    m_stCom.strokeWidth = cmn_strokeW;
    m_stCom.dashArray = cmn_stroke_dash;
    m_stCom.transform.readData(cmn_transform);
    m_stCom.display = common[JSON_TAG_SVG_COMMON_DISPLAY].toBool();
}
//---------------------------------------------------------------------------
void SvgPath::setPathJson(const QJsonObject& common, const QJsonArray& pathData, const qreal& dpi)
{
    setAttribute(common);

    QString dStr;
    foreach (QJsonValue d, pathData) {
        QJsonArray dArray = d.toArray();
        foreach (QJsonValue detail, dArray) {
            if(detail.isString() == true) {
                dStr += detail.toString();
            } else if(detail.isDouble() == true) {
                dStr += QString::number(detail.toDouble());
            }

            dStr += " ";
        }
    }
    loadPath(dStr, EN_SVG_UNIT_NON, dpi);
}
//---------------------------------------------------------------------------
QList<QPointList> SvgPath::getAllPoint()
{
    QList<QPointList> pointAll;

    //すべてpx単位のままサブポイント化する
    //パスエレメントごとに区切る
    QPointList tempPoints;
    foreach(SvgPathEle elem, m_listPathEle) {
        QPointList points = elem.getPxPointList();
        if(tempPoints.isEmpty() == true) {
            tempPoints.append(points);
        } else {
            if(tempPoints.last() == points.first()) {
                points.takeFirst();
            }
            tempPoints.append(points);
        }
        if(elem.getPathEle() == EN_SVG_PATH_ELE_Z) {
            pointAll.append(tempPoints);
            tempPoints.clear();
        }
    }
    if(tempPoints.isEmpty() == false) {
        pointAll.append(tempPoints);
    }

    return pointAll;
}
//---------------------------------------------------------------------------
QList<qreal> SvgPath::createDashList(EN_SVG_UNIT unit, qreal dpi)
{
    QStringList strDash = m_stCom.dashArray.split(",", QString::SkipEmptyParts);
    if(strDash.count() < 2) {
        return QList<qreal>();
    }

    //破線属性の解析
    QList<qreal> dashDistanceList ;
    bool isOk = true;
    foreach (QString str, strDash) {
        dashDistanceList.append(UnitTransfer::SvgPixelto(str.toDouble(&isOk), unit, dpi));
        if(isOk == false) {
            return QList<qreal>();
        }
    }

    return dashDistanceList;
}
//---------------------------------------------------------------------------
void SvgPath::getDashLines(QSubPointList* distSubpoint, QSubPointList srcSubpoint, QList<qreal> dashDistanceList)
{
    //全部のパスをサブポイント化
    QSubPointList retPathes;

    //処理開始
    foreach (QPointList currPoints, srcSubpoint) {
        QPointF lastPos = currPoints.first();
        QPointF currPos = currPoints.first();
        int pointIndex = 1, distIndex = 0;
        qreal currDash = dashDistanceList.first();
        qreal cumulativeLength = 0;
        EN_SVG_PATH_ELE pathType = EN_SVG_PATH_ELE_L;

        QPointList path;
        path.append(lastPos);
        while(1) {
            if(pointIndex >= currPoints.count()) { break; }

            //最後の点から現在地までの距離を測り、破線の長さ以上かどうかを確認する
            currPos = currPoints.at(pointIndex);
            QPointF diffPos = currPos - lastPos;
            qreal  distance = diffPos.manhattanLength();
            if(cumulativeLength + distance < currDash) {
                //距離が破線以下なら位置をずらして続行
                if(pathType == EN_SVG_PATH_ELE_L) {
                    path.append(currPos);
                }
                pointIndex++;
                lastPos = currPos;
                cumulativeLength += distance;
            } else {
                //距離が破線以上であれば、距離が破線の長さと同じになる点を求める
                qreal lastScale = (currDash - cumulativeLength) / distance;
                lastPos = lastPos + (diffPos * lastScale);

                path.append(lastPos);

                if(pathType == EN_SVG_PATH_ELE_M) {
                    pathType = EN_SVG_PATH_ELE_L;
                } else {
                    retPathes.append(path);
                    path.clear();
                    pathType = EN_SVG_PATH_ELE_M;
                }

                // 後処理
                distIndex++;
                if(distIndex >= dashDistanceList.count()) {
                    distIndex = 0;
                }
                currDash = dashDistanceList.at(distIndex);
                cumulativeLength = 0;
            }
        }
        if(pathType == EN_SVG_PATH_ELE_L) {
            path.append(currPoints.first());
            retPathes.append(path);
        }
    }
    distSubpoint->append(retPathes);

}
//---------------------------------------------------------------------------
void SvgPath::setDisplay(bool display)
{
    m_stCom.display = display;
}
//---------------------------------------------------------------------------
QDomElement SvgPath::content()
{
    return m_content;
}
//---------------------------------------------------------------------------
QString SvgPath::id()
{
    QDomElement elem = m_content;
    if(elem.hasAttribute("id") == true) {
        return elem.attribute("id");
    }
    return "";
}
//---------------------------------------------------------------------------
QString SvgPath::referenceId()
{
    QDomElement elem = m_content;
    if(elem.hasAttribute("href") == true) {
        return elem.attribute("href").replace("#", "");
    }
    return "";
}
//---------------------------------------------------------------------------
QString SvgPath::contentTag()
{
    return m_content.tagName();
}
//---------------------------------------------------------------------------
bool SvgPath::isEmpty()
{
    return m_listPathEle.isEmpty();
}
//---------------------------------------------------------------------------
void SvgPath::copyPathElement(SvgPath srcPath)
{
    m_listPathEle << srcPath.m_listPathEle;
}
//---------------------------------------------------------------------------
void SvgPath::copyPathElement(QList<SvgPath> srcPath)
{
    foreach (SvgPath path, srcPath) {
        m_listPathEle << path.m_listPathEle;
    }
}
//---------------------------------------------------------------------------
void SvgPath::applyViewPos(qreal x, qreal y)
{
    ST_SVG_BBOX stBBox;
    if(getBBox(&stBBox) == false) { return; }

    QList<SvgPathEle> retElem;
    foreach (SvgPathEle elem, m_listPathEle) {
        elem.applyViewBox(stBBox.p1, QPointF(x, y), 1, 1);
        retElem << elem;
    }
    m_listPathEle.clear();
    m_listPathEle << retElem;
}
//---------------------------------------------------------------------------
QList<SvgPath> SvgPath::parseUse(SvgParser* parser)
{
    QList<SvgPath> parsedChildren;

    //子を持つ場合は子を子を解析し結果をまとめてリターン
    if(m_childPath.isEmpty() == false) {
        foreach (SvgPath childPath, m_childPath) {
            parsedChildren << childPath.parseUse(parser);
        }
        return parsedChildren;
    }

    if("use" != contentTag()) {
        //Useでない場合は自分をリターン
        parsedChildren << *this;
        return parsedChildren;
    }

    // 以下Useの場合　参照IDを持つSvgPathを引っ張ってくる
    SvgPath refPath = parser->getPathById(referenceId());
    if(refPath.id() == "") {
        return parsedChildren;
    }
    QList<SvgPath> parsedUse;
    parsedUse << refPath;

    //子をもつ場合はさらに解析
    foreach (SvgPath refChild, refPath.m_childPath) {
        parsedUse << refChild.parseUse(parser);
    }

    QDomElement elem = content().toElement();
    qreal x = 0, y = 0, wid = 1, hgt = 1;
    if(elem.hasAttribute("x") == true) { x = elem.attribute("x").toDouble() ; }
    if(elem.hasAttribute("y") == true) { y = elem.attribute("y").toDouble() ; }
    if(elem.hasAttribute("width") == true) { wid = elem.attribute("width").toDouble() ; }
    if(elem.hasAttribute("height") == true) { hgt = elem.attribute("height").toDouble() ; }


    foreach (SvgPath parsedChild, parsedUse) {
        if(parsedChild.m_listPathEle.count() == 0) {
            continue;
        }
        parsedChild.setDisplay(m_stCom.display);

        //　解析したUseを指定したサイズに適用
        if(wid > 0 && hgt > 0) {
            parsedChild.applyViewPos(x, y);
        }
        parsedChildren << parsedChild;
    }
    return parsedChildren;
}
//---------------------------------------------------------------------------
QList<SvgPath> SvgPath::getDisplayPaths()
{
    QList<SvgPath> retList;
    //末端のPathまで解析
    foreach (SvgPath path, m_childPath) {
        retList << path.getDisplayPaths();
    }
    //末端の場合はDisplayをチェックし、Trueなら自身の入ったListを返す
    if(m_stCom.display == true) {
        if(SysParam::getInstance()->getEffectiveStrokeWid() == true) {
            m_stCom.strokeWidth = SysParam::getInstance()->getStrokeWidth();
        } else {
            m_stCom.strokeWidth = 1;
        }
        retList.append(*this);
    }
    return retList;
}
//---------------------------------------------------------------------------
ST_SVG_ELE_COM SvgPath::getComm()
{
    return m_stCom;
}
//---------------------------------------------------------------------------
void SvgPath::addChildrenPath(SvgPath path)
{
    m_childPath.append(path);
}
//---------------------------------------------------------------------------
void SvgPath::addChildrenPath(QList<SvgPath> paths)
{
    m_childPath.append(paths);
}
//---------------------------------------------------------------------------
SvgPath SvgPath::getPathById(QString Id)
{
    QString curId = id();
    if(curId == Id) {
        return *this;
    }
    foreach (SvgPath path, m_childPath) {
        SvgPath found = path.getPathById(Id);
        if(found.id() != "") {
            return found;
        }
    }
    return SvgPath();
}
//---------------------------------------------------------------------------
bool SvgPath::hasEffectiveStrokeColor()
{
    if(m_stCom.stroke.isEffectiveColor() == false || contentTag() == "use") {
        return false;
    } else {
        return true;
    }
}
//---------------------------------------------------------------------------
bool SvgPath::hasEffectiveFillColor()
{
    if(m_stCom.fill.isEffectiveColor() == false || contentTag() == "use" || SysParam::getInstance()->getEffectiveFill() == false) {
        return false;
    } else {
        return true;
    }
}
//---------------------------------------------------------------------------
void SvgPath::getFillLines(QSubPointList* subpoint, QSubPointList pointAll)
{
    //処理開始
    subpoint->clear();
    qreal pitch = SysParam::getInstance()->getFillPitch();
    QList<QLineF> shapeLines;
    QRectF boundRect ;
    foreach (QPointList currPoints, pointAll) {
        if(currPoints.count() < 3) { continue; }
        QPolygonF shape;
        shape.append(currPoints.toVector());
        shapeLines << polygonToLines(shape);
        boundRect = boundRect.united(shape.boundingRect());
    }

    QSubPointList retPoints;
    QLineF currentLine(boundRect.topLeft() + QPointF(-1, pitch), boundRect.topRight() + QPointF(1, pitch));
    while(currentLine.p1().y() < boundRect.bottom()) {
        QList<QPointF> crossPos = crossPoint(currentLine, &shapeLines);

        if(crossPos.count() > 1 && crossPos.count() % 2 == 0) {
            for(int crossIndex = 0; crossIndex < crossPos.count(); crossIndex = crossIndex + 2) {
                QPointList path;
                path << crossPos.at(crossIndex) << crossPos.at(crossIndex + 1);
                retPoints.append(path);
            }
        }

        currentLine.setLine(currentLine.p1().x(), currentLine.p1().y() + pitch,
                            currentLine.p2().x(), currentLine.p2().y() + pitch);
    }
    subpoint->append(retPoints);
}

QList<QLineF> SvgPath::polygonToLines(QPolygonF polygon)
{
    QList<QLineF> lines;
    QList<QPointF> points = polygon.toList();
    QPointF prev = points.takeFirst();
    foreach (QPointF pos, points) {
        QLineF line(prev, pos);
        lines << line;
        prev = pos;
    }
    return lines;
}

QList<QPointF> SvgPath::crossPoint(QLineF checkLine, QList<QLineF>* shapeLines)
{
    QList<SortablePointF> retPoints;
    QList<QLineF> retLines;

    while(shapeLines->count() > 0) {
        SortablePointF cross;
        QLineF currentLine = shapeLines->takeFirst();
        if(currentLine.intersect(checkLine, &cross) == QLineF::BoundedIntersection) {
            if(cross - currentLine.p1() == QPointF()) {
                currentLine.setP1(currentLine.p1() + QPointF(0, 1));
                shapeLines->insert(0, currentLine);
            } else if(cross - currentLine.p2() == QPointF()) {
                currentLine.setP2(currentLine.p2() + QPointF(0, 1));
                shapeLines->insert(0, currentLine);
            } else {
                QPointF bigY = currentLine.p1();
                if(currentLine.p1().y() < currentLine.p2().y()) {
                    bigY = currentLine.p2();
                }

                retLines << QLineF(cross, bigY);
                retPoints << cross;
            }
        } else {
            retLines << currentLine;
        }
    }

    shapeLines->append(retLines);

    qSort(retPoints.begin(), retPoints.end(), SortablePointF::compareX);

    QList<QPointF> points;
    foreach (SortablePointF sotable, retPoints) {
        points << sotable.pointF();
    }
    return points;
}
