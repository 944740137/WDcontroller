#include <fstream>  //note 文件
#include <iostream> //note 标准IO，终端
#include "libjsoncpp/json.h"
#include "config.h"
#include "controller/controller.h"
#include "robot/robot.h"
#include "processCom/communication.h"
#include "wdLog/log.h"
#include "initParam.h"

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

void initRobotParam(Robot *robot, std::string robotName)
{
    Json::Value root;
    if (!getJsonValueFromFile(RobotJsonPath, root))
        wdlog_e("initControllerParam", "readJson error!!\n");

    std::vector<double> qMax, qMin, dqLimit, ddqLimit, dddqLimit;
    for (int i = 0; i < robot->getRobotDof(); i++)
    {
        qMax.push_back(root[robotName]["qMax"][i].asDouble());
        qMin.push_back(root[robotName]["qMin"][i].asDouble());
        dqLimit.push_back(root[robotName]["dqLimit"][i].asDouble());
        ddqLimit.push_back(root[robotName]["ddqLimit"][i].asDouble());
        dddqLimit.push_back(root[robotName]["dddqLimit"][i].asDouble());
    }
    controller->setLimit(robot, qMax, qMin, dqLimit, ddqLimit, dddqLimit);
}

void initControllerParam()
{
    Json::Value root;
    if (!getJsonValueFromFile(ControllerJsonPath, root))
        wdlog_e("initControllerParam", "readJson error!!\n");
    root["ControllerID"] = std::string(__DATE__) + " " + __TIME__;
    setJsonValueToFile(ControllerJsonPath, root);
    controller->setJogSpeed(root["jogspeed"].asDouble());
    controller->setRunSpeed(root["runSpeed"].asDouble());
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
        wdlog_i("ipcTask", "创建进程通信任务, 消息队列号: %x, 共享内存号: %x\n", SM_ID, MS_ID);
    else
    {
        wdlog_e("startIPC", "进程通任务建立失败，退出程序\n");
        exit(1);
    }
    communication->clearMsg();

    if (controller == nullptr)
        controller = new Controller();
    if (robot == nullptr)
        robot = new Robot(pControllerState->robotDof);

    robot->setpRobotData(pRobotData);
    controller->setpControllerCommand(pControllerCommand);
    controller->setpControllerState(pControllerState);
}
