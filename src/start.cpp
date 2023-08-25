#include <unistd.h>
#include "taskDefine/taskDefine.h"
#include "processCom/communication.h"

Communication *communication;
RobotData *pRobotData;
ControllerCommand *pControllerCommand;

void *thread_function(void *arg)
{
	// pthread_t thread_id = pthread_self();
	// int policy;
	// struct sched_param param;

	// pthread_getschedparam(thread_id, &policy, &param);
	// printf("线程: %lu, Policy: %s, Priority: %d\n", thread_id,
	// 	   (policy == SCHED_FIFO) ? "SCHED_FIFO" : (policy == SCHED_RR)	 ? "SCHED_RR"
	// 										   : (policy == SCHED_OTHER) ? "SCHED_OTHER"
	// 																	 : "Unknown",
	// 	   param.sched_priority);

	int *a = (int *)arg;
	while (1)
	{
		(*a)++;
		usleep(1000 * 1000);
	}

	pthread_exit(NULL);
}
int robotRun()
{
	bool connectStatus = false;
	while (1)
	{
		connectStatus = communication->comRecvMessage();
		if (connectStatus)
		{
			printf("q0: %f q1: %f q2: %f q3: %f q4: %f q5: %f q6: %f \n ", pRobotData->q[0], pRobotData->q[1],
				   pRobotData->q[2], pRobotData->q[3], pRobotData->q[4], pRobotData->q[5], pRobotData->q[6]);
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
	// 初始化进程
	struct sched_param param = {40};
	if (sched_setscheduler(getpid(), SCHED_FIFO, &param) != 0)
	{
		printf("主进程初始化失败\n");
	};
	sched_getparam(0, &param);
	printf("主进程初始化成功，调度策略: %d, 调度优先级: %d\n", sched_getscheduler(0), param.sched_priority);

	int a = 0;
	createTask(thread_function, &a, TaskName::task1);

	// 进程通信
	if (communication == nullptr)
		communication = new Communication;
	if (communication->createConnect((key_t)SM_ID, (key_t)MS_ID, pRobotData, pControllerCommand))
		printf("通信信道建立成功\n");
	communication->clearMsg();

	robotRun();
}