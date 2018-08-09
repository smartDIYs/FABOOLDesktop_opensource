#ifndef CUSTOMTABLEWIDGET_H
#define CUSTOMTABLEWIDGET_H

#include <QObject>
#include <QWidget>
#include <QTableWidget>

#include "common/common.h"

class CustomTableWidget : public QTableWidget
{
    Q_OBJECT
    QTTEST_OBJECT
public:
    CustomTableWidget(QWidget *parent = 0);

protected:
    void mousePressEvent(QMouseEvent* e);

    void dragEnterEvent(QDragEnterEvent* e);
    void dragMoveEvent(QDragMoveEvent* e);
    void dropEvent(QDropEvent* e);

    bool eventFilter(QObject *, QEvent *);

signals:
    void moveRow(int oldRow, int newRow);

private:
    int mDragedItemIndex;
};

#endif // CUSTOMTABLEWIDGET_H
