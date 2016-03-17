#include "globalHeader.h"

QSettings* settings;
QString date_time_format = "yy-MM-dd_HH-mm_";
QString time_format = "HH-mm-ss-zzz";

void loadConfig(){

    settings = new QSettings("ArenaUI", QSettings::IniFormat);

    if(!settings->value("exists").toBool()) {
        QString work_path = QDir::currentPath();

        settings->setValue("logFolder", work_path + "/log/");
        settings->setValue("camFolder", work_path + "/cam/");
        settings->setValue("arenaFolder", work_path + "/arena/");
        settings->setValue("trendTimeSpan", QTime(0,5,0));
        settings->setValue("trendSampleTime_ms", 500);
        settings->setValue("log_on", true);
        settings->setValue("forceLog", true);

        settings->setValue("exists",true);
    }

    settings->setValue("logSubFolder", settings->value("logFolder").toString() + QDate::currentDate().toString("yy-MM-dd") + "/");
    settings->setValue("camSubFolder", settings->value("camFolder").toString() + QDate::currentDate().toString("yy-MM-dd") + "/");

    if(!QDir(settings->value("logFolder").toString()).exists())QDir().mkdir(settings->value("logFolder").toString());
    if(!QDir(settings->value("camFolder").toString()).exists())QDir().mkdir(settings->value("camFolder").toString());
    if(!QDir(settings->value("logSubFolder").toString()).exists())QDir().mkdir(settings->value("logSubFolder").toString());
    if(!QDir(settings->value("camSubFolder").toString()).exists())QDir().mkdir(settings->value("camSubFolder").toString());

    settings->setValue("log_on", settings->value("forceLog").toBool());
}
