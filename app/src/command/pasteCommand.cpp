#include "pasteCommand.h"
#include "graphics/graphicsFlexibleFrame.h"

PasteCommand::PasteCommand(MainScene* scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mLookingScene   = scene;
    mInstance       = DataManager::getInstance();

    QList<GraphicsFlexibleFrame*> list = mLookingScene->copyList();
    foreach(GraphicsFlexibleFrame* item, list) {
        if(item->centeredItem()->type() == QGraphicsItemGroup::Type) {
            GroupInfo* group = mLookingScene->groupInfo();
            group->pastedCountUp();
            mStocks.append(mInstance->loadImage(group, QPointF(-1, -1)));
            continue;
        }
        EN_LOAD_STATE retLoad = EN_LOAD_UNLOADED;
        ItemInfo* info = mInstance->loadImage(item->parentInfo(), &retLoad);
        if(retLoad == EN_LOAD_SUCCESS) {
            mStocks.append(info);
        }  //ItemInfoのコピーコンストラクタ リネーム機能付き
    }
}

PasteCommand::PasteCommand(MainScene* scene, QPointF pos, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mLookingScene = scene;
    mInstance     = DataManager::getInstance();

    QList<GraphicsFlexibleFrame*> list = mLookingScene->copyList();
    QPointF boundPos = mLookingScene->selectedRectPos();

    foreach(GraphicsFlexibleFrame* item, list) {
        if(item->centeredItem()->type() == QGraphicsItemGroup::Type) {
            mStocks.append(mInstance->loadImage(mLookingScene->groupInfo(), pos - boundPos));
            continue;
        }
        EN_LOAD_STATE retLoad = EN_LOAD_UNLOADED;
        ItemInfo* info = mInstance->loadImage(item->parentInfo(), &retLoad);
        if(retLoad == EN_LOAD_SUCCESS) {
            QPointF diff =  boundPos - item->rectPos();
            info->setPosX(pos.x() - diff.x());
            info->setPosY(pos.y() - diff.y());
            mStocks.append(info);  //ItemInfoのコピーコンストラクタ リネーム機能付き
        }
    }
}

void PasteCommand::undo()
{
    mLookingScene->unselect();
    ItemInfo* indexedItem = NULL;

    for(int index = 0; index < mStocks.count(); index++) {
        indexedItem = mStocks.at(index);
        indexedItem->removeItemFromScene(mLookingScene);
    }
    mInstance->removeItems(mStocks);
}

void PasteCommand::redo()
{
    mLookingScene->unselect();
    ItemInfo* indexedItem = NULL;

    for(int index = 0; index < mStocks.count(); index++) {
        indexedItem = mStocks.at(index);
        indexedItem->setItemToScene(mLookingScene);
    }
    mInstance->appendItems(mStocks);
}

int  PasteCommand::id() const
{
    return ID;
}
