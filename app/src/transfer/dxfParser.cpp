//---------------------------------------------------------------------------
#include "dxfParser.h"
//---------------------------------------------------------------------------
DxfParser::DxfParser()
{
}
//---------------------------------------------------------------------------
DxfParser::~DxfParser()
{

}
//---------------------------------------------------------------------------
bool DxfParser::dxfToSvg(const QString &strDxfPath, QByteArray &strSvg)
{
    // ファイルからDXFデーターを読み込む
    QStringList contentLines;
    if (toLines(strDxfPath, &contentLines) == false) {
        return false;
    }

    QList<ST_DXF_TYPE_A_VALUE> typesAndValues;
    if (convertToTypesAndValues(contentLines, &typesAndValues) == false) {
        return false;
    }
    // メモリ解放
    contentLines.clear();

    QList<QList<ST_DXF_TYPE_A_VALUE>> sections;
    separateSections(typesAndValues, &sections);
    // メモリ解放
    typesAndValues.clear();

    ST_DXF_SECTIONS stSections;
    reduceSection(sections, &stSections);
    // メモリ解放
    for (int i = 0; i < sections.length(); ++i) { sections[i].clear(); }
    sections.clear();

    // SVGに変換
    toSVG(stSections, strSvg);

    return true;
}
//---------------------------------------------------------------------------
bool DxfParser::toLines(const QString &strDxfPath, QStringList* pLines)
{
    QFile file(strDxfPath);
    if (file.open(QIODevice::ReadOnly | QIODevice::Text) == false) {
        return false;
    }
    QTextStream in(&file);
    while (in.atEnd() == false) {
        pLines->append(in.readLine().trimmed());
    }
    file.close();

    return true;
}
//---------------------------------------------------------------------------
bool DxfParser::convertToTypesAndValues(const QStringList &lines, QList<ST_DXF_TYPE_A_VALUE>* pTypesAndValues)
{
    bool ok;
    bool bTypeFlg = true;
    ST_DXF_TYPE_A_VALUE stValue;
    for (int i = 0; i < lines.length(); ++i) {
        if (bTypeFlg == true) {
            stValue.iType = lines[i].toInt(&ok);
            stValue.dValue = 0.0;
            stValue.iValue = 0;
            stValue.strValue = "";
            if (ok == false) {
                return false;
            }
            bTypeFlg = false;
        } else {
            if ((stValue.iType >= 10) && (stValue.iType < 60)) {
                stValue.enValue = EN_DXF_VALUE_FLOAT;
                stValue.dValue = lines[i].toDouble(&ok);
                if (ok == false) {
                    return false;
                }
            } else if ((stValue.iType >= 210) && (stValue.iType < 240)) {
                stValue.enValue = EN_DXF_VALUE_FLOAT;
                stValue.dValue = lines[i].toDouble(&ok);
                if (ok == false) {
                    return false;
                }
            } else if ((stValue.iType >= 60) && (stValue.iType < 100)) {
                stValue.enValue = EN_DXF_VALUE_INT;
                stValue.iValue = lines[i].toInt(&ok);
                if (ok == false) {
                    double fixed = lines[i].toDouble(&ok);
                    if(ok == true)
                    { stValue.iValue = (int)fixed; }
                    else
                    { return false; }
                }
            } else {
                stValue.enValue = EN_DXF_VALUE_STR;
                stValue.strValue = lines[i];
                if (ok == false) {
                    return false;
                }
            }
            pTypesAndValues->append(stValue);
            bTypeFlg = true;
        }
    }
    return true;
}
//---------------------------------------------------------------------------
void DxfParser::separateSections(const QList<ST_DXF_TYPE_A_VALUE> &typesAndValues, QList<QList<ST_DXF_TYPE_A_VALUE>>* pSections)
{
    QList<ST_DXF_TYPE_A_VALUE>* pSectionTuples = NULL;

    for (int i = 0; i < typesAndValues.length(); ++i) {
        if (typesAndValues[i].iType == 0 && typesAndValues[i].enValue == EN_DXF_VALUE_STR &&
                typesAndValues[i].strValue == "SECTION") {
            pSectionTuples = new QList<ST_DXF_TYPE_A_VALUE>();
        } else if (typesAndValues[i].iType == 0 && typesAndValues[i].enValue == EN_DXF_VALUE_STR &&
                   typesAndValues[i].strValue == "ENDSEC") {
            pSections->append(*pSectionTuples);
            delete pSectionTuples;
            pSectionTuples = NULL;
        } else if (pSectionTuples != NULL) {
            pSectionTuples->append(typesAndValues[i]);
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::reduceSection(const QList<QList<ST_DXF_TYPE_A_VALUE>> &sections, ST_DXF_SECTIONS* pstSections)
{
    for (int i = 0; i < sections.length(); ++i) {
        if (sections[i][0].enValue != EN_DXF_VALUE_STR) {
            continue;
        }
        QString sectionType = sections[i][0].strValue;
        if (sectionType == "HEADER") {
            headerHandler(sections[i].mid(1), &(pstSections->stHeader));
        }
        if (sectionType == "TABLES") {
            tablesHandler(sections[i].mid(1), &(pstSections->stTables));
        }
        if (sectionType == "BLOCKS") {
            blocksHandler(sections[i].mid(1), &(pstSections->stBlocks));
        }
        if (sectionType == "ENTITIES") {
            entitiesHandler(sections[i].mid(1), &(pstSections->stEntities));
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::headerHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_HEADER* pstHeader)
{
    QString state;

    for (int i = 0; i < tuples.length(); ++i) {
        if (tuples[i].enValue == EN_DXF_VALUE_STR) {
            if (tuples[i].strValue == "$EXTMIN") {
                pstHeader->extMin = QPointF(0.0, 0.0);
                state = "extMin";
            }
            if (tuples[i].strValue == "$EXTMAX") {
                pstHeader->extMax = QPointF(0.0, 0.0);
                state = "extMax";
            }
        } else {
            if (state == "extMin") {
                switch (tuples[i].iType) {
                case 10:
                    pstHeader->extMin.setX(tuples[i].dValue);
                    break;
                case 20:
                    pstHeader->extMin.setY(tuples[i].dValue);
                    break;
                case 30:
                    state = "";
                    break;
                }
            }
            if (state == "extMax") {
                switch (tuples[i].iType) {
                case 10:
                    pstHeader->extMax.setX(tuples[i].dValue);
                    break;
                case 20:
                    pstHeader->extMax.setY(tuples[i].dValue);
                    break;
                case 30:
                    state = "";
                    break;
                }
            }
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::tablesHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_TABLES* pstTables)
{
    QList<QList<ST_DXF_TYPE_A_VALUE>> tableGroups;
    QList<ST_DXF_TYPE_A_VALUE> tableTuples;

    for (int i = 0; i < tuples.length(); ++i) {
        if (tuples[i].enValue == EN_DXF_VALUE_STR && tuples[i].strValue == "TABLE") {
            tableGroups.append(tableTuples);
            tableTuples.clear();
        } else if (tuples[i].enValue == EN_DXF_VALUE_STR && tuples[i].strValue == "ENDTAB") {
            tableGroups.append(tableTuples);
        } else {
            tableTuples.append(tuples[i]);
        }
    }

    QList<ST_DXF_TYPE_A_VALUE> stylesTuples;
    QList<ST_DXF_TYPE_A_VALUE> ltypesTuples;
    QList<ST_DXF_TYPE_A_VALUE> layersTuples;
    for (int i = 0; i < tableGroups.length(); ++i) {
        if(tableGroups[i].length() == 0) { // 空要素へのアクセス防止・他に良い方法はないか？
            continue;
        }
        if (tableGroups[i][0].enValue != EN_DXF_VALUE_STR) {
            continue;
        }
        if (tableGroups[i][0].strValue == "STYLE") {
            stylesTuples.append(tableGroups[i]);
        }
        if (tableGroups[i][0].strValue == "LTYPE") {
            ltypesTuples.append(tableGroups[i]);
        }
        if (tableGroups[i][0].strValue == "LAYER") {
            layersTuples.append(tableGroups[i]);
        }
    }

    layerTableHandler(layersTuples, &(pstTables->mapLayers));
    stylesTableHandler(stylesTuples, &(pstTables->mapStyles));
}
//---------------------------------------------------------------------------
void DxfParser::layerTableHandler(const QList<ST_DXF_TYPE_A_VALUE> &layersTuples, QMap<QString, ST_DXF_TABLE_LAYER>* pMapLayers)
{
    QList<QList<ST_DXF_TYPE_A_VALUE>> tableRowsTuples;
    QList<ST_DXF_TYPE_A_VALUE> tableRowTuples;

    for (int i = 0; i < layersTuples.length(); ++i) {
        if ((layersTuples[i].iType == 0 || layersTuples[i].iType == 2) &&
                layersTuples[i].enValue == EN_DXF_VALUE_STR && layersTuples[i].strValue == "LAYER") {
            tableRowsTuples.append(tableRowTuples);
            tableRowTuples.clear();

        } else {
            tableRowTuples.append(layersTuples[i]);
        }
    }

    for (int i = 0; i < tableRowsTuples.length(); ++i) {
        ST_DXF_TABLE_LAYER  stLayer;
        for (int j = 0; j < tableRowsTuples[i].length(); ++j) {
            switch (tableRowsTuples[i][j].iType) {
            case 2:
                stLayer.name = tableRowsTuples[i][j].strValue;
                break;
            case 6:
                stLayer.lineTypeName = tableRowsTuples[i][j].strValue;
                break;
            case 62:
                stLayer.colorNumber = tableRowsTuples[i][j].iValue;
                break;
            case 70:
                stLayer.flags = tableRowsTuples[i][j].iValue;
                break;
            case 390:
                stLayer.lineWeightEnum = tableRowsTuples[i][j].strValue;
                break;
            default:
                break;
            }
        }
        if (stLayer.name != "") {
            pMapLayers->insert(stLayer.name, stLayer);
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::stylesTableHandler(const QList<ST_DXF_TYPE_A_VALUE> &stylesTuples, QMap<QString, ST_DXF_TABLE_STYLE>* pMapStyles)
{
    QList<QList<ST_DXF_TYPE_A_VALUE>> tableRowsTuples;
    QList<ST_DXF_TYPE_A_VALUE> tableRowTuples;

    for (int i = 0; i < stylesTuples.length(); ++i) {
        if ((stylesTuples[i].iType == 0 || stylesTuples[i].iType == 2) &&
                stylesTuples[i].enValue == EN_DXF_VALUE_STR && stylesTuples[i].strValue == "STYLE") {
            tableRowsTuples.append(tableRowTuples);
            tableRowTuples.clear();
        } else {
            tableRowTuples.append(stylesTuples[i]);
        }
    }

    for (int i = 0; i < tableRowsTuples.length(); ++i) {
        ST_DXF_TABLE_STYLE  stStyle;
        for (int j = 0; j < tableRowsTuples[i].length(); ++j) {
            switch (tableRowsTuples[i][j].iType) {
            case 2:
                stStyle.name = tableRowsTuples[i][j].strValue;
                break;
            case 6:
                stStyle.lineTypeName = tableRowsTuples[i][j].strValue;
                break;
            case 40:
                stStyle.fixedTextHeight = tableRowsTuples[i][j].dValue;
                break;
            case 41:
                stStyle.widthFactor = tableRowsTuples[i][j].dValue;
                break;
            case 50:
                stStyle.obliqueAngle = tableRowsTuples[i][j].dValue;
                break;
            case 71:
                stStyle.flags = tableRowsTuples[i][j].iValue;
                break;
            case 42:
                stStyle.lastHeightUsed = tableRowsTuples[i][j].dValue;
                break;
            case 3:
                stStyle.primaryFontFileName = tableRowsTuples[i][j].strValue;
                break;
            case 4:
                stStyle.bigFontFileName = tableRowsTuples[i][j].strValue;
                break;
            default:
                break;
            }
        }
        if (stStyle.name != "") {
            pMapStyles->insert(stStyle.name, stStyle);
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::blocksHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_BLOCKS* pstBlocks)
{
    QString state;
    ST_DXF_BLOCK block;
    QList<ST_DXF_TYPE_A_VALUE> entitiesTuples;

    for (int i = 0; i < tuples.length(); ++i) {
        if (tuples[i].enValue == EN_DXF_VALUE_STR && tuples[i].strValue == "BLOCK") {
            state = "block";
            block.xref = "";
            block.name = "";
            block.x = 0.0;
            block.y = 0.0;
            block.stEntities.listEntitie.clear();
            entitiesTuples.clear();
        } else if (tuples[i].enValue == EN_DXF_VALUE_STR && tuples[i].strValue == "ENDBLK") {
            if (state == "entities") {
                entitiesHandler(entitiesTuples, &(block.stEntities));
                pstBlocks->listbook.append(block);
            } else {
                block.stEntities.listEntitie.clear();
            }
            entitiesTuples.clear();
            state = "";
        } else if (state == "block" && tuples[i].iType != 0) {
            switch (tuples[i].iType) {
            case 1:
                block.xref = tuples[i].strValue;
                break;
            case 2:
                block.name = tuples[i].strValue;
                break;
            case 10:
                block.x = tuples[i].iValue;
                break;
            case 20:
                block.y = tuples[i].iValue;
            default:
                break;
            }
        } else if (state == "block" && tuples[i].iType == 0) {
            state = "entities";
            entitiesTuples.append(tuples[i]);
        } else if (state == "entities") {
            entitiesTuples.append(tuples[i]);
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::entitiesHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIES* pstEntities)
{
    QList<QList<ST_DXF_TYPE_A_VALUE>> entityGroups;
    QList<ST_DXF_TYPE_A_VALUE> currentEntityTuples;

    for (int i = 0; i < tuples.length(); ++i) {
        if (tuples[i].iType == 0 && currentEntityTuples.count() != 0) {
            entityGroups.append(currentEntityTuples);
            currentEntityTuples.clear();
        }
        currentEntityTuples.append(tuples[i]);
    }
    entityGroups.append(currentEntityTuples);


    ST_DXF_ENTITIE_POLYLINE* pCurrentPolyline = NULL;
    for (int i = 0; i < entityGroups.length(); ++i) {
        QString entityType = entityGroups[i][0].strValue;
        ST_DXF_ENTITIE  stEntitie;

        if (entityType == "ARC") {
            arcHandler(entityGroups[i].mid(1), &stEntitie);
            pstEntities->listEntitie.append(stEntitie);
        }
        if (entityType == "CIRCLE") {
            circleHandler(entityGroups[i].mid(1), &stEntitie);
            pstEntities->listEntitie.append(stEntitie);
        }
        if (entityType == "ELLIPSE") {
            ellipseHandler(entityGroups[i].mid(1), &stEntitie);
            pstEntities->listEntitie.append(stEntitie);
        }
        if (entityType == "INSERT") {
            insertHandler(entityGroups[i].mid(1), &stEntitie);
            pstEntities->listEntitie.append(stEntitie);
        }
        if (entityType == "LINE") {
            lineHandler(entityGroups[i].mid(1), &stEntitie);
            pstEntities->listEntitie.append(stEntitie);
        }
        if (entityType == "LWPOLYLINE") {
            lwpolylineHandler(entityGroups[i].mid(1), &stEntitie);
            pstEntities->listEntitie.append(stEntitie);
        }
        if (entityType == "POINT") {
            pointHandler(entityGroups[i].mid(1), &stEntitie);
            pstEntities->listEntitie.append(stEntitie);
        }
        if (entityType == "SOLID") {
            solidHandler(entityGroups[i].mid(1), &stEntitie);
            pstEntities->listEntitie.append(stEntitie);
        }
        if (entityType == "SPLINE") {
            splineHandler(entityGroups[i].mid(1), &stEntitie);
            pstEntities->listEntitie.append(stEntitie);
        }
        if (entityType == "POLYLINE") {
            polylineHandler(entityGroups[i].mid(1), &stEntitie);
            pstEntities->listEntitie.append(stEntitie);
            pCurrentPolyline = &(pstEntities->listEntitie[pstEntities->listEntitie.length() - 1].polyline);
        }
        if (entityType == "VERTEX" && pCurrentPolyline != NULL) {
            ST_DXF_ENTITIE_VERTEX stVertex;
            vertexHandler(entityGroups[i].mid(1), &stVertex);
            pCurrentPolyline->vertices.append(stVertex);
        }
        if (entityType == "SEQEND") {
            pCurrentPolyline = NULL;
        }
    }

}
//---------------------------------------------------------------------------
void DxfParser::commonHandler(const ST_DXF_TYPE_A_VALUE &tuple, ST_DXF_ENTITIE_COMMON* pstCommon)
{
    switch (tuple.iType) {
    case 6:
        pstCommon->lineTypeName = tuple.strValue;
        break;
    case 8:
        pstCommon->layer = tuple.strValue;
        break;
    case 48:
        pstCommon->lineTypeScale = tuple.dValue;
        break;
    case 60:
        if (tuple.iValue == 0) {
            pstCommon->visible = 1;
        } else {
            pstCommon->visible = 0;
        }
        break;
    case 62:
        pstCommon->colorNumber = tuple.iValue;
        break;
    case 210:
        pstCommon->extrusionX = tuple.dValue;
        break;
    case 220:
        pstCommon->extrusionY = tuple.dValue;
        break;
    case 230:
        pstCommon->extrusionZ = tuple.dValue;
        break;
    default:
        break;
    }
}
//---------------------------------------------------------------------------
void DxfParser::arcHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie)
{
    pstEntitie->enEntitie = EN_DXF_ENTITIE_ARC;

    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 10:
            pstEntitie->arc.x = tuples[i].dValue;
            break;
        case 20:
            pstEntitie->arc.y = tuples[i].dValue;
            break;
        case 39:
            pstEntitie->arc.thickness = tuples[i].dValue;
            break;
        case 40:
            pstEntitie->arc.r = tuples[i].dValue;
            break;
        case 50:
            pstEntitie->arc.startAngle = tuples[i].dValue / 180.0 * M_PI;
            break;
        case 51:
            pstEntitie->arc.endAngle = tuples[i].dValue / 180.0 * M_PI;
            break;
        default:
            commonHandler(tuples[i], &(pstEntitie->common));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::circleHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie)
{
    pstEntitie->enEntitie = EN_DXF_ENTITIE_CIRCLE;

    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 10:
            pstEntitie->arc.x = tuples[i].dValue;
            break;
        case 20:
            pstEntitie->arc.y = tuples[i].dValue;
            break;
        case 40:
            pstEntitie->arc.r = tuples[i].dValue;
            break;
        default:
            commonHandler(tuples[i], &(pstEntitie->common));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::ellipseHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie)
{
    pstEntitie->enEntitie = EN_DXF_ENTITIE_ELLIPSE;

    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 10:
            pstEntitie->ellipse.x = tuples[i].dValue;
            break;
        case 11:
            pstEntitie->ellipse.majorX = tuples[i].dValue;
            break;
        case 20:
            pstEntitie->ellipse.y = tuples[i].dValue;
            break;
        case 21:
            pstEntitie->ellipse.majorY = tuples[i].dValue;
            break;
        case 40:
            pstEntitie->ellipse.axisRatio = tuples[i].dValue;
            break;
        case 41:
            pstEntitie->ellipse.startAngle = tuples[i].dValue;
            break;
        case 42:
            pstEntitie->ellipse.endAngle = tuples[i].dValue;
            break;
        default:
            commonHandler(tuples[i], &(pstEntitie->common));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::insertHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie)
{
    pstEntitie->enEntitie = EN_DXF_ENTITIE_INSERT;

    // Initialize transform values.
    pstEntitie->insert.x = 0;
    pstEntitie->insert.y = 0;
    pstEntitie->insert.xscale = 1;
    pstEntitie->insert.yscale = 1;
    pstEntitie->insert.rotation = 0;

    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 2:
            pstEntitie->insert.block = tuples[i].strValue;
            break;
        case 10:
            pstEntitie->insert.x = tuples[i].dValue;
            break;
        case 20:
            pstEntitie->insert.y = tuples[i].dValue;
            break;
        case 41:
            pstEntitie->insert.xscale = tuples[i].dValue;
            break;
        case 42:
            pstEntitie->insert.yscale = tuples[i].dValue;
            break;
        case 44:
            pstEntitie->insert.columnSpacing = tuples[i].dValue;
            break;
        case 45:
            pstEntitie->insert.rowSpacing = tuples[i].dValue;
            break;
        case 50:
            pstEntitie->insert.rotation = tuples[i].dValue;
            break;
        case 70:
            pstEntitie->insert.columnCount = tuples[i].iValue;
            break;
        case 71:
            pstEntitie->insert.rowCount = tuples[i].iValue;
            break;
        case 210:
            pstEntitie->insert.xExtrusion = tuples[i].dValue;
            break;
        case 220:
            pstEntitie->insert.yExtrusion = tuples[i].dValue;
            break;
        default:
            commonHandler(tuples[i], &(pstEntitie->common));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::lineHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie)
{
    pstEntitie->enEntitie = EN_DXF_ENTITIE_LINE;

    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 10:
            pstEntitie->pos.x[0] = tuples[i].dValue;
            break;
        case 20:
            pstEntitie->pos.y[0] = tuples[i].dValue;
            break;
        case 39:
            pstEntitie->pos.thickness = tuples[i].dValue;
            break;
        case 11:
            pstEntitie->pos.x[1] = tuples[i].dValue;
            break;
        case 21:
            pstEntitie->pos.y[1] = tuples[i].dValue;
            break;
        default:
            commonHandler(tuples[i], &(pstEntitie->common));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::lwpolylineHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie)
{
    pstEntitie->enEntitie = EN_DXF_ENTITIE_LWPOLYLINE;

    ST_DXF_ENTITIE_VERTEX vertex;
    ST_DXF_ENTITIE_VERTEX* pVertex = NULL;
    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 70:
            pstEntitie->polyline.closed = (tuples[i].iValue & 1) == 1;
            break;
        case 10:
            vertex.x = tuples[i].dValue;
            vertex.y = 0.0;
            vertex.bulge = 0.0;
            pstEntitie->polyline.vertices.append(vertex);
            pVertex = &(pstEntitie->polyline.vertices[pstEntitie->polyline.vertices.length() - 1]);
            break;
        case 20:
            if (pVertex != NULL) {
                pVertex->y = tuples[i].dValue;
            }
            break;
        case 39:
            pstEntitie->polyline.thickness = tuples[i].dValue;
            break;
        case 42:
            if (pVertex != NULL) {
                pVertex->bulge = tuples[i].dValue;
            }
            break;
        default:
            commonHandler(tuples[i], &(pstEntitie->common));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::pointHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie)
{
    pstEntitie->enEntitie = EN_DXF_ENTITIE_POINT;

    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 10:
            pstEntitie->pos.x[0] = tuples[i].dValue;
            break;
        case 20:
            pstEntitie->pos.y[0] = tuples[i].dValue;
            break;
        case 39:
            pstEntitie->pos.thickness = tuples[i].dValue;
            break;
        default:
            commonHandler(tuples[i], &(pstEntitie->common));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::solidHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie)
{
    pstEntitie->enEntitie = EN_DXF_ENTITIE_SOLID;

    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 10:
            pstEntitie->pos.x[0] = tuples[i].dValue;
            break;
        case 20:
            pstEntitie->pos.y[0] = tuples[i].dValue;
            break;
        case 11:
            pstEntitie->pos.x[1] = tuples[i].dValue;
            break;
        case 21:
            pstEntitie->pos.y[1] = tuples[i].dValue;
            break;
        case 12:
            pstEntitie->pos.x[2] = tuples[i].dValue;
            break;
        case 22:
            pstEntitie->pos.y[2] = tuples[i].dValue;
            break;
        case 13:
            pstEntitie->pos.x[3] = tuples[i].dValue;
            break;
        case 23:
            pstEntitie->pos.y[3] = tuples[i].dValue;
            break;
        case 39:
            pstEntitie->pos.thickness = tuples[i].dValue;
            break;
        default:
            commonHandler(tuples[i], &(pstEntitie->common));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::splineHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie)
{
    pstEntitie->enEntitie = EN_DXF_ENTITIE_SPLINE;

    QPointF controlPoint;
    QPointF* pControlPoint = NULL;;
    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 10:
            controlPoint.setX(tuples[i].dValue);
            controlPoint.setY(0.0);
            pstEntitie->spline.controlPoints.append(controlPoint);
            pControlPoint = &(pstEntitie->spline.controlPoints[pstEntitie->spline.controlPoints.length() - 1]);
            break;
        case 20:
            if (pControlPoint != NULL) {
                pControlPoint->setY(tuples[i].dValue);
            }
            break;
        case 40:
            pstEntitie->spline.knots.append(tuples[i].dValue);
            break;
        case 42:
            pstEntitie->spline.knotTolerance = tuples[i].dValue;
            break;
        case 43:
            pstEntitie->spline.controlPointTolerance = tuples[i].dValue;
            break;
        case 44:
            pstEntitie->spline.fitTolerance = tuples[i].dValue;
            break;
        case 70:
            pstEntitie->spline.flag = tuples[i].iValue;
            pstEntitie->spline.closed = (tuples[i].iValue & 1) == 1;
            break;
        case 71:
            pstEntitie->spline.degree = tuples[i].iValue;
            break;
        case 72:
            pstEntitie->spline.numberOfKnots = tuples[i].iValue;
            break;
        case 73:
            pstEntitie->spline.numberOfControlPoints = tuples[i].iValue;
            break;
        case 74:
            pstEntitie->spline.numberOfFitPoints = tuples[i].iValue;
            break;
        default:
            commonHandler(tuples[i], &(pstEntitie->common));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::polylineHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE* pstEntitie)
{
    pstEntitie->enEntitie = EN_DXF_ENTITIE_POLYLINE;

    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 70:
            pstEntitie->polyline.closed = (tuples[i].iValue & 1) == 1;
            break;
        case 39:
            pstEntitie->polyline.thickness = tuples[i].dValue;
            break;
        default:
            commonHandler(tuples[i], &(pstEntitie->common));
            break;
        }
    }
}
//---------------------------------------------------------------------------
void DxfParser::vertexHandler(const QList<ST_DXF_TYPE_A_VALUE> &tuples, ST_DXF_ENTITIE_VERTEX* pstVertex)
{
    for (int i = 0; i < tuples.length(); ++i) {
        switch (tuples[i].iType) {
        case 10:
            pstVertex->x = tuples[i].dValue;
            break;
        case 20:
            pstVertex->y = tuples[i].dValue;
            break;
        case 42:
            pstVertex->bulge = tuples[i].dValue;
            break;
        default:
            break;
        }
    }
}
//---------------------------------------------------------------------------
QList<ST_DXF_POINT> DxfParser::createArcForLWPolyine(const ST_DXF_POINT from, const ST_DXF_POINT to, const qreal bulge)
{
    // Resolution in degrees
    int resolution = 5;

    // If the bulge is < 0, the arc goes clockwise. So we simply
    // reverse a and b and invert sign
    // Bulge = tan(theta/4)
    qreal theta;
    QVector2D a;
    QVector2D b;
    if(bulge < 0) {
        theta = qAtan(-bulge) * 4;
        a = QVector2D(from.x, from.y);
        b = QVector2D(to.x, to.y);
    } else {
        theta = qAtan(bulge) * 4;
        a = QVector2D(to.x, to.y);
        b = QVector2D(from.x, from.y);
    }

    QVector2D ab = b - a;
    qreal     l_ab = ab.length();
    QVector2D c = a + (ab * 0.5);
    qreal     l_cd = qAbs((l_ab / 2) / qTan(theta / 2));
    QVector2D norm_ab = ab.normalized();

    QVector2D d;
    if (theta < M_PI) {
        QVector2D norm_dc(norm_ab.x() * qCos(M_PI / 2) - norm_ab.y() * qSin(M_PI / 2),
                          norm_ab.y() * qCos(M_PI / 2) + norm_ab.x() * qSin(M_PI / 2));
        // D is the center of the arc
        d = c + (norm_dc * -l_cd);
    } else {
        QVector2D norm_cd(norm_ab.x() * qCos(M_PI / 2) - norm_ab.y() * qSin(M_PI / 2),
                          norm_ab.y() * qCos(M_PI / 2) + norm_ab.x() * qSin(M_PI / 2));
        // D is the center of the arc
        d = c + (norm_cd * l_cd);
    }

    // Add points between start start and eng angle relative
    // to the center point
    qreal startAngle = qAtan2(b.y() - d.y(), b.x() - d.x()) / M_PI * 180;
    qreal endAngle   = qAtan2(a.y() - d.y(), a.x() - d.x()) / M_PI * 180;
    if(endAngle < startAngle) {
        endAngle += 360;
    }
    qreal r = (b - d).length();

    int startInter = qFloor(startAngle / resolution) * resolution + resolution;
    int endInter   = qCeil (endAngle   / resolution) * resolution - resolution;

    QList<ST_DXF_POINT> points;
    for(int i = startInter; i <= endInter; i += resolution) {

        QVector2D point = d + QVector2D(qCos(qreal(i) / 180 * M_PI) * r, qSin(qreal(i) / 180 * M_PI) * r);
        points.append(ST_DXF_POINT(point.x(), point.y()));
    }

    // Maintain the right ordering to join the from and to points
    if (bulge < 0) {
        for(int k = 0; k < (points.size() / 2); k++) { points.swap(k, points.size() - (1 + k)); }
    }
    return points;
}
//---------------------------------------------------------------------------
ST_DXF_POINT DxfParser::bspline(qreal t, int order, QList<QPointF> points, QList<double> knots)
{
    ST_DXF_POINT nullPoint(0, 0);

    int n = points.length();
    int d = 2; // 2-d only

    if(order < 2) {
        CRITICAL_STRING("order must be at least 2 (linear)");
        return nullPoint;
    }
    if(order > n) {
        CRITICAL_STRING("order must be less than point count");
        return nullPoint;
    }
    // build weight vector of length [n]
    QList<int> weights;
    for(int i = 0; i < n; i++) {
        weights.append(1);
    }
    // build knot vector of length [n + order]
    if(knots.length() == 0) {
        for(int i = 0; i < n + order; i++) {
            knots.append(i);
        }
    } else {
        if(knots.length() != n + order) {
            CRITICAL_STRING("bad knot vector length");
            return nullPoint;
        }
    }

    int domain[] = {order - 1, knots.length() - 1 - (order - 1)};

    // remap t to the domain where the spline is defined
    int low  = knots[domain[0]];
    int high = knots[domain[1]];
    t = t * (high - low) + low;

    if(t < low || t > high) {
        CRITICAL_STRING("out of bounds");
        return nullPoint;
    }
    // find s (the spline segment) for the [t] value provided
    int s = 0;
    for(s = domain[0]; s < domain[1]; s++) {
        if(t >= knots[s] && t <= knots[s + 1]) {
            break;
        }
    }
    // convert points to homogeneous coordinates
    QList<QList<qreal>> v;
    for(int i = 0; i < n; i++) {
        QList<qreal> vi;
        vi.append(points[i].x() * weights[i]);
        vi.append(points[i].y() * weights[i]);
        vi.append(weights[i]);
        v.append(vi);
    }
    for(int i = n; i <= s; i++) {
        QList<qreal> vi;
        vi.append(0);
        vi.append(0);
        vi.append(0);
        v.append(vi);
    }
    // l (level) goes from 1 to the curve order
    qreal alpha = 0;
    for(int l = 1; l <= order; l++) {
        for(int i = s; i > s - order + l; i--) {
            alpha = (t - knots[i]) / (knots[i + order - l] - knots[i]);

            for(int j = 0; j < d + 1; j++) {
                v[i][j] = (1 - alpha) * v[i - 1][j] + alpha * v[i][j];
            }
        }
    }

    ST_DXF_POINT result;
    result.x = v[s][0] / v[s][2];
    result.y = v[s][1] / v[s][2];

    return result;
}
//---------------------------------------------------------------------------
QList<ST_DXF_POINT> DxfParser::interpolateElliptic(qreal cx, qreal cy, qreal rx, qreal ry, qreal start, qreal end, qreal rotationAngle)
{
    if(end < start) {
        end += M_PI * 2;
    }

    // ----- Relative points -----
    // Start point
    QList<ST_DXF_POINT> points;

    qreal dTheta = M_PI * 2 / 72;
    qreal EPS = 1e-6;
    for (qreal theta = start; theta < end - EPS; theta += dTheta) {
        ST_DXF_POINT point( qCos(theta)*rx, qSin(theta)*ry );
        points.append(point);
    }
    ST_DXF_POINT endPoint( qCos(end)*rx, qSin(end)*ry );
    points.append(endPoint);

    // ----- Rotate -----
    for(int i = 0; i < points.count(); i++) {
        points[i].rotate(rotationAngle);
    }
    // ----- Offset center -----
    for(int i = 0; i < points.count(); i++) {
        points[i].shift(cx, cy);
    }
    return points;
}
//---------------------------------------------------------------------------
QList<ST_DXF_ENTITIE> DxfParser::denormalise(const QList<ST_DXF_ENTITIE> &entities, const QList<ST_DXF_TRANSFORM> &transforms, const ST_DXF_BLOCKS &blocks)
{
    QList<ST_DXF_ENTITIE> current;

    for(ST_DXF_ENTITIE entitie : entities) {

        if(entitie.enEntitie == EN_DXF_ENTITIE_INSERT) {

            ST_DXF_BLOCK block;
            for(ST_DXF_BLOCK itrBlock : blocks.listbook) {
                if(itrBlock.name == entitie.insert.block) {
                    block = itrBlock;
                    break;
                }
            }

            ST_DXF_TRANSFORM transform;
            transform.x = entitie.insert.x + block.x;
            transform.y = entitie.insert.y + block.y;
            transform.xScale = entitie.insert.xscale;
            transform.yScale = entitie.insert.yscale;
            transform.rotation = entitie.insert.rotation;

            QList<ST_DXF_TRANSFORM> copiedTransforms = transforms;
            copiedTransforms.append(transform);

            // Use the insert layer
            QList<ST_DXF_ENTITIE> blockEntities;
            for(int i = 0; i < block.stEntities.listEntitie.length(); i++) {
                ST_DXF_ENTITIE copiedEntite = block.stEntities.listEntitie[i];
                copiedEntite.common.layer = entitie.common.layer;
                blockEntities.append(copiedEntite);
            }
            current.append(denormalise(blockEntities, copiedTransforms, blocks));

        } else {
            ST_DXF_ENTITIE copiedEntitie = entitie;
            copiedEntitie.transforms = transforms;
            current.append(copiedEntitie);
        }
    }
    return current;
}
//---------------------------------------------------------------------------
QList<ST_DXF_POINT> DxfParser::entityToPolyline(const ST_DXF_ENTITIE &entitie)
{

    QList<ST_DXF_POINT> polyline;

    if(entitie.enEntitie == EN_DXF_ENTITIE_LINE) {

        ST_DXF_POINT start(entitie.pos.x[0], entitie.pos.y[0]);
        ST_DXF_POINT   end(entitie.pos.x[1], entitie.pos.y[1]);
        polyline.append(start);
        polyline.append(end);
    }
    if(entitie.enEntitie == EN_DXF_ENTITIE_LWPOLYLINE || entitie.enEntitie == EN_DXF_ENTITIE_POLYLINE) {

        int il = entitie.polyline.vertices.length();
        for(int i = 0; i < il - 1; i++) {

            ST_DXF_POINT from(entitie.polyline.vertices[i].x, entitie.polyline.vertices[i].y);
            ST_DXF_POINT to(entitie.polyline.vertices[i + 1].x, entitie.polyline.vertices[i + 1].y);
            polyline.append(from);

            if(entitie.polyline.vertices[i].bulge != 0) {
                QList<ST_DXF_POINT> bulgePoints = createArcForLWPolyine(from, to, entitie.polyline.vertices[i].bulge);
                polyline.append(bulgePoints);
            }
            if(i == il - 2) {
                polyline.append(to);
            }
        }
        if(entitie.polyline.closed == 1) {
            polyline.append(polyline[0]);
        }
    }
    if(entitie.enEntitie == EN_DXF_ENTITIE_CIRCLE) {

        polyline = interpolateElliptic(entitie.arc.x, entitie.arc.y,
                                       entitie.arc.r, entitie.arc.r,
                                       0, M_PI * 2,
                                       0);
    }
    if(entitie.enEntitie == EN_DXF_ENTITIE_ELLIPSE) {
        qreal rx = qSqrt(qPow(entitie.ellipse.majorX, 2) + qPow(entitie.ellipse.majorY, 2));
        qreal ry = entitie.ellipse.axisRatio * rx;
        qreal majorAxisRotation = -qAtan2(-entitie.ellipse.majorY, entitie.ellipse.majorX);
        polyline = interpolateElliptic(entitie.ellipse.x, entitie.ellipse.y,
                                       rx, ry,
                                       entitie.ellipse.startAngle, entitie.ellipse.endAngle,
                                       majorAxisRotation);

        // JS版では extrusionZ === -1 のときにY軸に対して反転させているが割愛する
    }
    if(entitie.enEntitie == EN_DXF_ENTITIE_ARC) {
        polyline = interpolateElliptic(entitie.arc.x, entitie.arc.y,
                                       entitie.arc.r, entitie.arc.r,
                                       entitie.arc.startAngle,
                                       entitie.arc.endAngle,
                                       0);

        // JS版では extrusionZ === -1 のときにY軸に対して反転させているが割愛する
    }
    if(entitie.enEntitie == EN_DXF_ENTITIE_SPLINE) {

        int order = entitie.spline.degree + 1;
        QList<double> knots = entitie.spline.knots;

        for(int t = 0; t <= 100; t++) {
            ST_DXF_POINT point = bspline(qreal(t / 100.0), order, entitie.spline.controlPoints, knots);
            polyline.append(point);
        }
    }

    // applyTransform
    // JS版ではあらかじめ transforms の順番を反転させているが、こちらでは配列の後ろから変換を適用させて対応している
    if(entitie.transforms.length() != 0) {
        for(int t = entitie.transforms.length() - 1; t >= 0; t--) {
            for(int p = 0; p < polyline.length(); p++) {
                polyline[p].scale(entitie.transforms[t].xScale, entitie.transforms[t].yScale);
                polyline[p].rotate(entitie.transforms[t].rotation / 180 * M_PI);
                polyline[p].shift(entitie.transforms[t].x, entitie.transforms[t].y);
            }
        }
    }

    return polyline;
}
//---------------------------------------------------------------------------
void DxfParser::toSVG(const ST_DXF_SECTIONS & stSections, QByteArray & strSvg)
{
    QDomDocument doc;
    QDomElement  root = doc.createElement("svg");

    // Generator label
    QDomComment  comment = doc.createComment("Generator: FaboolDxfToSvg");
    doc.appendChild(comment);

    // toSVGs
    QList<ST_DXF_TRANSFORM> transforms;
    QList<ST_DXF_ENTITIE> entities = denormalise(stSections.stEntities.listEntitie, transforms, stSections.stBlocks);

    QList<QList<ST_DXF_POINT>> polylines;
    for(int i = 0; i < entities.count(); i++) {
        QList<ST_DXF_POINT> polyline = entityToPolyline(entities[i]);
        polylines.append(polyline);
    }

    // Bounding box
    ST_DXF_BOUNDINGBOX bbox;
    for(int i = 0; i < polylines.length(); i++) {
        for(int j = 0; j < polylines[i].length(); j++) {
            bbox.expandByPoint(polylines[i][j]);
        }
    }

    for(int l = 0; l < polylines.length(); l++) {
        for(int p = 0; p < polylines[l].length(); p++) {
            polylines[l][p].y = bbox.max.y - polylines[l][p].y;
        }

        // Layer
        ST_DXF_ENTITIE entitie = entities[l];
        ST_DXF_TABLE_LAYER layerTable = stSections.stTables.mapLayers[entitie.common.layer];
        stSections.stTables.mapLayers.find(entitie.common.layer);

        QColor color = DXF_DEFINED_COLOR::IndexOf[layerTable.colorNumber];
        if(color.name() == "#ffffff") {
            color = QColor(0, 0, 0);
        }

        QString d = "";
        for(int p = 0; p < polylines[l].length(); p++) {
            d += (p == 0) ? "M" : "L";
            d += QString("%1,%2").arg(polylines[l][p].x).arg(polylines[l][p].y);
        }

        QDomElement element = doc.createElement("path");
        element.setAttribute("fill",         "none");
        element.setAttribute("stroke",       color.name());
        element.setAttribute("stroke-width", "1");
        element.setAttribute("d", d);
        root.appendChild(element);
    }

    // SVG全体の属性設定
    QString viewBoxStr = QString("%1 %2 %3 %4").arg(-1 + bbox.min.x).arg(-1).arg(bbox.width() + 2).arg(bbox.height() + 2);
    root.setAttribute("xmlns", "http://www.w3.org/2000/svg");
    root.setAttribute("xmlns:xlink", "http://www.w3.org/1999/xlink");
    root.setAttribute("version", "1.1");
    root.setAttribute("preserveAspectRatio", "xMinYMin meet");
    root.setAttribute("viewBox", viewBoxStr);
    root.setAttribute("width",  "100%");
    root.setAttribute("height", "100%");
    doc.appendChild(root);

    strSvg = doc.toByteArray();
}
//---------------------------------------------------------------------------
