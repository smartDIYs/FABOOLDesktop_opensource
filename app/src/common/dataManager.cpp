#include <QFileInfo>
#include <QFile>

#include <QGraphicsItem>
#include <QGraphicsPixmapItem>

#include <QJsonObject>
#include <QJsonDocument>

#include "common.h"
#include "dataManager.h"

#include "graphics/itemInfo.h"
#include "graphics/vectorImageItem.h"
#include "graphics/graphicsFlexibleFrame.h"

#include "transfer/processTime.h"


// 画像リストの消去
void DataManager::dataClear()
{
    foreach (ItemInfo* item, mPictureList) {
        delete item;
    }
    mPictureList.clear();
    mProjectName = "";
}

//　画像アイテムをリストへ追加
void  DataManager::appendItems(QList<ItemInfo*> items)
{
    this->mPictureList << (items);
}

//　画像アイテムをリストへ追加
void  DataManager::appendItem(ItemInfo* item)
{
    this->mPictureList << (item);
}

//　リストから選択的にアイテムを取りだす
//　アイテム自体はメモリ上に残る
void  DataManager::removeItems(QList<ItemInfo*> items)
{
    for(int i = 0; i < items.count(); i++) {
        this->mPictureList.removeOne(items[i]);
    }
}

//　画像のロード
//　関数の外からロードしたい画像のパスをもらう
QList<ItemInfo*> DataManager::loadImage(const QStringList& targetList, QMap<QString, EN_LOAD_STATE>* resultLoad )
{
    QList<ItemInfo*> distList;
    foreach (QString target, targetList) {
        EN_LOAD_STATE ret = EN_LOAD_UNLOADED;
        ItemInfo* item = loadImage(target, &ret);
        if(ret == EN_LOAD_SUCCESS) {
            distList.append(item);
        }
        resultLoad->insert(target, ret);
    }

    return distList;
}

//　画像のロード処理本体
//　DataManager上でNewすることで、すべての画像情報をDataManagerのメモリで管理する
ItemInfo* DataManager::loadImage(const QString& targetName, EN_LOAD_STATE* result)
{
    ItemInfo* distInfo = new ItemInfo();
    *result = distInfo->setup(targetName, mMaterial.engraveParam);
    if(*result != EN_LOAD_SUCCESS) {
        delete distInfo ;
        return NULL;
    }
    checkName(distInfo);
    return distInfo;
}

// 画像アイテムをもとに画像アイテムを複製
ItemInfo* DataManager::loadImage(ItemInfo* srcItem, EN_LOAD_STATE* result)
{
    ItemInfo* distInfo = new ItemInfo();
    *result = distInfo->setup(*srcItem);
    if(*result != EN_LOAD_SUCCESS) {
        delete distInfo ;
        return NULL;
    }
    checkName(distInfo);
    return distInfo;
}

// ライブラリからロードしたデータでItemInfo生成
ItemInfo* DataManager::loadImage(LibraryListedData* libData, EN_LOAD_STATE* result)
{
    ItemInfo* distInfo = new ItemInfo();
    *result = distInfo->setup(*libData, mMaterial.engraveParam);
    if(*result != EN_LOAD_SUCCESS) {
        delete distInfo ;
        distInfo = NULL;
    }
    checkName(distInfo);
    mPictureList.append(distInfo);   // ライブラリのロードはRedoUndo対象ではないので直接リストへ放り込む
    return distInfo;
}

QList<ItemInfo*> DataManager::loadImage(GroupInfo* group, QPointF boundPos)
{
    if(group == NULL) { return QList<ItemInfo*>(); }
    QList<ItemInfo* > retInfos;
    foreach (GraphicsFlexibleFrame* factor, group->factorFrameList()) {
        EN_LOAD_STATE retState = EN_LOAD_UNLOADED;
        ItemInfo* item = loadImage(factor->parentInfo(), &retState);
        if(retState != EN_LOAD_SUCCESS) { continue; }

        group->apply(factor, item->item());

        if(boundPos == QPointF(-1, -1)) {
            item->setCopiedPos(group->pastedCount());
        } else {
            QPointF diff = item->item()->rectPos();
            item->setPosX(boundPos.x() + diff.x());
            item->setPosY(boundPos.y() + diff.y());
        }
        retInfos << item;
    }
    return retInfos;
}

//　ロード済み画像情報の各画像名を返す
QList<QString> DataManager::imageNameList()
{
    QStringList nameList ;
    foreach(ItemInfo* item, this->mPictureList) {
        nameList.append(item->formalName());
    }
    return nameList;
}

//　名前をキーにアイテムを取り出す
ItemInfo* DataManager::getImageWithFormalName(QString imageName)
{
    foreach (ItemInfo* item, mPictureList) {
        if(item->formalName() == imageName)
        { return item; }
    }
    return NULL;
}

//　プロジェクト名
QString DataManager::projectName()
{
    return mProjectName;
}

//　ロード済みアイテムのリスト
QList<ItemInfo*> DataManager::ImageItem()
{
    return mPictureList;
}

int  DataManager::saveProject(QString newPath)
{
    mProjectPath = newPath;

    QString projName = newPath.split("/").last();
    mProjectName     = projName.left(projName.length() - 4);

    int ret = saveProject();
    return ret;
}

int  DataManager::saveProject()
{
    QFile file(mProjectPath);
    if(file.open(QIODevice::WriteOnly) == false) {
        WARNING_STRING(ERR_PROJ_SAVE_FILE_OPEN_STR);
        return ERR_PROJ_SAVE_FILE_OPEN;
    }

    QJsonObject body;
    bool ret = writeDown(body);
    if(ret == false) {
        file.close();
        WARNING_STRING(ERR_PROJ_SAVE_INVAILD_DATA_STR);
        return ERR_PROJ_SAVE_INVAILD_DATA;
    }

    QJsonDocument doc(body);
    file.write(doc.toJson());

    file.close();

    return SUCCESS;
}

int  DataManager::loadProject(QString path)
{
    QFile file(path);
    if(file.open(QIODevice::ReadOnly) == false) {
        WARNING_STRING(ERR_PROJ_LOAD_FILE_OPEN_STR);
        return ERR_PROJ_LOAD_FILE_OPEN;
    }
    mProjectPath = path;

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc(QJsonDocument::fromJson(data));
    bool ret = readData(doc.object());
    if(ret == false) {
        WARNING_STRING(ERR_PROJ_LOAD_INVAILD_DATA_STR);
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }

    mSaved = true;
    mStockedGcode.clear();
    mProjectPath = path;

    return SUCCESS;
}

int  DataManager::loadBrowserProject(QString path)
{
    QFile file(path);
    if(file.open(QIODevice::ReadOnly) == false) {
        WARNING_STRING(ERR_PROJ_LOAD_FILE_OPEN_STR);
        return ERR_PROJ_LOAD_FILE_OPEN;
    }

    QByteArray data = file.readAll();
    file.close();

    QString projName = path.split("/").last();
    mProjectName     = projName.left(projName.length() - 4);

    QJsonDocument doc(QJsonDocument::fromJson(data));
    bool ret = readBrowserProjectData(doc.object());
    if(ret == false) {
        WARNING_STRING(ERR_PROJ_LOAD_INVAILD_DATA_STR);
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }

    mSaved = false;
    mStockedGcode.clear();
    mProjectPath = path.left(path.length() - 4) + FILE_SUFFIX_PROJECT;

    return SUCCESS;
}

QMap<QString, QString> DataManager::dataCheck(QRectF workArea)
{
    QMap<QString, QString> reasons;

    foreach (ItemInfo* info, mPictureList) {
        if(info->inWorkArea(workArea) == false) {
            QString mes = QString(info->formalName());
            reasons.insert(mes, "out of work area");
            DEBUG_STRING(QString("%1 is out of range").arg(mes));
        }
    }

    return reasons;
}

void DataManager::changeOrder(int prev, int curr)
{
    if(prev < 0 || prev >= mPictureList.count()) { return; }
    if(curr < 0 || curr >= mPictureList.count()) { return; }
    ItemInfo* tmp = mPictureList.takeAt(prev);
    mPictureList.insert(curr, tmp);
}

QByteArray DataManager::getProjectData()
{
    QJsonObject body;
    bool ret = writeDown(body);

    QJsonDocument doc(body);
    QString docStr(doc.toJson(QJsonDocument::Compact));
    QByteArray dataArray(docStr.toLocal8Bit().data());
    return dataArray;
}

bool DataManager::readData(const QJsonObject &body)
{
    bool ret = true;
    mProjectName = body[JSON_TAG_PROJ_NAME].toString();

    QJsonArray imageArray = body[JSON_TAG_IMAGE].toArray();
    foreach (QJsonValue imageObj, imageArray) {
        ItemInfo* item = new ItemInfo();
        if(item->setup(imageObj.toObject()) != SUCCESS) {
            CRITICAL_STRING("Failed to parse Json");
            delete item;
            return false;
        }
        mPictureList.append(item);
    }
    return ret;
}

bool DataManager::readBrowserProjectData(const QJsonObject &body)
{
    bool ret = true;

    QJsonArray images = body["params"].toArray();
    QJsonObject canvasObj = body["fabCanvas"].toObject();
    QJsonArray canvas = canvasObj["objects"].toArray();
    int count = 0;
    foreach(QJsonValue img, images) {
        ItemInfo* item = new ItemInfo();
        QJsonObject imgObj = canvas[count].toObject();
        if(item->setupBrowserData(img.toObject(), imgObj) != SUCCESS) {
            CRITICAL_STRING("There is invalid data.");
            delete item;
            mPictureList.clear();
            return false;
        }
        checkName(item);
        mPictureList.append(item);
        count++;
    }

    return ret;
}

bool DataManager::writeDown(QJsonObject &body) const
{
    bool ret = true;
    body[JSON_TAG_PROJ_NAME]  = mProjectName;
    body[JSON_TAG_FORMAT_VER] = "0.1";

    QJsonArray imageArray;
    foreach (ItemInfo* item, mPictureList) {
        QJsonObject imageObj;
        item->writeDown(imageObj);
        imageArray.append(imageObj);
    }
    body[JSON_TAG_IMAGE] = imageArray;
    return ret;
}

int DataManager::checkName(ItemInfo* info)
{
    int count = 0;
    QString baseName = info->itemName();
    QString goodName = info->itemName();

    ItemInfo* sameNameItem = NULL;
    while(1) {
        sameNameItem = getImageWithFormalName(goodName + "." + info->extension());
        if(sameNameItem == NULL) {
            break;
        }
        count++;
        goodName = baseName + QString("_%1").arg(count);
    }

    info->setCheckedCopied(goodName, count);
    return SUCCESS;
}

void DataManager::modified()
{
    mSaved = false;
    mStockedGcode.clear();
}

void DataManager::createGcode()
{
    mStockedGcode.clear();

    QString header = QString(GCODE_PROCESS_START_HEADER);
    QString footer = "M81\nS0\nG0X0Y0\n";

    QStringList gcodes;
    gcodes.append(GCODE_RETURN_TO_ORIGIN);
    gcodes.append(header);
    foreach (ItemInfo* item, mPictureList) {
        item->transformToGcode();
        gcodes.append(item->getGcode());
    }
    gcodes.append(footer);
    mStockedGcode << gcodes;
}

QStringList DataManager::getGcode()
{
    return mStockedGcode;
}

QRectF DataManager::processRange()
{
    QRectF totalBoundRect;
    foreach (ItemInfo* info, mPictureList) {
        if(info->isSkipData() == false) {
            info->applyProcessRange(&totalBoundRect);
        }
    }

    return totalBoundRect;
}

QString  DataManager::fileExtentions()
{
    QString extentions;
    foreach (ItemInfo* item, mPictureList) {
        QString extention = item->extension();
        if(extentions.contains(extention) == false) {
            extentions += QString(extention + ",");
        }
    }
    extentions.resize(extentions.length() - 1);
    return extentions;
}

QString  DataManager::useTypes()
{
    QString types;
    foreach (ItemInfo* item, mPictureList) {
        foreach (ParamData* data, item->parameters()) {
            PARAM_USE_TYPE type = data->useType();
            types += QString("%1,").arg((int)type);
        }
    }
    types.resize(types.length() - 1);
    return types;
}

void DataManager::setMaterialData(MaterialListedData* materialData)
{
    mMaterial.name  = materialData->tag();
    mMaterial.thick = (materialData->thick() * 100);

    mMaterial.engraveParam = materialData->param(PARAM_USE_TYPE_ENGRAVE);
    mMaterial.cutParam     = materialData->param(PARAM_USE_TYPE_CUT);

    // LibData 適用
    foreach (ItemInfo* item, mPictureList) {
        foreach (ParamData* data, item->parameters()) {
            PARAM_USE_TYPE type = data->useType();
            if(type == PARAM_USE_TYPE_CUT) {
                data->setHeadSpeed(mMaterial.cutParam.headSpeed());
                data->setLaserPower(mMaterial.cutParam.laserPower());
                data->setProcessTimes(mMaterial.cutParam.processTimes());
            } else {
                data->setHeadSpeed(mMaterial.engraveParam.headSpeed());
                data->setLaserPower(mMaterial.engraveParam.laserPower());
                data->setProcessTimes(mMaterial.engraveParam.processTimes());
            }
        }
    }
}

QGraphicsSvgItem* DataManager::cloneCenterGraphics(GraphicsFlexibleFrame* frame)
{
    QGraphicsSvgItem* item = new QGraphicsSvgItem();
    QGraphicsSvgItem* src = (QGraphicsSvgItem*)(frame->centeredItem());
    item->setSharedRenderer(src->renderer());
    qreal factor = frame->defRect().width() / item->boundingRect().width();
    item->setScale(factor);
    item->setTransform(frame->transform());
    return item;
}
