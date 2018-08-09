#ifndef SELECTLISTDIALOG_H
#define SELECTLISTDIALOG_H

#include <QtWidgets>
#include <QAbstractListModel>

#include "ui/listView/selectListView.h"

namespace Ui
{
class SelectListDialog;
}

class SelectListDialog : public QDialog
{
    Q_OBJECT

public:
    explicit SelectListDialog(QWidget *parent = 0);
    ~SelectListDialog();

    void setListModel(ListModelCore* model);

    QModelIndex selectedIndex();
    void createTag(QList<QString> tagList);

public slots:
    void tabChange(int);

signals:
    void select();
    void cancel();

private:
    Ui::SelectListDialog *ui;

    SelectListView*       mListView;
    QTabBar*              mTagTab;
};

#endif // SELECTLISTDIALOG_H
