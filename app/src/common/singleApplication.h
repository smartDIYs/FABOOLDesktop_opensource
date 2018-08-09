#ifndef SINGLEAPPLICATION_H
#define SINGLEAPPLICATION_H

#include <QApplication>
#include <QObject>

class SingleApplication : public QApplication
{
public:
    SingleApplication(int &argc, char **argv);
    ~SingleApplication();

    bool isActivated() { return mIsActivated; }

private:
    bool mIsActivated;
};

#endif // SINGLEAPPLICATION_H
