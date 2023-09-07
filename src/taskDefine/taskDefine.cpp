#include <stdio.h>
#include <pthread.h>
#include "taskDefine.h"
#include "wdLog/log.h"
TaskPropertiesDefineMap taskPropertiesDefineList = {
    {TaskName::task1, {40 + 5, SCHED_RR, true}},
    {TaskName::task2, {40 + 10, SCHED_RR, true}},
    {TaskName::KeyboardIO_, {40 + 50, SCHED_RR, true}}};

bool createTask(TaskFun task, void *arg, int priority, int policy, bool isDetach)
{
    struct sched_param param = {priority};
    pthread_t thread_id;
    pthread_attr_t attr;

    if (pthread_attr_init(&attr) != 0 || pthread_attr_setschedpolicy(&attr, policy) != 0 || pthread_attr_setschedparam(&attr, &param) != 0 ||
        (isDetach ? pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED) : pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE)) != 0 ||
        pthread_attr_setinheritsched(&attr, PTHREAD_EXPLICIT_SCHED) != 0 || pthread_create(&thread_id, &attr, task, arg) != 0)
    {
        pthread_attr_destroy(&attr);
        return false;
    }
    wdlog_i("createTask", "创建任务: %lu 优先级: %d\n", thread_id, param.sched_priority);
    pthread_attr_destroy(&attr);
    return true;
}

bool createTask(TaskFun task, void *arg, TaskName taskName)
{
    return createTask(task, arg, taskPropertiesDefineList[taskName].priority,
                      taskPropertiesDefineList[taskName].policy,
                      taskPropertiesDefineList[taskName].isDetach);
}
