#include <fstream>  //note 文件
#include <iostream> //note 标准IO，终端
#include "libjsoncpp/json.h"
#include "config.h"
#include "controller/controller.h"
#include "processCom/communication.h"
#include "wdLog/log.h"
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

void startIPC(Communication *&communication, Controller *&controller, key_t messageKey, key_t sharedMemorykey)
{
    RobotData *pRobotData;
    ControllerCommand *pControllerCommand;
    ControllerState *pControllerState;

    if (communication->createConnect(messageKey, sharedMemorykey, pRobotData, pControllerCommand, pControllerState))
        wdlog_i("startIPC", "通信信道建立成功, 消息队列号: %x, 共享内存号: %x\n", SM_ID, MS_ID);
    communication->clearMsg();

    controller->setpRobotData(pRobotData);
    controller->setpControllerCommand(pControllerCommand);
    controller->setpControllerState(pControllerState);
}
