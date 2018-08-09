
#include <QUndoCommand>

#include "shapeChangeCommand.h"
#include "ui/graphicArea/mainScene.h"
#include "graphics/graphicsFlexibleFrame.h"
#include "graphics/vectorImageItem.h"
#include "graphics/itemInfo.h"


ShapeChangeCommand::ShapeChangeCommand(MainScene* scene, bool ishead, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    QList<GraphicsFlexibleFrame*> list = scene->selectedFlexFrame();
    foreach(GraphicsFlexibleFrame* item, list) {
        mStocksPrev.insert(item, QPair<QRectF, qreal>(item->previousRect(), item->previousAngle()));
        mStocksCurr.insert(item, QPair<QRectF, qreal>(item->currentRect(), item->rectRotate()));
    }
    mIsHead = ishead;
}

void ShapeChangeCommand::undo()
{
    foreach (GraphicsFlexibleFrame* frame, mStocksPrev.keys()) {
        frame->setCurrentRect(mStocksPrev.value(frame).first);
        frame->setRectRotate(mStocksPrev.value(frame).second);
        frame->setAnchorCenter();
        frame->updateMatrix();

    }
}

void ShapeChangeCommand::redo()
{
    foreach (GraphicsFlexibleFrame* frame, mStocksCurr.keys()) {
        frame->setCurrentRect(mStocksCurr.value(frame).first);
        frame->setRectRotate(mStocksCurr.value(frame).second);
        if(mIsHead == true) { frame->setAnchorCenter(); } //軸の設定は初回だけ
        frame->updateMatrix();
    }
}

int  ShapeChangeCommand::id() const
{
    return ID;
}

bool ShapeChangeCommand::mergeWith(const QUndoCommand* other)
{
    if(other->id() != id() || static_cast<const ShapeChangeCommand*>(other)->mIsHead == true) {
        return false;
    }

    mStocksCurr = static_cast<const ShapeChangeCommand*>(other)->mStocksCurr;

    return true;
}
