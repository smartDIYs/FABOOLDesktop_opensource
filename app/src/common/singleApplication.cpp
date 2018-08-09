#include "singleApplication.h"

#include <QtWidgets>

SingleApplication::SingleApplication(int &argc, char **argv):
    QApplication(argc, argv)
{
// 2重起動防止
    QFileInfo finfo("./semop");
    if(finfo.exists() == true) {
        mIsActivated = true;
    } else {
        QFile semphFile("./semop");
        semphFile.open(QFile::ReadWrite);
        semphFile.close();
        mIsActivated = false;
    }
}

SingleApplication::~SingleApplication()
{
// 2重起動防止　後始末
    if(mIsActivated == false)
    { QFile::remove("./semop"); }
}
