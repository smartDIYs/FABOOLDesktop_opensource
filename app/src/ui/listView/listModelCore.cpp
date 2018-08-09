#include "listModelCore.h"

#include <QIcon>

ListModelCore::ListModelCore(QObject* parent):
    QAbstractListModel(parent)
{

}

ListModelCore::~ListModelCore()
{
    mTagList.clear();
    mData.clear();
}

QVariant ListModelCore::data(const QModelIndex &index, int role) const
{
    if (!index.isValid())
    { return QVariant(); }

    if (index.row() >= mData.size() || index.row() < 0)
    { return QVariant(); }

    if (role == Qt::DecorationRole) {
        return QIcon(((mData.at(index.row()))->icon()));
    } else if (role == Qt::DisplayRole) {
        return mData.at(index.row())->name();
    } else if (role == Qt::UserRole) {
        return QVariant::fromValue(static_cast<ListedDataCore*>((mData.at(index.row()))));
    }

    return QVariant();
}

int ListModelCore::rowCount(const QModelIndex &parent) const
{
    if(parent.isValid() == true)
    { return 0; }
    else
    { return mData.size(); }
}
