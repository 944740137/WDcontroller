#include <sys/signal.h>
#include <stdlib.h>
#include <unistd.h>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>

#include "signalEvent.h"
#include "wdLog/log.h"

void signal_cb(int fd, short event, void *argc)
{
    wdlog_i("signalEvent", "收到信号\n");
}

void *singalTask(void *)
{

    wdlog_i("singalTask", "创建信号监听任务\n");

    // 创建event_base base应该可以多个
    struct event_base *base;
    base = event_base_new();

    // ctrl c
    struct event *signal_event = evsignal_new(base, SIGUSR1, signal_cb, base);
    event_add(signal_event, NULL);

    // 启动循环监听
    event_base_dispatch(base);

    // 释放event_base
    event_base_free(base);

    return nullptr;
}