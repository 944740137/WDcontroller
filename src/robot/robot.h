#pragma once

#include "messageData.h"
#include <vector>

class Robot
{
private:
    RobotData *pRobotData = nullptr;

public:
    int robotDof = 0;
    ~Robot();
    Robot(int dof);

    Robot() = delete;
    Robot(const Robot &) = delete;
    void operator=(const Robot &) = delete;

    void setRobotDof(double Dof);
    void setpRobotData(RobotData *pRobotData);
    const RobotData *getpRobotData();
};
