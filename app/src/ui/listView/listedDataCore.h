#ifndef LISTEDDATACORE_H
#define LISTEDDATACORE_H

#include <QObject>
#include <QVariant>
#include <QPixmap>

#include "common/common.h"

enum LIST_DATA_TYPE {
    LIST_DATA_CORE = 0,
    LIST_DATA_LIBRARY,
    LIST_DATA_MATERIAL
};

class ListedDataCore
{
    QTTEST_OBJECT
public:
    ListedDataCore(const QString& name, const QString& tag);
    ListedDataCore();
    ~ListedDataCore();

    QPixmap icon()const;
    QString name()const { return mName; }
    QString tag()const { return mTag; }

    virtual void setData(const QJsonObject& json); // MetaType登録には純粋仮想不可

    enum { Type = LIST_DATA_CORE };
    virtual int type() const
    {
        return Type;
    }

protected:
    QString mName;
    QString mTag;
    QPixmap mPixmap;
};
Q_DECLARE_METATYPE(ListedDataCore*)
#endif // LISTEDDATACORE_H
