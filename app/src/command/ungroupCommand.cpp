#include <QObject>
#include <QUndoCommand>

#include "ungroupCommand.h"
#include "ui/graphicArea/mainScene.h"
#include "graphics/graphicsFlexibleFrame.h"
#include "graphics/vectorImageItem.h"
#include "graphics/itemInfo.h"


UngroupCommand::UngroupCommand(MainScene* scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mLookingScene   = scene;
    mInstance       = DataManager::getInstance();
    mGroupInfo      = mLookingScene->groupInfo();
}

void UngroupCommand::undo()
{
    mLookingScene->setGroup(mGroupInfo);
    mLookingScene->toggleActionGroup();
}

void UngroupCommand::redo()
{
    mLookingScene->unsetGroup();
    mLookingScene->toggleActionGroup();
}

int  UngroupCommand::id() const
{
    return ID;
}
