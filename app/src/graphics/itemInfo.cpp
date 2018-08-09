
#include <QGraphicsItem>
#include <QtSvg>

#include "common/common.h"
#include "common/dataManager.h"
#include "common/sysParam.h"

#include "itemInfo.h"
#include "paramData.h"
#include "vectorImageItem.h"
#include "graphicsFlexibleFrame.h"

#include "transfer/imgParser.h"
#include "transfer/svgParser.h"

#include "ui/listView/libraryListedData.h"


QStringList RasterSuffixList{"jpeg", "jpg", "bmp", "png", "gif", "tiff", "tif"};
QStringList VectorSuffixList{"svg", "dxf"};

ItemInfo::ItemInfo()
{
    mItem = NULL;
    mParsedData = NULL;
    mExtension = "copied";
}

ItemInfo::~ItemInfo()
{
    foreach(ParamData * para, mPathParam) {
        delete para;
    }
}

EN_LOAD_STATE ItemInfo::setup(const QString& path, const ParamData& data)
{
// setup Names
    mPath = path;
    QString formalName = path.split("/").last();
    QStringList splited = formalName.split(".");

    if(splited.count() > 1) {
        mExtension = splited.takeLast();
    }

    QString name = "";
    while(splited.count() > 0) {
        name += splited.takeFirst();
    }
    mItemName = name;

    mCopiedCount = -1;

// setup Frame
    EN_LOAD_STATE retCreate = createFrame(path);
    if(retCreate != EN_LOAD_SUCCESS) {
        mItemName = "";
    } else {
// setup param
        createParam(data);
    }
    return retCreate;
}

EN_LOAD_STATE ItemInfo::setup(const ItemInfo& srcItem)
{
// setup Names
    mPath        = srcItem.mPath;
    mExtension   = srcItem.mExtension;
    mItemName    = srcItem.mItemName;
    mCopiedCount = 1;

// setup Frame
    createFrame(srcItem.mParsedData);
    setPosX(srcItem.posX());
    setPosY(srcItem.posY());
    setWidth(srcItem.width());
    setHeight(srcItem.height());
    setRotate(srcItem.rotate());
    update();

// setup Param
    foreach (ParamData* srcParam, srcItem.mPathParam) {
        mPathParam.append(new ParamData(srcParam));
    }
    return EN_LOAD_SUCCESS;
}

EN_LOAD_STATE ItemInfo::setup(const LibraryListedData& core, const ParamData& data)
{
// setup Names
    mItemName = core.name();
    mExtension = "Library";
    mCopiedCount = -1;
    mPath = "";

// setup Frame
    createFrame(core.parser());

// setup Param
    createParam(data);

    return EN_LOAD_SUCCESS;
}

EN_LOAD_STATE  ItemInfo::setup(const QJsonObject &json)
{
// setup Names
    mItemName = json[JSON_TAG_IMG_NAME].toString();
    mExtension = json[JSON_TAG_IMG_EXTENTION].toString();
    mPath = "";
    mCopiedCount = -1;

// setup Frame
    EN_LOAD_STATE retCreate = createFrame(json);
    if(retCreate != EN_LOAD_SUCCESS) {
        CRITICAL_STRING("Failed to create frame");
        return retCreate;
    }

    mItem->setRectPosX(json[JSON_TAG_POS_X].toDouble());
    mItem->setRectPosY(json[JSON_TAG_POS_Y].toDouble());
    mItem->setRectWidth(json[JSON_TAG_WIDTH].toDouble());
    mItem->setRectHeight(json[JSON_TAG_HEIGHT].toDouble());
    mItem->setRectRotate(json[JSON_TAG_ROTATE].toDouble());

// setup Param
    QJsonArray dParamArray = json[JSON_TAG_DETAIL_PATH].toArray();
    foreach (QJsonValue dParaVal, dParamArray) {
        ParamData* param = new ParamData();
        param->readData(dParaVal.toObject());
        mPathParam.append(param);
    }
    return EN_LOAD_SUCCESS;
}

int  ItemInfo::setupBrowserData(const QJsonObject &paths, const QJsonObject &canvas)
{
// setup Names
    mPath = "";
    QString formalName = paths["fileName"].toString() ;
    QStringList splited = formalName.split(".");

    if(splited.count() > 1) {
        mExtension = splited.takeLast();
    }
    QString name = "";
    while(splited.count() > 0) {
        name += splited.takeFirst();
    }
    mItemName = name;
    mCopiedCount = -1;

// setup Frame
    QJsonObject forCreateFrame;
    if(mExtension.contains("svg", Qt::CaseInsensitive) ||
            mExtension.contains("dxf", Qt::CaseInsensitive)) {
        forCreateFrame = transferSvgToCurrent(canvas);
    } else {
        forCreateFrame = transferImgToCurrent(canvas);
    }
    if(createFrame(forCreateFrame) != EN_LOAD_SUCCESS) {
        CRITICAL_STRING("Failed to create frame.");
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }

// setup property
    qreal dpi = canvas["dpi"].toDouble();
    if(dpi == 0) {
        CRITICAL_STRING("There is wrang DPI parameter.");
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }

    qreal posX = UnitTransfer::SvgPixelto( canvas["originLeft"].toDouble(), EN_SVG_UNIT_MM, dpi);
    qreal posY = UnitTransfer::SvgPixelto( canvas["originTop"].toDouble(), EN_SVG_UNIT_MM, dpi);
    if(qIsFinite(posX) == false || qIsFinite(posY) == false) {
        CRITICAL_STRING("Pos is not finite ");
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }
    mItem->setRectPosX(posX);
    mItem->setRectPosY(posY);

    qreal width  = UnitTransfer::SvgPixelto(canvas["width"].toDouble(), EN_SVG_UNIT_MM, dpi);
    qreal height = UnitTransfer::SvgPixelto(canvas["height"].toDouble(), EN_SVG_UNIT_MM, dpi);
    if(qIsFinite(width) == false || qIsFinite(width) == false) {
        CRITICAL_STRING("Size is not finite ");
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }
    qreal scaleX = canvas["originScaleX"].toDouble();
    qreal scaleY = canvas["originScaleY"].toDouble();
    if(qIsFinite(scaleX) == false || qIsFinite(scaleY) == false) {
        CRITICAL_STRING("Scale is not finite ");
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }
    mItem->setRectWidth(width * scaleX);
    mItem->setRectHeight(height * scaleY);
    mItem->setRectRotate(canvas["angle"].toDouble());

// rotate origin center to lefttop
    mItem->setAnchorCenter();
    mItem->updateScale();
    mItem->updateMatrix();
    QTransform form = mItem->transform();
    QPointF translated = form.map(QPointF(0, 0));
    QPointF diff = translated - QPointF(posX, posY);
    mItem->setRectPosX(posX - diff.x());
    mItem->setRectPosY(posY - diff.y());

// setup Param
    QList<QColor> colorMap;
    QJsonArray colors = paths["colors"].toArray();
    foreach(QJsonValue colorVal, colors) {
        QStringList colorStr = colorVal.toString().replace("rgb(", "").replace(")", "").split(",");
        colorMap << QColor(colorStr[0].toInt(), colorStr[1].toInt(), colorStr[2].toInt());
    }
    QList<QColor> remainColors;
    remainColors << colorMap;

    QJsonArray params = paths["passes"].toArray();
    QList<ParamData*> hasNoColorParam;
    foreach (QJsonValue paramVal, params) {
        QJsonObject paramObj = paramVal.toObject();

        qreal speed = 3000;
        if(paramObj["feedrate"].isDouble() == true) {
            speed = paramObj["feedrate"].toDouble();
        } else {
            speed = paramObj["feedrate"].toString().toDouble();
        }

        int lsrPow = 100;
        if(paramObj["intensity"].isDouble() == true) {
            lsrPow = paramObj["intensity"].toInt();
        } else {
            lsrPow = paramObj["intensity"].toString().toInt();
        }

        int process = 1;
        if(paramObj["processTimes"].isDouble() == true) {
            process = paramObj["processTimes"].toInt();
        } else {
            process = paramObj["processTimes"].toString().toInt();
        }

        QStringList colors;
        QJsonArray colorsAry = paramObj["colors"].toArray();

        foreach(QJsonValue colorVal, colorsAry) {
            QJsonObject colorObj = colorVal.toObject();
            if(colorObj["enabled"].toBool() == true) {
                colors << colorObj["hex"].toString() ;
            }
        }

        if(colors.count() == 0) {
            continue;
        } else if(colors.count() == 1) {
            ParamData* param = new ParamData();
            param->setColor(QColor(colors[0]));
            param->setHeadSpeed(speed);
            param->setLaserPower(lsrPow);
            param->setProcessTimes(process);
            mPathParam.append(param);
            remainColors.removeOne(QColor(colors[0]));
        } else {
            int processIndx = 0;
            if(process > 0) { processIndx = 1; }
            for (; processIndx <= process; processIndx++) {
                foreach (QString colorStr, colors) {
                    ParamData* param = new ParamData();
                    param->setColor(QColor(colorStr));
                    param->setHeadSpeed(speed);
                    param->setLaserPower(lsrPow);
                    if(process == 0) {
                        param->setProcessTimes(0);
                    } else {
                        param->setProcessTimes(1);
                    }
                    mPathParam.append(param);
                    remainColors.removeOne(QColor(colorStr));
                }
            }
        }
    }

    foreach (QColor remains, remainColors) {
        ParamData* param = new ParamData();
        param->setColor(remains);
        param->setHeadSpeed(3000);
        param->setLaserPower(100);
        param->setProcessTimes(0);
        mPathParam.append(param);
    }

    return SUCCESS;
}

void ItemInfo::setItemToScene(QGraphicsScene* scene)
{
    if(!scene) { return ; }

    if(inWorkArea(scene->sceneRect()) == false) {
        fixSizeAndPos(scene->sceneRect());
    }

    if(GraphicsFlexibleFrame::Type == mItem->type()) {
        ((GraphicsFlexibleFrame*)mItem)->setToScene(scene);
    }
}

void ItemInfo::removeItemFromScene(QGraphicsScene* scene)
{
    if(!scene) { return ; }

    if(GraphicsFlexibleFrame::Type == mItem->type()) {
        ((GraphicsFlexibleFrame*)mItem)->removeFromScene(scene);
    }
}

void ItemInfo::update()
{
    mItem->setAnchorCenter();
    mItem->updateScale();
}

void ItemInfo::createParam(const ParamData& data)
{
    QList<QColor> colors = mParsedData->colorMap();
    foreach (QColor color, colors) {
        ParamData* Param = new ParamData(color, data.headSpeed(), data.laserPower(), data.useType(), data.processTimes());
        mPathParam.append(Param);
    }
}

void ItemInfo::switchParam(int oldIndx, int newIndx)
{
    mPathParam.move(oldIndx, newIndx);
}

void ItemInfo::setCheckedCopied(QString name, int count)
{
    mItemName = name;

    if(mCopiedCount >= 0) {
        mCopiedCount = count;
        setPosX(posX() + 10 * count);
        setPosY(posY() + 10 * count);
    }
}

void ItemInfo::setCopiedPos(int count)
{
    QPointF diff = item()->rectPos();
    setPosX(diff.x() + 10 * count);
    setPosY(diff.y() + 10 * count);
}

qreal ItemInfo::posX() const  { return mItem->rectPos().x(); }
qreal ItemInfo::posY() const  { return mItem->rectPos().y(); }
qreal ItemInfo::rotate() const { return mItem->rectRotate(); }
qreal ItemInfo::width()  const { return mItem->rectWidth();  }
qreal ItemInfo::height() const { return mItem->rectHeight(); }
void  ItemInfo::setPosX(qreal x)    { mItem->setRectPosX(x); }
void  ItemInfo::setPosY(qreal y)    { mItem->setRectPosY(y); }
void  ItemInfo::setRotate(qreal r)  { mItem->setRectRotate(r); }
void  ItemInfo::setWidth(qreal w)   { mItem->setRectWidth(w);  }
void  ItemInfo::setHeight(qreal h)  { mItem->setRectHeight(h); }

EN_LOAD_STATE ItemInfo::createFrame(QString path)
{
    QFileInfo   fileInfo(path);
    QString     targetSuffix = fileInfo.suffix().toLower();

    if(RasterSuffixList.contains(targetSuffix)) {
        mParsedData = new ImgParser(SysParam::getInstance()->getRasterDealType());
    } else if(VectorSuffixList.contains(targetSuffix)) {
        mParsedData = new SvgParser();
    } else {
        INFO_STRING("Unsupported image type.");
        return EN_LOAD_UNSUPPORTED;
    }

    if(mParsedData->loadFile(path) == false) {
        WARNING_STRING(QString("Fail to load image %1.").arg(path));
        return EN_LOAD_FAILED;
    }

    mItem = new GraphicsFlexibleFrame(new VectorImageItem(mParsedData));
    mItem->setParentInfo(this);
    mItem->setRectPos(mParsedData->getStartPoint());

    //　画像は回転させない
    if(mParsedData->type() == PARSER_TYPE_RASTER) {
        mItem->hideRotateGrip();
    }

    return EN_LOAD_SUCCESS;
}

EN_LOAD_STATE ItemInfo::createFrame(QJsonObject itemObj)
{
    int type = itemObj[JSON_TAG_IMG_TYPE].toInt();
    int ret = SUCCESS;
    switch(type) {
    case PARSER_TYPE_RASTER:
        mParsedData = new ImgParser(SysParam::getInstance()->getRasterDealType());
        ret = mParsedData->readData(itemObj[JSON_TAG_IMG_DATA].toObject());
        break;
    case PARSER_TYPE_VECTOR:
        mParsedData = new SvgParser();
        ret = mParsedData->readData(itemObj[JSON_TAG_IMG_DATA].toObject());
        break;
    }
    if(ret != SUCCESS) {
        CRITICAL_STRING("Failed to read data.");
        return EM_LOAD_JSON_FAILED;
    }

    mItem = new GraphicsFlexibleFrame(new VectorImageItem(mParsedData));
    mItem->setParentInfo(this);
    mItem->setRectPos(mParsedData->getStartPoint());

    //　画像は回転させない
    if(mParsedData->type() == PARSER_TYPE_RASTER) {
        mItem->hideRotateGrip();
    }

    return EN_LOAD_SUCCESS;
}

EN_LOAD_STATE ItemInfo::createFrame(ParserCore* parser)
{
    switch(parser->type()) {
    case PARSER_TYPE_RASTER: {
            ImgParser* imgParser = (ImgParser*)(parser);
            mParsedData = new ImgParser(*imgParser);
        }
        break;
    case PARSER_TYPE_VECTOR: {
            SvgParser* svgParser = (SvgParser*)(parser);
            mParsedData = new SvgParser(*svgParser);
        }
        break;
    }

    mItem = new GraphicsFlexibleFrame(new VectorImageItem(mParsedData));
    mItem->setParentInfo(this);
    mItem->setRectPos(mParsedData->getStartPoint());

    //　画像は回転させない
    if(mParsedData->type() == PARSER_TYPE_RASTER) {
        mItem->hideRotateGrip();
    }

    return EN_LOAD_SUCCESS;
}

QJsonObject ItemInfo::transferImgToCurrent(const QJsonObject& json)
{
    QJsonObject retJson;
    retJson[JSON_TAG_IMG_TYPE] = PARSER_TYPE::PARSER_TYPE_RASTER;
    QJsonArray paths = json["paths"].toArray();
    foreach (QJsonValue pathVal, paths) {
        QJsonObject pathObj = pathVal.toObject();
        QString srcData = pathObj["src"].toString().split(",")[1];
        QJsonObject imgData;
        imgData[JSON_TAG_IMG_DATA] = srcData;
        retJson[JSON_TAG_IMG_DATA] = imgData;
    }

    return retJson;
}

QJsonObject ItemInfo::transferSvgToCurrent(const QJsonObject& json)
{
    QJsonObject retJson;
    retJson[JSON_TAG_IMG_TYPE] = PARSER_TYPE::PARSER_TYPE_VECTOR;
    QJsonObject imgDataObj;
    imgDataObj[JSON_TAG_SVG_MADE_BY] = EN_SVG_IMPORTED;

    qreal dpi = json["dpi"].toDouble();
    imgDataObj[JSON_TAG_SVG_DPI]  = dpi;

    QJsonArray pathList;
    QJsonArray paths = json["paths"].toArray();
    SvgParser parser;
    foreach (QJsonValue pathVal, paths) {
        QJsonObject pathObj = pathVal.toObject();
        QJsonObject exchanged = parser.transferPathToCurrent(pathObj, dpi);
        pathList.append(exchanged);
    }
    imgDataObj[JSON_TAG_SVG_PATH_LIST] = pathList;
    imgDataObj[JSON_TAG_SVG_UNIT] = EN_SVG_UNIT_MM;

    QRectF boxRect = parser.getBBox();

    QJsonObject  vb;
    vb[JSON_TAG_SVG_VIEW_BOX_X] = boxRect.x();
    vb[JSON_TAG_SVG_VIEW_BOX_Y] = boxRect.y();
    vb[JSON_TAG_SVG_VIEW_BOX_W] = boxRect.width();
    vb[JSON_TAG_SVG_VIEW_BOX_H] = boxRect.height();
    imgDataObj[JSON_TAG_SVG_VIEW_BOX] = vb;
    imgDataObj[JSON_TAG_SVG_WIDTH] = boxRect.width();
    imgDataObj[JSON_TAG_SVG_HEIGHT] = boxRect.height();
    QJsonArray colorArray;
    foreach (QColor color, parser.colorMap()) {
        QJsonObject colorObj;
        colorObj[JSON_TAG_SVG_COLOR] = color.name();
        colorArray.append(colorObj);
    }
    imgDataObj[JSON_TAG_SVG_COLOR_MAP] = colorArray;

    QJsonObject common;
    common[JSON_TAG_SVG_COMMON_DISPLAY] = json["visible"].toBool();
    QJsonObject commonFill;
    commonFill[JSON_TAG_SVG_COLOR] = json["fill"].toString();
    common[JSON_TAG_SVG_COMMON_FILL] = commonFill;
    QJsonObject commonStroke;
    commonStroke[JSON_TAG_SVG_COLOR] = json["stroke"].toString();
    common[JSON_TAG_SVG_COMMON_STROKE] = commonStroke;
    qreal strokeWid = json["strokeWidth"].toDouble();
    if(strokeWid == 0) { strokeWid = 1; }
    common[JSON_TAG_SVG_COMMON_STROKE_WIDTH] = strokeWid;

    QJsonObject commonTransform;
    commonTransform[JSON_TAG_SVG_TRANSFORM_0] = 1;
    commonTransform[JSON_TAG_SVG_TRANSFORM_1] = 0;
    commonTransform[JSON_TAG_SVG_TRANSFORM_2] = 0;
    commonTransform[JSON_TAG_SVG_TRANSFORM_3] = 1;
    commonTransform[JSON_TAG_SVG_TRANSFORM_4] = 0;
    commonTransform[JSON_TAG_SVG_TRANSFORM_5] = 0;
    common[JSON_TAG_SVG_COMMON_TRANSFORM] = commonTransform;

    imgDataObj[JSON_TAG_SVG_COMMON_SETTING] = common;
    imgDataObj[JSON_TAG_SVG_KEEP_ASPTECT] = true;
    imgDataObj[JSON_TAG_SVG_EFFECTIVE_ATTR] = QJsonArray{"g",
                                                         "path",
                                                         "circle",
                                                         "ellipse",
                                                         "rect",
                                                         "line",
                                                         "polyline",
                                                         "polygon"
                                                        };

    retJson[JSON_TAG_IMG_DATA] = imgDataObj;
    return retJson;
}

void  ItemInfo::writeDown(QJsonObject &json) const
{
    json[JSON_TAG_IMG_NAME]   = mItemName;
    json[JSON_TAG_IMG_TYPE]   = mParsedData->type();
    json[JSON_TAG_IMG_EXTENTION] = mExtension;
    QJsonObject dataObj;
    mParsedData->writeDown(dataObj);
    json[JSON_TAG_IMG_DATA]   = dataObj;
    json[JSON_TAG_POS_X]  = mItem->rectPos().x();
    json[JSON_TAG_POS_Y]  = mItem->rectPos().y();
    json[JSON_TAG_WIDTH]  = mItem->rectWidth();
    json[JSON_TAG_HEIGHT] = mItem->rectHeight();
    json[JSON_TAG_ROTATE] = mItem->rectRotate();

    QJsonArray dParamArray;
    foreach (ParamData* param, mPathParam) {
        QJsonObject dParam;
        param->writeDown(dParam);
        dParamArray.append(dParam);
    }
    json[JSON_TAG_DETAIL_PATH] = dParamArray;
}

void ItemInfo::transformToGcode()
{
    qreal d1 = mItem->transformParam0();
    qreal d2 = mItem->transformParam1();
    qreal d3 = mItem->transformParam2();
    qreal d4 = mItem->transformParam3();
    qreal d5 = mItem->transformParam4();
    qreal d6 = mItem->transformParam5();
    mGcode = mParsedData->toGcode(d1, d2, d3, d4, d5, d6, mPathParam);
}

bool ItemInfo::isRaster()
{
    return mItem->isRaster();
}

bool ItemInfo::inWorkArea(QRectF workArea)
{
    return workArea.contains(mItem->sceneBoundingRect());
}

void ItemInfo::applyProcessRange(QRectF* amountRect)
{
    if(amountRect->contains(mItem->sceneBoundingRect()) == false) {
        *amountRect = amountRect->united(mItem->sceneBoundingRect());
    }
}

QString ItemInfo::getLoadStateStr(EN_LOAD_STATE resultLoad)
{
    switch(int(resultLoad)) {
    case EN_LOAD_FAILED:
    case EM_LOAD_JSON_FAILED:
        return QObject::tr("load failed");
    case EN_LOAD_UNSUPPORTED:
        return QObject::tr("unsupported file");
    case EN_LOAD_TOO_SMALL:
        return QObject::tr("too small");
    case EN_LOAD_SUCCESS:
    default:
        return "";
    }
}

bool ItemInfo::isSkipData()
{
    bool ret = true;
    foreach (ParamData* prm, mPathParam) {
        if(prm->processTimes() > 0) {
            ret = false;
            break;
        }
    }
    return ret;
}

void ItemInfo::fixSizeAndPos(QRectF worksize)
{
    qreal workW = worksize.width();
    qreal workH = worksize.height();
    qreal scale = 1.0;
    qreal scaleW = 1.0, scaleH = 1.0;

    if(width()  > workW || height() > workH) {
        scaleW = workW / width();
        scaleH = workH / height();
        if(scaleW < scaleH) { scale = scaleW; }
        else                { scale = scaleH; }
        setWidth(width()   * scale);
        setHeight(height() * scale);
    }

    if(posX() < 0) { setPosX(0); }
    if(posY() < 0) { setPosY(0); }

    qreal rightDiff  = posX() + width()  - workW;
    qreal bottomDiff = posY() + height() - workH;
    if( rightDiff  > 0 ) { setPosX(posX() - rightDiff); }
    if( bottomDiff > 0 ) { setPosY(posY() - bottomDiff); }
}
