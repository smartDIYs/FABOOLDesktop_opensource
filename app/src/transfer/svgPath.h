//---------------------------------------------------------------------------
// SVG PATH管理
//---------------------------------------------------------------------------
#ifndef SVGPATH_H
#define SVGPATH_H
//---------------------------------------------------------------------------
#include <QtCore>
#include <QtXml>
#include "common/common.h"
#include "svgCommon.h"
#include "svgPathEle.h"

class SvgParser;
class SvgPath
{
    QTTEST_OBJECT

public:
    SvgPath();
    SvgPath(const SvgPath &src);
    ~SvgPath();

    SvgPath &operator=(const SvgPath &src);
    bool setNode(const QDomElement &element, ST_SVG_ELE_COM stEleCom, EN_SVG_UNIT enUnit, qreal dDpi);
    QColor getStrokeColor();
    QColor getFillColor();
    QDomElement getNodeElement(QDomDocument* pDoc);
    bool getBBox(ST_SVG_BBOX* pstBBox);
    void getSubPoint(QSubPointList* pSubPointList, const SvgTransform &transform, qreal dDpi, qreal tolerance2, QPointF offset);
    void setStrokeWid(qreal wid) ;
    void readData(const QJsonObject &json);
    void writeDown(QJsonObject &json) const;
    void setPathJson(const QJsonObject& common, const QJsonArray& paths, const qreal& dpi);

    void    setDisplay(bool display);
    QDomElement content();
    QString id();
    bool    isEmpty();

    QList<SvgPath> parseUse(SvgParser* parser);
    QList<SvgPath> getDisplayPaths();
    ST_SVG_ELE_COM getComm();
    void    addChildrenPath(SvgPath path);
    void    addChildrenPath(QList<SvgPath> paths);
    SvgPath getPathById(QString Id);

    QList<qreal> createDashList(EN_SVG_UNIT unit, qreal dpi);
    static void getDashLines(QSubPointList* distSubpoint, QSubPointList srcSubpoint, QList<qreal> dashDistanceList);

    bool hasEffectiveStrokeColor();
    bool hasEffectiveFillColor();
    static void getFillLines(QSubPointList* subpoint, QSubPointList parentPoints);

protected:
    void initMenber();
    void deleteMenber();
    SvgPath &copyMenber(const SvgPath &src);
    bool getPoints(const QString &strPoints, QList<QPointF>* pPoints, EN_SVG_UNIT enUnit, qreal dDpi);
    bool loadPath(const QString  &strPath, EN_SVG_UNIT enUnit, qreal dDpi);
    void pathSplit(QStringList* pListPathStr, const QString  &strPath);
    bool pathNextIsNum(QStringList* pListPathStr);
    bool pathGetNextCmd(QStringList* pListPathStr, EN_SVG_PATH_ELE* penEle, EN_SVG_PATH_VALUE* penValue);
    bool pathGetNextNum(QStringList* pListPathStr, qreal* pdNum, EN_SVG_UNIT enUnit, qreal dDpi, bool bTransfer = true);
    QString getPathStr();
    void setAttribute(const QJsonObject& common);
    QList<QPointList> getAllPoint();

    QString referenceId();
    QString contentTag();
    void    copyPathElement(SvgPath srcPath);
    void    copyPathElement(QList<SvgPath> srcPath);
    void    applyViewPos(qreal x, qreal y);

    static QList<QLineF> polygonToLines(QPolygonF polygon);
    static QList<QPointF> crossPoint(QLineF checkLine, QList<QLineF>* shapeLines);

private:
    ST_SVG_ELE_COM      m_stCom;
    QList<SvgPath>      m_childPath;
    QList<SvgPathEle>   m_listPathEle;

    QDomElement         m_content;
};
Q_DECLARE_METATYPE(SvgPath)
//---------------------------------------------------------------------------
#endif // SVGPATH_H
