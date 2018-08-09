#include "debugPanel.h"
#include "ui_debugPanel.h"

DebugPanel::DebugPanel(QWidget *parent, FaboolMachine *machine) : QMainWindow(parent), ui(new Ui::DebugPanel)
{
    ui->setupUi(this);

    _faboolMachine = machine;

    connect(_faboolMachine, SIGNAL(processRateDidUpdated(int)), this, SLOT(onProcessRateUpdated(int)));
    connect(ui->consoleEnableCheckBox, SIGNAL(clicked(bool)),   this, SLOT(onConsoleEnableCheckBoxChanged(bool)));
    connect(ui->clearConsoleButton,    SIGNAL(clicked(bool)),   this, SLOT(onClearConsoleButtonPushed()));

    // for debug.
    _processTimer = new QTimer;
    connect(_processTimer,            SIGNAL(timeout()),     this, SLOT(onUpdateTimerLabel()));
    connect(ui->updateConsoleButton,  SIGNAL(clicked(bool)), this, SLOT(onUpdateConsoleButtonPushed()));
    connect(&Debug::sharedInstance(), SIGNAL(updateStatus(QStringList)), this, SLOT(onUpdateStatusView(QStringList)));
    connect(&Debug::sharedInstance(), SIGNAL(updateGCode(QString)),      this, SLOT(onUpdateGCodeView(QString)));
    connect(&Debug::sharedInstance(), SIGNAL(updateConsole()),           this, SLOT(onUpdateConsoleButtonPushed()));
    Debug::sharedInstance().setEnable(ui->consoleEnableCheckBox->isEnabled());
    Debug::log("--- Console Log ---");
    onUpdateConsoleButtonPushed();
}

DebugPanel::~DebugPanel()
{
    delete ui;
}

void DebugPanel::onProcessRateUpdated(int rate)
{
    if (rate == -1) {
        return;
    }
    QString rateString = QString::number(rate) + "%";
    ui->processRateLabel->setText(rateString);
    ui->ProcessRateProgressBar->setValue(rate);
}

void DebugPanel::onUpdateTimerLabel()
{
    int elapsedSec = _processStartTime.secsTo(QTime::currentTime());
    QString elapsedTime = QString("%1:%2:%3").arg( elapsedSec / 3600, 2, 10, QChar('0'))
                                             .arg( elapsedSec /   60, 2, 10, QChar('0'))
                                             .arg((elapsedSec)%   60, 2, 10, QChar('0'));
    ui->timeLabel->setText(elapsedTime);
}

void DebugPanel::onUpdateStatusView(QStringList stringList)
{
    ui->statusListView->setText(stringList.join("\n"));
}

void DebugPanel::onUpdateGCodeView(QString gcode)
{
    ui->gcodeTextBox->setPlainText(gcode);
}

void DebugPanel::onUpdateConsoleButtonPushed()
{
    ui->console->setPlainText(Debug::sharedInstance().getConsoleString().join("\n"));
    ui->console->verticalScrollBar()->setValue(ui->console->verticalScrollBar()->maximum());
}

void DebugPanel::onClearConsoleButtonPushed()
{
    Debug::sharedInstance().clearConsoleStringList();
    onUpdateConsoleButtonPushed();
}

void DebugPanel::onConsoleEnableCheckBoxChanged(bool enable)
{
    Debug::sharedInstance().setEnable(enable);
}
