#include "groupInfo.h"
#include "vectorImageItem.h"
#include "common/dataManager.h"

GroupInfo::GroupInfo(QList<GraphicsFlexibleFrame*> list)
{
    mCenterItem = NULL;
    mGroupFrame = new GraphicsFlexibleFrame(NULL);
    foreach (GraphicsFlexibleFrame* frame, list) {
        mStocksPrev.insert(frame, QPair<QRectF, qreal>(frame->currentRect(), frame->rectRotate()) );
        mFactorFrameList.append(frame);
        mCenterMap.insert(frame, frame->rectCenter());
    }
    mPastedCount = 0;
}

void GroupInfo::set()
{
    bool isFirst = false;
    if(mCenterItem == NULL) {
        mCenterItem = new QGraphicsItemGroup();
        isFirst = true;
    }

    foreach (GraphicsFlexibleFrame* frame, mFactorFrameList) {
        QPair<QRectF, qreal> data = mStocksPrev.value(frame);

        frame->setCurrentRect(data.first);
        frame->setRectRotate(data.second);
        frame->setAnchorCenter();
        frame->updateMatrix();

        if(isFirst == true) {
            QGraphicsItem* temp = DataManager::getInstance()->cloneCenterGraphics(frame);
            mCenterItem->addToGroup(temp);
        }
    }
    if(isFirst == true) {
        mGroupFrame->setCenteredItem(mCenterItem);
    }
}

void GroupInfo::apply()
{
    foreach(GraphicsFlexibleFrame* frame, mFactorFrameList) {

        QPointF newCenter   = mGroupFrame->calcNewCenter(mCenterMap.value(frame));
        qreal   scaleX      = mGroupFrame->rectScaleX();
        qreal   scaleY      = mGroupFrame->rectScaleY();
        qreal   addedAngle  = mGroupFrame->rectRotate();

        frame->applyGroupModify(newCenter, scaleX, scaleY, addedAngle);
        frame->updateMatrix();
    }
}

void GroupInfo::apply(GraphicsFlexibleFrame* srcFrame, GraphicsFlexibleFrame* distFrame)
{
    if(srcFrame  == NULL) { return ; }
    if(distFrame == NULL) { return ; }

    QPointF newCenter   = mGroupFrame->calcNewCenter(mCenterMap.value(srcFrame));
    qreal   scaleX      = mGroupFrame->rectScaleX();
    qreal   scaleY      = mGroupFrame->rectScaleY();
    qreal   addedAngle  = mGroupFrame->rectRotate();

    distFrame->applyGroupModify(newCenter, scaleX, scaleY, addedAngle);
    distFrame->updateMatrix();
}

void GroupInfo::show()
{
    foreach (GraphicsFlexibleFrame* frame, mFactorFrameList) {
        frame->hide();
    }
    mGroupFrame->show();
    mGroupFrame->setSelected(true);
}

void GroupInfo::hide()
{
    foreach(GraphicsFlexibleFrame* frame, mFactorFrameList) {
        frame->show();
        frame->setSelected(true);
    }
    mGroupFrame->hide();
}

bool GroupInfo::isShown()
{
    return mGroupFrame->isVisible();
}
