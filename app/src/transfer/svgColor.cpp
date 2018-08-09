//---------------------------------------------------------------------------
#include "svgColor.h"
//---------------------------------------------------------------------------
SvgColor::SvgColor()
{
    initMenber();
}
//---------------------------------------------------------------------------
SvgColor::SvgColor(const SvgColor &src)
{
    initMenber();
    copyMenber(src);
}
//---------------------------------------------------------------------------
SvgColor::~SvgColor()
{
    deleteMenber();
}
//---------------------------------------------------------------------------
SvgColor &SvgColor::operator=(const SvgColor &src)
{
    deleteMenber();

    return copyMenber(src);
}
//---------------------------------------------------------------------------
void SvgColor::initMenber()
{
    color.setRgb(0, 0, 0);
}
//---------------------------------------------------------------------------
void SvgColor::deleteMenber()
{
}
//---------------------------------------------------------------------------
SvgColor &SvgColor::copyMenber(const SvgColor &src)
{
    color = src.color;

    return *this;
}
//---------------------------------------------------------------------------
bool SvgColor::setColor(const QString &strColor)
{
    QString strBuf = strColor.trimmed();
    QStringList list;
    int iRgba[3];

    if (strBuf.startsWith("rgb(") && strBuf.endsWith(")")) {
        // rgb
        list = strBuf.mid(4, strBuf.length() - 5).split(',');
        if (list.length() != 3) {
            color.setNamedColor("none");
            return false;
        }
        // rgb取得
        if (getRgb(list, iRgba) == false) {
            color.setNamedColor("none");
            return false;
        }
        // 色設定
        color.setRgb(iRgba[0], iRgba[1], iRgba[2]);
    } else if (strBuf.startsWith("rgba(") && strBuf.endsWith(")")) {
        // rgba
        list = strBuf.mid(5, strBuf.length() - 6).split(',');
        if (list.length() != 4) {
            color.setNamedColor("none");
            return false;
        }
        // rgb取得
        if (getRgb(list, iRgba) == false) {
            color.setNamedColor("none");
            return false;
        }
        // 色設定
        color.setRgb(iRgba[0], iRgba[1], iRgba[2]);
    } else {
        // 色キーワード
        color.setNamedColor(strBuf);
    }

    return true;
}
//---------------------------------------------------------------------------
bool SvgColor::getRgb(const QStringList &list, int iRgba[])
{
    QString strValue;
    bool ok;

    for (int i = 0; i < 3; i++) {
        strValue = list.at(i).trimmed();
        if (strValue.endsWith("%")) {
            int iPer = strValue.mid(0, strValue.length() - 1).toInt(&ok);
            if (ok == false) {
                return false;
            }
            iRgba[i] = (int)(iPer * 2.55);
        } else {
            iRgba[i] = strValue.toInt(&ok);
            if (ok == false) {
                return false;
            }
        }
    }

    return true;
}
//---------------------------------------------------------------------------
QString SvgColor::toString()
{
    if (color.isValid() == false) {
        return "none";
    }

    return color.name();
}
//---------------------------------------------------------------------------
QColor SvgColor::getColorObj()
{
    return color;
}
//---------------------------------------------------------------------------
void  SvgColor::readData(const QJsonObject &json)
{
    color.setNamedColor(json[JSON_TAG_SVG_COLOR].toString());
}
//---------------------------------------------------------------------------
void  SvgColor::writeDown(QJsonObject &json) const
{
    if (color.isValid() == false) {
        json[JSON_TAG_SVG_COLOR] = "none";
    } else {
        json[JSON_TAG_SVG_COLOR] = color.name();
    }
}
//---------------------------------------------------------------------------
bool SvgColor::isEffectiveColor()
{
    if(color.alphaF() == 0 || color.name() == "#ffffff" || color.name() == "#fff" || color.isValid() == false) {
        return false;
    } else {
        return true;
    }
}
