#ifndef MATERIALLISTMODEL_H
#define MATERIALLISTMODEL_H

#include "listModelCore.h"

#include <QObject>
#include <QAbstractListModel>
#include <QList>
#include <QPixmap>
#include <QPoint>
#include <QStringList>

class MaterialListModel : public ListModelCore
{
public:
    explicit MaterialListModel(QObject* parent = 0);

    virtual void setData(const QJsonObject &json);

    enum { Type = LIST_MODEL_MATERIAL };
    int type() const
    {
        return Type;
    }

};

#endif // MATERIALLISTMODEL_H
