#ifndef PARAMWIDGET_H
#define PARAMWIDGET_H

// 画像が持つパス情報を表示するWidget

#include <QWidget>

#include "common/common.h"
#include "graphics/itemInfo.h"
#include "paramRow.h"

namespace Ui
{
class ParamWidget;
}

class ParamWidget : public QWidget
{
    Q_OBJECT
    QTTEST_OBJECT
public:
    explicit ParamWidget(ItemInfo* item, QWidget *parent = 0);
    ~ParamWidget();

    QString itemName() { return mLookingItem->formalName(); }

    int     setBackParam();
    void    updateParameter();

public:
    static int mColorIndx;

signals:
    void    shapeChanged();

public slots:
    void    toggleAction();
    void    moveRow(int prev, int curr);

private:
    QString abridgementStr(QString target, QFont font, qreal threshold);
    void    widgetCreate();
    void    setRowData(ParamRow* rowData);
    void    updatePosition();

private:
    Ui::ParamWidget *ui;

    ItemInfo* mLookingItem;

    QList<ParamRow*> mRowList;

};


#endif // PARAMWIDGET_H
