#include "selectListDialog.h"
#include "ui_selectListDialog.h"

SelectListDialog::SelectListDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::SelectListDialog)
{
    ui->setupUi(this);
    QVBoxLayout* layout = new QVBoxLayout(ui->ListWidget);
    mTagTab = new QTabBar();
    mTagTab->addTab(tr("All"));
    mTagTab->setFixedHeight(20);
    mListView = new SelectListView();
    mListView->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    layout->addWidget(mTagTab);
    layout->addWidget(mListView);
    layout->setSpacing(0);

    connect(mTagTab, SIGNAL(currentChanged(int)), this, SLOT(tabChange(int)));
    connect(ui->CancelButton, SIGNAL(clicked()), this, SIGNAL(cancel()));
    connect(ui->SelectButton, SIGNAL(clicked()), this, SIGNAL(select()));
    connect(mListView,        SIGNAL(doubleClicked(QModelIndex)), this, SIGNAL(select()));
}

SelectListDialog::~SelectListDialog()
{
    delete mTagTab;
    delete mListView;
    delete ui;
}

void SelectListDialog::setListModel(ListModelCore* model)
{
    if(model == NULL)
    { return; }

    mListView->setListModel(model);
    createTag(model->tagList());

}

QModelIndex SelectListDialog::selectedIndex()
{
    return mListView->selectionModel()->currentIndex();
}


void SelectListDialog::createTag(QList<QString> tagList)
{
    for(int i = 0; i < mTagTab->count(); i++) {
        QString tabText = mTagTab->tabText(i);
        if(tagList.contains(tabText) == true) {
            tagList.removeOne(tabText);
        }
    }

    foreach (QString tag, tagList) {
        mTagTab->addTab(tag);
    }
}

void SelectListDialog::tabChange(int index)
{
    mListView->tabChange(mTagTab->tabText(index));
}

