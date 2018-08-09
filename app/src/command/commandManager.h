#ifndef QUNDOMANAGER_H
#define QUNDOMANAGER_H

#include <QObject>
#include <QUndoCommand>
#include <QUndoStack>

#include "common/common.h"
#include "ui/mainDialog.h"
#include "ui/graphicArea/mainScene.h"
#include "importCommand.h"
#include "deleteCommand.h"
#include "shapeChangeCommand.h"
#include "pasteCommand.h"
#include "groupCommand.h"
#include "ungroupCommand.h"

#include "graphics/itemInfo.h"

// RedoUndoを管理するクラス
// 画像のインポート、デリート、コピー＆ペースト、移動＆スケーリング＆回転 の4種類
// それぞれコマンドをクラス化してスタックに積んでいく

class CommandManager : public QObject
{
    Q_OBJECT
    QTTEST_OBJECT
public:
    CommandManager(MainScene* scene, QObject* parent = 0);
    ~CommandManager();

public slots:
    void importCommnad();       // インポートボタン押下時にコールバックされるスロット       インポート対象がストックされる
    void importCommnad(QStringList );       // インポートボタン押下時にコールバックされるスロット       インポート対象がストックされる
    void deleteCommand();       // デリートボタン or Ctrl+Del でコールバックされるスロット デリート対象をストックする
    void deleteAllCommand();
    void pasteCommnad();        // ペーストボタン or Ctrl+V   でコールバックされるスロット MainSceneが保持しているコピーリストにあるItemInfoをストックする
    void pasteCommnad(QPointF pos); // 右クリックメニューでのペースト　ペースト位置の反映考慮
    void shapeChangeCommand(bool isHead);  // Graphic操作でコールバックされるスロット
    void groupCommand();        //　アイテムのグループ化
    void ungroupCommand();      //　アイテムのグループ化解除

    void undo() { mUndoStack->undo(); emit update(); }   // Undoボタンを押すとスタックに積んだ一番最後のコマンドをUndoする
    void redo() { mUndoStack->redo(); emit update(); }

    void stackClear() { mUndoStack->clear(); }

signals:
    void update();  // UI を更新するためのシグナル MainDialogで受け取る
    void loadErr(const QMap<QString, QString>);

private:
    MainScene*     mLookingScene;
    QUndoStack*    mUndoStack;
    DataManager*   mInstance;

};
#endif // QUNDOMANAGER_H
