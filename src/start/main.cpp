#include <unistd.h>
#include <iostream> //note 标准IO，终端
#include <iomanip>

#include "wdLog/log.h"
#include "event/signalEvent.h"
#include "keyboardIO/keyboardIO.h"
#include "taskDefine/taskDefine.h"

#include "processCom/communication.h"
#include "teachBoxCom/teachBoxCom.h"
#include "controller/controller.h"
#include "robot/robot.h"
#include "initParam.h"

Communication *communication;
Controller *controller;
Robot *robot;

extern void initControllerParam();
extern void startIPC(Communication *&communication, Controller *&controller, Robot *&robot, key_t messageKey, key_t sharedMemorykey);

int robotRun()
{
	CommunicationStatus communicationStatus = communication->comRecvMessage();
	if (communicationStatus == CommunicationStatus::successConnect)
	{
		wdlog_i("robotRun", "连接从站 robotName: %s robotDof: %d\n", controller->getpControllerState()->name,
				controller->getpControllerState()->robotDof);
		robot->setRobotDof(controller->getpControllerState()->robotDof);
		initRobotParam(robot, controller->getpControllerState()->name);
		controller->connect = true;
	}

	if (communicationStatus == CommunicationStatus::disconnected)
	{
		wdlog_i("robotRun", "从站断开\n");
		controller->connect = false;
	}
	//
	return 0;
}

int main(void)
{
	std::cout << "编译日期:" << __DATE__ << "\n";
	std::cout << "编译时刻:" << __TIME__ << std::endl;

	// 启动日志系统
	startLog();

	// 初始化主进程
	struct sched_param param = {MainThreadPolicy};
	if (sched_setscheduler(getpid(), SCHED_FIFO, &param) != 0)
	{
		wdlog_e("main", "-------------主进程初始化失败-------------\n");
	};
	sched_getparam(0, &param);
	wdlog_i("main", "-------------主进程初始化成功，调度策略: %d, 调度优先级: %d-------------\n",
			sched_getscheduler(0), param.sched_priority);

	// 创建进程通信，创建控制器和机器人
	startIPC(communication, controller, robot, (key_t)SM_ID, (key_t)MS_ID);

	// 加载控制器参数
	initControllerParam();

	// 启动键盘控制任务
	createTask(KeyboardTask, nullptr, TaskName::KeyboardTask_);

	// 启动事件监听任务（信号/定时器）
	createTask(singalTask, nullptr, TaskName::singalTask_);

	// 启动示教器通信
	createTask(teachBoxComTask, nullptr, TaskName::teachBoxComTask_);

	// 启动webserver
	// createTask(KeyboardIO, nullptr, TaskName::KeyboardIO_);

	// 1ms任务
	while (1)
	{
		robotRun();
		usleep(1000);
	}

	// wdlog_e("mytag","wdwd: %s %d %d\n", "qw", i, i);
	// wdlog_w("mytag","wdwd: %s %d %d\n", "qw", i, i);
	// wdlog_d("mytag","wdwd: %s %d %d\n", "qw", i, i);
	// wdlog_i("mytag","wdwd: %s %d %d\n", "qw", i, i);
}
