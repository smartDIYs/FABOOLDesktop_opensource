#ifndef DEBUG_H
#define DEBUG_H

#include <QtCore>
#include <QTextEdit>
#include "stdio.h"
#include <iostream>

class Debug : public QObject
{
    Q_OBJECT
private:
    Debug()  = default;
    ~Debug() = default;

    static Debug* _sharedInstance;
    QStringList   _consoleStringList;
    bool          _enableConsole = false;
    QMap<QString, QString> _statusDictionary;

public:

    Debug(const Debug&) = delete;
    Debug& operator = (const Debug&) = delete;
    Debug(Debug&&)  = delete;
    Debug& operator = (Debug&&) = delete;

    static Debug& sharedInstance() {
        static Debug *_sharedInstance = 0;
        if (_sharedInstance == 0 ) {
            _sharedInstance = new Debug();
        }
        return *_sharedInstance;
    }

    static void log(QString string);
    static void failed(QString string);
    static void error(QString error);
    static void strong(QString string);
    static void plain(QString string);
    static void read(QString string);
    static void write(QString string);
    static void console(QString string);
    static void status(QString key, QString value);
    static void gcode(QString string);

    void addConsole(QString string);
    QStringList getConsoleString();
    void clearConsoleStringList();
    void setEnable(bool value);
    void setStatus(QString key, QString value);
    void setGCode(QString gcode);

signals:
    void showConsole(QString);
    void updateStatus(QStringList);
    void updateGCode(QString);
    void updateConsole();
};

#endif // DEBUG_H
