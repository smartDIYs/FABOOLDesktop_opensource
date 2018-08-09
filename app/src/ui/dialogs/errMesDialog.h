#ifndef ERRMESDIALOG_H
#define ERRMESDIALOG_H

#include <QDialog>

namespace Ui
{
class ErrMesDialog;
}

class ErrMesDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ErrMesDialog(const QString& errDiscription, QWidget *parent = 0);
    ~ErrMesDialog();

//    void setStrings(QStringList);
    void setStrings(QMap<QString, QString>);

public slots:
    void onOk();

private:
    Ui::ErrMesDialog *ui;
};

#endif // ERRMESDIALOG_H
