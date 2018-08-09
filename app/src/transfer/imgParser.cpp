//---------------------------------------------------------------------------
#include "imgParser.h"
#include <QtGui>
#include <QtXml>
#include "unitTransfer.h"
#include <qmath.h>
//---------------------------------------------------------------------------
#define DF_IMG_COLOR_NUM            3
#define DF_IMG_RASTER_WIDTH_MAX     1024
#define DF_CHUNK_SIZE               59 //mean  64 char - ”\nG8 D”

//---------------------------------------------------------------------------
ImgParser::ImgParser(int rasterDealType):
    ParserCore()
{
    initMenber();
    m_rasterDealType = rasterDealType;
}
//---------------------------------------------------------------------------
ImgParser::ImgParser(const ImgParser &src):
    ParserCore()
{
    initMenber();
    copyMenber(src);
}
//---------------------------------------------------------------------------
ImgParser::~ImgParser()
{
    deleteMenber();
}
//---------------------------------------------------------------------------
ImgParser &ImgParser::operator=(const ImgParser &src)
{
    deleteMenber();

    return copyMenber(src);
}
//---------------------------------------------------------------------------
void ImgParser::initMenber()
{
    // 初期化
    m_dDpi = 90.0;
    m_ImgWidth = 0;
    m_ImgHeight = 0;
    m_pucImg = NULL;
}
//---------------------------------------------------------------------------
void ImgParser::deleteMenber()
{
    // メモリ解放
    if (m_pucImg != NULL) {
        delete m_pucImg;
    }

    initMenber();
}
//---------------------------------------------------------------------------
ImgParser &ImgParser::copyMenber(const ImgParser &src)
{
    quint32 dImgLen;
    m_ImgWidth = src.m_ImgWidth;
    m_ImgHeight = src.m_ImgHeight;
    dImgLen = m_ImgWidth * m_ImgHeight;

    if (src.m_pucImg != NULL && dImgLen > 0) {
        m_pucImg = new quint8[dImgLen];
        memcpy(m_pucImg, src.m_pucImg, sizeof(quint8) * dImgLen);
    }

    return *this;
}
//---------------------------------------------------------------------------
// 初期位置
QPointF ImgParser::getStartPoint()
{
    return QPointF();
}
//---------------------------------------------------------------------------
// 画像読み込み
bool ImgParser::loadFile(const QString &strImgPath)
{
    QImage image;
    int idx;

    // 画像ファイルを読み込む
    if (image.load(strImgPath) == false) {
        return false;
    }
    // 画像サイズの変更
    if (image.width() >= DF_IMG_RASTER_WIDTH_MAX) {
        qreal scale = (DF_IMG_RASTER_WIDTH_MAX - 1) / ((qreal)image.width());
        qreal dstWidth = image.width() * scale;
        qreal dstHeight = image.height() * scale;
        image = image.scaled(dstWidth, dstHeight, Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }

    // 画像を1次元配列に保存
    m_ImgWidth = image.width();
    m_ImgHeight = image.height();

    m_pucImg = new quint8[m_ImgWidth * m_ImgHeight];
    for (int i = 0; i < m_ImgHeight; ++i) {
        for (int j = 0; j < m_ImgWidth; ++j) {
            // グレースケール変換
            idx = (j + i * m_ImgWidth);
            if((quint8)qAlpha(image.pixel(j, i)) == 255) //透過の場合は255（＝白）で固定
            { m_pucImg[idx] = (quint8)qGray(image.pixel(j, i)); }
            else
            { m_pucImg[idx] = 255; }// 透過画像対策
        }
    }

    if(m_rasterDealType == SYS_PARA_RASTER_DEAL_F_S_METHOD) {
        froydSteinbergMethod();
    } else {
        binarization();
    }

    return true;
}
//---------------------------------------------------------------------------
void ImgParser::froydSteinbergMethod()
{
    int idx;
    int err;
    int newPixel;

    for (int i = 0; i < m_ImgHeight; ++i) {
        for (int j = 0; j < m_ImgWidth; ++j) {
            idx = (j + i * m_ImgWidth);
            newPixel = m_pucImg[idx] < 128 ? 0 : 255;
            err = ((int)m_pucImg[idx]) - newPixel;
            m_pucImg[idx] = (quint8)newPixel;

            if ((i != (m_ImgHeight - 1)) && (j != 0) && (j != (m_ImgWidth - 1))) {
                m_pucImg[idx + 1                 ] = saturated_add(m_pucImg[idx + 1                 ], (err * 7) / 16);
                m_pucImg[idx + 1 * m_ImgWidth - 1] = saturated_add(m_pucImg[idx + 1 * m_ImgWidth - 1], (err * 3) / 16);
                m_pucImg[idx + 1 * m_ImgWidth    ] = saturated_add(m_pucImg[idx + 1 * m_ImgWidth    ], (err * 5) / 16);
                m_pucImg[idx + 1 * m_ImgWidth + 1] = saturated_add(m_pucImg[idx + 1 * m_ImgWidth + 1], (err * 1) / 16);
            }
        }
    }
}
//---------------------------------------------------------------------------
void ImgParser::binarization()
{
    int sum = 0;
    for(int i = 0; i < m_ImgWidth * m_ImgHeight; i++) {
        sum += (int)m_pucImg[i];
    }
    int avg = sum / ((int)m_ImgWidth * (int)m_ImgHeight);

    if(avg == 0){
        avg = 1;
    }

    for (int y = 0; y < m_ImgHeight; ++y) {
        for (int x = 0; x < m_ImgWidth; ++x) {
            int newPix = m_pucImg[x + y * m_ImgWidth] < avg ? 0 : 255;
            m_pucImg[x + y * m_ImgWidth] = (quint8)newPix;
        }
    }

}
//---------------------------------------------------------------------------
quint8 ImgParser::saturated_add(quint8 val1, int val2)
{
    qint16 val1_int = val1;
    qint16 val2_int = val2;
    qint16 tmp = val1_int + val2_int;

    if(tmp > 255) {
        return 255;
    } else if(tmp < 0) {
        return 0;
    } else {
        return (quint8)tmp;
    }
}
//---------------------------------------------------------------------------
// 描画用のSVG文字列を返す
QByteArray ImgParser::toByte()
{
    int idx;
    quint8* pucShowImg = NULL;
    QString strAttr;
    QDomDocument doc;
    QDomElement root;
    QDomElement node;

    root = doc.createElement("svg");
    strAttr = QString("0 0 %1 %2").arg(m_ImgWidth).arg(m_ImgHeight);
    root.setAttribute("viewBox", strAttr);
    doc.appendChild(root);

    pucShowImg = new quint8[m_ImgWidth * m_ImgHeight * 4];
    for (int i = 0; i < m_ImgHeight; ++i) {
        for (int j = 0; j < m_ImgWidth; ++j) {
            idx = (j + i * m_ImgWidth);
            pucShowImg[idx * 4] = m_pucImg[idx];
            pucShowImg[idx * 4 + 1] = m_pucImg[idx];
            pucShowImg[idx * 4 + 2] = m_pucImg[idx];
            pucShowImg[idx * 4 + 3] = 255;
        }
    }
    QImage image(pucShowImg, m_ImgWidth, m_ImgHeight, QImage::Format_ARGB32);

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");

    node = doc.createElement("image");
    node.setAttribute("x", "0");
    node.setAttribute("y", "0");
    node.setAttribute("width", QString::number(m_ImgWidth));
    node.setAttribute("height", QString::number(m_ImgHeight));
    node.setAttribute("xlink:href", "data:image/png;base64," + QString::fromLatin1(byteArray.toBase64().data()));
    root.appendChild(node);

    if (pucShowImg != NULL) {
        delete[] pucShowImg;
    }

    return doc.toByteArray();
}
//---------------------------------------------------------------------------
// Jsonへ書き込む
void ImgParser::writeDown(QJsonObject &json) const
{
    quint8* pucShowImg = new quint8[m_ImgWidth * m_ImgHeight * 4];
    for (int i = 0; i < m_ImgHeight; ++i) {
        for (int j = 0; j < m_ImgWidth; ++j) {
            int idx = (j + i * m_ImgWidth);
            pucShowImg[idx * 4] = m_pucImg[idx];
            pucShowImg[idx * 4 + 1] = m_pucImg[idx];
            pucShowImg[idx * 4 + 2] = m_pucImg[idx];
            pucShowImg[idx * 4 + 3] = 255;
        }
    }
    QImage image(pucShowImg, m_ImgWidth, m_ImgHeight, QImage::Format_ARGB32);

    QByteArray byteArray;
    QBuffer buffer(&byteArray);
    buffer.open(QIODevice::WriteOnly);
    image.save(&buffer, "PNG");

    json[JSON_TAG_IMG_DATA] = QString(byteArray.toBase64().data());

    delete[] pucShowImg;

}
//---------------------------------------------------------------------------
// Jsonから読み込んで解析
int ImgParser::readData(const QJsonObject &json)
{
    QString data = json[JSON_TAG_IMG_DATA].toString();

    QByteArray byteArray = QByteArray::fromBase64(data.toLocal8Bit().data());

    QImage image;
    int idx;

    QBuffer buffer(&byteArray);
    if(buffer.open(QIODevice::ReadOnly) == false) {
        CRITICAL_STRING("Cannot open Img Buffer");
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }

    if(image.load(&buffer, "PNG") == false) {
        CRITICAL_STRING("Failed to load Img");
        return ERR_PROJ_LOAD_INVAILD_DATA;
    }

    m_ImgHeight = image.height();
    m_ImgWidth  = image.width();

    m_pucImg = new quint8[m_ImgWidth * m_ImgHeight];
    for (int i = 0; i < m_ImgHeight; ++i) {
        for (int j = 0; j < m_ImgWidth; ++j) {
            idx = (j + i * m_ImgWidth);
            m_pucImg[idx] = (quint8)qGray(image.pixel(j, i));
        }
    }

    return SUCCESS;
}
//---------------------------------------------------------------------------
// 使う色を返す
QList<QColor> ImgParser::colorMap()
{
    QList<QColor>list = {(QColor(Qt::black))};
    return list;
}
//---------------------------------------------------------------------------
QStringList ImgParser::toGcode(qreal d1, qreal d2, qreal d3, qreal d4, qreal d5, qreal d6, QList<ParamData*> paramList)
{
    mNeededTime = 0;
    QStringList gcode;
    // 開始位置xy(mm)
    QPointF sPointMM = UnitTransfer::SvgPixelPointto(QPointF(0.0, 0.0), EN_SVG_UNIT_MM, m_dDpi);
    sPointMM = applyTransform(sPointMM, d1, d2, d3, d4, d5, d6);

    // 幅・高さ(mm)
    qreal   widthMM;
    qreal   heightMM;
    widthMM = UnitTransfer::SvgPixelto((qreal)m_ImgWidth, EN_SVG_UNIT_MM, m_dDpi);
    heightMM = UnitTransfer::SvgPixelto((qreal)m_ImgHeight, EN_SVG_UNIT_MM, m_dDpi);
    applyTransformWithOutPos(widthMM, heightMM, &widthMM, &heightMM, d1, d2, d3, d4);

    // 加工パラメーター
    qint16 processTime = paramList[0]->processTimes();
    qint16 headSpeed = paramList[0]->headSpeed();
    qreal lasetPower = (qreal)(paramList[0]->laserPower()) * 255 / 100;

    qreal dot_pitch = widthMM / (qreal)m_ImgWidth;
    qreal offset = 0.5 * headSpeed * headSpeed / 8000000.0;
    offset *= 1.1;
    if (offset < 5.0) {
        offset = 5.0;
    }

    // 単体Gコード作成
    gcode.append("G00X" + QString::number(sPointMM.x(), 'f', 2) + "Y" + QString::number(sPointMM.y(), 'f', 2) + "\n");
    gcode.append("G08P" + QString::number(dot_pitch, 'f', 4) + "\n");
    gcode.append("G08X" + QString::number(offset, 'f', 2) + "Z0\n");
    gcode.append("G08N0\n");
    gcode.append("S" + QString::number((int)lasetPower) + "\n");

    bool reverse = false;
    int LineCnt = 0;
    int chunksize = DF_CHUNK_SIZE;

    bool emptyPrev = false;
    int limY = int(m_ImgHeight);
    for (int y = 0; y < limY; y += 1) {
        int line = y * m_ImgWidth;
        int count = 0;
        bool empty = true;
        QString raster = "G8 D";

        if (reverse == false) {
            int limX = int(m_ImgWidth);
            for (int x = 0; x < limX; x += 1) {
                int pixel = line + x;
                if (m_pucImg[pixel] == 0) {
                    raster += "1";
                    empty = false;
                } else {
                    raster += "0";
                }
                count++;
                if (count % chunksize == 0) {
                    raster += "\nG8 D";
                }
            }
        } else {
            int limX = int(m_ImgWidth) - 1;
            for (int x = limX; x >= 0; x -= 1) {
                int pixel = line + x;
                if (m_pucImg[pixel] == 0) {
                    raster += "1";
                    empty = false;
                } else {
                    raster += "0";
                }
                count++;
                if (count % chunksize == 0) {
                    raster += "\nG8 D";
                }
            }
        }
        if (empty == false) {
            if (reverse == false) {
                gcode.append("G8 R0\n");
                reverse = true;
            } else {
                gcode.append("G8 R1\n");
                reverse = false;
            }
            foreach (QString str, raster.split("\n")) {
                gcode.append(str + "\n");
            }
            gcode.append("G8 N0\n");
            emptyPrev = false;
        } else {
            if(emptyPrev == true) { gcode.removeLast(); }
            gcode.append("G00X" + QString::number(sPointMM.x(), 'f', 2) + "Y" + QString::number(sPointMM.y() + (dot_pitch * LineCnt), 'f', 2) + "\n");
            reverse = false;
            emptyPrev = true;
        }
        LineCnt++;
    }

    QStringList allGcode;
    allGcode.append("S0\n");
    allGcode.append("G1F" + QString::number(headSpeed) + "\n");
    for (int i = 0; i < processTime; ++i) {
        allGcode.append(gcode);
        allGcode.append("\n~\n");
    }

    return allGcode;
}
//---------------------------------------------------------------------------
QSizeF ImgParser::outerSize()
{
    return QSizeF(m_ImgWidth, m_ImgHeight);
}
//---------------------------------------------------------------------------
qreal ImgParser::dpi()
{
    return 90.0;
}
//---------------------------------------------------------------------------
QPointF ImgParser::applyTransform(const QPointF &p, qreal d1, qreal d2, qreal d3, qreal d4, qreal d5, qreal d6)
{
    return QPointF(d1 * p.x() + d3 * p.y() + d5, d2 * p.x() + d4 * p.y() + d6);
}
//---------------------------------------------------------------------------
void ImgParser::applyTransformWithOutPos(const qreal &sw, const qreal &sh, qreal *pdw, qreal *pdh, qreal d1, qreal d2, qreal d3, qreal d4)
{
    *pdw = d1 * sw + d3 * sh;
    *pdh = d2 * sw + d4 * sh;
}
//---------------------------------------------------------------------------
