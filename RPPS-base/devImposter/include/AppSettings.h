// Description: Настройки приложений
#ifndef APPSETTINGS_H
#define APPSETTINGS_H

#include <QSettings>

class AppSettings
{
public:
    AppSettings();
    static
    QString GetSettingsPath(const char *newParameter = ".conf");
    static
    void SaveIntoSettingsList(const QString &name, const QString & listValue);
    static
    QStringList GetReversedList(const QSettings& settings, const QString& listName);
    static
    QStringList GetReversedList(const QString& listName);
};

#endif // APPSETTINGS_H
