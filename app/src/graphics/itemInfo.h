#ifndef QBASEITEM_H
#define QBASEITEM_H

// プロジェクトファイルが管理する、画像1枚ごとの情報クラス
// Sceneに張り付けるGraphicsitemのほかに、パスの情報や画像名を保持する
// original クラス

#include <QObject>
#include <QGraphicsItem>

#include "common/common.h"
#include "graphics/paramData.h"
#include "transfer/parserCore.h"
#include "ui/listView/libraryListedData.h"

enum EN_LOAD_STATE {
    EN_LOAD_SUCCESS = 0,
    EN_LOAD_FAILED,
    EM_LOAD_JSON_FAILED,
    EN_LOAD_UNSUPPORTED,
    EN_LOAD_UNLOADED,
    EN_LOAD_TOO_SMALL
};

class GraphicsFlexibleFrame;

class ItemInfo
{
    QTTEST_OBJECT
public:
// constructor & destructor
    ItemInfo();
    ~ItemInfo();

// setup
    EN_LOAD_STATE setup(const QString& path, const ParamData& data);
    EN_LOAD_STATE setup(const ItemInfo& srcItem);
    EN_LOAD_STATE setup(const LibraryListedData& core, const ParamData& data);
    EN_LOAD_STATE setup(const QJsonObject &json);
    int setupBrowserData(const QJsonObject &paths, const QJsonObject &canvas);

// scene
    void   setItemToScene(QGraphicsScene* scene);
    void   removeItemFromScene(QGraphicsScene* scene);
    bool   inWorkArea(QRectF workArea);
    void   applyProcessRange(QRectF* amountRect);

//  property
    QString formalName() const { return mItemName + "." + mExtension; }
    QString itemName() const { return mItemName; }
    QString extension() const { return mExtension; }
    QString itemPath() const { return mPath; }
    void    setCheckedCopied(QString name, int count);
    void    setCopiedPos(int count);

    qreal  posX()   const;
    qreal  posY()   const;
    qreal  rotate() const;
    qreal  width()  const;
    qreal  height() const;
    void   setPosX(qreal x);
    void   setPosY(qreal y);
    void   setRotate(qreal r);
    void   setWidth(qreal w);
    void   setHeight(qreal h);

    void    switchParam(int oldIndx, int newIndx);
    QList<ParamData*>   parameters()  { return mPathParam ; }

    bool    isRaster();
    GraphicsFlexibleFrame* item() {return mItem;}

// state
    static QString  getLoadStateStr(EN_LOAD_STATE);
    bool            isSkipData();

// parser control
    void   writeDown(QJsonObject &json) const;

// gcode
    void        transformToGcode();
    QStringList getGcode()   { return mGcode; }
    int         neededTime() { return mParsedData->neededTime(); }

private:
    EN_LOAD_STATE  createFrame(QString path);
    EN_LOAD_STATE  createFrame(QJsonObject );
    EN_LOAD_STATE  createFrame(ParserCore* );//ライブラリ用

    void        createParam(const ParamData& data);
    void        update();
    QJsonObject transferImgToCurrent(const QJsonObject& json);
    QJsonObject transferSvgToCurrent(const QJsonObject& json);

    void        fixSizeAndPos(QRectF worksize);

private:
    QList<ParamData*>      mPathParam;    // 詳細なパス設定
    GraphicsFlexibleFrame* mItem;
    QString                mItemName;
    QString                mPath;
    QString                mExtension;
    int                    mCopiedCount;
    ParserCore*            mParsedData;

    QStringList            mGcode;

};

#endif // QBASEITEM_H
