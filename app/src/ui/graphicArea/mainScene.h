#ifndef QMAINSCENE_H
#define QMAINSCENE_H

// 画像を表示するキャンバスのようなもの
// SceneにGraphicsitemを追加すると画像が表示される
// 画像のドラッグや選択はここでイベント処理できる
// ドラッグ等で画像を動かすと、Itemが持っているPosition情報も書き換わる


#include <QObject>
#include <QGraphicsScene>
#include <QGraphicsItem>
#include <QMenu>

#include "common/common.h"
#include "graphics/itemInfo.h"
#include "graphics/groupInfo.h"

class GraphicsFlexibleFrame;
class GroupInfo;
class MainScene : public QGraphicsScene
{
    Q_OBJECT
    QTTEST_OBJECT
public:
// Qt Override
    MainScene(QObject *parent = 0);
    ~MainScene();

// Original
    void setMovableRange(QSize size);
    QGraphicsRectItem* movableRenge() { return mMovableRange; }

    void createGrid();          // 背景のグリッドの生成
    void destructGrid();        // 背景のグリッドの破棄
    void showGrid();            // 背景グリッドの表示
    void hideGrid();            // 背景グリッドの隠蔽
    void toggleDetailedLine();
    void showDetailGrid();
    void hideDetailGrid();

    void copyFlexFrame();                               // Ctrl+C で選択中のFlexibleFrameを覚える
    QList<GraphicsFlexibleFrame*> copyList();           // コピーで記憶したFlexibleFrame返す
    QList<GraphicsFlexibleFrame*> selectedFlexFrame();  // 選択中のFlexibleFrame返す
    QPointF                       selectedRectPos() { return mSelectedBoundPos; }
    void                          allSelect();
    void                          unselect();

    void setGroup(GroupInfo* inf); //　グループ化
    void unsetGroup();       //　グループ化解除
    GroupInfo* groupInfo() { return mGroupInfo; }

    void toggleActionPaste();   // 右クリックメニューのペーストのEnable切り替え
    void toggleActionGroup(bool unset = false);   // Group、UngroupのEnable状態のスイッチ

    bool hasGroup();

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* e);      // マウスが押された瞬間に発行するイベント
    void mouseMoveEvent(QGraphicsSceneMouseEvent* e);       // 押されているいないに関わらずマウスが移動すると発生するイベント
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* e);    // マウスが離された瞬間に発生するイベント
    void dragEnterEvent(QGraphicsSceneDragDropEvent* e);
    void dragMoveEvent(QGraphicsSceneDragDropEvent* e);
    void dropEvent(QGraphicsSceneDragDropEvent* e);
    void keyPressEvent(QKeyEvent* e);                       // 方向キー移動、複数選択モードOn　など

public slots:
    void emitPasteOnMenu();     // 右クリックメニューペースト時の仲介スロット
    void sendChanged();

signals:
    void modify(bool isHead);      // アイテム変更通知シグナル
    void grouped();
    void ungrouped();
    void copied();
    void pasted();
    void pastedOnMenu(QPointF pos);
    void deleted();
    void selectionChanged(QGraphicsItem* item);
    void dropFiles(QStringList);

private:
    QGraphicsRectItem*        mMovableRange;        // 可動エリア
    QList<QGraphicsLineItem*> mGridLine;            // グリッド
    QList<QGraphicsLineItem*> mDetailedGridLine;    // 詳細グリッド

    GroupInfo* mGroupInfo;

    QList<GraphicsFlexibleFrame* > mCopyList;               // コピーを実施したときに選択中だったFlexibleFrameを記憶しておく
    QPointF                        mSelectedBoundPos;

    QMenu    mMenu;         // 右クリックメニュー　以下アクション
    QAction* mActCopy;
    QAction* mActPaste;
    QAction* mActDelete;
    QAction* mActGroup;
    QAction* mActUngroup;

    QPointF  mClickedPos; // 右クリックメニューペースト時の位置反映のための変数

    bool    firstEmit;

};

#endif // QMAINSCENE_H
