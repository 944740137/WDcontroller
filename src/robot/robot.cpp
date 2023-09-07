#include "robot.h"
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