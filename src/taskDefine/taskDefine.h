#include <map>

enum TaskName
{
    task1 = 1,
    task2 = 2,
    task3 = 3
};
struct TaskProperties
{
    int priority = 80;
    int policy = SCHED_RR;
    bool isDetach = true;
};

typedef void *(*TaskFun)(void *);
bool createTask(TaskFun task, void *arg, TaskName taskName);
bool createTask(TaskFun task, void *arg, int priority, int policy = SCHED_RR, bool isDetach = true);
typedef std::map<TaskName, TaskProperties> TaskPropertiesDefineMap;

/* 
typedef struct
{
    int detachstate;         // 线程的分离状态
    int schedpolicy;         // 线程的调度策略
    struct sched schedparam; // 线程的调度参数
    int inheritsched;        // 线程的继承性
    int scope;               // 线程的作用域
    size_t guardsize;        // 线程栈末尾的警戒缓冲区大小
    int stackaddr_set;       // 线程栈的设置
    void *stackaddr;         // 线程栈的启始位置
    size_t stacksize;        // 线程栈大小
} pthread_attr_t; 
*/