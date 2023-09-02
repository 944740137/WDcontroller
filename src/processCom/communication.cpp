#include <stdio.h>
#include "communication.h"
Communication::~Communication()
{
}
Communication::Communication()
{
}
bool Communication::checkConnect()
{
    this->timeoutCount++;
    if (this->timeoutCount >= this->maxTimeoutCount)
    {
        if (this->HeartBeatRecord == this->sharedMemoryBuff->slaveHeartbeat)
            this->isConnect = false;
        else
            this->isConnect = true;
        this->HeartBeatRecord = this->sharedMemoryBuff->slaveHeartbeat;
        this->timeoutCount = 0;
    }
    return this->isConnect;
}
bool Communication::createConnect(key_t messageKey, key_t sharedMemorykey, RobotData *&robotData,
                                  ControllerCommand *&controllerCommand, ControllerState *&controllerState)
{
    void *shared_memory = nullptr;
    robotData = &(this->messageBuff.robotData);

    this->shm_id = shmget((key_t)SM_ID, sizeof(struct SharedMemory), 0666 | IPC_CREAT);
    if (this->shm_id < 0)
    {
        printf("第一次共享内存创建失败\n");
        return false;
    }
    else
        // printf("共享内存创建成功\n");

        shared_memory = shmat(this->shm_id, NULL, 0);
    if (shared_memory == nullptr)
    {
        printf("共享内存映射失败\n");
        return false;
    }
    else
        // printf("共享内存映射成功\n");

        this->sharedMemoryBuff = (struct SharedMemory *)shared_memory;
    this->sharedMemoryBuff->masterHeartbeat = 0;
    this->HeartBeatRecord = this->sharedMemoryBuff->slaveHeartbeat;

    controllerCommand = &(this->sharedMemoryBuff->controllerCommand);
    controllerState = &(this->sharedMemoryBuff->controllerState);

    this->msgid = msgget((key_t)messageKey, 0666 | IPC_CREAT);
    if (this->msgid == -1)
    {
        printf("消息队列创建失败\n");
        return false;
    }
    else
        // printf("消息队列创建成功\n");

        return true;
}
bool Communication::comSendMessage()
{
    return this->connectStatus;
}
bool Communication::comRecvMessage()
{
    if (checkConnect())
    {
        /*赋值*/
        if (!this->connectStatus)
        {
            printf("从站连接\n");
            this->connectStatus = true;
        }
        if (msgrcv(this->msgid, (void *)&(this->messageBuff), sizeof(struct Message) - sizeof(long), 1, IPC_NOWAIT) == -1)
        {
            // printf("读取失败\n");
        }
        else
        {
            // printf("读取成功\n");
        }
        // printf("从站在线\n");
    }
    else
    {
        if (connectStatus)
        {
            printf("从站断开\n");
            this->clearMsg();
            this->connectStatus = false;
        }
        // printf("从站离线\n");
    }

    this->sharedMemoryBuff->masterHeartbeat++;
    return this->connectStatus;
}
void Communication::clearMsg()
{
    printf("清空消息队列中");
    Message message;
    while (msgrcv(msgid, &message, sizeof(Message) - sizeof(long), 0, IPC_NOWAIT) != -1)
    {
        printf(".");
    };
    printf(" :清空消息队列完成！\n");
}
bool Communication::closeConnect()
{
    return true;
}

