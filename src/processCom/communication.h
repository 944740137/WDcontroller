/**进程通信头文件*/
#pragma once
#include <sys/msg.h>
#include <sys/shm.h>
#include "messageData.h"

class Communication
{
private:
    int msgid = -1;
    int shm_id = -1;

    bool isConnect = false;     // 当前连接
    bool connectStatus = false; // 连接状态
    int timeoutCount = 0;
    int HeartBeatRecord = 0;

    const int maxTimeoutCount = 10;

    Message messageBuff;
    SharedMemory *sharedMemoryBuff = nullptr;

public:
    Communication();
    ~Communication();
    Communication(const Communication &) = delete;
    void operator=(const Communication &) = delete;

    bool checkConnect();
    bool createConnect(key_t messageKey, key_t sharedMemorykey, RobotData *&robotData,
                       ControllerCommand *&controllerCommand, ControllerState *&controllerState);
    bool comSendMessage();
    bool comRecvMessage(bool &isConnect);
    bool closeConnect();
    void clearMsg();
};