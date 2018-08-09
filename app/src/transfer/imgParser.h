//---------------------------------------------------------------------------
// 画像ファイル解析
//---------------------------------------------------------------------------
#ifndef IMGPASER_H
#define IMGPASER_H
//---------------------------------------------------------------------------
#include <QtCore>

#include "common/common.h"
#include "parserCore.h"
//---------------------------------------------------------------------------
class ImgParser : public ParserCore
{
    QTTEST_OBJECT

public:
    ImgParser(int rasterDealType = SYS_PARA_RASTER_DEAL_F_S_METHOD);
    ImgParser(const ImgParser &src);
    virtual ~ImgParser();
    ImgParser &operator=(const ImgParser &src);

    virtual QPointF getStartPoint() override;
    virtual bool loadFile(const QString &strSvgPath) override;
    virtual QByteArray toByte() override;
    virtual int  readData(const QJsonObject &json) override;
    virtual void writeDown(QJsonObject &json) const override;
    virtual QList<QColor> colorMap() override;
    virtual QStringList toGcode(qreal d1, qreal d2, qreal d3, qreal d4, qreal d5, qreal d6, QList<ParamData*> paramList) override;
    virtual QSizeF outerSize() override;
    virtual qreal dpi() override;

    enum { Type =  PARSER_TYPE_RASTER };
    int type() const
    {
        return Type;
    }
protected:
    void initMenber();
    void deleteMenber();
    ImgParser &copyMenber(const ImgParser &src);
    quint8 saturated_add(quint8 val1, int val2);
    QPointF applyTransform(const QPointF &p, qreal d1, qreal d2, qreal d3, qreal d4, qreal d5, qreal d6);
    void applyTransformWithOutPos(const qreal &sw, const qreal &sh, qreal *pdw, qreal *pdh, qreal d1, qreal d2, qreal d3, qreal d4);

    int  optimizedDivision();

    void froydSteinbergMethod();
    void binarization();
private:
    qreal   m_dDpi;
    qint32  m_ImgWidth;
    qint32  m_ImgHeight;

    quint8* m_pucImg;

    int     m_rasterDealType;
};
Q_DECLARE_METATYPE(ImgParser)
//---------------------------------------------------------------------------
#endif // IMGPASER_H
