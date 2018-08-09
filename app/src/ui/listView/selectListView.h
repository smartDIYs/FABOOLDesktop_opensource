#ifndef SELECTLISTVIEW_H
#define SELECTLISTVIEW_H

#include <QObject>
#include <QWidget>
#include <QListView>
#include <QTabBar>

#include "ui/listView/listModelCore.h"

class SelectListView : public QListView
{
public:
    SelectListView(QWidget* parent = 0);
    ~SelectListView();

    QString currentTag();
    QModelIndex selectedIndex();

    void setListModel(ListModelCore* model);
    void tabChange(QString selectedTag);

};

#endif // SELECTLISTVIEW_H
