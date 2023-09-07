#include <fstream>  //note 文件
#include <iostream> //note 标准IO，终端
#include "libjsoncpp/json.h"
#include "config.h"
#include "controller/controller.h"
#include "robot/robot.h"
#include "processCom/communication.h"
#include "wdLog/log.h"
extern Communication *communication;
extern Controller *controller;
extern Robot *robot;
/*--------------------------------------------------json--------------------------------------------------*/
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
    
    // controller
    if (!getJsonValueFromFile(ControllerJsonPath, root))
        wdlog_e("initControllerParam", "readJson error!!\n");
    root["ControllerID"] = std::string(__DATE__) + " " + __TIME__;
    setJsonValueToFile(ControllerJsonPath, root);
    controller->setJogSpeed(root["jogspeed"].asDouble());
    controller->setRunSpeed(root["runSpeed"].asDouble());

    // robot
    if (!getJsonValueFromFile(RobotJsonPath, root))
        wdlog_e("initControllerParam", "readJson error!!\n");
    double qMax[controller->robotDof], qMin[controller->robotDof], dqLimit[controller->robotDof], ddqLimit[controller->robotDof];
    for (int i = 0; i < controller->robotDof; i++)
    {
        qMax[i] = root["qMax"][i].asDouble();
        qMin[i] = root["qMin"][i].asDouble();
        dqLimit[i] = root["dqLimit"][i].asDouble();
        ddqLimit[i] = root["ddqLimit"][i].asDouble();
    }
    controller->setLimit(qMax, qMin, dqLimit, ddqLimit);
}

/*--------------------------------------------------startIPC--------------------------------------------------*/
void startIPC(Communication *&communication, Controller *&controller, Robot *&robot, key_t messageKey, key_t sharedMemorykey)
{
    RobotData *pRobotData;
    ControllerCommand *pControllerCommand;
    ControllerState *pControllerState;

    // 初始化对象
    if (communication == nullptr)
        communication = new Communication;

    if (communication->createConnect(messageKey, sharedMemorykey, pRobotData, pControllerCommand, pControllerState))
        wdlog_i("startIPC", "通信信道建立成功, 消息队列号: %x, 共享内存号: %x\n", SM_ID, MS_ID);
    else
    {
        wdlog_e("startIPC", "通信信道建立失败\n");
        exit(1);
    }
    communication->clearMsg();

    if (controller == nullptr)
        controller = new Controller(pControllerState->robotDof);
    if (robot == nullptr)
        robot = new Robot(pControllerState->robotDof);

    robot->setpRobotData(pRobotData);
    controller->setpControllerCommand(pControllerCommand);
    controller->setpControllerState(pControllerState);
}
