#ifndef GLOBAL
#define GLOBAL

#include <yaml-cpp/yaml.h>
#include <QDir>
#include <QString>
#include <QDateTime>
#include <fstream>

using namespace YAML;

//YAML_CONFIG VARIABLES
extern Node ui_config;

extern QString logFolder;
extern QString camFolder;
extern QString arenaFolder;

extern QString logSubFolder;
extern QString camSubFolder;

extern QTime trendTimeSpan;
extern int trendSampleTime_ms;

extern bool log_on;

//RUNTIME GLOBAL VARIABLES
extern QDateTime start_time;
extern QString date_time_format;
extern QString time_format;

//CONFIG FUNCTIONS

void loadConfig();
void saveConfig();
void createConfig();



#endif // GLOBAL

