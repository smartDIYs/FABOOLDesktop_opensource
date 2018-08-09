#ifndef QVECTORPICTUREITEM_H
#define QVECTORPICTUREITEM_H

// ベクタ画像クラス

#include <QObject>
#include <QtSvg/QGraphicsSvgItem>

#include "common/common.h"
#include "transfer/parserCore.h"

class VectorImageItem : public QGraphicsSvgItem
{
    QTTEST_OBJECT
public:
    VectorImageItem(ParserCore* parser);
    ~VectorImageItem();

    enum { Type = UserType + 2 };
    int type() const
    {
        return Type;
    }
    QSizeF getSizeInMM();

    void applyParser(ParserCore* parser);

private:
    QSvgRenderer* mRenderer;
    qreal mmW;
    qreal mmH;
};
#endif // QVECTORPICTUREITEM_H
