#include "selectListView.h"

SelectListView::SelectListView(QWidget* parent):
    QListView(parent)
{

}

SelectListView::~SelectListView()
{
}

void SelectListView::tabChange(QString selectedTag)
{
    int rowCount = model()->rowCount();
    int colCount = model()->columnCount();
    QModelIndex index = QModelIndex();

    ListModelCore* modelCore = ((ListModelCore*)model());
    for(int c = 0; c < colCount; c++) {
        for(int r = 0; r < rowCount; r++) {
            index = model()->index(r, c);
            QVariant data = modelCore->data(index, Qt::UserRole);
            ListedDataCore* dataCore = data.value<ListedDataCore*>();
            if(selectedTag == "All")
            { setRowHidden(r, false); }
            else if(dataCore->tag() != selectedTag)
            { setRowHidden(r, true); }
            else
            { setRowHidden(r, false); }
        }
    }
}

QModelIndex SelectListView::selectedIndex()
{
    return selectedIndexes().at(0);
}

void SelectListView::setListModel(ListModelCore* model)
{
    setModel(model);

    if(model->type() == LIST_MODEL_LIBRARY) {
        setViewMode(QListView::IconMode);
        setIconSize(QSize(40, 40));
        setGridSize(QSize(60, 60));  //　読み込んだアイテムの数で可変にする予定
        setSpacing(10);
        setMovement(QListView::Snap);
    }
}
