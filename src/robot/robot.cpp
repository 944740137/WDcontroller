#include "robot.h"
#include "config.h"
#include "wdLog/log.h"
Robot::Robot(int dof)
{
    this->robotDof = dof;
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
double Robot::getpRobotJointPosition(int axis)
{
    if (axis < 1 || axis > 7)
        return 0;
    // wdlog_d("cmdParsing", "getpRobotCartesianPosition: %f\n", this->pRobotData->q[axis - 1] * 180 / PI);
    return this->pRobotData->q[axis - 1] * 180 / PI;
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
        return this->pRobotData->orientation[(axis - 3) - 1] * 180 / PI;
    }
}