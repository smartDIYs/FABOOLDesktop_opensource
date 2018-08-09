#include <QtSvg>

#include "vectorImageItem.h"
#include "itemInfo.h"
#include "transfer/unitTransfer.h"

VectorImageItem::VectorImageItem(ParserCore* parser):
    QGraphicsSvgItem(NULL)
{
    setFlags(ItemIsSelectable | ItemIsMovable | this->flags());
    mRenderer = new QSvgRenderer(parser->toByte());
    setSharedRenderer(mRenderer); // !!! ここでboundingRectがintに丸められてしまう

    QSizeF size = parser->outerSize();
    qreal dpi  = parser->dpi();
    mmW = UnitTransfer::SvgPixelto(size.width(),  EN_SVG_UNIT_MM, dpi);
    mmH = UnitTransfer::SvgPixelto(size.height(), EN_SVG_UNIT_MM, dpi);

    qreal factor = mmW / size.width();
    setScale(factor);
}

QSizeF VectorImageItem::getSizeInMM()
{
    return QSizeF(mmW, mmH);
}

VectorImageItem::~VectorImageItem()
{
    delete mRenderer;
}

void VectorImageItem::applyParser(ParserCore* parser)
{
    mRenderer->load(parser->toByte());
    setSharedRenderer(mRenderer);
}
