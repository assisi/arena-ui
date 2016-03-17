#ifndef GLOBAL
#define GLOBAL

#include <QString>
#include <QDateTime>
#include <QSettings>
#include <QDir>


extern QSettings* settings;
extern QString date_time_format;
extern QString time_format;

void loadConfig();

#endif // GLOBAL

