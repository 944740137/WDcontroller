#pragma once

#include "messageData.h"
#include "robot/robot.h"
#include <vector>
class Controller
{
private:
    RobotData *pRobotData;
    ControllerCommand *pControllerCommand;
    ControllerState *pControllerState;
    unsigned int commandNum = 0;

public:
    ~Controller();
    Controller();

    Controller(const Controller &) = delete;
    void operator=(const Controller &) = delete;

    void setpControllerCommand(ControllerCommand *pControllerCommand);
    void setpControllerState(ControllerState *pControllerState);

    const ControllerCommand *getpControllerCommand();
    const ControllerState *getpControllerState();

    bool createRunTask(const std::vector<double> &q, TaskSpace plannerTaskSpace);
    void stopRun();
    void setRunSpeed(double runSpeedRatio);
    void setJogSpeed(double jogSpeedRatio);
    double getRunSpeed();
    double getJogSpeed();

    bool changeControllerLaw(ControllerLawType type);
    void setLimit(Robot *robot, std::vector<double> &qMax, std::vector<double> &qMin, std::vector<double> &dqLimit,
                  std::vector<double> &ddqLimit, std::vector<double> &dddqLimit);
    void getLimit(Robot *robot, std::vector<double> &qMax, std::vector<double> &qMin, std::vector<double> &dqLimit,
                  std::vector<double> &ddqLimit, std::vector<double> &dddqLimit);
};