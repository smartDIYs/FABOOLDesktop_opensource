#include "debug.h"
//===============================================================================
// Class Methods
//===============================================================================
void Debug::log(QString string)
{
    qDebug().noquote() << string;
//    Debug::sharedInstance().addConsole(string);
}

void Debug::failed(QString string)
{
    QString printString = "[Failed] " + string;
//    qDebug().noquote() << printString;
    Debug::sharedInstance().addConsole(printString);
}

void Debug::error(QString string)
{
    QString printString = "[Error] " + string;
//    qDebug().noquote() << printString;
    Debug::sharedInstance().addConsole(printString);
}

void Debug::strong(QString string)
{
    QString printString = ">> " + string;
    qDebug().noquote() << printString;
//    Debug::sharedInstance().addConsole(printString);
}

void Debug::read(QString string)
{
    QString printString = "<<read>>  " + string.replace("\n", "\\n");
//    qDebug() << printString;
    Debug::sharedInstance().addConsole(printString);
}

void Debug::write(QString string)
{
    QString printString = "<<write>> " + string.replace("\n", "\\n");
//    qDebug() << printString;
    Debug::sharedInstance().addConsole(printString);
}

void Debug::console(QString string)
{
    QString printString = "[console] " + string;
//    qDebug() << printString;
    Debug::sharedInstance().addConsole(printString);
}

void Debug::gcode(QString string)
{
    Debug::sharedInstance().setGCode(string);
}

void Debug::plain(QString string)
{
    QTextStream out(stdout);
    out << string;
}

void Debug::status(QString key, QString value)
{
    Debug::sharedInstance().setStatus(key, value);
}
//===============================================================================
// Instance Methods
//===============================================================================
void Debug::addConsole(QString string)
{
    if (_enableConsole) {
        _consoleStringList.append(string);
//        emit updateConsole();
    }
}

QStringList Debug::getConsoleString()
{
    return _consoleStringList;
}

void Debug::clearConsoleStringList()
{
    _consoleStringList.clear();
}

void Debug::setEnable(bool value)
{
    _enableConsole = value;
}

void Debug::setGCode(QString gcode)
{
    emit updateGCode(gcode);
}

void Debug::setStatus(QString key, QString value)
{
    _statusDictionary[key] = value;
    QStringList stringList;
    stringList.append("[update :" + QTime::currentTime().toString("HH:mm:ss:zzz") + "]");
    foreach (QString key, _statusDictionary.keys()) {
        stringList.append(key + " \t " + _statusDictionary[key]);
    }
    emit updateStatus(stringList);
}
