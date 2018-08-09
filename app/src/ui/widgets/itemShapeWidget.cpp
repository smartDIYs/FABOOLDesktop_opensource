#include "itemShapeWidget.h"
#include "ui_itemShapeWidget.h"

ItemShapeWidget::ItemShapeWidget(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ItemShapeWidget)
{
    ui->setupUi(this);

    mSelectedFrame = NULL;
    ui->posXEdit->setValue(0);
    ui->posYEdit->setValue(0);
    ui->heightEdit->setValue(0);
    ui->widthEdit->setValue(0);
    ui->rotateEdit->setValue(0);

    connect(ui->actualFitButton, SIGNAL(clicked()), this, SLOT(actualSize()));
}

ItemShapeWidget::~ItemShapeWidget()
{
    delete ui;
}

//　Sceneの選択アイテムが変わった時の処理
//　Widgetの中身を書き換える
void ItemShapeWidget::selectChange(QGraphicsItem* newItem)
{
    if(newItem == NULL || newItem->type() != GraphicsFlexibleFrame::Type) {
        mSelectedFrame = NULL;
        connectOff();
        ui->posXEdit->setValue(0);
        ui->posYEdit->setValue(0);
        ui->heightEdit->setValue(0);
        ui->widthEdit->setValue(0);
        ui->rotateEdit->setValue(0);
    } else {
        mSelectedFrame = (GraphicsFlexibleFrame*)newItem;

        connectOff();
        ui->posXEdit->setValue(mSelectedFrame->rectPos().x());
        ui->posYEdit->setValue(mSelectedFrame->rectPos().y());
        ui->heightEdit->setValue(mSelectedFrame->rectWidth());
        ui->widthEdit->setValue(mSelectedFrame->rectHeight());
        ui->rotateEdit->setValue(mSelectedFrame->rectRotate());
        connectOn();

        // ラスタ画像の回転を防止
        if(mSelectedFrame->isRaster() == true) {
            ui->rotateEdit->setDisabled(true);
        } else {
            ui->rotateEdit->setEnabled(true);
        }
    }
}

//　UIと更新Slotを接続
void ItemShapeWidget::connectOn()
{
    connect(ui->posXEdit,       SIGNAL(editingFinished()), this, SLOT(updateShape()));
    connect(ui->posYEdit,       SIGNAL(editingFinished()), this, SLOT(updateShape()));
    connect(ui->widthEdit,      SIGNAL(editingFinished()), this, SLOT(updateShapeWidth()));
    connect(ui->heightEdit,     SIGNAL(editingFinished()), this, SLOT(updateShapeHeight()));
    connect(ui->rotateEdit,     SIGNAL(editingFinished()), this, SLOT(updateShape()));
}

//　UIと更新Slotを切断
void ItemShapeWidget::connectOff()
{
    disconnect(ui->posXEdit,       SIGNAL(editingFinished()), this, SLOT(updateShape()));
    disconnect(ui->posYEdit,       SIGNAL(editingFinished()), this, SLOT(updateShape()));
    disconnect(ui->widthEdit,      SIGNAL(editingFinished()), this, SLOT(updateShapeWidth()));
    disconnect(ui->heightEdit,     SIGNAL(editingFinished()), this, SLOT(updateShapeHeight()));
    disconnect(ui->rotateEdit,     SIGNAL(editingFinished()), this, SLOT(updateShape()));
}

//　アスペクト比を維持するかどうか
bool ItemShapeWidget::isAspectLocked()
{
    return ui->aspectLockButton->isChecked();
}

//　Editの表示内容を更新
//　更新することでSlotが呼ばれるのを防ぐために、一度Connectを切っておく
void ItemShapeWidget::updateEdit()
{
    if(mSelectedFrame == NULL ) { return; }
    connectOff();
    ui->posXEdit->setValue(mSelectedFrame->rectPos().x());
    ui->posYEdit->setValue(mSelectedFrame->rectPos().y());
    ui->rotateEdit->setValue(mSelectedFrame->rectRotate());
    ui->widthEdit->setValue(mSelectedFrame->rectWidth());
    ui->heightEdit->setValue(mSelectedFrame->rectHeight());
    connectOn();
}

//　位置、回転のEditが変更されたときの処理
//　選択中のアイテムへ情報を適用
void ItemShapeWidget::updateShape()
{
    if(mSelectedFrame == NULL ) { return; }
    connectOff();
    mSelectedFrame->passCurAngle();
    mSelectedFrame->passCurRect();
    mSelectedFrame->setRectPosX(ui->posXEdit->value());
    mSelectedFrame->setRectPosY(ui->posYEdit->value());
    mSelectedFrame->setRectRotate(ui->rotateEdit->value());
    emit shapeChanged(true);
    connectOn();
}

//　WidthのEditが変更されたときの処理
//　WidthとHeightはアスペクト比の維持の是非により挙動が変わるので別関数にする
void ItemShapeWidget::updateShapeWidth()
{
    if(mSelectedFrame == NULL ) { return; }
    connectOff();
    qreal aspectedScale = 1;
    if(isAspectLocked() == true) { aspectedScale = ui->widthEdit->value() / mSelectedFrame->rectWidth(); }
    mSelectedFrame->passCurAngle();
    mSelectedFrame->passCurRect();
    mSelectedFrame->setRectWidth(ui->widthEdit->value());
    mSelectedFrame->setRectHeight(ui->heightEdit->value()*aspectedScale);
    emit shapeChanged(true);
    connectOn();
}

//　HeightのEditが変更されたときの処理
//　WidthとHeightはアスペクト比の維持の是非により挙動が変わるので別関数にする
void ItemShapeWidget::updateShapeHeight()
{
    if(mSelectedFrame == NULL ) { return; }
    connectOff();
    qreal aspectedScale = 1;
    if(isAspectLocked() == true) { aspectedScale = ui->heightEdit->value() / mSelectedFrame->rectHeight(); }
    mSelectedFrame->passCurAngle();
    mSelectedFrame->passCurRect();
    mSelectedFrame->setRectWidth(ui->widthEdit->value()*aspectedScale);
    mSelectedFrame->setRectHeight(ui->heightEdit->value());
    emit shapeChanged(true);
    connectOn();
}

void ItemShapeWidget::actualSize()
{
    if(mSelectedFrame == NULL ) { return; }
    mSelectedFrame->actualFit();
    emit shapeChanged(true);
}
