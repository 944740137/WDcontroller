#include "controller.h"
#include "robot/robot.h"
#include "wdLog/log.h"
#include <iostream>
Controller::~Controller()
{
}
Controller::Controller(int dof)
{
    this->robotDof = dof;
}
void Controller::setpControllerCommand(ControllerCommand *pControllerCommand)
{
    this->pControllerCommand = pControllerCommand;
}
void Controller::setpControllerState(ControllerState *pControllerState)
{
    this->pControllerState = pControllerState;
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
    pControllerCommand->planTaskNum++;
    pControllerCommand->plannerTaskSpace = plannerTaskSpace;
    for (int i = 0; i < q.size(); i++)
    {
        pControllerCommand->q_final[i] = q[i];
    }
    return true;
}
bool Controller::changeControllerLaw(ControllerLawType type)
{
    if (pControllerState->controllerStatus != RunStatus::wait_)
    {
        wdlog_w("Controller", "机器人运动中，切换控制律失败\n");
        return false;
    }
    pControllerCommand->controllerLawType_d = type;
    return true;
}
void Controller::stopRun()
{
    if (pControllerState->controllerStatus != RunStatus::run_)
    {
        wdlog_d("Controller", "机器人未运动，停止失败\n");
        return;
    }
    pControllerCommand->stopTaskNum++;
}
void Controller::setRunSpeed(double runSpeedRatio)
{
    this->pControllerCommand->runSpeed_d = std::max(0.01, std::min(1.0, runSpeedRatio));
}
void Controller::setJogSpeed(double jogSpeedRatio)
{
    this->pControllerCommand->jogSpeed_d = std::max(0.01, std::min(1.0, jogSpeedRatio));
}
void Controller::setRobotDof(double Dof)
{
    this->robotDof = Dof;
}
double Controller::getRunSpeed()
{
    return this->pControllerCommand->runSpeed_d;
}
double Controller::getJogSpeed()
{
    return this->pControllerCommand->jogSpeed_d;
}
void Controller::setLimit(double qMax[], double qMin[], double dqLimit[], double ddqLimit[], double dddqLimit[])
{
    for (int i = 0; i < this->robotDof; i++)
    {
        this->pControllerCommand->qMax[i] = qMax[i];
        this->pControllerCommand->qMin[i] = qMin[i];
        this->pControllerCommand->dqLimit[i] = dqLimit[i];
        this->pControllerCommand->ddqLimit[i] = ddqLimit[i];
    }
}
void Controller::getLimit(double qMax[], double qMin[], double dqLimit[], double ddqLimit[], double dddqLimit[])
{
    for (int i = 0; i < this->robotDof; i++)
    {
        qMax[i] = pControllerCommand->qMax[i];
        qMin[i] = pControllerCommand->qMin[i];
        dqLimit[i] = pControllerCommand->dqLimit[i];
        ddqLimit[i] = pControllerCommand->ddqLimit[i];
        ddqLimit[i] = pControllerCommand->dddqLimit[i];
    }
}
