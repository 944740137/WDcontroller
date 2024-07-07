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
    unsigned int jogTimeCount = 0;

public:
    bool connect = false; // 主从

    ~Controller();
    Controller();
    Controller(const Controller &) = delete;
    void operator=(const Controller &) = delete;

    // init
    void setpControllerCommand(ControllerCommand *pControllerCommand);
    void setpControllerState(ControllerState *pControllerState);
    const ControllerCommand *getpControllerCommand();
    const ControllerState *getpControllerState();

    // task
    bool createRunTask(Robot *robot, const std::vector<double> &q, TaskSpace plannerTaskSpace);
    void stopRun();
    // jog
    void startJogMove(int jogNum, int dir, TaskSpace type);
    void stopJogMove();
    void resetJogTimeOut();
    void jogCheckTimeOut();
    // vel
    void setRunSpeed(int runSpeedRatio);
    void setJogSpeed(int jogSpeedRatio);
    int getRunSpeed();
    int getJogSpeed();
    // 规控 坐标系
    bool changeControllerLaw(ControllerLawType type);
    ControllerLawType getControllerLaw();
    bool changePlanner(PlannerType type);
    PlannerType getPlanner();
    bool changeSpace(TaskSpace type);
    TaskSpace getSpace();

    // 限位
    void setUserJointLimit(Robot *robot, std::vector<double> &qMax, std::vector<double> &qMin, std::vector<double> &dqLimit,
                           std::vector<double> &ddqLimit, std::vector<double> &dddqLimit);
    void getUserJointLimit(Robot *robot, std::vector<double> &qMax, std::vector<double> &qMin, std::vector<double> &dqLimit,
                           std::vector<double> &ddqLimit, std::vector<double> &dddqLimit);
    bool checkUserJointLimit(Robot *robot, const std::vector<double> &q_d);
    // zero
    bool setZero(Robot *robot);
    bool backToZero(Robot *robot);
};