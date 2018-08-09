#ifndef LIBRARYLISTMODEL_H
#define LIBRARYLISTMODEL_H

#include <QObject>
#include <QWidget>

#include "listModelCore.h"

class LibraryListModel : public ListModelCore
{
public:
    LibraryListModel(QObject* parent = 0);

    //QVariant data(const QModelIndex &index, int role) const override;
    virtual void setData(const QJsonObject &json);

    enum { Type = LIST_MODEL_LIBRARY };
    int type() const
    {
        return Type;
    }

};

#endif // LIBRARYLISTMODEL_H
