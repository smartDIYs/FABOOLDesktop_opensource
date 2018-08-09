#ifndef UNGROUPCOMMAND_H
#define UNGROUPCOMMAND_H


#include <QUndoCommand>

#include "common/common.h"
#include "common/dataManager.h"
#include "ui/graphicArea/mainScene.h"
#include "graphics/itemInfo.h"
#include "graphics/groupInfo.h"

class UngroupCommand : public QUndoCommand
{
    QTTEST_OBJECT
public:
    UngroupCommand(MainScene* scene, QUndoCommand *parent = 0);
    ~UngroupCommand() {;}

    void undo() override;
    void redo() override;

    enum { ID = 6 };
    int id() const Q_DECL_OVERRIDE;

private:
    DataManager*    mInstance;
    MainScene*      mLookingScene;
    GroupInfo*      mGroupInfo;
};
#endif // GROUPCOMMAND_H
