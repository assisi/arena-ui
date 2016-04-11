#ifndef GLOBAL
#define GLOBAL

#include <QString>
#include <QDateTime>
#include <QSettings>
#include <QDir>

class _assisi{
public :
    QString name, arenaFile, depFile, ngbFile, arenaLayer;
};

extern QSettings* settings;
extern QString date_time_format;
extern QString time_format;
extern _assisi assisiFile;

void loadConfig();

#endif // GLOBAL

