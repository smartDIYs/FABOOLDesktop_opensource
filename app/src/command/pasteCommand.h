#ifndef QPASTECOMMAND_H
#define QPASTECOMMAND_H


// Pasteコマンド
// Ctrl+C で、選択状態のFlexibleFrameをSceneが記憶する
// Ctrl+V で、このオブジェクトのインスタンスが作られて、記憶していたFlexibleFrameからItemInfoを複製する

#include <QUndoCommand>

#include "common/common.h"
#include "common/dataManager.h"
#include "ui/graphicArea/mainScene.h"
#include "graphics/itemInfo.h"

class PasteCommand : public QUndoCommand
{
    QTTEST_OBJECT
public:
    PasteCommand(MainScene* scene, QUndoCommand *parent = 0);
    PasteCommand(MainScene* scene, QPointF pos, QUndoCommand *parent = 0);
    ~PasteCommand() {;}

    void undo() override;
    void redo() override;

    enum { ID = 3 };
    int id() const Q_DECL_OVERRIDE;

private:
    DataManager*       mInstance;
    QList<ItemInfo*>   mStocks;
    MainScene*         mLookingScene;
};
#endif // QPASTECOMMAND_H
