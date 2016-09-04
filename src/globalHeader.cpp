#include "globalHeader.h"

QSettings* g_settings;
QString g_date_time_format = "yy-MM-dd_HH-mm_";
QString g_time_format = "HH-mm-ss-zzz";
_assisi g_assisiFile;

void loadConfig(){

    g_settings = new QSettings("ArenaUI", QSettings::IniFormat);

    if(!g_settings->value("exists").toBool()) {
        QString work_path = QDir::currentPath();

        g_settings->setValue("logFolder", "/home/");
        g_settings->setValue("camFolder", "/home/");
        g_settings->setValue("arenaFolder", "/home/");
        g_settings->setValue("simulator", "/home/");
        g_settings->setValue("trendTimeSpan", QTime(0,5,0));
        g_settings->setValue("trendSampleTime_ms", 500);
        g_settings->setValue("log_on", true);
        g_settings->setValue("IR_on", true);
        g_settings->setValue("temp_on", true);
        g_settings->setValue("air_on", true);
        g_settings->setValue("vibr_on",true);
        g_settings->setValue("avgTime_on", true);
        g_settings->setValue("forceLog", true);
        g_settings->setValue("forceScene", true);

        g_settings->setValue("exists",true);
    }

    g_settings->setValue("logSubFolder", g_settings->value("logFolder").toString() + QDate::currentDate().toString("yy-MM-dd") + "/");
    g_settings->setValue("camSubFolder", g_settings->value("camFolder").toString() + QDate::currentDate().toString("yy-MM-dd") + "/");

    if(!QDir(g_settings->value("logFolder").toString()).exists())QDir().mkdir(g_settings->value("logFolder").toString());
    if(!QDir(g_settings->value("camFolder").toString()).exists())QDir().mkdir(g_settings->value("camFolder").toString());
    if(!QDir(g_settings->value("logSubFolder").toString()).exists())QDir().mkdir(g_settings->value("logSubFolder").toString());
    if(!QDir(g_settings->value("camSubFolder").toString()).exists())QDir().mkdir(g_settings->value("camSubFolder").toString());

    if(g_settings->value("forceLog").toBool()) g_settings->setValue("log_on", true);
    if(g_settings->value("forceScene").toBool()){
        g_settings->setValue("IR_on", true);
        g_settings->setValue("temp_on", true);
        g_settings->setValue("air_on", true);
        g_settings->setValue("vibr_on", true);
        g_settings->setValue("avgTime_on", true);
    }
}
