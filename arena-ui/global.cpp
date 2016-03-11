#include "global.h"

//extern variables initialization
QString log_folder;
QString cam_folder;
QString arena_folder;
bool log_on_def;
bool log_on;
QString date_time_format = "yy-MM-dd_HH-mm";
QString time_format = "HH-mm-ss-zzz";
Node ui_config;

void createConfig(){
    fs::path work_path(fs::current_path());

    Emitter new_config;
    new_config << BeginMap;
    new_config << Key << "log_folder" << Value << work_path.string() + "/log/";
    new_config << Key << "cam_folder" << Value << work_path.string() + "/cam/";
    new_config << Key << "arena_folder" << Value << work_path.string() + "/arena/";
    new_config << Key << "log_on_def" << Value << "1";
    new_config << EndMap;

    std::ofstream fout("ui.config", std::ofstream::out);
    fout << new_config.c_str();

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

    log_folder = QString::fromStdString(ui_config["log_folder"].as<std::string>());
    cam_folder = QString::fromStdString(ui_config["cam_folder"].as<std::string>());
    arena_folder = QString::fromStdString(ui_config["arena_folder"].as<std::string>());
    log_on_def = ui_config["log_on_def"].as<int>() != 0;
    log_on = log_on_def;
}

void saveConfig(){
    Emitter new_config;
    new_config << BeginMap;
    new_config << Key << "log_folder" << Value << log_folder.toStdString();
    new_config << Key << "cam_folder" << Value << cam_folder.toStdString();
    new_config << Key << "arena_folder" << Value << arena_folder.toStdString();
    new_config << Key << "log_on_def" << Value << (log_on_def?1:0);
    new_config << EndMap;

    std::ofstream fout("ui.config");
    fout << new_config.c_str();
}
