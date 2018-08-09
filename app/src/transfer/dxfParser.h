//---------------------------------------------------------------------------
// DXF解析
//---------------------------------------------------------------------------
#ifndef DXFPARSER_H
#define DXFPARSER_H
//---------------------------------------------------------------------------
#include <QtCore>
#include <QtXml>
#include <QVector2D>
#include "dxfDefinedColor.h"
#include "common/common.h"
//---------------------------------------------------------------------------
enum EN_DXF_VALUE {
    EN_DXF_VALUE_FLOAT = 0,
    EN_DXF_VALUE_INT,
    EN_DXF_VALUE_STR,

    EN_DXF_VALUE_MAX
};
Q_DECLARE_METATYPE(EN_DXF_VALUE)
struct ST_DXF_TYPE_A_VALUE {
    int             iType;
    EN_DXF_VALUE    enValue;
    qreal           dValue;
    int             iValue;
    QString         strValue;

public:
    ST_DXF_TYPE_A_VALUE()
    {
        iType = 0;
        enValue = EN_DXF_VALUE_FLOAT;
        dValue = 0;
        iValue = 0;
        strValue = "";
    }
};
Q_DECLARE_METATYPE(ST_DXF_TYPE_A_VALUE)
//---------------------------------------------------------------------------
struct ST_DXF_HEADER {
    QPointF extMin;
    QPointF extMax;
public:
    ST_DXF_HEADER()
    {
        extMax = QPointF();
        extMin = QPointF();
    }
};
Q_DECLARE_METATYPE(ST_DXF_HEADER)
//---------------------------------------------------------------------------
struct ST_DXF_TABLE_LAYER {
    QString name;
    QString lineTypeName;
    int colorNumber;
    int flags;
    QString lineWeightEnum;
public:
    ST_DXF_TABLE_LAYER()
    {
        name = "";
        lineTypeName = "";
        colorNumber = 0;
        flags = 0;
        lineWeightEnum = "";
    }
};
Q_DECLARE_METATYPE(ST_DXF_TABLE_LAYER)
struct ST_DXF_TABLE_STYLE {
    QString name;
    QString lineTypeName;
    qreal fixedTextHeight;
    qreal widthFactor;
    qreal obliqueAngle;
    int flags;
    qreal   lastHeightUsed;
    QString primaryFontFileName;
    QString bigFontFileName;
public:
    ST_DXF_TABLE_STYLE()
    {
        name = "";
        lineTypeName = "";
        fixedTextHeight = 0;
        widthFactor = 0;
        obliqueAngle = 0;
        flags = 0;
        lastHeightUsed = 0;
        primaryFontFileName = "";
        bigFontFileName = "";
    }
};
Q_DECLARE_METATYPE(ST_DXF_TABLE_STYLE)
struct ST_DXF_TABLES {
    QMap<QString, ST_DXF_TABLE_LAYER>   mapLayers;
    QMap<QString, ST_DXF_TABLE_STYLE>   mapStyles;
public:
    ST_DXF_TABLES()
    {
        mapLayers.clear();
        mapStyles.clear();
    }
};
Q_DECLARE_METATYPE(ST_DXF_TABLES)
//---------------------------------------------------------------------------
enum EN_DXF_ENTITIE {
    EN_DXF_ENTITIE_ARC = 0,
    EN_DXF_ENTITIE_CIRCLE,
    EN_DXF_ENTITIE_ELLIPSE,
    EN_DXF_ENTITIE_INSERT,
    EN_DXF_ENTITIE_LINE,
    EN_DXF_ENTITIE_LWPOLYLINE,
    EN_DXF_ENTITIE_POINT,
    EN_DXF_ENTITIE_POLYLINE,
    EN_DXF_ENTITIE_SOLID,
    EN_DXF_ENTITIE_SPLINE,

    EN_DXF_ENTITIE_MAX
};
Q_DECLARE_METATYPE(EN_DXF_ENTITIE)
struct ST_DXF_ENTITIE_VERTEX {
    qreal x;
    qreal y;
    qreal bulge;

public:
    ST_DXF_ENTITIE_VERTEX()
    {
        x = 0;
        y = 0;
        bulge = 0;
    }
};
Q_DECLARE_METATYPE(ST_DXF_ENTITIE_VERTEX)
struct ST_DXF_ENTITIE_COMMON {
    QString lineTypeName;
    QString layer;
    qreal lineTypeScale;
    int visible;
    int colorNumber;
    qreal extrusionX;
    qreal extrusionY;
    qreal extrusionZ;
public:
    ST_DXF_ENTITIE_COMMON()
    {
        lineTypeName = "";
        layer = "";
        lineTypeScale = 1;
        visible = 0;
        colorNumber = 0;
        extrusionX = 0;
        extrusionY = 0;
        extrusionZ = 0;
    }
};
Q_DECLARE_METATYPE(ST_DXF_ENTITIE_COMMON)
struct ST_DXF_ENTITIE_POS {
    qreal x[4];
    qreal y[4];
    qreal thickness;
public:
    ST_DXF_ENTITIE_POS()
    {
        x[0] = 0;
        x[1] = 0;
        x[2] = 0;
        x[3] = 0;
        y[0] = 0;
        y[1] = 0;
        y[2] = 0;
        y[3] = 0;
        thickness = 0;
    }
};
Q_DECLARE_METATYPE(ST_DXF_ENTITIE_POS)
struct ST_DXF_ENTITIE_ARC {
    qreal x;
    qreal y;
    qreal thickness;
    qreal r;
    qreal startAngle;
    qreal endAngle;
public:
    ST_DXF_ENTITIE_ARC()
    {
        x = 0;
        y = 0;
        thickness = 0;
        r = 0;
        startAngle = 0;
        endAngle = 0;
    }
};
Q_DECLARE_METATYPE(ST_DXF_ENTITIE_ARC)
struct ST_DXF_ENTITIE_ELLIPSE {
    qreal x;
    qreal y;
    qreal majorX;
    qreal majorY;
    qreal axisRatio;
    qreal startAngle;
    qreal endAngle;
public:
    ST_DXF_ENTITIE_ELLIPSE()
    {
        x = 0;
        y = 0;
        majorX = 0;
        majorY = 0;
        axisRatio = 0;
        startAngle = 0;
        endAngle = 0;
    }
};
Q_DECLARE_METATYPE(ST_DXF_ENTITIE_ELLIPSE)
struct ST_DXF_ENTITIE_INSERT {
    QString block;
    qreal x;
    qreal y;
    qreal xscale;
    qreal yscale;
    qreal columnSpacing;
    qreal rowSpacing;
    qreal rotation;
    int columnCount;
    int rowCount;
    qreal xExtrusion;
    qreal yExtrusion;
public:
    ST_DXF_ENTITIE_INSERT()
    {
        block = "";
        x = 0;
        y = 0;
        xscale = 0;
        yscale = 0;
        columnSpacing = 0;
        rowSpacing = 0;
        rotation = 0;
        columnCount = 0;
        rowCount = 0;
        xExtrusion = 0;
        yExtrusion = 0;
    }
};
Q_DECLARE_METATYPE(ST_DXF_ENTITIE_INSERT)
struct ST_DXF_ENTITIE_POLYLINE {
    int closed;
    qreal thickness;
    QList<ST_DXF_ENTITIE_VERTEX> vertices;
public:
    ST_DXF_ENTITIE_POLYLINE()
    {
        closed = 0;
        thickness = 0;
        vertices.clear();
    }
};
Q_DECLARE_METATYPE(ST_DXF_ENTITIE_POLYLINE)
struct ST_DXF_ENTITIE_SPLINE {
    QList<QPointF> controlPoints;
    QList<qreal> knots;
    qreal knotTolerance;
    qreal controlPointTolerance;
    qreal fitTolerance;
    int flag;
    int closed;
    int degree;
    int numberOfKnots;
    int numberOfControlPoints;
    int numberOfFitPoints;
public:
    ST_DXF_ENTITIE_SPLINE()
    {
        controlPoints.clear();
        knots.clear();
        knotTolerance = 0;
        controlPointTolerance = 0;
        fitTolerance = 0;
        flag = 0;
        closed = 0;
        degree = 0;
        numberOfKnots = 0;
        numberOfControlPoints = 0;
        numberOfFitPoints = 0;
    }
};
Q_DECLARE_METATYPE(ST_DXF_ENTITIE_SPLINE)
struct ST_DXF_TRANSFORM {
    qreal x;
    qreal y;
    qreal xScale;
    qreal yScale;
    qreal rotation;
public:
    ST_DXF_TRANSFORM()
    {
        x = 0;
        y = 0;
        xScale = 1;
        yScale = 1;
        rotation = 0;
    }
};
Q_DECLARE_METATYPE(ST_DXF_TRANSFORM)
struct ST_DXF_ENTITIE {
    EN_DXF_ENTITIE  enEntitie;
    ST_DXF_ENTITIE_COMMON   common;
    ST_DXF_ENTITIE_POS  pos;
    ST_DXF_ENTITIE_ARC  arc;
    ST_DXF_ENTITIE_ELLIPSE  ellipse;
    ST_DXF_ENTITIE_INSERT  insert;
    ST_DXF_ENTITIE_POLYLINE  polyline;
    ST_DXF_ENTITIE_SPLINE  spline;
    QList<ST_DXF_TRANSFORM> transforms;

public:
    ST_DXF_ENTITIE()
    {
        enEntitie = EN_DXF_ENTITIE_ARC;
        transforms.clear();
    }
};
Q_DECLARE_METATYPE(ST_DXF_ENTITIE)
struct ST_DXF_ENTITIES {
    QList<ST_DXF_ENTITIE>  listEntitie;
public:
    ST_DXF_ENTITIES()
    {
        listEntitie.clear();
    }
};
Q_DECLARE_METATYPE(ST_DXF_ENTITIES)
//---------------------------------------------------------------------------
struct ST_DXF_BLOCK {
    QString xref;
    QString name;
    qreal x;
    qreal y;
    ST_DXF_ENTITIES  stEntities;
public:
    ST_DXF_BLOCK()
    {
        xref = "";
        name = "";
        x = 0;
        y = 0;
    }
};
Q_DECLARE_METATYPE(ST_DXF_BLOCK)
struct ST_DXF_BLOCKS {
    QList<ST_DXF_BLOCK> listbook;
public:
    ST_DXF_BLOCKS()
    {
        listbook.clear();
    }
};
Q_DECLARE_METATYPE(ST_DXF_BLOCKS)
//---------------------------------------------------------------------------
struct ST_DXF_SECTIONS {
    ST_DXF_HEADER   stHeader;
    ST_DXF_TABLES   stTables;
    ST_DXF_BLOCKS   stBlocks;
    ST_DXF_ENTITIES   stEntities;
public:
    ST_DXF_SECTIONS()
    {

    }
};
Q_DECLARE_METATYPE(ST_DXF_SECTIONS)
//---------------------------------------------------------------------------

struct ST_DXF_POINT {
    qreal x;
    qreal y;

public:
    ST_DXF_POINT(qreal x = 0, qreal y = 0)
    {
        this->x = x;
        this->y = y;
    }
    void shift(qreal x, qreal y)
    {
        this->x += x;
        this->y += y;
    }
    void rotate(qreal angle)
    {
        qreal rotX = this->x * qCos(angle) - this->y * qSin(angle);
        qreal rotY = this->y * qCos(angle) + this->x * qSin(angle);
        this->x = rotX;
        this->y = rotY;
    }
    void scale(qreal xScale, qreal yScale)
    {
        this->x *= xScale;
        this->y *= yScale;
    }
};
Q_DECLARE_METATYPE(ST_DXF_POINT)
//---------------------------------------------------------------------------
struct ST_DXF_BOUNDINGBOX {
    ST_DXF_POINT min;
    ST_DXF_POINT max;
public:
    ST_DXF_BOUNDINGBOX()
    {
        max.x = -qInf();
        max.y = -qInf();
        min.x =  qInf();
        min.y =  qInf();
    }
    qreal width()
    {
        return max.x - min.x;
    }
    qreal height()
    {
        return max.y - min.y;
    }
    void expandByPoint(ST_DXF_POINT point)
    {
        max.x = qMax(max.x, point.x);
        max.y = qMax(max.y, point.y);
        min.x = qMin(min.x, point.x);
        min.y = qMin(min.y, point.y);
    }
};
Q_DECLARE_METATYPE(ST_DXF_BOUNDINGBOX)
//---------------------------------------------------------------------------

class DxfParser
{
    QTTEST_OBJECT

public:
    DxfParser();
    ~DxfParser();
    bool dxfToSvg(const QString &strDxfPath, QByteArray &strSvg);

protected:
    bool toLines(const QString &strDxfPath, QStringList* pLines);
    bool convertToTypesAndValues(const QStringList &lines, QList<ST_DXF_TYPE_A_VALUE>* pTypesAndValues);
    void separateSections(const QList<ST_DXF_TYPE_A_VALUE> &typesAndValues, QList<QList<ST_DXF_TYPE_A_VALUE>>* pSections);
    void reduceSection(const QList<QList<ST_DXF_TYPE_A_VALUE>> &sections, ST_DXF_SECTIONS* pstSections);
    void headerHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_HEADER* pstHeader);
    void tablesHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_TABLES* pstTables);
    void blocksHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_BLOCKS* pstBlocks);
    void entitiesHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIES* pstEntities);
    void layerTableHandler(const QList<ST_DXF_TYPE_A_VALUE> &layersTuples, QMap<QString, ST_DXF_TABLE_LAYER>* pMapLayers);
    void stylesTableHandler(const QList<ST_DXF_TYPE_A_VALUE> &stylesTuples, QMap<QString, ST_DXF_TABLE_STYLE>* pMapStyles);

    void commonHandler(const ST_DXF_TYPE_A_VALUE &tuple, ST_DXF_ENTITIE_COMMON* pstCommon);
    void arcHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void circleHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void ellipseHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void insertHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void lineHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void lwpolylineHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void mtextHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void pointHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void solidHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void splineHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void polylineHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie);
    void vertexHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE_VERTEX* pstVertex);

    QList<ST_DXF_ENTITIE> denormalise(const QList<ST_DXF_ENTITIE> &entities, const QList<ST_DXF_TRANSFORM> &transforms, const ST_DXF_BLOCKS &blocks);
    QList<ST_DXF_POINT>   interpolateElliptic(qreal cx, qreal cy, qreal rx, qreal ry, qreal start, qreal end, qreal rotationAngle);
    QList<ST_DXF_POINT>   createArcForLWPolyine(const ST_DXF_POINT from, const ST_DXF_POINT to, const qreal bulge);
    QList<ST_DXF_POINT>   entityToPolyline(const ST_DXF_ENTITIE &entitie);
    ST_DXF_POINT          bspline(qreal t, int order, QList<QPointF> points, QList<double> knots);

    void toSVG(const ST_DXF_SECTIONS &stSections, QByteArray &strSvg);

private:


};
Q_DECLARE_METATYPE(DxfParser)
//---------------------------------------------------------------------------
#endif // DXFPARSER_H
