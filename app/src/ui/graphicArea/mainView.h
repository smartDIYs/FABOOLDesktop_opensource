#ifndef QMAINVIEW_H
#define QMAINVIEW_H

// Sceneの見える範囲を指定するクラス
//

#include <QObject>
#include <QLabel>
#include <QGraphicsView>
#include <QGraphicsScene>

#include "common/common.h"
#include "mainScene.h"

class MainView : public QGraphicsView
{
    Q_OBJECT
    QTTEST_OBJECT
public:
    MainView(QWidget *parent = 0);
    void setSceneAndRange(MainScene* scene);

protected:
    void wheelEvent(QWheelEvent* e);        // ViewScaling

public slots:

    void zoomIn();  // scale in
    void zoomOut(); // scale out
    void actualSize();
    void fitInViewRect();

private:
    QRectF   mViewRect;
    QRectF   mMovableRange;

    QLabel  mMaterialLabel;
};

#endif // QMAINVIEW_H
