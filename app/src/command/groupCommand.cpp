#include <QObject>
#include <QUndoCommand>

#include "groupCommand.h"
#include "ui/graphicArea/mainScene.h"
#include "graphics/graphicsFlexibleFrame.h"
#include "graphics/vectorImageItem.h"
#include "graphics/itemInfo.h"


GroupCommand::GroupCommand(MainScene* scene, QUndoCommand *parent)
    : QUndoCommand(parent)
{
    mLookingScene   = scene;
    mInstance       = DataManager::getInstance();
    mGroupInfo      = new GroupInfo(mLookingScene->selectedFlexFrame());
}

void GroupCommand::undo()
{
    mLookingScene->unsetGroup();
    mLookingScene->toggleActionGroup();
}

void GroupCommand::redo()
{
    mLookingScene->setGroup(mGroupInfo);
    mLookingScene->toggleActionGroup();
}

int  GroupCommand::id() const
{
    return ID;
}
