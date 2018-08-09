#ifndef DEVICE_H
#define DEVICE_H

#include <QtCore>
#include <QSerialPort>
#include <QSerialPortInfo>
#include <QIODevice>
#include <QTimer>
#include "src/machine/util/debug.h"

class Device : public QSerialPort
{
    Q_OBJECT
public:
    Device(const QSerialPortInfo &port, QObject *parent);
    ~Device();
    QByteArray safeRead(qint64 maxlen);
    qint64     safeWrite(const char *data);

    bool _deviceWillClose;

public slots:
    void onErrorOccurred(QSerialPort::SerialPortError error);

};

#endif // DEVICE_H
