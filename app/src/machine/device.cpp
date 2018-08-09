#include "device.h"

Device::Device(const QSerialPortInfo &port, QObject *parent) : QSerialPort(parent)
{
    setPort(port);
    setBaudRate(QSerialPort::Baud115200);
    setDataBits(QSerialPort::Data8);
    setParity(QSerialPort::NoParity);
    setStopBits(QSerialPort::OneStop);
    setFlowControl(QSerialPort::NoFlowControl);

    connect(this, SIGNAL(errorOccurred(QSerialPort::SerialPortError)), this, SLOT(onErrorOccurred(QSerialPort::SerialPortError)));
    Debug::console("::::: Device init");

    _deviceWillClose = false;
}

QByteArray Device::safeRead(qint64 maxlen)
{
    if(_deviceWillClose == false) {
        return read(maxlen);
    } else {
        close();
        return "";
    }
}

qint64 Device::safeWrite(const char *data)
{
    if(_deviceWillClose == false) {
        return write(data);
    } else {
        close();
        return 0;
    }
}

Device::~Device()
{
    Debug::console("::::: Device delete");
    close();
}

void Device::onErrorOccurred(QSerialPort::SerialPortError error)
{
    // Unknown error is necessary for connection check of windows 7.
    if (error == SerialPortError::ResourceError || error == SerialPortError::UnknownError) {
        _deviceWillClose = true;
    }
}
