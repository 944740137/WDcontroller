#include "controller.h"
#include <iostream>
Controller::~Controller()
{
}
Controller::Controller()
{
}
void Controller::setpRobotData(RobotData *pRobotData)
{
    this->pRobotData = pRobotData;
}
void Controller::setpControllerCommand(ControllerCommand *pControllerCommand)
{
    this->pControllerCommand = pControllerCommand;
}
void Controller::setpControllerState(ControllerState *pControllerState)
{
    this->pControllerState = pControllerState;
}
const RobotData *Controller::getpRobotData()
{
    return pRobotData;
}
const ControllerCommand *Controller::getpControllerCommand()
{
    return pControllerCommand;
}
const ControllerState *Controller::getpControllerState()
{
    return pControllerState;
}
bool Controller::createRunTask(const std::vector<double> &q, TaskSpace plannerTaskSpace)
{
    if (pControllerState->controllerStatus != RunStatus::wait_)
        return false;
    pControllerCommand->commandNum++;
    pControllerCommand->plannerTaskSpace = plannerTaskSpace;
    for (int i = 0; i < q.size(); i++)
    {
        pControllerCommand->q_final[i] = q[i];
    }
    return true;
}
void Controller::setRunSpeed(double runSpeed)
{
    if (runSpeed > 1.0)
        runSpeed = 1.0;
    if (runSpeed < 0)
        runSpeed = 0.01;
    pControllerCommand->runSpeed = runSpeed;
}
void Controller::setJogSpeed(double jogSpeed)
{
    if (jogSpeed > 1.0)
        jogSpeed = 1.0;
    if (jogSpeed < 0)
        jogSpeed = 0.01;
    pControllerCommand->jogSpeed = jogSpeed;
}
double Controller::getRunSpeed()
{
    return pControllerCommand->runSpeed;
}
double Controller::getJogSpeed()
{
    return pControllerCommand->jogSpeed;
}