#ifndef QSHAPECHANGECOMMAND_H
#define QSHAPECHANGECOMMAND_H

#include <QUndoCommand>

#include "common/common.h"
#include "common/dataManager.h"
#include "ui/graphicArea/mainScene.h"
#include "graphics/itemInfo.h"

class ShapeChangeCommand : public QUndoCommand
{
    QTTEST_OBJECT
public:
    ShapeChangeCommand(MainScene* scene, bool ishead, QUndoCommand *parent = 0);
    ~ShapeChangeCommand() {;}

    void undo() override;
    void redo() override;

    enum { ID = 4 };
    int  id() const Q_DECL_OVERRIDE;
    bool mergeWith(const QUndoCommand *other)Q_DECL_OVERRIDE;


private:
//    DataManager*       mInstance;
    QMap< GraphicsFlexibleFrame*, QPair<QRectF, qreal> >  mStocksPrev;
    QMap< GraphicsFlexibleFrame*, QPair<QRectF, qreal> >  mStocksCurr;
    bool mIsHead;
//    MainScene*         mLookingScene;


};
#endif // QSHAPECHANGECOMMAND_H
