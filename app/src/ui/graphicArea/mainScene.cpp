#include <QGraphicsScene>
#include <QGraphicsLineItem>
#include <QGraphicsSceneEvent>
#include <QPen>
#include <QCursor>
#include <QKeyEvent>
#include <QMenu>

#include "common/common.h"
#include "mainScene.h"
#include "graphics/graphicsFlexibleFrame.h"

#include "graphics/groupInfo.h"

MainScene::MainScene(QObject *parent): QGraphicsScene(parent)
{
    mMovableRange     = new QGraphicsRectItem(0, 0, 0, 0);
    mGroupInfo        = NULL;

// 右クリックメニュー作成
    mActCopy    = mMenu.addAction(tr("copy"));
    mActPaste   = mMenu.addAction(tr("paste"));
    mActPaste->setDisabled(true);
    mActDelete  = mMenu.addAction(tr("delete"));
    mMenu.addSeparator();
    mActGroup   = mMenu.addAction(tr("group"));
    mActUngroup = mMenu.addAction(tr("ungroup"));
    mActUngroup->setDisabled(true);

    connect(mActCopy,        SIGNAL(triggered()),    this,   SIGNAL(copied()));
    connect(mActPaste,       SIGNAL(triggered()),    this,   SLOT(emitPasteOnMenu()));
    connect(mActDelete,      SIGNAL(triggered()),    this,   SIGNAL(deleted()));
    connect(mActGroup,       SIGNAL(triggered()),    this,   SIGNAL(grouped()));
    connect(mActUngroup,     SIGNAL(triggered()),    this,   SIGNAL(ungrouped()));

    DEBUG_STRING("Scene Init Done.");
}

MainScene::~MainScene()
{
    delete mMovableRange;
    mMovableRange = NULL;

    destructGrid();
    clear();
    DEBUG_STRING("Scene Destruct Done.");
}

// 移動可能範囲の設定
void MainScene::setMovableRange(QSize workArea)
{
    mMovableRange->setRect(0, 0, workArea.width(), workArea.height());
    setSceneRect(0, 0, workArea.width(), workArea.height());
    DEBUG_STRING(QString("Set Scene Rect (W:%1 H:%2).").arg(workArea.width()).arg(workArea.height()));
}

// グリッド作成
void MainScene::createGrid()
{
// movable range
    mMovableRange->setPen(QPen(QBrush(Qt::red), 0.1, Qt::DotLine));
    mMovableRange->setBrush(QBrush(Qt::white));
    this->addItem(mMovableRange);

// guideLine
    QGraphicsLineItem* line = NULL;

    qint16 widthMax  = mMovableRange->rect().width();
    qint16 heightMax = mMovableRange->rect().height();
    QPen pen(QBrush(Qt::darkGray), 0.1);
    for(int posX = 0; posX <= widthMax; posX = posX + 50) {
        line = new QGraphicsLineItem(posX, 0, posX, heightMax);
        line->setPen(pen);
        this->mGridLine.append(line);
    }
    for(int posY = 0; posY <= heightMax; posY = posY + 50) {
        line = new QGraphicsLineItem(0, posY, widthMax, posY);
        line->setPen(pen);
        this->mGridLine.append(line);
    }

//DetailedLine
    pen.setBrush(QBrush(Qt::red));
    pen.setStyle(Qt::DashDotDotLine);
    for(int posX = 25; posX <= widthMax; posX = posX + 50) {
        line = new QGraphicsLineItem(posX, 0, posX, heightMax);
        line->setPen(pen);
        this->mDetailedGridLine.append(line);
    }
    for(int posY = 25; posY <= heightMax; posY = posY + 50) {
        line = new QGraphicsLineItem(0, posY, widthMax, posY);
        line->setPen(pen);
        this->mDetailedGridLine.append(line);
    }

    foreach(QGraphicsLineItem * line, mGridLine)this->addItem(line);
    //foreach(QGraphicsLineItem * line, mDetailedGridLine)this->addItem(line);
}

void MainScene::destructGrid()
{
    foreach (QGraphicsLineItem* line, mGridLine) {
        delete line;
    }
    mGridLine.clear();

    foreach (QGraphicsLineItem* line, mDetailedGridLine) {
        delete line;
    }
    mDetailedGridLine.clear();
}

// グリッド表示
void MainScene::showGrid()
{
    foreach(QGraphicsLineItem * line, mGridLine)line->setVisible(true);
}

// グリッド非表示
void MainScene::hideGrid()
{
    foreach(QGraphicsLineItem * line, mGridLine)line->setVisible(false);
}

// 詳細グリッドの表示切替
void MainScene::toggleDetailedLine()
{
    static bool detailShown = false;

    if(detailShown == false) {
        foreach(QGraphicsLineItem * line, mDetailedGridLine) this->addItem(line);
    } else {
        foreach(QGraphicsLineItem * line, mDetailedGridLine)this->removeItem(line);
    }
}

// 詳細グリッドの表示
void MainScene::showDetailGrid()
{
    foreach(QGraphicsLineItem * line, mDetailedGridLine) this->addItem(line);
}

// 詳細グリッドの非表示
void MainScene::hideDetailGrid()
{
    foreach(QGraphicsLineItem * line, mDetailedGridLine)this->removeItem(line);
}

// マウスプレスイベント
//　右クリック時はコマンドが選択できるメニューが出る
//　左クリック時は移動・スケーリング・回転のコマンドが走る
void MainScene::mousePressEvent(QGraphicsSceneMouseEvent* e)
{
    QGraphicsScene::mousePressEvent(e);

    if(e->button() == Qt::RightButton) {
        if(selectedFlexFrame().count() != 0) { // アイテム選択時右クリックではすべてのアクションを表示する
            mActCopy->setVisible(true);
            mActDelete->setVisible(true);
            mActGroup->setVisible(true);
            mActUngroup->setVisible(true);
            mMenu.exec(e->screenPos());
        } else {                                //　アイテム非選択時の右クリックではPasteのみ実行可能に
            mClickedPos = e->scenePos();
            mActCopy->setVisible(false);
            mActDelete->setVisible(false);
            mActGroup->setVisible(false);
            mActUngroup->setVisible(false);
            mMenu.exec(e->screenPos());
        }
    } else {                                    //　左クリック時はアイテムの選択や移動を検知する
        if(selectedFlexFrame().count() <= 0) { return; }
        firstEmit = false;
    }
}

// マウスムーブイベント　（MouseTracking解除状態だとPress中のみ本処理が走る）
//　右クリック時は無視
//　左クリック時は移動・スケーリング・回転のコマンドが走る
void MainScene::mouseMoveEvent(QGraphicsSceneMouseEvent* e)
{
    QGraphicsScene::mouseMoveEvent(e);

    if(e->button() == Qt::RightButton) { // 右クリックは無視
        return ;
    }

    if(selectedFlexFrame().count() <= 0) { return; }
    foreach (GraphicsFlexibleFrame* frame, selectedFlexFrame()) {
        frame->passCurAngle();
        frame->passCurRect();
    }

    if(firstEmit == false) {
        emit modify(true);
        firstEmit = true;
    } else {
        emit modify(false);
    }

}

// マウスリリースイベント
//　右クリック時は無視
//　左クリック時は移動・スケーリング・回転のコマンドが走る
void MainScene::mouseReleaseEvent(QGraphicsSceneMouseEvent* e)
{
    QGraphicsScene::mouseReleaseEvent(e);

    if(e->button() == Qt::RightButton) {    // 右クリックは無視
        return ;
    }
    if(selectedFlexFrame().count() <= 0) { return; }

    foreach (GraphicsFlexibleFrame* frame, selectedFlexFrame()) {
        frame->updateRectFit();
        frame->setPos(0, 0);
        frame->setAnchorCenter();
        frame->passCurAngle();
        frame->passCurRect();
    }
    emit modify(false);
}

// キープレスイベント
//　方向キーで選択中のアイテムを移動できる
//　Ctrlキー同時押しで移動量を減少
void MainScene::keyPressEvent(QKeyEvent *e)
{
    QList<GraphicsFlexibleFrame*> graphList = selectedFlexFrame();

    if(graphList.count() == 0) { e->ignore(); }

    qreal moveStep  = 5;
    qreal moveX     = 0;
    qreal moveY     = 0;
    bool  needUpdate = false;

    if(e->modifiers() == Qt::ControlModifier) {
        moveStep        = 1;
    }

    switch(e->key()) {
    case Qt::Key_Up:
        moveX = 0;
        moveY = -moveStep;
        needUpdate = true;
        break;
    case Qt::Key_Down:
        moveX = 0;
        moveY = moveStep;
        needUpdate = true;
        break;
    case Qt::Key_Right:
        moveX = moveStep;
        moveY = 0;
        needUpdate = true;
        break;
    case Qt::Key_Left:
        moveX = -moveStep;
        moveY = 0;
        needUpdate = true;
        break;
    }

    if(needUpdate == true) {
        foreach (GraphicsFlexibleFrame* item, graphList) {
            item->setRectPosX(item->rectPos().x() + moveX);
            item->setRectPosY(item->rectPos().y() + moveY);
            item->setAnchorCenter();
            item->updateMatrix();
        }
    }
}

//　コピー処理
//　選択中のFlexibleFrameをリストに記憶する
void MainScene::copyFlexFrame()
{
    if(selectedFlexFrame().count() != 0) {
        mCopyList.clear();
        mCopyList.append(selectedFlexFrame());
        QRectF boundRect;
        foreach (GraphicsFlexibleFrame* item, mCopyList) {
            boundRect = boundRect.united(item->currentRect());
        }
        mSelectedBoundPos = boundRect.topLeft();
        toggleActionPaste();    //　Pasteが実行可能になる
    }
}

//　コピーリストを返す
QList<GraphicsFlexibleFrame*> MainScene::copyList()
{
    return mCopyList;
}

//　選択中のアイテムのうちFlexibleFrameだけ抽出
QList<GraphicsFlexibleFrame*> MainScene::selectedFlexFrame()
{
    QList<GraphicsFlexibleFrame*> graphList;
    foreach(QGraphicsItem* item, this->selectedItems()) {
        if(item->type() == GraphicsFlexibleFrame::Type) {
            graphList.append((GraphicsFlexibleFrame*)item);
        }
    }

    return graphList;
}

void MainScene::allSelect()
{
    foreach(QGraphicsItem* item, this->items()) {
        if(item->type() == GraphicsFlexibleFrame::Type) {
            item->setSelected(true);
        }
    }
}

void MainScene::unselect()
{
    foreach(QGraphicsItem* item, this->items()) {
        if(item->type() == GraphicsFlexibleFrame::Type) {
            item->setSelected(false);
        }
    }
}


//
void MainScene::setGroup(GroupInfo* info)
{
    mGroupInfo = info;
    mGroupInfo->set();
    addItem(mGroupInfo->groupFrame());
    mGroupInfo->show();
}

//　グループ化解除
//　RedoUndoのため、GroupはDeleteしない
void MainScene::unsetGroup()
{
    if(mGroupInfo != NULL) {
        mGroupInfo->apply();
        removeItem(mGroupInfo->groupFrame());
        mGroupInfo->hide();
        mGroupInfo = NULL;
    }
}

//　ペーストの有効無効を切り替える
//　コピーされてないのにPasteはできないから
void MainScene::toggleActionPaste()
{
    if(mActPaste->isEnabled() == false)
    { mActPaste->setEnabled(true); }
}

//　グループ化の有効無効を切り替える
//　すでにグループ化されている場合はグループ化不可になっている
void MainScene::toggleActionGroup(bool unset)
{
    if(mGroupInfo == NULL) {
        mActGroup->setEnabled(true);
        mActUngroup->setEnabled(false);
    } else {
        mActGroup->setEnabled(false);
        mActUngroup->setEnabled(true);
    }
}

bool MainScene::hasGroup()
{
    if(mGroupInfo == NULL) {
        return false;
    } else {
        return true;
    }
}

//　クリックした位置にペーストするための踏台
void MainScene::emitPasteOnMenu()
{
    emit pastedOnMenu(mClickedPos);
}

void MainScene::sendChanged()
{
    if(selectedFlexFrame().count() == 0)
    { emit selectionChanged(NULL); }
    else
    { emit selectionChanged(selectedFlexFrame().at(0)); }
}

void MainScene::dragEnterEvent(QGraphicsSceneDragDropEvent * e)
{
    if(e->mimeData()->hasUrls()) {
        e->acceptProposedAction();
    }
}

void MainScene::dragMoveEvent(QGraphicsSceneDragDropEvent * e)
{

}

void MainScene::dropEvent(QGraphicsSceneDragDropEvent * e)
{
    QStringList fileList;
    foreach (const QUrl& url, e->mimeData()->urls()) {
        fileList.append( url.toLocalFile() );
    }

    emit dropFiles(fileList);
}

