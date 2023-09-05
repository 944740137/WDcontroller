#include <unistd.h>
#include <iostream> //note 标准IO，终端
#include <iomanip>

#include "config.h"
#include "wdLog/log.h"
#include "keyboardIO/keyboardIO.h"
#include "taskDefine/taskDefine.h"
#include "controller/controller.h"
#include "processCom/communication.h"

Communication *communication;
Controller *controller;

extern void initControllerParam();
extern void startIPC(Communication *&communication, Controller *&controller, key_t messageKey, key_t sharedMemorykey);

int robotRun()
{
	int i = 0;
	bool connectStatus = false;
	while (1)
	{
		i++;
		// wdlog_e("mytag","wdwd: %s %d %d\n", "qw", i, i);
		// wdlog_i("mytag","wdwd: %s %d %d\n", "qw", i, i);
		// wdlog_d("mytag","wdwd: %s %d %d\n", "qw", i, i);

		connectStatus = communication->comRecvMessage();
		if (connectStatus)
		{
			// printf("q0: %f q1: %f q2: %f q3: %f q4: %f q5: %f q6: %f ", controller->getpRobotData()->q[0], controller->getpRobotData()->q[1],
			// 	   controller->getpRobotData()->q[2], controller->getpRobotData()->q[3], controller->getpRobotData()->q[4], controller->getpRobotData()->q[5], controller->getpRobotData()->q[6]);
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
		wdlog_e("main","主进程初始化失败\n");
	};
	sched_getparam(0, &param);
	wdlog_i("main","主进程初始化成功，调度策略: %d, 调度优先级: %d\n", sched_getscheduler(0), param.sched_priority);

	// 初始化
	if (communication == nullptr)
		communication = new Communication;
	if (controller == nullptr)
		controller = new Controller;

	// 启动进程通信
	startIPC(communication, controller, (key_t)SM_ID, (key_t)MS_ID);

	// 加载控制器参数
	initControllerParam();

	// 启动键盘控制
	createTask(KeyboardIO, nullptr, TaskName::KeyboardIO_);

	// 1ms任务
	robotRun();
}