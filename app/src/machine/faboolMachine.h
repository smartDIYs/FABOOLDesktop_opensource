#ifndef FABOOLMACHINE_H
#define FABOOLMACHINE_H

#include <QtCore>
#include <QObject>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QTimer>
#include <QRegExp>
#include "common/common.h"
#include "src/machine/util/debug.h"
#include "serialManager.h"
//===============================================================================
// FaboolMachineBehavior Class
//===============================================================================
class FaboolMachine;
class FaboolMachineState
{
public:
    enum Port { Connect, Disconnect };
    enum Process { Stop, Running, Pause, Maintenance };

    FaboolMachineState();
    bool isConnect();
    bool isRunning();
    bool isStop();
    bool isPause();
    bool isDoorOpen();
    bool isChillerOff();
    bool isLimitHit();
    bool isSafety();

    void setPortState(Port state);
    void setProcessState(Process process);
    void setMachineState(SerialManager::Status state);
    Process getProcessState();

private:
    bool _doorOpen;
    bool _chillerOff;
    bool _limitHit;

    SerialManager::Status _boardState;
    Port    _portState;
    Process _processState;
};
//===============================================================================
// FaboolMachineBehavior Class
//===============================================================================
class FaboolMachine;
class FaboolMachineBehavior : public QObject
{
    Q_OBJECT
public:
    FaboolMachineBehavior();
    ~FaboolMachineBehavior();

    SerialManager* _serialManager;

private:
    bool FIRMWARE_FLG;
    bool _isGCodeSending;
    FaboolMachineState _machineState;

    QTimer* _loopTimer;

    void updateProgress();
    void updateState();

signals:
    void processEnd();

    void processRateUpdate(int);
    void sendGCodeFinish();
    void machineStateUpdate(FaboolMachineState);

public slots:
    void process();
    void run_with_callback();
    void executeGCode(QString gcode);
    void executeMaintenance(QString gcode);
    void setPauseMode(bool value);
};
//===============================================================================
// FaboolMachine Class
//===============================================================================
class FaboolMachine : public QObject
{
    Q_OBJECT
public:

    explicit FaboolMachine(QObject *parent = 0);
    ~FaboolMachine();

    void start();
    void stop();
    void pause(bool enable);
    void goToOrigin();
    void maintenance(QString gcode);
    void maintenance(QStringList gcode);
    void setGCode(QString gcode);
    void setGCode(QStringList gcode);
    void setParameter(QString gcode);
    void setParameter(QStringList gcode);
    FaboolMachineState getState();

private:
    QString  _gcode;
    bool     _isPause;
    QThread* _behaviorThread;
    FaboolMachineBehavior* _machineBehavior;
    FaboolMachineState _machineState;

signals:
    void sendGCode(QString);
    void sendMaintenanceGCode(QString);
    void startProcess(QString);
    void stopProcess();
    void processRateDidUpdated(int);
    void sendGCodeDidFinished();
    void setPauseMode(bool);
    void machineStateDidUpdated();
    void stopProcessBySafetyWarning();

public slots:
    void processRateUpdated(int rate);
    void sendGCodeFinished();
    void machineStateUpdated(FaboolMachineState state);
};

#endif // FABOOLMACHINE_H
