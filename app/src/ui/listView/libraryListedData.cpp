#include "libraryListedData.h"

#include <QIcon>
#include <QSvgRenderer>
#include <QPainter>

LibraryListedData::LibraryListedData(const QString& name, const QString& tag):
    ListedDataCore(name, tag)
{
    mParser = new SvgParser();
}

LibraryListedData::LibraryListedData():
    ListedDataCore()
{
    mParser = NULL;
    //　基本的には使わない
    //　Q_DECLARE_METATYPE宣言に必要なコンストラクタ
}

LibraryListedData::~LibraryListedData()
{
    if(mParser != NULL) {
        delete mParser;
        mParser = NULL;
    }
}

void LibraryListedData::setData(const QJsonObject& json)
{
    mParser->readData(json[JSON_TAG_IMG_DATA].toObject());
    QSvgRenderer render(mParser->toByte());
    QPixmap image(mParser->outerSize().toSize());
    image.fill(Qt::transparent);

    QPainter painter;
    painter.begin(&image);
    render.render(&painter);
    painter.end();

    mPixmap = image.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation);
}
