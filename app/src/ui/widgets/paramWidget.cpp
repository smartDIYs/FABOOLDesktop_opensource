#include "paramWidget.h"
#include "ui_paramWidget.h"

#include "common/common.h"
#include "graphics/itemInfo.h"
#include "graphics/paramData.h"
#include "paramRow.h"


int ParamWidget::mColorIndx = 0;

ParamWidget::ParamWidget(ItemInfo* item, QWidget *parent) :
    QWidget(parent),
    ui(new Ui::ParamWidget)
{
    ui->setupUi(this);
    mLookingItem = item;

    this->widgetCreate();

//　画像がわかりやすいように画像名のエリアに色を付ける
    mColorIndx++;
    QString color;
    switch(mColorIndx % 6) {
    case 1:
        { color = QString(COLOR_PARAM_HEADER_1);  break; }
    case 2:
        { color = QString(COLOR_PARAM_HEADER_2);  break; }
    case 3:
        { color = QString(COLOR_PARAM_HEADER_3);  break; }
    case 4:
        { color = QString(COLOR_PARAM_HEADER_4);  break; }
    case 5:
        { color = QString(COLOR_PARAM_HEADER_5);  break; }
    case 0:
        { color = QString(COLOR_PARAM_HEADER_6);  break; }
    default:
        { color = "blue"; break; }
    }
    QString tooltipCSS = "QToolTip {"
                         "color: rgb(255, 255, 255);font: 15px;"
                         "}\n";
    ui->ImageNameTitle->setStyleSheet(tooltipCSS + "*{background-color:" + color + "}");
    ui->ImageNameTitle->setCursor(Qt::PointingHandCursor);
    ui->ImageNameTitle->setToolTip(item->formalName());

    connect(ui->PathParamTable, SIGNAL(moveRow(int, int)), this, SLOT(moveRow(int, int)));
}

ParamWidget::~ParamWidget()
{
    foreach (ParamRow* row, mRowList) {
        delete row;
    }

    delete ui;
}

QString ParamWidget::abridgementStr(QString str, QFont font, qreal threshold)
{
    QFontMetricsF fm(font);
    qreal abridgLength = fm.width(QString("..."));
    while(1) {
        if(fm.width(str) > (threshold - abridgLength)) {
            str.chop(1);
        } else {
            break;
        }
    }
    str += "...";
    return str;
}

//　画像が持つパス情報をもとに、汎用パスパラメータと詳細パスパラメータのWidgetを作る
void ParamWidget::widgetCreate()
{
    QString name = mLookingItem->formalName();
    QFont font = ui->ImageName->font();
    QFontMetricsF fm(font);
    if(fm.width(name) > 300) {
        name = abridgementStr(name, font, 300);
    }
    ui->ImageName->setText(name);

    ui->PathParamTable->setSelectionMode(QAbstractItemView::ContiguousSelection);
    ui->PathParamTable->setSelectionBehavior( QAbstractItemView::SelectRows );

    // HorizontalHeaderを作る
    QStringList header;
    header << QString(tr(PARAM_TABLE_HEADER_COL_COLOR_STR))       << QString(tr(PARAM_TABLE_HEADER_COL_USE_TYPE_STR))
           << QString(tr(PARAM_TABLE_HEADER_COL_HEAD_SPEED_STR))  << QString(tr(PARAM_TABLE_HEADER_COL_LASER_POWER_STR))
           << QString(tr(PARAM_TABLE_HEADER_COL_PROCESS_TIME_STR)) ;

    ui->PathParamTable->setRowCount(0);
    ui->PathParamTable->setColumnCount(header.count());
    ui->PathParamTable->setHorizontalHeaderLabels(header);

    ui->PathParamTable->setColumnWidth(PARAM_TABLE_HEADER_COL_COLOR, 40);
    ui->PathParamTable->setColumnWidth(PARAM_TABLE_HEADER_COL_USE_TYPE, 90);
    ui->PathParamTable->setColumnWidth(PARAM_TABLE_HEADER_COL_HEAD_SPEED, 90);
    ui->PathParamTable->setColumnWidth(PARAM_TABLE_HEADER_COL_LASER_POWER, 90);
    ui->PathParamTable->setColumnWidth(PARAM_TABLE_HEADER_COL_PROCESS_TIME, 50);
    ui->PathParamTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

    // LookingItemをみてTableItemを作る
    foreach(ParamData* data, mLookingItem->parameters()) {
        ParamRow* rowData = new ParamRow(mLookingItem->isRaster(), data);
        setRowData(rowData);
    }
    connect(ui->MinimizeButton,     SIGNAL(clicked()),  this, SLOT(toggleAction()));
}

//　詳細パスパラメータのテーブルにUIをセットする
void ParamWidget::setRowData(ParamRow* rowData)
{
    this->mRowList.append(rowData);

    CustomTableWidget* table = ui->PathParamTable;
    table->insertRow(table->rowCount());

    qint16 rowIndex = table->rowCount() - 1;
    qint16 colIndex = 0;

    for(; colIndex < table->columnCount(); colIndex++) {
        table->setCellWidget(rowIndex, colIndex, rowData->getWidget(colIndex));
    }
}

//　プロジェクト保存時にUIの情報をプロジェクトへ反映
int  ParamWidget::setBackParam()
{
    int ret = 0;
    foreach (ParamRow* row, mRowList) {
        ret = row->setBackParam();
        if(ret != SUCCESS) {
            return ret;
        }
    }
    return SUCCESS;
}

void ParamWidget::updateParameter()
{
    foreach (ParamRow* row, mRowList) {
        row->updateParam();
    }
}

//　パラメータのMinimize切り替え
void ParamWidget::toggleAction()
{
    if(ui->MinimizeButton->isChecked() == true) {
        ui->TableFrame->hide();
    } else {
        ui->TableFrame->show();
        ui->TableFrame->raise();
    }
}

void ParamWidget::moveRow(int prev, int curr)
{
    CustomTableWidget* table = ui->PathParamTable;

    ParamRow* tempRow = mRowList.takeAt(prev);
    ParamRow* newRow = new ParamRow(mLookingItem->isRaster(), tempRow->getParam());
    table->removeRow(prev);
    delete tempRow;

    mRowList.insert(curr, newRow);
    table->insertRow(curr);
    for(int colIndex = 0; colIndex < table->columnCount(); colIndex++) {
        table->setCellWidget(curr, colIndex, newRow->getWidget(colIndex));
    }

    mLookingItem->switchParam(prev, curr);
}
