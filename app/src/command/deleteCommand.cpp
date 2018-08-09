#include <QObject>
#include <QUndoCommand>

#include "deleteCommand.h"
#include "ui/graphicArea/mainScene.h"
#include "graphics/graphicsFlexibleFrame.h"
#include "graphics/vectorImageItem.h"
#include "graphics/itemInfo.h"

DeleteCommand::DeleteCommand(MainScene* scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mLookingScene   = scene;
    mInstance       = DataManager::getInstance();
    QList<GraphicsFlexibleFrame*> list;

    mGroupInfo = NULL;
    foreach (GraphicsFlexibleFrame* frame, mLookingScene->selectedFlexFrame()) {
        if(frame->centeredItem()->type() == QGraphicsItemGroup::Type) {
            mGroupInfo = mLookingScene->groupInfo();
            list += mGroupInfo->factorFrameList();
        } else {
            list.append(frame);
        }
    }
    foreach(GraphicsFlexibleFrame* item, list) {
        mStocks.append(item->parentInfo());
    }
}

void DeleteCommand::undo()
{
    ItemInfo* indexedItem = NULL;

    for(int index = 0; index < mStocks.count(); index++) {
        indexedItem = mStocks.at(index);
        indexedItem->setItemToScene(mLookingScene);
    }
    mInstance->appendItems(mStocks);
    if(mGroupInfo != NULL) {
        mLookingScene->setGroup(mGroupInfo);
        mLookingScene->toggleActionGroup();
    }
}

void DeleteCommand::redo()
{
    ItemInfo* indexedItem = NULL;

    if(mGroupInfo != NULL) {
        mLookingScene->unsetGroup();
        mLookingScene->toggleActionGroup();
    }

    for(int index = 0; index < mStocks.count(); index++) {
        indexedItem = mStocks.at(index);
        indexedItem->removeItemFromScene(mLookingScene);
    }
    mInstance->removeItems(mStocks);

}

int  DeleteCommand::id() const
{
    return ID;
}
