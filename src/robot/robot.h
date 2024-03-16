#pragma once

#include "messageData.h"

#include <vector>

class Robot
{

private:
    RobotData *pRobotData = nullptr;
    int robotDof = 0;
    std::vector<double> userZeroPosition;
    std::vector<double> mechanicalZeroPosition;

public:
    ~Robot();
    Robot(int dof);

    Robot() = delete;
    Robot(const Robot &) = delete;
    void operator=(const Robot &) = delete;

    void setRobotDof(double Dof);
    int getRobotDof();
    //
    void setpRobotData(RobotData *pRobotData);
    const RobotData *getpRobotData();
    // 角度制
    double getpRobotCartesianPosition(int axis);
    double getpRobotJointPosition(int axis, bool isMechanicalPosition);
    // zeroPosition
    const std::vector<double> &getUserZeroPosition();
    const std::vector<double> &getMechanicalZeroPosition();
    void setUserZeroPosition(std::vector<double> &&userZeroPosition);
    void setMechanicalZeroPosition(std::vector<double> &&mechanicalZeroPosition);
};
