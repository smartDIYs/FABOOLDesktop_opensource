#ifndef QIMPORTCOMMAND_H
#define QIMPORTCOMMAND_H

#include <QUndoCommand>

#include "common/common.h"
#include "common/dataManager.h"
#include "ui/graphicArea/mainScene.h"
#include "graphics/itemInfo.h"


// 画像をScene上へImportするコマンド
// コマンドのスタックの上限は決まっているので、破棄されたときにデータが消えないように
// 画像アイテムの生成はDataManagerが行う

class ImportCommand : public QUndoCommand
{
    QTTEST_OBJECT
public:
    ImportCommand(MainScene* scene, QList<ItemInfo*> imgList, QUndoCommand *parent = 0);
    ~ImportCommand() {;}

    void undo() override;
    void redo() override;

    enum { ID = 1 };
    int id() const Q_DECL_OVERRIDE;

private:
    DataManager*       mInstance;
    QList<ItemInfo*>   mStocks;
    MainScene*         mLookingScene;
};
#endif // QIMPORTCOMMAND_H
