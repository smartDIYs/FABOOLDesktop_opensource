//---------------------------------------------------------------------------
// SVG色クラス
//---------------------------------------------------------------------------
#ifndef SVGCOLOR_H
#define SVGCOLOR_H
//---------------------------------------------------------------------------
#include <QtCore>
#include <QColor>
#include "common/common.h"
//---------------------------------------------------------------------------
class SvgColor
{
    QTTEST_OBJECT

public:
    SvgColor();
    SvgColor(const SvgColor &src);
    ~SvgColor();
    SvgColor &operator=(const SvgColor &src);
    bool setColor(const QString &strColor);
    QString toString();
    QColor getColorObj();

    void readData(const QJsonObject &json);
    void writeDown(QJsonObject &json) const;

    bool isEffectiveColor();

protected:
    void initMenber();
    void deleteMenber();
    SvgColor &copyMenber(const SvgColor &src);
    bool getRgb(const QStringList &list, int iRgba[]);

private:
    QColor  color;
};
Q_DECLARE_METATYPE(SvgColor)
//---------------------------------------------------------------------------
#endif // SVGCOLOR_H
