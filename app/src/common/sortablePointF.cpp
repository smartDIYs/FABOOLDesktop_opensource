#include "sortablePointF.h"

SortablePointF::SortablePointF():
    QPointF()
{

}

SortablePointF::SortablePointF(QPointF pos):
    QPointF(pos)
{

}

SortablePointF::SortablePointF(qreal x, qreal y):
    QPointF(x, y)
{

}

SortablePointF::~SortablePointF()
{

}

bool SortablePointF::compareX(const SortablePointF& d1, const SortablePointF& d2)
{
    return d1.x() < d2.x();
}

bool SortablePointF::compareY(const SortablePointF& d1, const SortablePointF& d2)
{
    return d1.y() < d2.y();
}

QPointF SortablePointF::pointF()
{
    return *this;
}
