#ifndef DEBUGPANEL_H
#define DEBUGPANEL_H

#include <QMainWindow>
#include <QTextEdit>
#include <QScrollBar>
#include <QTimer>
#include <QTime>
#include "machine/faboolMachine.h"

namespace Ui {
class DebugPanel;
}

class DebugPanel : public QMainWindow
{
    Q_OBJECT

public:
    explicit DebugPanel(QWidget *parent, FaboolMachine *machine);
    ~DebugPanel();

private:
    Ui::DebugPanel *ui;

    QTimer* _processTimer;
    QTime   _processStartTime;

    FaboolMachine* _faboolMachine;

public slots:
    void onProcessRateUpdated(int rate);
    void onUpdateConsoleButtonPushed();
    void onClearConsoleButtonPushed();
    void onConsoleEnableCheckBoxChanged(bool enable);
    void onUpdateTimerLabel();
    void onUpdateStatusView(QStringList stringList);
    void onUpdateGCodeView(QString gcode);

};

#endif // DEBUGPANEL_H
