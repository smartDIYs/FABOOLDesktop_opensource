#ifndef LIBRARYLISTEDDATA_H
#define LIBRARYLISTEDDATA_H

#include <QObject>
#include <QWidget>

#include "listedDataCore.h"
#include "transfer/svgParser.h"

class LibraryListedData : public ListedDataCore
{
public:
    LibraryListedData(const QString& name, const QString& tag);
    LibraryListedData();
    ~LibraryListedData();

    virtual void setData(const QJsonObject& json) override;
    SvgParser* parser()const { return mParser; }

    enum { Type = LIST_DATA_LIBRARY };
    int type() const
    {
        return Type;
    }
private:
    SvgParser* mParser;
};
Q_DECLARE_METATYPE(LibraryListedData*)
#endif // LIBRARYLISTEDDATA_H
