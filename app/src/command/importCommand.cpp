#include <QObject>
#include <QFileDialog>
#include <QUndoCommand>

#include "importCommand.h"
#include "ui/graphicArea/mainScene.h"
#include "graphics/vectorImageItem.h"
#include "graphics/itemInfo.h"


ImportCommand::ImportCommand(MainScene* scene, QList<ItemInfo*> imgList, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mLookingScene   = scene;
    mInstance       = DataManager::getInstance();

    mStocks = imgList;
}

void ImportCommand::undo()
{
    ItemInfo* indexedItem = NULL;
    mLookingScene->unselect();

    for(int index = 0; index < mStocks.count(); index++) {
        indexedItem = mStocks.at(index);
        indexedItem->removeItemFromScene(mLookingScene);
    }
    mInstance->removeItems(mStocks);
}

void ImportCommand::redo()
{
    ItemInfo* indexedItem = NULL;
    mLookingScene->unselect();

    for(int index = 0; index < mStocks.count(); index++) {
        indexedItem = mStocks.at(index);
        indexedItem->setItemToScene(mLookingScene);
    }
    mInstance->appendItems(mStocks);
}

int  ImportCommand::id() const
{
    return ID;
}
