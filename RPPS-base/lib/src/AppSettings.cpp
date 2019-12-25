// Description: Настройки приложений
#ifdef ANDROID_1


#else
#include <QApplication>
#include <QProcessEnvironment>
#include <QDir>

#include <algorithm>
#include "AppSettings.h"

AppSettings::AppSettings()
{
}
const QString configSubDir = ".cfg_advent_dev";
const QString homePathEnvVarName =
#ifdef __unix__
("HOME");
#else
("HOMEPATH");
#endif

QString GetHomePath()
{
    return  QProcessEnvironment::systemEnvironment().value(homePathEnvVarName);
}

QString AppSettings::GetSettingsPath(const char *configFileExt)
{
    QString homePath = GetHomePath();
    QString cfgPath = homePath + QDir::separator() + configSubDir;

    QDir directory( cfgPath );
    if( directory.exists() == false)
        directory.mkpath( cfgPath );
    QString cfgFileName = QApplication::applicationName();
    if (cfgFileName.endsWith("4")||cfgFileName.endsWith("5"))
        cfgFileName.chop(1);
    if (cfgFileName.endsWith("4d")||cfgFileName.endsWith("5d"))
        cfgFileName.chop(2);
    cfgFileName += configFileExt;
    return cfgPath + QDir::separator() + cfgFileName;
}

void AppSettings::SaveIntoSettingsList(const QString &name, const QString &listValue)
{
    QSettings settings( GetSettingsPath(), QSettings::IniFormat);
    QStringList list = settings.value( name ).toStringList();
    if (list.size() == 1 && list.at(0) == "")
        list.removeAt(0);
    if (list.contains( listValue ))
        return;
    list << listValue;
    settings.setValue( name, list );
}

QStringList AppSettings::GetReversedList(const QString &listName)
{
    QSettings settings( GetSettingsPath(), QSettings::IniFormat);
    return GetReversedList( settings, listName );
}
QStringList AppSettings::GetReversedList(const QSettings &settings, const QString &listName)
{
    QStringList projFileList = settings.value( listName).toStringList();
    std::reverse(projFileList.begin(), projFileList.end());

    return projFileList;
}

#endif
