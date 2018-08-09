#ifndef ITEMSHAPEWIDGET_H
#define ITEMSHAPEWIDGET_H

#include <QWidget>
#include <QGraphicsItem>

#include "common/common.h"
#include "graphics/graphicsFlexibleFrame.h"

// 選択中のアイテムのサイズや位置、回転を数字でコントロールするためのWidget
//

namespace Ui
{
class ItemShapeWidget;
}

class ItemShapeWidget : public QWidget
{
    Q_OBJECT
    QTTEST_OBJECT
public:
    explicit ItemShapeWidget(QWidget *parent = 0);
    ~ItemShapeWidget();

    void connectOn();       //　EditUIと更新関数を接続する
    void connectOff();      //　EditUIと更新関数を切断する
    bool isAspectLocked();  //　拡大時のアスペクト比を維持するか

public slots:
    void selectChange(QGraphicsItem* newItem);  //　アイテムの選択状態が変わった場合に表示内容を切り替える処理
    void updateEdit();                          //　WidgetのEditUIを更新する処理
    void updateShape();                         //　EditUIの情報をScene上のアイテムに適用する処理
    void updateShapeWidth();                    //　WidthのEditUIの内容をアイテムに適用する処理
    void updateShapeHeight();                   //　HeightののEditUIの内容をアイテムに適用する処理
    void actualSize();                          // 画像を本来のサイズに修正

signals:
    void shapeChanged(bool isHead);


private:
    Ui::ItemShapeWidget *ui;

    GraphicsFlexibleFrame* mSelectedFrame;


};

#endif // ITEMSHAPEWIDGET_H
