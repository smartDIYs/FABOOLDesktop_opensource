#include "customTableWidget.h"

#include <QtWidgets>

CustomTableWidget::CustomTableWidget(QWidget *parent):
    QTableWidget(parent)
{
    setAcceptDrops(true);
}

void CustomTableWidget::mousePressEvent(QMouseEvent* e)
{
    if(e->button() == Qt::LeftButton) {
        mDragedItemIndex = -1;

        QObject* object     = qApp->widgetAt(e->globalX(), e->globalY());
        QString  name       = object->objectName();
        QWidget* targetWid  = NULL;

        if( name == "PathColor" ) {
            object->installEventFilter(this);
            QModelIndex index = indexAt(e->pos());
            mDragedItemIndex = index.row();
            targetWid = (QWidget*)(object);

            QDrag* drag = new QDrag(targetWid);
            drag->setMimeData(new QMimeData);
            drag->exec(Qt::MoveAction);
            object->removeEventFilter(this);
        }
    }
}

void CustomTableWidget::dragEnterEvent(QDragEnterEvent* e)
{
    e->acceptProposedAction();
}

void CustomTableWidget::dragMoveEvent(QDragMoveEvent* e)
{
    // 宣言しておかないとドロップイベントが発生しない
}

void CustomTableWidget::dropEvent(QDropEvent* e)
{
    QObject* object = e->source();
    QPoint considerHeader = e->pos();
    if(horizontalHeader()->isVisible() == true) {
        considerHeader.setY(e->pos().y() + horizontalHeader()->height());
    }
    QPoint dropGpos = mapToGlobal(considerHeader);
    QObject *tgtObj = qApp->widgetAt(dropGpos.x(), dropGpos.y());

    if(object == NULL || tgtObj == NULL) {
        return;
    }

    QString  name     = object->objectName();
    QString  tgtName  = tgtObj->objectName();

    if(object == tgtObj) {
        return;
    }
    if(object->parent() != tgtObj->parent()) {
        return;
    }
    if( name != "PathColor" || tgtName != "PathColor") {
        return;
    }

    QModelIndex index = indexAt(e->pos());
    int newRow = index.row();

    emit moveRow(mDragedItemIndex, newRow);
}

bool CustomTableWidget::eventFilter(QObject *obj, QEvent *e)
{
    QEvent::Type type = e->type();

    if(type == QEvent::DragEnter) {
        return false;
    }
    return true;
}
