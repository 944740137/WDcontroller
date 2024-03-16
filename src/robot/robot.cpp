#include "robot.h"
#include "config.h"
#include "wdLog/log.h"
Robot::Robot(int dof)
{
    this->robotDof = dof;
    this->userZeroPosition.resize(dof);
    this->mechanicalZeroPosition.resize(dof);
};

Robot::~Robot(){

};
void Robot::setpRobotData(RobotData *pRobotData)
{
    this->pRobotData = pRobotData;
}
const RobotData *Robot::getpRobotData()
{
    return pRobotData;
}
void Robot::setRobotDof(double Dof)
{
    this->robotDof = Dof;
}
int Robot::getRobotDof()
{
    return this->robotDof;
}
double Robot::getpRobotJointPosition(int axis, bool isMechanicalPosition)
{
    if (axis < 1 || axis > 7)
        return 0;
    // wdlog_d("cmdParsing", "getpRobotCartesianPosition: %f\n", this->pRobotData->q[axis - 1] * 180 / PI);
    if (isMechanicalPosition)
        return this->pRobotData->q[axis - 1];
    else
        return (this->pRobotData->q[axis - 1] - this->userZeroPosition[axis - 1]);
}
double Robot::getpRobotCartesianPosition(int axis)
{
    if (axis < 1 || axis > 6)
        return 0;

    if (axis <= 3)
        return this->pRobotData->position[axis - 1];
    else
    {
        // wdlog_d("cmdParsing", "getpRobotJointPosition: %f\n", this->pRobotData->orientation[axis - 1 - 3] * 180 / PI);
        return this->pRobotData->orientation[(axis - 3) - 1];
    }
}

// zero
const std::vector<double> &Robot::getUserZeroPosition()
{
    return this->userZeroPosition;
}
const std::vector<double> &Robot::getMechanicalZeroPosition()
{
    return this->mechanicalZeroPosition;
}
void Robot::setUserZeroPosition(std::vector<double> &&userZeroPosition)
{
    this->userZeroPosition = std::move(userZeroPosition);
}
void Robot::setMechanicalZeroPosition(std::vector<double> &&mechanicalZeroPosition)
{
    this->mechanicalZeroPosition = std::move(mechanicalZeroPosition);
}