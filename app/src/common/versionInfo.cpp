#include "versionInfo.h"
#include "faboolApi.h"

VersionInfo::VersionInfo() : QObject()
{
    mCurrentMajor = APP_VERSION_MAJOR;
    mCurrentMinor = APP_VERSION_MINOR;
    mCurrentBuild = APP_VERSION_BUILD;
    qInfo() << "Software Version: V" + QString("%1.%2.%3").arg(mCurrentMajor).arg(mCurrentMinor).arg(mCurrentBuild);
}

QString VersionInfo::getCurrentVersionString()
{
    return QString("%1.%2.%3").arg(mCurrentMajor).arg(mCurrentMinor).arg(mCurrentBuild);
}

bool VersionInfo::fetchLatestVersion()
{
    FaboolAPIResponse *response = FaboolAPI::getLatestFaboolDesktopVersionInfo();
    if(response->hasReply() == false) {
        return false;
    }

    QString latestVersion = response->getData()["latestVersion"].toString();
    QStringList nums = latestVersion.split(".");
    if(nums.count() != 3) {
        return false;
    }

    mLatestMajor = nums[0].toInt();
    mLatestMinor = nums[1].toInt();
    mLatestBuild = nums[2].toInt();

    return true;
}

bool VersionInfo::isLatestVersion()
{
    int latest  = mLatestMajor  * 100 + mLatestMinor  * 10 + mLatestBuild;
    int current = mCurrentMajor * 100 + mCurrentMinor * 10 + mCurrentBuild;
    if(latest > current)
    { return false; }
    else
    { return true; }
}
