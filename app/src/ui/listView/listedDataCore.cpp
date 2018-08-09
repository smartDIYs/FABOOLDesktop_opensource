#include "listedDataCore.h"

ListedDataCore::ListedDataCore(const QString& name, const QString& tag)
{
    mName = name;
    mTag  = tag;
//    mIcon = NULL;
}

ListedDataCore::ListedDataCore()
{
    mName = "";
    mTag  = "";

    //　基本的には使わない
    //　Q_DECLARE_METATYPE宣言に必要なコンストラクタ
}

ListedDataCore::~ListedDataCore()
{
}

QPixmap ListedDataCore::icon()const
{
    return mPixmap;
}

void ListedDataCore::setData(const QJsonObject& json)
{

}
