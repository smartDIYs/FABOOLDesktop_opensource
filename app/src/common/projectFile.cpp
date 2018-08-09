#include <QtCore>
#include <QtGui>
#include <QFileDialog>
#include <QtXml>

#include "projectFile.h"
#include "dataManager.h"
#include "common.h"

ProjectFile::ProjectFile()
{

}


// プロジェクトにパスが設定されている（=ロードされたプロジェクト）場合の保存処理
int ProjectFile::save()
{
    DataManager* instance = DataManager::getInstance();
    QString projPath = instance->projectPath();
    QFile file(projPath);

    QString projName = instance->projectName();
    if(projName == "") {
        projName = projPath.split("/").last();
        projName = projName.left(projName.length() - 4);
    }

    QString version     = "";

    if(file.open(QFile::WriteOnly | QFile::Truncate)) {
        const qint16 indent = 4;
        QDomDocument doc;

//　プロジェクト全体の情報
        QDomElement domRoot     = doc.createElement("Project");
        //QDomElement domProjName = doc.createElement("Name");
        //QDomElement domVersion  = doc.createElement("Version");
        QDomElement domProjName = doc.createElement("ProjectName");
        QDomElement domVersion  = doc.createElement("FormatVersion");
        QDomElement domImageElm;
        QDomElement domImageData;
        QDomElement domGenericParam;
        QDomElement domDetailParam;

        //domProjName.setAttribute("ProjectName", projName);
        //domVersion.setAttribute("FormatVersion", version);
        domProjName.setNodeValue(projName);
        domVersion.setNodeValue(version);

        doc.appendChild(domRoot);
        domRoot.appendChild(domProjName);
        domRoot.appendChild(domVersion);

// 画像アイテムの情報
        foreach (ItemInfo* item, instance->ImageItem()) {
            domImageElm = doc.createElement("Image");
            domImageElm.setAttribute("ImageName",   item->itemName());
            //domImageElm.setAttribute("ImagePath",   item->itemPath());
            domImageElm.setAttribute("PosX",        item->posX());
            domImageElm.setAttribute("PosY",        item->posY());
            domImageElm.setAttribute("Width",       item->width());
            domImageElm.setAttribute("Height",      item->height());
            domImageElm.setAttribute("Rotation",    item->rotate());

            domImageData = doc.createElement("ImageData");
            domImageData.setNodeValue("");

// 汎用パスパラメータの解析
            foreach (ParamData* genericParam, item->genericParam()) {
                domGenericParam = doc.createElement("GenericPath");
                domGenericParam.setAttribute("Speed",        (int)genericParam->headSpeed());
                domGenericParam.setAttribute("LaserPower",   (int)genericParam->lasetPower());
                domGenericParam.setAttribute("ProcessTimes", (int)genericParam->processTimes());
                foreach (QString color, genericParam->colorSet().keys()) {
                    domGenericParam.setAttribute(color, genericParam->colorSet().value(color));
                }
                domImageElm.appendChild(domGenericParam);
            }
//　詳細パラメータの解析
            foreach (ParamData* detailParam, item->parameters()) {
                domDetailParam = doc.createElement("DetailPath");
                domDetailParam.setAttribute("Speed",        (int)detailParam->headSpeed());
                domDetailParam.setAttribute("LaserPower",   (int)detailParam->lasetPower());
                domDetailParam.setAttribute("ProcessTimes", (int)detailParam->processTimes());
                foreach (QString color, detailParam->colorSet().keys()) {
                    domDetailParam.setAttribute(color, detailParam->colorSet().value(color));
                }
                domImageElm.appendChild(domDetailParam);
            }
            domRoot.appendChild(domImageElm);
        }

        QTextStream out(&file);

        doc.save(out, indent);

        instance->setProjectName(projName);
        instance->setProjectPath(projPath);
    } else {
        return ERR_PROJ_SAVE_FILE_OPEN;
    }
    return SUCCESS;

}

// ファイルパスが存在しない場合　or　名前を付けて保存を実行した場合に呼ばれる
int ProjectFile::saveAs(QString filePath)
{
    DataManager* instance = DataManager::getInstance();
    instance->setProjectName("");
    instance->setProjectPath(filePath);
    return save();
}

//　プロジェクトの読み出し
int ProjectFile::load(QString filePath)
{
    QFile file(filePath);
    if(file.open(QFile::ReadOnly)) {
        DataManager* instance = DataManager::getInstance();
        instance->setProjectPath(filePath);

        QDomDocument doc;
        doc.setContent(&file);

        //QDomNodeList domProjName = doc.elementsByTagName("Name");
        //instance->setProjectName(domProjName.at(0).toElement().attribute("ProjectName"));
        QDomNodeList domProjName = doc.elementsByTagName("ProjectName");
        instance->setProjectName(domProjName.at(0).toElement().nodeValue());

        QDomNodeList domImgList  = doc.elementsByTagName("Image");

        for(int imgIndx = 0; imgIndx < domImgList.length(); imgIndx++) {
            QDomElement element = domImgList.at(imgIndx).toElement();

//ImageItem Load
            ItemInfo* item  = instance->loadImage(element.attribute("ImagePath"));
            qreal posX      = element.attribute("PosX").toDouble();
            qreal posY      = element.attribute("PosY").toDouble();
            qreal width     = element.attribute("Width").toDouble();
            qreal height    = element.attribute("Height").toDouble();
            qreal rotation  = element.attribute("Rotation").toDouble();
            item->setPosX(posX);
            item->setPosY(posY);
            item->setWidth(width);
            item->setHeight(height);
            item->setRotate(rotation);

//GenericPath Load
            qreal       speed ;
            qreal       laserPower ;
            qint16      processTimes ;
            QMap<QString, bool> colorMap ;
            QDomNodeList genericPathList = element.elementsByTagName("GenericPath");
            for(int Indx = 0; Indx < genericPathList.length(); Indx++) {
                QDomElement genericPath =  genericPathList.at(Indx).toElement();
                for(int attIndx = 0; attIndx < genericPath.attributes().length(); attIndx++) {
                    QString attr = genericPath.attributes().item(attIndx).nodeName();
                    if(attr == "Speed")             { speed        = genericPath.attribute(attr).toDouble(); }
                    else if(attr == "LaserPower")   { laserPower   = genericPath.attribute(attr).toDouble(); }
                    else if(attr == "ProcessTimes") { processTimes = genericPath.attribute(attr).toInt(); }
                    else                            { colorMap.insert(attr, (bool)(genericPath.attribute(attr).toInt())); }
                }
                item->setGenericParam(Indx, speed, laserPower, colorMap, processTimes);
                if(Indx + 1 !=  genericPathList.length()) { item->createGenericParam(); }
            }

//DetailPath Load
            QDomNodeList detailPathList = element.elementsByTagName("DetailPath");
            for(int detailIndx = 0; detailIndx < detailPathList.length(); detailIndx++) {
                QDomElement detailPath =  detailPathList.at(detailIndx).toElement();
                for(int attIndx = 0; attIndx < detailPath.attributes().length(); attIndx++) {
                    QString attr = detailPath.attributes().item(attIndx).nodeName();
                    if(attr == "Speed")             { speed        = detailPath.attribute(attr).toDouble(); }
                    else if(attr == "LaserPower")   { laserPower   = detailPath.attribute(attr).toDouble(); }
                    else if(attr == "ProcessTimes") { processTimes = detailPath.attribute(attr).toInt(); }
                    else                            { colorMap.insert(attr, (bool)(detailPath.attribute(attr).toInt())); }
                }
                item->setParam(detailIndx, speed, laserPower, colorMap, processTimes);
                if(detailIndx + 1 != detailPathList.length()) { item->createParam(); }
            }
            instance->appendItem(item);
        }
    } else {
        return ERR_PROJ_LOAD_FILE_OPEN;
    }
    return SUCCESS;
}

