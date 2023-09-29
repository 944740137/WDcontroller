#include <unistd.h>
#include <iostream> //note 标准IO，终端
#include <iomanip>

#include "config.h"

#include "wdLog/log.h"
#include "event/signalEvent.h"
#include "keyboardIO/keyboardIO.h"
#include "taskDefine/taskDefine.h"

#include "processCom/communication.h"
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
	int i = 0;
	bool connectStatus = false;
	while (1)
	{
		i++;
		// wdlog_e("mytag","wdwd: %s %d %d\n", "qw", i, i);
		// wdlog_w("mytag","wdwd: %s %d %d\n", "qw", i, i);
		// wdlog_d("mytag","wdwd: %s %d %d\n", "qw", i, i);
		// wdlog_i("mytag","wdwd: %s %d %d\n", "qw", i, i);
		bool isConnect = false;
		connectStatus = communication->comRecvMessage(isConnect);
		if (isConnect)
		{
			wdlog_i("robotRun", "robotName: %s robotDof: %d\n", controller->getpControllerState()->name,
					controller->getpControllerState()->robotDof);
			robot->setRobotDof(controller->getpControllerState()->robotDof);
			initRobotParam(robot, controller->getpControllerState()->name);
		}

		if (connectStatus)
		{
			if (i % 500 == 0)
			{
				// printf("q0: %.4f q1: %.4f q2: %.4f q3: %.4f q4: %.4f q5: %.4f q6: %.4f \n", robot->getpRobotData()->q[0],
				// 	   robot->getpRobotData()->q[1], robot->getpRobotData()->q[2], robot->getpRobotData()->q[3],
				// 	   robot->getpRobotData()->q[4], robot->getpRobotData()->q[5], robot->getpRobotData()->q[6]);
				// printf("dq0: %.4f dq1: %.4f dq2: %.4f dq3: %.4f dq4: %.4f dq5: %.4f dq6: %.4f \n", robot->getpRobotData()->dq[0],
				// 	   robot->getpRobotData()->dq[1], robot->getpRobotData()->dq[2], robot->getpRobotData()->dq[3],
				// 	   robot->getpRobotData()->dq[4], robot->getpRobotData()->dq[5], robot->getpRobotData()->dq[6]);
				// printf("tau: %.4f tau: %.4f tau: %.4f tau: %.4f tau: %.4f tau: %.4f tau: %.4f \n", robot->getpRobotData()->tau[0],
				// 	   robot->getpRobotData()->tau[1], robot->getpRobotData()->tau[2], robot->getpRobotData()->tau[3],
				// 	   robot->getpRobotData()->tau[4], robot->getpRobotData()->tau[5], robot->getpRobotData()->tau[6]);
			}
			// printf("Status: %d\n", controller->getpControllerState()->controllerStatus);
		}
		else
		{
		}
		usleep(1000);
	}
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

	// 启动进程通信任务，创建控制器和机器人
	startIPC(communication, controller, robot, (key_t)SM_ID, (key_t)MS_ID);

	// 加载控制器参数
	initControllerParam();

	// 启动键盘控制任务
	createTask(KeyboardTask, nullptr, TaskName::KeyboardTask_);

	// 启动示教器通信
	// startTeachBox(communication, controller, robot, (key_t)SM_ID, (key_t)MS_ID);

	// 启动事件监听（信号/定时器）
	createTask(singalTask, nullptr, TaskName::singalTask_);

	// 启动webserver
	// createTask(KeyboardIO, nullptr, TaskName::KeyboardIO_);

	// 1ms任务
	robotRun();
}