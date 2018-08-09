#include "serialManager.h"

SerialManager::SerialManager() : QObject()
{
    // ****** Original *******
    getControlBoardName();
    // ****** Original *******

    device = NULL;

    rx_buffer = "";
    tx_buffer = "";
    remoteXON = true;

    // TX_CHUNK_SIZE - this is the number of bytes to be
    // written to the device in one go. It needs to match the device.
    TX_CHUNK_SIZE = 64;
    RX_CHUNK_SIZE = 256;
    nRequested = 0;

    // used for calculating percentage done
    job_size   = 0;
    job_active = 0;

    // status flags
    reset_status();

    GRBL1_FIRST_STRING = "LasaurGrbl";
    GRBL2_FIRST_STRING = "SmartLaserMini";
    GRBL3_FIRST_STRING = "FaboolLaserMini";
    GRBL4_FIRST_STRING = "FaboolLaserCo2";
    GRBL5_FIRST_STRING = "SmartLaserCo2";
    GRBL6_FIRST_STRING = "FaboolLaserDS";

    GRBL_NAME = "";

    // fec_redundancy = 2;   // use forward error correction
    // fec_redundancy = 1;   // use error detection
    fec_redundancy = 0;    // no checksum

    ready_char         = '\x12';
    request_ready_char = '\x14';
    last_request_ready = 0;

    _lastSendStringLine = "";
    _isForceReadMode = false;
    _last_status_check_time = QTime::currentTime();
}

SerialManager::~SerialManager()
{
    close();
}

void SerialManager::reset_status()
{
    status.ready                         = true;
    status.paused                        = false;
    status.buffer_overflow               = false;
    status.transmission_error            = false;
    status.bad_number_format_error       = false;
    status.expected_command_letter_error = false;
    status.unsupported_statement_error   = false;
    status.power_off                     = false;
    status.limit_hit                     = false;
    status.serial_stop_request           = false;
    status.door_open                     = false;
    status.chiller_off                   = false;
    status.x                             = false;
    status.y                             = false;
    status.firmware_version              = "";
    status.grbl_name                     = "";
}

QSerialPortInfo SerialManager::match_device(QString search_regex)
{
    foreach (const QSerialPortInfo &info, QSerialPortInfo::availablePorts()) {
        if (info.description() != search_regex) {
            continue;
        }

//        delete(device);
//        device = new Device(info, baudrate, this);
        if (connect(info) == false) {
            return QSerialPortInfo("");
        }
        device->waitForReadyRead();
        QString lasaur_hello = QString(device->read(32));
        qDebug() << "<<<<< CONNECT >>>>>>" << lasaur_hello;

        if (lasaur_hello.indexOf(GRBL1_FIRST_STRING) > -1) {
            GRBL_NAME = GRBL1_FIRST_STRING;
            close();
            return info;
        }
        if (lasaur_hello.indexOf(GRBL2_FIRST_STRING) > -1) {
            GRBL_NAME = GRBL2_FIRST_STRING;
            close();
            return info;
        }
        if (lasaur_hello.indexOf(GRBL3_FIRST_STRING) > -1) {
            GRBL_NAME = GRBL3_FIRST_STRING;
            close();
            return info;
        }
        if (lasaur_hello.indexOf(GRBL4_FIRST_STRING) > -1) {
            GRBL_NAME = GRBL4_FIRST_STRING;
            close();
            return info;
        }
        if (lasaur_hello.indexOf(GRBL5_FIRST_STRING) > -1) {
            GRBL_NAME = GRBL5_FIRST_STRING;
            close();
            return info;
        }
        if (lasaur_hello.indexOf(GRBL6_FIRST_STRING) > -1) {
            GRBL_NAME = GRBL6_FIRST_STRING;
            close();
            return info;
        }
        close();
    }
    return QSerialPortInfo("");
}

bool SerialManager::connect(const QSerialPortInfo &port)
{
    rx_buffer = "";
    tx_buffer = "";
    remoteXON = "";
    job_size  = 0;
    reset_status();

    close();
    device = new Device(port, this);

    if (device->open(QIODevice::ReadWrite) == false) {
        Debug::failed("@Device port open failed" + port.description());
        return false;
    }
    return true;
}

bool SerialManager::close()
{
    if (device != NULL) {
        device->flush();
        device->close();
        delete(device);
        device = NULL;
        status.ready = false;
        nRequested = 0;

        return true;
    } else {
        return false;
    }
}

bool SerialManager::is_connected()
{
    if (device == NULL) {
        return false;
    }
    if (device->isOpen()) {
        return true;
    } else {
        delete(device);
        device = NULL;
        return false;
    }
    return false;
}

SerialManager::Status SerialManager::get_hardware_status()
{
    if (is_queue_empty() == true) {
        queue_gcode_line("?");
    }
    return status;
}

void SerialManager::queue_gcode_line(QString gcode)
{
    Debug::log("@SerialManager::queue_gcode_line()");

    // replace continuous linebrake to single.
    gcode.replace(QRegExp("(\n)+"), "\n");
    setForceReadMode(false);

    // TODO: [Fix check sum]
    if (gcode != "" && is_connected() == true) {
        gcode = gcode.trimmed();

        if (gcode[0] == "%") {
            return;
        } else if (gcode[0] == "!") {
            cancel_queue();
            reset_status();
            tx_buffer  = "!\n";
            job_size   = 2;
            job_active = true;
        } else {
            if (gcode != "?") {
                status.ready = false;
            }
            if (fec_redundancy > 0) {
                int checksum = 0;
                foreach (QString c, gcode) {
                    int ascii_ord = c.constData()->unicode();
                    if (ascii_ord > QString(" ").constData()->unicode() && c != "~" && c != "!") {
                        checksum += ascii_ord;
                        if (checksum >= 128) {
                            checksum -= 128;
                        }
                    }
                }
                checksum = (checksum >> 1) + 128;
                QString gcode_redundant = "";
                for (int i = 0; i < fec_redundancy - 1; i++) {
                    gcode_redundant += "^" + QString(QChar(checksum)) + gcode + '\n';
                }
                gcode = gcode_redundant + "*" + QString(QChar(checksum)) + gcode;
            }
            tx_buffer += gcode + '\n';
            job_size  += gcode.length() + 1;
            job_active = true;
        }
    }

    Debug::gcode(gcode);
}

void SerialManager::cancel_queue()
{
    tx_buffer  = "";
    job_size   = 0;
    job_active = false;
}

bool SerialManager::is_queue_empty()
{
    return tx_buffer.length() == 0;
}

int SerialManager::get_queue_percentage_done()
{
    if (job_size == 0) {
        return -1;
    }
    int percentage = 100 - (100 * (qreal(tx_buffer.length()) / qreal(job_size)));
    return percentage;
}

bool SerialManager::set_pause(bool flag)
{
    // returns pause status
    if (is_queue_empty() == true) {
        return false;
    } else {
        if (flag == true) {
            status.paused = true;
            return true;
        } else {
            status.paused = false;
            return false;
        }
    }
}

void SerialManager::send_queue_as_ready()
{

    // Continuously call this to keep processing queue.
    QByteArray chars;
    if (device != NULL && status.paused == false) {

        Debug::status("*tx_buffer_size*", QString::number(tx_buffer.size()));
        Debug::status("*nRequested_size*", QString::number(nRequested));
//        // ****************************
//        device->waitForReadyRead(100);
//        chars = device->safeRead(RX_CHUNK_SIZE);
//        if (_isForceReadMode && (chars.length() == 0 || chars == "\u0012")) {
//            return;
//        }
//        setForceReadMode(false);
//        // ****************************

        // ****************************
        device->waitForReadyRead(100);
        chars = device->safeRead(RX_CHUNK_SIZE);

        if (chars.indexOf(QString(ready_char)) > -1) {
            nRequested = TX_CHUNK_SIZE;
            chars = chars.replace(QString(ready_char), "");
        }
        if (_isForceReadMode && (chars.length() == 0 || chars == "\u0012")) {
            return;
        }
        setForceReadMode(false);
        // ****************************

        if (chars.length() > 0) {

            QString returnChars(chars);
            Debug::read("[" + QString::number(QString(chars).length()) + "]" + returnChars); // <<read>>

            if (chars.indexOf(QString(ready_char)) > -1) {
                nRequested = TX_CHUNK_SIZE;
                chars = chars.replace(QString(ready_char), "");
            }
            rx_buffer += chars;
            while(true) {
                int posNewline = rx_buffer.indexOf('\n');
                if (posNewline == -1) {
                    break;
                } else {
                    QString line = rx_buffer.mid(0, posNewline);
                    rx_buffer = rx_buffer.mid(posNewline + 1);
                    process_status_line(line);
                }
            }
        }
        // sending
        if (tx_buffer != "") {
            if (nRequested > 0) {
                qint64 actuallySent = device->safeWrite(tx_buffer.mid(0, nRequested).toUtf8());
                device->waitForBytesWritten(SerialManager::WaitForWriteTime);

                if(actuallySent == -1) {
                    return;
                }

                // try ~ except
                Debug::write("[" + QString::number(QString(tx_buffer.mid(0, actuallySent)).length()) + "]" + QString(tx_buffer.mid(0, actuallySent)));

                // ****************************
                _lastSendStringLine += tx_buffer.mid(0, actuallySent).toUtf8();
                int newLineIndex = _lastSendStringLine.lastIndexOf("\n");
                if (newLineIndex > -1) {
                    _lastSendStringLine = _lastSendStringLine.mid(newLineIndex + 1);
                    setForceReadMode(true);
                }
                // ****************************

                tx_buffer = tx_buffer.mid(actuallySent);
                nRequested -= actuallySent;
                if (nRequested <= 0) {
                    last_request_ready = 0;
                }
            } else if (tx_buffer[0] == "!" || tx_buffer[0] == "~") {
                qint64 actuallySent = device->safeWrite(tx_buffer.mid(0, 1).toUtf8());
                device->waitForBytesWritten(SerialManager::WaitForWriteTime);

                Debug::write("[" + QString::number(QString(tx_buffer.mid(0, actuallySent)).length()) + "]" + QString(tx_buffer.mid(0, actuallySent)).replace("\n", "n"));
                // try ~ except
                tx_buffer = tx_buffer.mid(actuallySent);
                _lastSendStringLine = "";
            } else {
                if (QDateTime::currentDateTime().currentSecsSinceEpoch() - last_request_ready > 2.0) {
                    qint64 actuallySend = device->safeWrite(QString(request_ready_char).toUtf8());
                    device->waitForBytesWritten(SerialManager::WaitForWriteTime);
                    Debug::write("2.0sec... [" + QString::number(QString(request_ready_char).length()) + "]" + QString(request_ready_char));
                    if (actuallySend == 1) {
                        last_request_ready = QDateTime::currentDateTime().currentSecsSinceEpoch();
                    }
                }
            }
        } else {
            if (nRequested > 0) {
                if (_last_status_check_time.elapsed() > 1000) {
                    qint64 actuallySend = device->safeWrite(QString("?\n").toUtf8());
                    device->waitForBytesWritten(SerialManager::WaitForWriteTime);
                    Debug::write("[" + QString::number(QString(request_ready_char).length()) + "]" + "?");
                    if (actuallySend != -1) {
                        _last_status_check_time = QTime::currentTime();
                        nRequested -= actuallySend;
                        setForceReadMode(true);
                    }
                }
            } else {
                if (QDateTime::currentDateTime().currentSecsSinceEpoch() - last_request_ready > 2.0) {
                    qint64 actuallySend = device->safeWrite(QString(request_ready_char).toUtf8());
                    device->waitForBytesWritten(SerialManager::WaitForWriteTime);
                    Debug::write("Send ready char for ?... [" + QString::number(QString(request_ready_char).length()) + "]" + QString(request_ready_char));
                    if (actuallySend == 1) {
                        last_request_ready = QDateTime::currentDateTime().currentSecsSinceEpoch();
                    }
                }
            }
            if (job_active == true) {
                Debug::log("tx_buffer is empty, job done...");
                job_size     = 0;
                job_active   = false;
                status.ready = true;

            }
        }
        // try ~ except...
    } else {
        status.ready = false;
    }
}

void SerialManager::process_status_line(QString line)
{
    if (line.mid(0, 3).indexOf("#") > -1) {
        Debug::plain(line);
    } else if (line.count("^") != 0) {
//        Debug::plain("\nFEC Correction!\n");
    } else {
        if (line.count("!") != 0) {
            cancel_queue();
            status.ready = false;
//          Debug::plain(line);
            qCritical() << "Catch Exception";
        } else {
//            Debug::plain(".");
        }

        if (line.count("N") != 0) {
            status.bad_number_format_error = true;
//            Debug::log("bad_number_format_error");
            qCritical() << "Bad Number Error";
        }
        if (line.count("E") != 0) {
            status.expected_command_letter_error = true;
//            Debug::log("expected_command_letter_error");
            qCritical() << "Command Letter Error";
        }
        if (line.count("U") != 0) {
            status.unsupported_statement_error = true;
//            Debug::log("unsupported_statement_error");
            qCritical() << "Unsupported Error";
        }
        if (line.count("B") != 0) {
            status.buffer_overflow = true;
            Debug::status("buffer_overflow", "error");
            qCritical() << "Buffer Over Flow";
        } else {
            status.buffer_overflow = false;
            Debug::status("buffer_overflow", "safe");
        }
        if (line.count("T") != 0) {
            status.transmission_error = true;
            Debug::status("transmission", "error");
            qCritical() << "Transmission Error";
        } else {
            status.transmission_error = false;
            Debug::status("transmission", "safe");
        }
        if (line.count("P") != 0) {
            status.power_off = true;
            Debug::status("power_check", "off");
        } else {
            status.power_off = false;
            Debug::status("power_check", "on");
        }
        if (line.count("L") != 0) {
            status.limit_hit = true;
            Debug::status("limit_hit_check", "hit");
        } else {
            status.limit_hit = false;
            Debug::status("limit_hit_check", "-");
        }
        if (line.count("R") != 0) {
            status.serial_stop_request = true;
            Debug::status("stop_request", "stop");
        } else {
            status.serial_stop_request = false;
            Debug::status("stop_request", "-");
        }
        if (line.count("D") != 0) {
            status.door_open = true;
            Debug::status("door_state_check", "open");
        } else {
            status.door_open = false;
            Debug::status("door_state_check", "close");
        }
        if (line.count("C") != 0) {
            status.chiller_off = true;
            Debug::status("chiller_state_check", "off");
        } else {
            status.chiller_off = false;
            Debug::status("chiller_state_check", "on");
        }

        if (line.count("X") != 0) {
            int i = line.indexOf("X");
            int j = line.indexOf("Y");
            status.x = line.mid(i + 1, j - i - 1).toDouble();
        }
        if (line.count("Y") != 0) {
            int i = line.indexOf("Y");
            int j = line.indexOf("V");
            status.x = line.mid(i + 1, j - i - 1).toDouble();
        }
        if (line.count("V") != 0) {
            int i = line.indexOf("V");
            int j = line.indexOf("\n");
            status.firmware_version = line.mid(i + 1, j - i);
        }
    }
}

void SerialManager::setForceReadMode(bool enable)
{
    _isForceReadMode = enable;
    if (enable) {
        Debug::status("*force read mode*", "ON");
    } else {
        Debug::status("*force read mode*", "OFF");
    }
}

QString SerialManager::getControlBoardName()
{
    QString productType = QSysInfo::productType();
    if (productType == "windows") {
        return "STM32 Virtual ComPort";
    }
    if (productType == "macos") {
        return "STMicroelectronics Virtual COM Port";
    }
    return "STM32 Virtual ComPort";
}

SerialManager::Status SerialManager::getState()
{
    return status;
}
