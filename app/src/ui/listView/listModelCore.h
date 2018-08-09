#ifndef LISTMODELCORE_H
#define LISTMODELCORE_H

#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QPixmap>
#include <QPoint>
#include <QStringList>

#include "ui/listView/listedDataCore.h"
#include "common/common.h"

enum LIST_MODEL_TYPE {
    LIST_MODEL_CORE = 0,
    LIST_MODEL_LIBRARY,
    LIST_MODEL_MATERIAL
};

class ListModelCore : public QAbstractListModel
{
    QTTEST_OBJECT
public:
    ListModelCore(QObject* parent = 0);
    ~ListModelCore();

    virtual void setData(const QJsonObject &json) = 0;
    QList<QString> tagList()const { return mTagList; }

// override
    QVariant data(const QModelIndex &index, int role = Qt::DisplayRole) const override;
    int rowCount(const QModelIndex &parent) const override;

    enum { Type = LIST_MODEL_CORE };
    virtual int type() const
    {
        return Type;
    }

protected:
    QList<QString>          mTagList;
    QList<ListedDataCore*>  mData;      //ポリモーフィズムを利用するためにポインタを保持

};

#endif // LISTMODELCORE_H
