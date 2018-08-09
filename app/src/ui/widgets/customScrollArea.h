#ifndef CUSTOMSCROLLAREA_H
#define CUSTOMSCROLLAREA_H

#include <QObject>
#include <QScrollArea>
#include <QVBoxLayout>

class CustomScrollArea : public QScrollArea
{
    Q_OBJECT
public:
    CustomScrollArea(QWidget* parent = 0);

signals:
    void shapeChanged();

public slots:
    void updateParam();

};

#endif // CUSTOMSCROLLAREA_H
