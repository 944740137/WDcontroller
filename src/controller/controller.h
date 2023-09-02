#pragma once

#include "messageData.h"
#include <vector>
class Controller
{
private:
    RobotData *pRobotData;
    ControllerCommand *pControllerCommand;
    ControllerState *pControllerState;
    unsigned int commandNum = 0;

public:
    Controller();
    ~Controller();
    Controller(const Controller &) = delete;
    void operator=(const Controller &) = delete;

    void setpRobotData(RobotData *pRobotData);
    void setpControllerCommand(ControllerCommand *pControllerCommand);
    void setpControllerState(ControllerState *pControllerState);

    const RobotData *getpRobotData();
    const ControllerCommand *getpControllerCommand();
    const ControllerState *getpControllerState();

    bool createRunTask(const std::vector<double> &q, TaskSpace plannerTaskSpace);
    void setRunSpeed(double runSpeed);
    void setJogSpeed(double jogSpeed);
    double getRunSpeed();
    double getJogSpeed();
};