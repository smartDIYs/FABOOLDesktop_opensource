#ifndef SORTABLEPOINTF_H
#define SORTABLEPOINTF_H

#include <QPointF>

class SortablePointF : public QPointF
{
public:
    SortablePointF();
    SortablePointF(QPointF pos);
    SortablePointF(qreal x, qreal y);
    ~SortablePointF();
    static bool compareX(const SortablePointF& d1, const SortablePointF& d2);
    static bool compareY(const SortablePointF& d1, const SortablePointF& d2);

    QPointF pointF();
};

#endif // SORTABLEPOINTF_H
