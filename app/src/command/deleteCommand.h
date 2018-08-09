#ifndef QDELETECOMMAND_H
#define QDELETECOMMAND_H

#include <QUndoCommand>

#include "common/common.h"
#include "ui/graphicArea/mainScene.h"
#include "graphics/itemInfo.h"
#include "common/dataManager.h"

// シーン上にある画像を削除するコマンド
// Redoするために削除したデータを保持しておく

class DeleteCommand : public QUndoCommand
{
    QTTEST_OBJECT
public:
    DeleteCommand(MainScene* scene, QUndoCommand *parent = 0);
    ~DeleteCommand() {;}

    void undo() override;
    void redo() override;

    enum { ID = 2 };
    int id() const Q_DECL_OVERRIDE;

private:
    DataManager*       mInstance;
    QList<ItemInfo*>   mStocks;
    MainScene*         mLookingScene;
    GroupInfo*         mGroupInfo;
};
#endif // QDELETECOMMAND_H
