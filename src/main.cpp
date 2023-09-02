#include <unistd.h>
#include <iostream> //note 标准IO，终端
#include <iomanip>

#include "config.h"
#include "keyboardIO/keyboardIO.h"
#include "taskDefine/taskDefine.h"
#include "controller/controller.h"
#include "processCom/communication.h"

Communication *communication;
Controller *controller;

extern void initControllerParam();
extern void startController(Communication *&communication, Controller *&controller, key_t messageKey, key_t sharedMemorykey);

int robotRun()
{
	bool connectStatus = false;
	while (1)
	{
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
	std::cout.unsetf(std::ios::dec);
	std::cout.setf(std::ios::showbase); // 显示十六，八进制前缀
	std::cout << "编译日期:" << __DATE__ << "\n";
	std::cout << "编译时刻:" << __TIME__ << std::endl;

	// 初始化进程
	struct sched_param param = {MainThreadPolicy};
	if (sched_setscheduler(getpid(), SCHED_FIFO, &param) != 0)
	{
		printf("主进程初始化失败\n");
	};
	sched_getparam(0, &param);
	printf("主进程初始化成功，调度策略: %d, 调度优先级: %d\n", sched_getscheduler(0), param.sched_priority);

	// 创建进程通讯，初始化控制器
	startController(communication, controller, (key_t)SM_ID, (key_t)MS_ID);

	// 创建键盘事件
	createTask(KeyboardIO, nullptr, TaskName::KeyboardIO_);

	// 加载参数
	initControllerParam();

	// 上下通信
	robotRun();
}