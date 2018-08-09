#ifndef VERSIONINFO_H
#define VERSIONINFO_H

#include <QObject>

class VersionInfo : public QObject
{
    Q_OBJECT
public:
    explicit VersionInfo();
    VersionInfo(const VersionInfo&);

    QString getCurrentVersionString();
    bool fetchLatestVersion();
    bool isLatestVersion();

private:
    int mCurrentMajor;
    int mCurrentMinor;
    int mCurrentBuild;

    int mLatestMajor;
    int mLatestMinor;
    int mLatestBuild;
};

#endif // VERSIONINFO_H
