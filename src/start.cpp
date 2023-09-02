#include <fstream>  //note 文件
#include <iostream> //note 标准IO，终端
#include "libjsoncpp/json.h"
#include "config.h"
#include "controller/controller.h"
#include "processCom/communication.h"

extern Communication *communication;
extern Controller *controller;

bool getJsonValueFromFile(const std::string &filePath, Json::Value &root)
{
    Json::Reader reader;
    std::ifstream file;
    file.open(filePath);
    if (!file.is_open())
    {
        return false;
    }
    else
    {
        if (!reader.parse(file, root))
        {
            file.close();
            return false;
        }
    }
    file.close();
    return true;
}
bool setJsonValueToFile(const std::string &filePath, Json::Value &root)
{
    Json::StyledWriter styleWriter;
    std::ofstream file;
    std::string str = styleWriter.write(root);
    file.open(filePath);
    if (!file.is_open())
    {
        return false;
    }
    file << str;
    file.close();
    return true;
}

void initControllerParam()
{
    Json::Value root;
    if (!getJsonValueFromFile(ControllerJsonPath, root))
        std::cout << "readJson error!!" << std::endl;

    root["ControllerID"] = std::string(__DATE__) + " " + __TIME__;
    setJsonValueToFile(ControllerJsonPath, root);

    controller->setJogSpeed(root["jogspeed"].asDouble());
    controller->setRunSpeed(root["runSpeed"].asDouble());
}

void startController(Communication *&communication, Controller *&controller, key_t messageKey, key_t sharedMemorykey)
{
    RobotData *pRobotData;
    ControllerCommand *pControllerCommand;
    ControllerState *pControllerState;

    // 进程通信
    if (communication == nullptr)
        communication = new Communication;
    if (controller == nullptr)
        controller = new Controller;

    if (communication->createConnect(messageKey, sharedMemorykey, pRobotData, pControllerCommand, pControllerState))
        std::cout << "通信信道建立成功: qId:" << std::hex << SM_ID << " mId:" << MS_ID << "\n";
    communication->clearMsg();

    controller->setpRobotData(pRobotData);
    controller->setpControllerCommand(pControllerCommand);
    controller->setpControllerState(pControllerState);
}
