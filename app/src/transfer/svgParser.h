//---------------------------------------------------------------------------
// SVG解析・整形
//---------------------------------------------------------------------------
#ifndef SVGPARSER_H
#define SVGPARSER_H
//---------------------------------------------------------------------------
#include <QtCore>
#include <QtXml>

#include "common/common.h"
#include "parserCore.h"
#include "svgCommon.h"
#include "dxfParser.h"

class SvgPath;
//---------------------------------------------------------------------------
class SvgParser : public ParserCore
{
    QTTEST_OBJECT

public:
    SvgParser();
    SvgParser(const SvgParser &src);
    virtual ~SvgParser();
    SvgParser &operator=(const SvgParser &src);
    int getPathCount();

    virtual QPointF getStartPoint() override;
    virtual bool loadFile(const QString &strSvgPath) override;
    virtual QByteArray toByte() override;
    virtual int  readData(const QJsonObject &json) override;
    virtual void writeDown(QJsonObject &json) const override;
    virtual QList<QColor> colorMap() override;
    virtual QStringList toGcode(qreal d1, qreal d2, qreal d3, qreal d4, qreal d5, qreal d6, QList<ParamData*> paramList) override;
    virtual QSizeF outerSize() override;
    virtual qreal dpi() override;

    QJsonObject transferPathToCurrent(const QJsonObject& json, const qreal& dpi);
    QRectF getBBox();
    void setAttribute(const QJsonObject& json);

    enum { Type = PARSER_TYPE_VECTOR };
    int type() const
    {
        return Type;
    }
    SvgPath getPathById(QString id);

protected:
    void initEnableTags();
    void initMenber();
    void deleteMenber();
    SvgParser &copyMenber(const SvgParser &src);
    bool parseRoot(const QDomElement &root);
    bool parseStyle(const QDomElement &root);
    QList<SvgPath> parseChildren(const QDomElement &element, ST_SVG_ELE_COM stEleCom);
    bool parseChildren(SvgPath* parent);
    bool parseUse();
    void screeningDisplayPaths();
    bool getAttribute(const QDomElement &element, ST_SVG_ELE_COM* pstEleCom);
    bool applyStyle(const QString &style, ST_SVG_ELE_COM* pstEleCom);
    void addColorlist(QColor color);
    bool calcBBox();
    void getSubPoints(QMap<QString, QSubPointList>* boundarys, const SvgTransform &transform);
    void setColorSubpointMap(QMap<QString, QSubPointList>* boundarys, QPointList list, QString color, qreal epsilon2);
    void optimizeAll(QMap<QString, QSubPointList>& boundarys);
    void connect_segments(QSubPointList& path);
    void simplify_all(QSubPointList& path);
    QPointList simplify(const QPointList &V, qreal tol2);
    void simplifyDP(qreal tol2, const QPointList &v, int j, int k, bool mk[]);
    void sort_by_seektime(QSubPointList& path);
    void makeGCodeStr(QStringList& gcode, const QMap<QString, QSubPointList> &boundarys, const QList<ParamData*> &paramList);

    QPointF sum(const QPointF &u, const QPointF &v) { return QPointF(u.x() + v.x(), u.y() + v.y()); }
    QPointF diff(const QPointF &u, const QPointF &v) { return QPointF(u.x() - v.x(), u.y() - v.y()); }
    QPointF prod(const QPointF &u, const QPointF &v) { return QPointF(u.x() * v.x(), u.y() * v.y()); }
    qreal dot(const QPointF &u, const QPointF &v) { return u.x() * v.x() + u.y() * v.y(); }
    qreal norm2(const QPointF &v) { return v.x() * v.x() + v.y() * v.y(); }
    qreal norm(const QPointF &v) { return qSqrt(norm2(v)); }
    qreal d2(const QPointF &u, const QPointF &v) { return norm2(diff(u, v)); }
    qreal d(const QPointF &u, const QPointF &v) { return norm(diff(u, v)); }

private:

    // 有効属性
    QStringList     m_listEnableTags;

    // パスリスト
    QList<SvgPath>  m_listPath;
    // 色配列
    QList<QColor>   m_listColor;

    // SVG作成ソフト
    EN_SVG_SOFT     m_enSoft;
    // SVG DPI
    qreal           m_dDpi;
    // SVG幅
    qreal           m_dWidth;
    // SVG高さ
    qreal           m_dHeight;
    // SVG単位
    EN_SVG_UNIT     m_enUnit;
    // viewBox
    ST_SVG_VIEW_BOX m_stViewBox;
    // アスペクト比を保持する
    bool            m_bPreserveAspectRatio;
    // 共通設定
    ST_SVG_ELE_COM  m_stEleCom;
    // Bounding Box
    ST_SVG_BBOX     m_stBBox;
};
Q_DECLARE_METATYPE(SvgParser)
//---------------------------------------------------------------------------
#endif // SVGPARSER_H
