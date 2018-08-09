#ifndef VERTICALORDEREDWIDGET_H
#define VERTICALORDEREDWIDGET_H

#include <QObject>
#include <QtWidgets>
#include <QtGui>

#include "paramWidget.h"


class VerticalOrderedWidget : public QWidget
{
    Q_OBJECT
public:
    explicit VerticalOrderedWidget(QWidget *parent = 0);

    void addWidget(QWidget* widget);

protected:
    void mousePressEvent(QMouseEvent* e);

    void dragEnterEvent(QDragEnterEvent* e);
    void dropEvent(QDropEvent* e);

signals:
    void changeOrder(int prev, int curr);

private:
    QVBoxLayout mVLayout;

    int mDragedItemIndex;
};

#endif // VERTICALORDEREDWIDGET_H
