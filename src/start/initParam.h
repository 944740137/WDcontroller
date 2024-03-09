#pragma once
#include "libjsoncpp/json.h"
#include "config.h"
#include <string>
void initRobotParam(Robot *robot, std::string robotName);
void initControllerParam();
void setConfigParam(const std::string &filePath, Json::Value &newRoot);