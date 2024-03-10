#pragma once

#include "messageData.h"

#include <vector>

class Robot
{
private:
    RobotData *pRobotData = nullptr;
    int robotDof = 0;

public:
    ~Robot();
    Robot(int dof);

    Robot() = delete;
    Robot(const Robot &) = delete;
    void operator=(const Robot &) = delete;

    void setRobotDof(double Dof);
    int getRobotDof();
    void setpRobotData(RobotData *pRobotData);
    const RobotData *getpRobotData();
    double getpRobotCartesianPosition(int axis);
    double getpRobotJointPosition(int axis);
};
