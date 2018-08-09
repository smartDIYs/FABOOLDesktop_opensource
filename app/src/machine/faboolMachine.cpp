#include "faboolMachine.h"
//===============================================================================
// FaboolMachineStatus Class
//===============================================================================
FaboolMachineState::FaboolMachineState()
{
    _doorOpen   = false;
    _chillerOff = false;
    _limitHit   = false;
    _portState  = Port::Disconnect;
}

bool FaboolMachineState::isConnect()
{
    return _portState == Port::Connect;
}

bool FaboolMachineState::isChillerOff()
{
    return _chillerOff;
}

bool FaboolMachineState::isDoorOpen()
{
    return _doorOpen;
}

bool FaboolMachineState::isLimitHit()
{
    return _limitHit;
}

bool FaboolMachineState::isSafety()
{
    if(_chillerOff) {
        return false;
    }
    return true;
}

FaboolMachineState::Process FaboolMachineState::getProcessState()
{
    return _processState;
}

void FaboolMachineState::setProcessState(Process state)
{
    _processState = state;
}

void FaboolMachineState::setPortState(Port state)
{
    _portState = state;
}

void FaboolMachineState::setMachineState(SerialManager::Status state)
{
    _boardState  = state;
    _doorOpen    = state.door_open;
    _chillerOff  = state.chiller_off;
    _limitHit    = state.limit_hit;
}

//===============================================================================
// FaboolMachineBehavior Class
//===============================================================================
FaboolMachineBehavior::FaboolMachineBehavior() : QObject()
{
    FIRMWARE_FLG    = false;
    _isGCodeSending = false;
}

FaboolMachineBehavior::~FaboolMachineBehavior()
{
    delete(_serialManager);
    _serialManager = NULL;

    _loopTimer->stop();
    delete(_loopTimer);
    _loopTimer = NULL;
}

void FaboolMachineBehavior::process()
{
    // These need to be created in this thread.
    _serialManager = new SerialManager();
    _loopTimer     = new QTimer(this);

    connect(_loopTimer, SIGNAL(timeout()), this, SLOT(run_with_callback()));
    run_with_callback();
}

void FaboolMachineBehavior::run_with_callback()
{
    _loopTimer->stop();

    if (FIRMWARE_FLG == false) {
        if (_serialManager->is_connected()) {
            _serialManager->send_queue_as_ready();
        } else {
            _serialManager->cancel_queue();

            QSerialPortInfo SERIAL_PORT = _serialManager->match_device(CONTROL_BOARD);
            if (SERIAL_PORT.description() != "") {
                Debug::log("ConnectThread connect to port : " + SERIAL_PORT.description());
                _serialManager->connect(SERIAL_PORT);
            }
        }
    }
    updateProgress();
    updateState();

    // For receive signals, wait a littele time...
    _loopTimer->start(5);
}

void FaboolMachineBehavior::updateProgress()
{

    if (_isGCodeSending == true) {
        if (_serialManager->is_queue_empty()) {
            _isGCodeSending = false;
            emit sendGCodeFinish();
            emit processRateUpdate(100);
            _machineState.setProcessState(FaboolMachineState::Process::Stop);
        } else {
            emit processRateUpdate(_serialManager->get_queue_percentage_done());
        }
    }
}

void FaboolMachineBehavior::updateState()
{
    if (_serialManager->is_connected() == true) {
        _machineState.setPortState(FaboolMachineState::Port::Connect);
    } else {
        _machineState.setPortState(FaboolMachineState::Port::Disconnect);
    }
    _machineState.setMachineState(_serialManager->getState());
    emit machineStateUpdate(_machineState);
}

void FaboolMachineBehavior::executeGCode(QString gcode)
{
    _serialManager->queue_gcode_line(gcode);
    _isGCodeSending = true;

    _machineState.setProcessState(FaboolMachineState::Process::Running);
}

void FaboolMachineBehavior::executeMaintenance(QString gcode)
{
    if (_machineState.getProcessState() == FaboolMachineState::Process::Running) {
        return;
    }

    QString safetyGcode = "S0\n" + gcode.replace(QRegExp("S[0-9]+"), "S0");
    _serialManager->queue_gcode_line(safetyGcode);
    _isGCodeSending = true;

    _machineState.setProcessState(FaboolMachineState::Process::Maintenance);
}

void FaboolMachineBehavior::setPauseMode(bool value)
{
    _serialManager->set_pause(value);
}
//===============================================================================
// FaboolMachine Class
//===============================================================================
FaboolMachine::FaboolMachine(QObject *parent) : QObject(parent)
{
    _gcode   = "";
    _isPause = false;

    _behaviorThread  = new QThread(this);
    _machineBehavior = new FaboolMachineBehavior();
    _machineBehavior->moveToThread(_behaviorThread);

    connect(_behaviorThread,  SIGNAL(started()),    _machineBehavior, SLOT(process()));
    connect(_behaviorThread,  SIGNAL(finished()),   _behaviorThread,  SLOT(deleteLater()));
    connect(_machineBehavior, SIGNAL(processEnd()), _machineBehavior, SLOT(deleteLater()));
    connect(_machineBehavior, SIGNAL(processEnd()), _behaviorThread,  SLOT(quit()));

    qRegisterMetaType<FaboolMachineState>("FaboolMachineState");
    connect(_machineBehavior, SIGNAL(processRateUpdate(int)), this,  SLOT(processRateUpdated(int)));
    connect(_machineBehavior, SIGNAL(sendGCodeFinish()),      this,  SLOT(sendGCodeFinished()));
    connect(_machineBehavior, SIGNAL(machineStateUpdate(FaboolMachineState)), this,  SLOT(machineStateUpdated(FaboolMachineState)));

    connect(this, SIGNAL(setPauseMode(bool)), _machineBehavior, SLOT(setPauseMode(bool)));
    connect(this, SIGNAL(sendGCode(QString)), _machineBehavior, SLOT(executeGCode(QString)));
    connect(this, SIGNAL(sendMaintenanceGCode(QString)), _machineBehavior, SLOT(executeMaintenance(QString)));

    _behaviorThread->start();
}

FaboolMachine::~FaboolMachine()
{
    _behaviorThread->quit();
    _behaviorThread->wait();

    delete(_behaviorThread);
    _behaviorThread = NULL;

    delete(_machineBehavior);
    _machineBehavior = NULL;
}

void FaboolMachine::start()
{
    Debug::log("FaboolMachine::run()");
    emit setPauseMode(false);
    emit sendGCode(_gcode);
    processRateUpdated(0);
}

void FaboolMachine::stop()
{
    Debug::log("FaboolMachine::stop()");
    emit setPauseMode(false);
    emit sendGCode(GCODE_STOP_CYCLE);
}

void FaboolMachine::maintenance(QString gcode)
{
    Debug::log("FaboolMachine::maintenance()");
    emit setPauseMode(false);
    emit sendMaintenanceGCode(gcode);
}

void FaboolMachine::maintenance(QStringList gcode)
{
    maintenance(gcode.join("\n"));
}

void FaboolMachine::pause(bool enable)
{
    Debug::log("FaboolMachine::pause()");
    _isPause = enable;
    emit setPauseMode(enable);
}

void FaboolMachine::goToOrigin()
{
    Debug::log("FaboolMachine::goToOrigin()");
    emit setPauseMode(false);
    maintenance(GCODE_RETURN_TO_ORIGIN);
}

void FaboolMachine::setGCode(QString gcode)
{
    Debug::log("FaboolMachine::setGCode");
    _gcode = gcode;
}

void FaboolMachine::setGCode(QStringList gcode)
{
    setGCode(gcode.join("\n"));
}

void FaboolMachine::setParameter(QString gcode)
{
    Debug::log("FaboolMachine::setParamater");
    maintenance(gcode);
}

void FaboolMachine::setParameter(QStringList gcode)
{
    setParameter(gcode.join("\n"));
}

void FaboolMachine::processRateUpdated(int rate)
{
    emit processRateDidUpdated(rate);
}

void FaboolMachine::sendGCodeFinished()
{
    emit sendGCodeDidFinished();
}

FaboolMachineState FaboolMachine::getState()
{
    return _machineState;
}

void FaboolMachine::machineStateUpdated(FaboolMachineState state)
{
    _machineState = state;
    emit machineStateDidUpdated();

    if (_machineState.getProcessState() == FaboolMachineState::Process::Running && _machineState.isSafety() == false) {
        emit stopProcessBySafetyWarning();
    }
}
