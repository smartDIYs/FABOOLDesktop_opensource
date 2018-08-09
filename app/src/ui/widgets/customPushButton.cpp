#include <QtCore>
#include <QtGui>
#include <QtWidgets>

#include "customPushButton.h"


CustomPushButton::CustomPushButton(QWidget* parent):
    QPushButton(parent)
{
    setMouseTracking(true);
}

bool CustomPushButton::event(QEvent *e)
{
    if(e->type() == QEvent::ToolTip) {
        return false;
    }
    return QPushButton::event(e);
}

void CustomPushButton::mouseMoveEvent(QMouseEvent *e)
{
    QPushButton::mouseMoveEvent(e);
    QToolTip::showText(mapToGlobal(rect().bottomLeft() + QPoint(0, -10)), this->toolTip() + " " + this->shortcut().toString(QKeySequence::NativeText));
}
