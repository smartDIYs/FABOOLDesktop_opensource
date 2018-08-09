#ifndef PROCESSTIME_H
#define PROCESSTIME_H

#include <QtCore>
#include <QLine>
#include <QRegExp>
#include <QThread>
#include "common/dataManager.h"
//---------------------------------------------------------------------------
class ProcessLine
{
public:
    enum Type { Seek = 0, Feed };
    enum Direction {Zero = 0, Left = 1, Top = 2, Bottom = 3, Right = 4 };
    ProcessLine(QPointF from, QPointF to, Type type, int speed);

    void dump();
    ProcessLine::Direction xDirection();
    ProcessLine::Direction yDirection();
    QLineF getLine();
    qreal  getSpeed();
    ProcessLine::Type    getType();
    qreal  calcAngleTo(ProcessLine processLine);
    qreal  calcLength();
    qreal  calcProcessTime();

private:
    QLineF  _line;
    Type    _type;
    int     _speed;
    Direction _xDirection;
    Direction _yDirection;
};
//---------------------------------------------------------------------------
class ProcessTime
{
public:
    ProcessTime(QString gcode);

    void setSeekSpeed(int speed);
    int calc();


private:
    QString _gcode;
    int _seekSpeed;

    QList<QStringList> createElementOfPointList();
    QList<ProcessLine> createProcessLineList(QList<QStringList> elementOfPointList);
};
//---------------------------------------------------------------------------
class ProcessTimeCalculator;
class ProcessTimeCalculateBehavior : public QObject
{
    Q_OBJECT
public:
    ProcessTimeCalculateBehavior();

public slots:
    void processStart(QString gcode);

signals:
    void processFinished(QTime);

};
//---------------------------------------------------------------------------
class ProcessTimeCalculateBehavior;
class ProcessTimeCalculator : public QObject
{
    Q_OBJECT
public:
    ProcessTimeCalculator();
    ~ProcessTimeCalculator();
    void calculate(QString gcode);
    bool isCalculating();
    void setCalculating(bool isRunning);
    void setUpdateRequest(bool enable);
    bool hasUpdateRequest();

signals:
    void calcurateStart(QString);
    void calcurateFinished(QTime);
    void callRecalcurateRequest();

public slots:
    void onCalculateFinished(QTime time);

private:
    bool _isCalculating;
    bool _hasUpdateRequest;
    QThread* _thread;
    ProcessTimeCalculateBehavior* _behavior;
};
//---------------------------------------------------------------------------
#endif // PROCESSTIME_H
