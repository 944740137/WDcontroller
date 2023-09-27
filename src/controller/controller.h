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
    double qMax[7] = {0.0};
    double qMin[7] = {0.0};
    double dqLimit[7] = {0.0};
    double ddqLimit[7] = {0.0};
    double dddqLimit[7] = {0.0};

public:
    int robotDof = 0;

    ~Controller();
    Controller(int dof);

    Controller() = delete;
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
    void setRobotDof(double Dof);
    double getRunSpeed();
    double getJogSpeed();

    bool changeControllerLaw(ControllerLawType type);
    void setLimit(double qMax[], double qMin[], double dqLimit[], double ddqLimit[], double dddqLimit[]);
    void getLimit(double qMax[], double qMin[], double dqLimit[], double ddqLimit[], double dddqLimit[]);
};