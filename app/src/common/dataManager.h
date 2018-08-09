#ifndef QDATAMANAGER_H
#define QDATAMANAGER_H


// プロジェクトファイルを管理するクラス
// シングルトン設計
// GetInstanceを呼び出すことで唯一のデータにアクセスできる
// コマンドがアイテム読み出しや削除の発火を行うが、生成自体はDataManagerが行う

#include <QObject>
#include <QJsonObject>
#include <QGraphicsSvgItem>

#include "common.h"
#include "graphics/itemInfo.h"
#include "graphics/groupInfo.h"
#include "ui/listView/libraryListedData.h"
#include "ui/listView/materialListedData.h"

struct ST_MATERIAL_DATA {
    QString     name;
    int         thick;
    ParamData   engraveParam;
    ParamData   cutParam;
};

Q_DECLARE_METATYPE(ST_MATERIAL_DATA)

class DataManager
{
    QTTEST_OBJECT
// Original
public:
    static DataManager* getInstance()
    {
        static DataManager* dataManager = NULL;
        if(dataManager == NULL) { dataManager = new DataManager(); }
        return dataManager;
    }

    void                dataClear();
    void                appendItems(QList<ItemInfo*> items);    // 内部に持つ画像アイテムのリストに追加
    void                appendItem(ItemInfo* item);             //
    void                removeItems(QList<ItemInfo*> items);    // 内部に持つ画像アイテムをリストから削除

    QList<ItemInfo*>    loadImage(const QStringList&, QMap<QString, EN_LOAD_STATE>* ) ;   // ファイル名のリストをもとに画像アイテムをロードする
    ItemInfo*           loadImage(const QString& targetName, EN_LOAD_STATE* result) ;    // ファイル名1つをもとに画像アイテムロードする
    ItemInfo*           loadImage(ItemInfo* item, EN_LOAD_STATE* result) ;          // 画像アイテムをもとに画像アイテムを複製
    ItemInfo*           loadImage(LibraryListedData* libData, EN_LOAD_STATE* result) ;  // ライブラリからロードしたデータの生成
    QList<ItemInfo*>    loadImage(GroupInfo* info, QPointF boundPos) ;

    QList<QString>      imageNameList();
    ItemInfo*           getImageWithFormalName(QString imageName);

    QString             projectName();
    QList<ItemInfo*>    ImageItem();

    int                 saveProject();
    int                 saveProject(QString newPath);
    int                 loadProject(QString path);
    int                 loadBrowserProject(QString path);

    bool                isSaved() { return mSaved; }        // プロジェクトファイルに変更があった場合はFalseが返る
    void                saveDone() { mSaved = true; }       //　保存したときにTrueにセット
    void                modified() ;                        //　変更があったときにFalseにセット

    void                createGcode();
    QStringList         getGcode();
    QRectF              processRange();

    QString             fileExtentions();
    QString             useTypes();
    int                 gcoodeCount();

    void                setMaterialData(MaterialListedData* );
    QString             materialName() { return mMaterial.name; }
    int                 materialThick() { return mMaterial.thick; }

    QMap<QString, QString> dataCheck(QRectF );

    void                changeOrder(int prev, int curr);

    QString             projectPath() { return mProjectPath; }

    QByteArray          getProjectData();

    QGraphicsSvgItem*   cloneCenterGraphics(GraphicsFlexibleFrame*);

private:
    DataManager()
    {
        mProjectPath = "";
        mSaved = true;
        mMaterial.name = "None";
        mMaterial.thick = 1;
        mMaterial.engraveParam = ParamData();
        mMaterial.cutParam = ParamData();
        mStockedGcode.clear();
    }

    bool    readData(const QJsonObject &json);
    bool    readBrowserProjectData(const QJsonObject &json);
    bool    writeDown(QJsonObject &json) const;
    int     checkName(ItemInfo* info);

private:
    QList<ItemInfo*> mPictureList;
    QString          mProjectName;
    QString          mVersion;
    QString          mProjectPath;

    bool             mSaved;
    ST_MATERIAL_DATA mMaterial;

    QStringList      mStockedGcode;
};
#endif // QDATAMANAGER_H
