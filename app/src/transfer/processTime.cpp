#include "processTime.h"
// ==============================================================================
// ProcessLine class
// ==============================================================================
ProcessLine::ProcessLine(QPointF from, QPointF to, Type type, int speed)
{
    _line = QLineF(from.x(), from.y(), to.x(), to.y());
    _type = type;
    _speed = speed;

    if ((_line.p1().rx() - _line.p2().rx()) == 0) {
        _xDirection = ProcessLine::Direction::Zero;
    } else if ((_line.p1().rx() - _line.p2().rx()) < 0 ) {
        _xDirection = ProcessLine::Direction::Right;
    } else {
        _xDirection = ProcessLine::Direction::Left;
    }

    if ((_line.p1().ry() - _line.p2().ry()) == 0) {
        _yDirection = ProcessLine::Direction::Zero;
    } else if ((_line.p1().ry() - _line.p2().ry()) < 0 ) {
        _yDirection = ProcessLine::Direction::Bottom;
    } else {
        _yDirection = ProcessLine::Direction::Top;
    }
}

void ProcessLine::dump()
{
    qDebug() << "type: " << _type << " from: " << _line.p1() << " to: " << _line.p2()
             << "length:" << calcLength() << "speed :" << _speed << "xDir :" << _xDirection << "yDir :" << _yDirection;
}

ProcessLine::Direction ProcessLine::xDirection()
{
    return _xDirection;
}

ProcessLine::Direction ProcessLine::yDirection()
{
    return _yDirection;
}

QLineF ProcessLine::getLine()
{
    return _line;
}

qreal ProcessLine::getSpeed()
{
    return qreal(_speed);
}

ProcessLine::Type ProcessLine::getType()
{
    return _type;
}

qreal ProcessLine::calcAngleTo(ProcessLine processLine)
{
    return _line.angleTo(processLine.getLine());
}

qreal ProcessLine::calcLength()
{
    return _line.length();
}

qreal ProcessLine::calcProcessTime()
{
    qreal speedToMmPerSec = qreal(_speed) / 60.0;
    return calcLength() / speedToMmPerSec;
}

// ==============================================================================
// ProcessTime class
// ==============================================================================
ProcessTime::ProcessTime(QString gcode)
{
    _gcode = gcode;
}

void ProcessTime::setSeekSpeed(int speed)
{
    _seekSpeed = speed;
}


QList<QStringList> ProcessTime::createElementOfPointList()
{
    QStringList gcodeLines = _gcode.split("\n");

    QRegExp vectorPattern("[G](.+)[X](.+)[Y](.+)");
    QRegExp rasterOriginPointPattern("[G][0]+[X](.+)[Y](.+)");
    QRegExp rasterForwardFeedPattern("[G][8]\\s[R][0]");
    QRegExp rasterReverseFeedPattern("[G][8]\\s[R][1]");
    QRegExp rasterDataBinaryPattern("[G][8]\\s[D](.+)");
    QRegExp rasterPitchPattern("[G](.+)[P](.+)");
    QRegExp speedPattern("[G](.+)[F](.+)");

    QStringList originData;
    originData.append("origin");
    originData.append("0");
    originData.append("0");
    originData.append("0");
    originData.append("10000");

    QList<QStringList> elementOfPointList;
    elementOfPointList.append(originData);

    int currentSeekSpeed = _seekSpeed;
    int currentFeedSpeed = 6000;
    QStringList rasterPitchLine;

    QString dLine = "";
    qreal currentPitch = 0;
    qreal yAxisPoint = 0;
    QPointF currentRasterImageOrigin = QPointF(0, 0);

    foreach (QString line, gcodeLines) {

        int speedPatternPos = speedPattern.indexIn(line);
        if (speedPatternPos > -1) {
            ProcessLine::Type type = static_cast<ProcessLine::Type>(speedPattern.capturedTexts()[1].toInt());
            if (type == ProcessLine::Type::Feed) {
                currentFeedSpeed = speedPattern.capturedTexts()[2].toInt();
            } else {
                currentSeekSpeed = speedPattern.capturedTexts()[2].toInt();
            }
            continue;
        }

        // Vector move
        int vectorPatternPos = vectorPattern.indexIn(line);
        if (vectorPatternPos > -1) {
            QStringList point = vectorPattern.capturedTexts();
            ProcessLine::Type type = static_cast<ProcessLine::Type>(vectorPattern.capturedTexts()[1].toInt());
            if (type == ProcessLine::Type::Feed) {
                point.append(QString::number(currentFeedSpeed));
            } else {
                point.append(QString::number(currentSeekSpeed));
            }
            elementOfPointList.append(point);
        }

        // Raster ------------------------------------------------------------------------------
        // G08P Pitch setting
        int rasterPitchPatternPos = rasterPitchPattern.indexIn(line);
        if (rasterPitchPatternPos > -1) {
            rasterPitchLine = rasterPitchPattern.capturedTexts();
            currentPitch = rasterPitchLine[2].toDouble();
        }

        // G00 Image Origin Point
        int rasterOriginPointPatternPos = rasterOriginPointPattern.indexIn(line);
        if (rasterOriginPointPatternPos > -1) {
            QStringList rasterOriginPosList = rasterOriginPointPattern.capturedTexts();
            //currentImageOriginXAxis = rasterOriginPosList[1].toDouble();
            currentRasterImageOrigin.setX(rasterOriginPosList[1].toDouble());
            currentRasterImageOrigin.setY(rasterOriginPosList[2].toDouble());
            yAxisPoint = currentRasterImageOrigin.y();
        }

        // G8 R0 Forward Feed
        int rasterForwardFeedPatternPos = rasterForwardFeedPattern.indexIn(line);
        if (rasterForwardFeedPatternPos > -1) {

            QStringList rasterForwardList = rasterForwardFeedPattern.capturedTexts();

            qreal posX = currentRasterImageOrigin.x() - 5.0; //margin
            qreal posY = yAxisPoint;

            rasterForwardList.append(QString::number(ProcessLine::Type::Feed));
            rasterForwardList.append(QString::number(posX)); //X-axis
            rasterForwardList.append(QString::number(posY)); //Y-axis
            rasterForwardList.append(QString::number(currentFeedSpeed)); //speed

            elementOfPointList.append(rasterForwardList);

            if (posY == currentRasterImageOrigin.y()) {
                continue;
            }

            yAxisPoint += currentPitch;

            rasterForwardList.clear();
            rasterForwardList.append("G8 R0");
            rasterForwardList.append(QString::number(ProcessLine::Type::Feed));
            rasterForwardList.append(QString::number(posX)); //X-axis
            rasterForwardList.append(QString::number(posY + currentPitch)); //Y-axis
            rasterForwardList.append(QString::number(currentFeedSpeed)); //speed
            elementOfPointList.append(rasterForwardList);

            dLine = "";
        }

        // G8 R1 Reverse Feed
        int rasterReverseFeedPatternPos = rasterReverseFeedPattern.indexIn(line);
        if (rasterReverseFeedPatternPos > -1) {

            QStringList rasterReverseList = rasterReverseFeedPattern.capturedTexts();

            qreal length = currentPitch * dLine.length();
            qreal posX = currentRasterImageOrigin.x() + length + 5.0; //margin
            qreal posY = yAxisPoint;

            rasterReverseList.append(QString::number(ProcessLine::Type::Feed));
            rasterReverseList.append(QString::number(posX));
            rasterReverseList.append(QString::number(posY));
            rasterReverseList.append(QString::number(currentFeedSpeed));

            elementOfPointList.append(rasterReverseList);

            yAxisPoint += currentPitch;

            rasterReverseList.clear();
            rasterReverseList.append("G8 R1");
            rasterReverseList.append(QString::number(ProcessLine::Type::Feed));
            rasterReverseList.append(QString::number(posX));
            rasterReverseList.append(QString::number(posY + currentPitch));
            rasterReverseList.append(QString::number(currentFeedSpeed));
            elementOfPointList.append(rasterReverseList);

            dLine = "";
        }

        int rasterDataBinaryPatternPos = rasterDataBinaryPattern.indexIn(line);
        if (rasterDataBinaryPatternPos > -1) {
            dLine += rasterDataBinaryPattern.capturedTexts()[1];
        }
    }

    elementOfPointList.append(originData);

    return elementOfPointList;
}

QList<ProcessLine> ProcessTime::createProcessLineList(QList<QStringList> elementOfPointList)
{
    QList<ProcessLine> processLineList;

    for (int i = 0; i < elementOfPointList.count() - 1; i++) {

        QStringList fromData = elementOfPointList[i];
        QStringList toData   = elementOfPointList[i + 1];

        ProcessLine::Type toType = static_cast<ProcessLine::Type>(toData[1].toInt());
        QPointF from = QPointF(fromData[2].toDouble(), fromData[3].toDouble());
        QPointF to   = QPointF(toData[2].toDouble(),   toData[3].toDouble());
        int speed = toData[4].toInt();

        processLineList.append(ProcessLine(from, to, toType, speed));
    }
    return processLineList;
}

int ProcessTime::calc()
{

    QStringList originData;

    // Create element of point as QStringList from gcodes, format is
    // elementOfPoint : ["GCodeRawLine"]["posX"]["posY"]["seek/feed"]["speed"]
    QList<QStringList> elementOfPointList = createElementOfPointList();
    QList<ProcessLine> processLineList    = createProcessLineList(elementOfPointList);

    qreal totalTime = 0.0;
    qreal deg = 0;
    qreal timeTmp;

    for (int i = 0; i < processLineList.count() - 1; i++) {
        deg = processLineList[i].calcAngleTo(processLineList[i + 1]);
        timeTmp = 0;
        if (deg > 180) {
            deg = 360.0 - deg;
        }

        qreal lossTime    = (processLineList[i + 1].getSpeed() / 60.0) / 500.0;
        qreal needsLength = (processLineList[i + 1].getSpeed() / 60.0) * lossTime;
        qreal processTimeWithDelay = 0;

        if ((processLineList[i].calcLength() != 0) && (needsLength > processLineList[i].calcLength())) {
            processTimeWithDelay = qSqrt((4 * processLineList[i].calcLength()) / 500.0);
            lossTime = processTimeWithDelay - processLineList[i].calcProcessTime();
        }

        timeTmp += processLineList[i].calcProcessTime();

        if (deg != 0 &&
                (processLineList[i].xDirection() != processLineList[i + 1].xDirection() ||
                 processLineList[i].yDirection() != processLineList[i + 1].yDirection())) {
            timeTmp += lossTime;
        }

        totalTime += timeTmp;
    }
    return totalTime;
}
// ==============================================================================
// ProcessTimeCalculateBehavior class
// ==============================================================================
ProcessTimeCalculateBehavior::ProcessTimeCalculateBehavior() : QObject()
{

}

void ProcessTimeCalculateBehavior::processStart(QString gcode)
{
    ProcessTime processTime(gcode);
    int sec = processTime.calc();
    QTime time = QTime(0, 0, 0).addSecs(sec);
    emit processFinished(time);
}
// ==============================================================================
// ProcessTimeCalculator class
// ==============================================================================
ProcessTimeCalculator::ProcessTimeCalculator() : QObject()
{
    _thread   = new QThread;
    _behavior = new ProcessTimeCalculateBehavior();
    _isCalculating    = false;
    _hasUpdateRequest = false;

    _behavior->moveToThread(_thread);
    connect(_behavior, SIGNAL(processFinished(QTime)), this,       SLOT(onCalculateFinished(QTime)));
    connect(this,      SIGNAL(calcurateStart(QString)), _behavior, SLOT(processStart(QString)));
    _thread->start();
}

ProcessTimeCalculator::~ProcessTimeCalculator()
{
    _thread->exit();
    _thread->wait();
    _thread->deleteLater();
    delete(_thread);
    _thread = NULL;
}

void ProcessTimeCalculator::setUpdateRequest(bool enable)
{
    _hasUpdateRequest = enable;
}

bool ProcessTimeCalculator::hasUpdateRequest()
{
    return _hasUpdateRequest;
}

bool ProcessTimeCalculator::isCalculating()
{
    return _isCalculating;
}

void ProcessTimeCalculator::setCalculating(bool isRunning)
{
    _isCalculating = isRunning;
}

void ProcessTimeCalculator::calculate(QString gcode)
{
    _thread->terminate();
    _thread->wait();
    _thread->start();
    _isCalculating = true;
    emit calcurateStart(gcode);
}

void ProcessTimeCalculator::onCalculateFinished(QTime time)
{
    _isCalculating = false;
    emit calcurateFinished(time);

    if (_hasUpdateRequest) {
        emit callRecalcurateRequest();
        setUpdateRequest(false);
    }
}
