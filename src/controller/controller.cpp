#include "controller.h"
#include "wdLog/log.h"
#include <iostream>
#include "start/initParam.h"
#include "math.h"

Controller::~Controller()
{
}
Controller::Controller()
{
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

bool Controller::createRunTask(Robot *robot, const std::vector<double> &q, TaskSpace plannerTaskSpace)
{
    if (pControllerState->controllerStatus != RunStatus::wait_)
    {
        wdlog_w("Controller", "机器人运动中，创建运行任务失败\n");
        return false;
    }
    if (pControllerCommand->runSign)
    {
        wdlog_e("Controller", "runSign信号未清，创建运行任务失败\n");
        return false;
    }
    if (plannerTaskSpace == TaskSpace::jointSpace)
    {
        if (!this->checkUserJointLimit(robot, q))
        {
            wdlog_e("Controller", "目标位置超限:%f, %f, %f,%f,%f,%f,%f\n",
                    q[0], q[1], q[2], q[3], q[4], q[5], q[6]);
            return false;
        }
        pControllerCommand->plannerTaskSpace = plannerTaskSpace;
        for (int i = 0; i < q.size(); i++)
        {
            pControllerCommand->q_final[i] = q[i] + robot->getUserZeroPosition()[i];
        }
        pControllerCommand->runSign = true;
    }
    else if (plannerTaskSpace == TaskSpace::cartesianSpace)
    {
        pControllerCommand->plannerTaskSpace = plannerTaskSpace;
        for (int i = 0; i < q.size(); i++)
        {
            pControllerCommand->x_final[i] = q[i];
        }
        pControllerCommand->runSign = true;
    }
    return true;
}
void Controller::stopRun()
{
    if (pControllerState->controllerStatus != RunStatus::run_)
    {
        wdlog_w("Controller", "机器人未运动，停止失败\n");
        return;
    }
    if (pControllerCommand->stopSign)
    {
        wdlog_e("Controller", "stopSign信号未清，创建运行任务失败\n");
        return;
    }
    pControllerCommand->stopSign = true;
}

// jog
void Controller::startJogMove(int jogNum, int dir, TaskSpace type)
{
    if (!this->changeSpace(type))
    {
        wdlog_w("Controller", "机器人不处于停止状态，点动失败\n");
        return;
    }
    pControllerCommand->jogSign = true;
    pControllerCommand->jogNum = jogNum;
    pControllerCommand->jogDir = dir;
}
void Controller::stopJogMove()
{
    wdlog_d("Controller", "stopJogMove \n");
    pControllerCommand->jogSign = false;
}
void Controller::resetJogTimeOut()
{
    this->jogTimeCount = 0;
}
void Controller::jogCheckTimeOut()
{
    if (!pControllerCommand->jogSign)
        return;
    this->jogTimeCount++;
    if (this->jogTimeCount > 100)
    {
        this->jogTimeCount = 0;
        this->stopJogMove();
    }
}

// controller
bool Controller::changeControllerLaw(ControllerLawType type)
{
    if (pControllerState->controllerStatus != RunStatus::wait_)
    {
        wdlog_w("Controller", "机器人不处于停止状态，切换控制律失败\n");
        return false;
    }
    Json::Value root;
    root["controlLaw"] = (int)type;
    setConfigParam(ControllerJsonPath, root);
    pControllerCommand->controllerLawType_d = type;
    return true;
}
ControllerLawType Controller::getControllerLaw()
{
    return pControllerCommand->controllerLawType_d;
}

// planner
bool Controller::changePlanner(PlannerType type)
{
    if (pControllerState->controllerStatus != RunStatus::wait_)
    {
        wdlog_w("Controller", "机器人不处于停止状态，切换规划器失败\n");
        return false;
    }
    Json::Value root;
    root["planner"] = (int)type;
    setConfigParam(ControllerJsonPath, root);
    pControllerCommand->plannerType_d = type;
    return true;
}
PlannerType Controller::getPlanner()
{
    return pControllerCommand->plannerType_d;
}

bool Controller::changeSpace(TaskSpace type)
{
    if (pControllerState->controllerStatus != RunStatus::wait_)
    {
        wdlog_w("Controller", "机器人不处于停止状态，切换坐标系失败\n");
        return false;
    }
    Json::Value root;
    root["space"] = (int)type;
    setConfigParam(ControllerJsonPath, root);
    pControllerCommand->plannerTaskSpace = type;
    return true;
}
TaskSpace Controller::getSpace()
{
    return pControllerCommand->plannerTaskSpace;
}

// speed
void Controller::setRunSpeed(int runSpeedRatio)
{
    this->pControllerCommand->runSpeed_d = std::max(1, std::min(100, runSpeedRatio));
    Json::Value root;
    root["runSpeed"] = this->pControllerCommand->runSpeed_d;
    setConfigParam(ControllerJsonPath, root);
}
void Controller::setJogSpeed(int jogSpeedRatio)
{
    this->pControllerCommand->jogSpeed_d = std::max(1, std::min(100, jogSpeedRatio));
    Json::Value root;
    root["jogspeed"] = this->pControllerCommand->jogSpeed_d;
    setConfigParam(ControllerJsonPath, root);
}
int Controller::getRunSpeed()
{
    return this->pControllerCommand->runSpeed_d;
}
int Controller::getJogSpeed()
{
    return this->pControllerCommand->jogSpeed_d;
}

// limit
void Controller::setUserJointLimit(Robot *robot, std::vector<double> &qMax, std::vector<double> &qMin, std::vector<double> &dqLimit,
                                   std::vector<double> &ddqLimit, std::vector<double> &dddqLimit)
{
    if (pControllerCommand->newLimit)
    {
        wdlog_e("Controller", "newLimit信号未清，限位设置失败\n");
        return;
    }
    std::copy(qMax.begin(), qMax.end(), this->pControllerCommand->qMax);
    std::copy(qMin.begin(), qMin.end(), this->pControllerCommand->qMin);
    std::copy(dqLimit.begin(), dqLimit.end(), this->pControllerCommand->dqLimit);
    std::copy(ddqLimit.begin(), ddqLimit.end(), this->pControllerCommand->ddqLimit);
    std::copy(dddqLimit.begin(), dddqLimit.end(), this->pControllerCommand->dddqLimit);
    pControllerCommand->newLimit = true;
}
void Controller::getUserJointLimit(Robot *robot, std::vector<double> &qMax, std::vector<double> &qMin, std::vector<double> &dqLimit,
                                   std::vector<double> &ddqLimit, std::vector<double> &dddqLimit)
{
    // for (int i = 0; i < robot->getRobotDof(); i++)
    // {
    //     qMax[i] = pControllerCommand->qMax[i];
    //     qMin[i] = pControllerCommand->qMin[i];
    //     dqLimit[i] = pControllerCommand->dqLimit[i];
    //     ddqLimit[i] = pControllerCommand->ddqLimit[i];
    //     ddqLimit[i] = pControllerCommand->dddqLimit[i];
    // }
}
bool Controller::checkUserJointLimit(Robot *robot, const std::vector<double> &q_d)
{
    for (int i = 0; i < q_d.size(); i++)
    {
        if (q_d[i] < pControllerCommand->qMin[i] || q_d[i] > pControllerCommand->qMax[i])
            return false;
    }
    return true;
}

// zero
bool Controller::setZero(Robot *robot)
{
    if (pControllerState->controllerStatus != RunStatus::wait_)
    {
        wdlog_w("Controller", "机器人运动中，无法设置零点\n");
        return false;
    }
    return true;
}
bool Controller::backToZero(Robot *robot)
{
    std::vector<double> q(7, 0.0);
    return this->createRunTask(robot, q, TaskSpace::jointSpace);
}