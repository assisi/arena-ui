#include "global.h"

//extern variables initialization
QString logFolder;
QString camFolder;
QString logSubFolder;
QString camSubFolder;
QString arenaFolder;
QTime trendTimeSpan;
int trendSampleTime_ms;
bool log_on;
QString date_time_format = "yy-MM-dd_HH-mm_";
QString time_format = "HH-mm-ss-zzz";
Node ui_config;

void createConfig(){
    QString work_path = QDir::currentPath();

    Emitter new_config;
    new_config << BeginMap;
    new_config << Key << "logFolder" << Value << work_path.toStdString() + "/log/";
    new_config << Key << "camFolder" << Value << work_path.toStdString() + "/cam/";
    new_config << Key << "arenaFolder" << Value << work_path.toStdString() + "/arena/";
    new_config << Key << "trendTimeSpan" << Value << QTime(0,5,0).toString(time_format).toStdString();
    new_config << Key << "trendSampleTime_ms" << Value << 500;
    new_config << Key << "log_on" << Value << "1";
    new_config << EndMap;

    std::ofstream fout("ui.config", std::ofstream::out);
    fout << new_config.c_str();
    fout.close();

    loadConfig();
}

void loadConfig(){

    try{
         ui_config = LoadFile("ui.config");
    }
    catch(YAML::BadFile& e){
        createConfig();
        return;
    }

    logFolder = QString::fromStdString(ui_config["logFolder"].as<std::string>());
    camFolder = QString::fromStdString(ui_config["camFolder"].as<std::string>());
    arenaFolder = QString::fromStdString(ui_config["arenaFolder"].as<std::string>());
    trendTimeSpan = QTime().fromString(QString::fromStdString(ui_config["trendTimeSpan"].as<std::string>()), time_format);
    trendSampleTime_ms = ui_config["trendSampleTime_ms"].as<int>();
    log_on = ui_config["log_on"].as<int>() != 0;

    logSubFolder = logFolder + QDate::currentDate().toString("yy-MM-dd") + "/";
    camSubFolder = camFolder + QDate::currentDate().toString("yy-MM-dd") + "/";

    if(!QDir(logFolder).exists())QDir().mkdir(logFolder);
    if(!QDir(camFolder).exists())QDir().mkdir(camFolder);
    if(!QDir(logSubFolder).exists())QDir().mkdir(logSubFolder);
    if(!QDir(camSubFolder).exists())QDir().mkdir(camSubFolder);

}

void saveConfig(){
    Emitter new_config;
    new_config << BeginMap;
    new_config << Key << "logFolder" << Value << logFolder.toStdString();
    new_config << Key << "camFolder" << Value << camFolder.toStdString();
    new_config << Key << "arenaFolder" << Value << arenaFolder.toStdString();
    new_config << Key << "trendTimeSpan" << Value << trendTimeSpan.toString(time_format).toStdString();
    new_config << Key << "trendSampleTime_ms" << Value << trendSampleTime_ms;
    new_config << Key << "log_on" << Value << (log_on?1:0);
    new_config << EndMap;

    std::ofstream fout("ui.config");
    fout << new_config.c_str();
}
