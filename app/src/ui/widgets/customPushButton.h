#ifndef CUSTOMPUSHBUTTON_H
#define CUSTOMPUSHBUTTON_H

#include <QObject>
#include <QPushButton>

#include "common/common.h"

//　カスタムボタン
//　ツールチップをボタンの真下に表示するために作った
//　イベントをOverrideしてツールチップ表示イベントならボタンの下に配置する

class CustomPushButton : public QPushButton
{
    QTTEST_OBJECT
public:
    CustomPushButton(QWidget* parent = 0);
    ~CustomPushButton() {;}

protected:
    bool event(QEvent *e) override;

    void mouseMoveEvent(QMouseEvent* e) override;

};

#endif // CUSTOMPUSHBUTTON_H
