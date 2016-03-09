#ifndef GLOBAL
#define GLOBAL

#include <yaml-cpp/yaml.h>
#include <boost/filesystem/operations.hpp>
#include <boost/filesystem/path.hpp>
#include <QString>
#include <QDateTime>
#include <fstream>

using namespace YAML;

namespace fs = boost::filesystem;

//YAML_CONFIG VARIABLES
extern Node ui_config;

extern QString log_folder;
extern QString cam_folder;
extern QString arena_folder;

extern bool log_on_def;

//RUNTIME GLOBAL VARIABLES
extern bool log_on;
extern QDateTime start_time;
extern QString date_time_format;
extern QString time_format;

//CONFIG FUNCTIONS

void loadConfig();
void saveConfig();
void createConfig();



#endif // GLOBAL

