#ifndef SERIALMANAGER_H
#define SERIALMANAGER_H

#ifndef Q_OS_WIN
#define CONTROL_BOARD "STM32 Virtual ComPort"
#else
#define CONTROL_BOARD "STMicroelectronics Virtual COM Port"
#endif

#include <QtCore>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QObject>
#include <QSysInfo>
#include <QRegExp>
#include "device.h"
#include "src/machine/util/debug.h"

class SerialManager : public QObject
{
public:

    static const int WaitForReadTime  = 100;
    static const int WaitForWriteTime = 100;

    SerialManager();
    ~SerialManager();

    class Status
    {
    public:
        bool ready                         = true;
        bool paused                        = false;
        bool buffer_overflow               = false;
        bool transmission_error            = false;
        bool bad_number_format_error       = false;
        bool expected_command_letter_error = false;
        bool unsupported_statement_error   = false;
        bool power_off                     = false;
        bool limit_hit                     = false;
        bool serial_stop_request           = false;
        bool door_open                     = false;
        bool chiller_off                   = false;
        bool x                             = false;
        bool y                             = false;
        QString firmware_version           = "";
        QString grbl_name                  = "";
    };

    Device* device;
    QString rx_buffer;
    QString tx_buffer;
    bool    remoteXON;

    int TX_CHUNK_SIZE;
    int RX_CHUNK_SIZE;
    int nRequested;

    int    job_size;
    bool   job_active;
    Status status;

    QString GRBL1_FIRST_STRING = "LasaurGrbl";
    QString GRBL2_FIRST_STRING = "SmartLaserMini";
    QString GRBL3_FIRST_STRING = "FaboolLaserMini";
    QString GRBL4_FIRST_STRING = "FaboolLaserCo2";
    QString GRBL5_FIRST_STRING = "SmartLaserCo2";
    QString GRBL6_FIRST_STRING = "FaboolLaserDS";

    QString GRBL_NAME;

    int  fec_redundancy;
    char ready_char;
    char request_ready_char;
    int  last_request_ready;

    void    reset_status();
    QSerialPortInfo match_device(QString search_regex);
    bool    connect(const QSerialPortInfo &port); // void -> bool
    bool    close();
    bool    is_connected();
    Status  get_hardware_status();
    void    cancel_queue();
    bool    is_queue_empty();
    int     get_queue_percentage_done(); // string -> int
    bool    set_pause(bool flag);
    void    send_queue_as_ready();
    void    process_status_line(QString line);

public slots:
    void    queue_gcode_line(QString gcode);


//****** Original *******
public:
    QString getControlBoardName();

    bool    isGCodeSending();
    Status  getState();

private:
    QTime   _last_status_check_time;
    QString _lastSendStringLine;
    bool    _isForceReadMode;

    void    setForceReadMode(bool enable);

};

#endif // SERIALMANAGER_H
