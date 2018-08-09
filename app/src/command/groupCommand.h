#ifndef GROUPCOMMAND_H
#define GROUPCOMMAND_H


#include <QUndoCommand>

#include "ui/graphicArea/mainScene.h"
#include "graphics/itemInfo.h"
#include "common/dataManager.h"
#include "graphics/groupInfo.h"

class GroupCommand : public QUndoCommand
{
    QTTEST_OBJECT
public:
    GroupCommand(MainScene* scene, QUndoCommand *parent = 0);
    ~GroupCommand() {;}

    void undo() override;
    void redo() override;

    enum { ID = 5 };
    int id() const Q_DECL_OVERRIDE;

private:
    DataManager*    mInstance;
    MainScene*      mLookingScene;
    GroupInfo*      mGroupInfo;
};
#endif // GROUPCOMMAND_H
