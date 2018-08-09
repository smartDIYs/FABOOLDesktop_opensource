#ifndef MATERIALLISTEDDATA_H
#define MATERIALLISTEDDATA_H

#include <QObject>

#include "listedDataCore.h"
#include "graphics/paramData.h"

class MaterialListedData : public ListedDataCore
{
public:
    MaterialListedData(const QString& name, const QString& tag, qreal thicks);
    MaterialListedData();
    ~MaterialListedData();

    qreal thick() { return mThick; }
    ParamData param(PARAM_USE_TYPE type);

    virtual void setData(const QJsonObject& json) override;
    enum { Type = LIST_DATA_MATERIAL };
    int type() const
    {
        return Type;
    }

private:
    qreal       mThick;
    ParamData   mEngrave;
    ParamData   mCut;
};
Q_DECLARE_METATYPE(MaterialListedData*)
#endif // MATERIALLISTEDDATA_H
