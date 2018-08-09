#ifndef PROGRESSDIALOG_H
#define PROGRESSDIALOG_H

#include <QDialog>

namespace Ui
{
class ProgressDialog;
}

class ProgressDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProgressDialog(QWidget *parent = 0);
    ~ProgressDialog();

protected:
    void showEvent(QShowEvent* e);
    void hideEvent(QHideEvent* e);

public slots:
    void updateProgress(int);

signals:
    void onProcess(bool isPaused);
    void onReset();

private:
    Ui::ProgressDialog *ui;

    QTimer* timer;
};

#endif // PROGRESSDIALOG_H
