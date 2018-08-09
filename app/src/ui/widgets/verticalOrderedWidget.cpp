#include "verticalOrderedWidget.h"
#include "common/common.h"

VerticalOrderedWidget::VerticalOrderedWidget(QWidget *parent) : QWidget(parent)
{
    mVLayout.setObjectName("paramLayout");
    mVLayout.setSizeConstraint(QLayout::SetFixedSize);
    mVLayout.setSpacing(10);
    mVLayout.setMargin(5);
    setLayout(&mVLayout);
    setAcceptDrops(true);

    mDragedItemIndex = -1;
}

void VerticalOrderedWidget::mousePressEvent(QMouseEvent* e)
{
    if(e->button() == Qt::LeftButton) {
        mDragedItemIndex = -1;

        QObject* object     = qApp->widgetAt(e->globalX(), e->globalY());
        QString  name       = object->objectName();
        QString  parentName = object->parent()->objectName();
        QWidget* targetWid  = NULL;

        if( name == "ImageNameTitle" ) {
            targetWid = (QWidget*)(object->parent());
        } else if(parentName == "ImageNameTitle" ) {
            targetWid = (QWidget*)(object->parent()->parent());
        } else
        { return; }

        mDragedItemIndex = mVLayout.indexOf(targetWid);

        if(mDragedItemIndex >= 0) {
            QDrag* drag = new QDrag(targetWid);
            drag->setMimeData(new QMimeData);
            drag->exec(Qt::MoveAction);
        }
    }
}

void VerticalOrderedWidget::dragEnterEvent(QDragEnterEvent* e)
{
    e->acceptProposedAction();
}

void VerticalOrderedWidget::dropEvent(QDropEvent* e)
{
    if(mDragedItemIndex < 0) { return; }

    QObject* object = e->source();
    QString  name       = object->objectName();
    QWidget* targetWid  = NULL;

    if( name != "ParamWidget") {
        return;
    }

    QPoint dropGpos = mapToGlobal(e->pos());
    QObject *tgtObj = qApp->widgetAt(dropGpos.x(), dropGpos.y());

    if (object == tgtObj)
    { return; }

    if(tgtObj->objectName() == "ImageNameTitle") {
        targetWid = (QWidget*)(tgtObj->parent());
    } else if(tgtObj->parent()->objectName() == "ImageNameTitle") {
        targetWid = (QWidget*)(tgtObj->parent()->parent());
    } else
    { return; }

    int index = mVLayout.indexOf(targetWid);
    QLayoutItem* temp = mVLayout.takeAt(mDragedItemIndex);
    mVLayout.insertItem(index, temp);

    emit changeOrder(mDragedItemIndex, index);
}

void VerticalOrderedWidget::addWidget(QWidget* widget)
{
    mVLayout.addWidget(widget, 0, Qt::AlignTop);
}
