#include "protocol.h"
#include "teachBoxCom.h"
#include "libjsoncpp/json.h"
#include "wdLog/log.h"
#include "arpa/inet.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>

#include "controller/controller.h"
#include "robot/robot.h"

extern Controller *controller;
extern Robot *robot;

struct TcpMessage
{
    uint16_t commandNum;
    char buf[256] = {0};
};

void sendToTeachBox(const int &commandNum, const int &cfd, Json::Value &root)
{
    static TcpMessage tcpSendMessage;
    Json::StyledWriter styleWriter;
    std::string str;
    tcpSendMessage.commandNum = commandNum;
    str = styleWriter.write(root);
    strcpy(tcpSendMessage.buf, str.c_str());
    send(cfd, &tcpSendMessage, sizeof(tcpSendMessage), 0);
    memset(tcpSendMessage.buf, 0, sizeof(tcpSendMessage.buf)); // 清除缓存
}

void cmdParsing(const TcpMessage &tcpRecvMessage, const int &cfd)
{
    Json::Reader reader;
    Json::Value recvObj;
    std::string str;

    Json::Value sendObj;
    switch (tcpRecvMessage.commandNum)
    {

    case Request_ChangeController:
        str = tcpRecvMessage.buf;
        if (reader.parse(str, recvObj))
            wdlog_d("cmdParsing", "Request_ChangeController %d\n", recvObj["controlLaw"].asInt());
        else
            wdlog_e("cmdParsing", "Request_ChangeController error\n");
        if (controller->changeControllerLaw((ControllerLawType)recvObj["controlLaw"].asInt()))
        {
            sendObj["controlLaw"] = (int)controller->getControllerLaw();
            sendObj["result"] = true;
        }
        else
        {
            sendObj["controlLaw"] = (int)controller->getControllerLaw();
            sendObj["result"] = false;
            sendObj["error"] = "change control law error";
            // wdlog_d("cmdParsing", "Request_ChangeController error\n");
        }
        sendToTeachBox(Response_ChangeController, cfd, sendObj);
        break;

    case Request_ChangePlanner:
        str = tcpRecvMessage.buf;
        if (reader.parse(str, recvObj))
            wdlog_d("cmdParsing", "Request_Planner %d\n", recvObj["planner"].asInt());
        else
            wdlog_e("cmdParsing", "Request_Planner error\n");
        if (controller->changePlanner((Planner)recvObj["planner"].asInt()))
        {
            sendObj["planner"] = (int)controller->getPlanner();
            sendObj["result"] = true;
        }
        else
        {
            sendObj["planner"] = (int)controller->getPlanner();
            sendObj["result"] = false;
            sendObj["error"] = "change planner error";
        }
        sendToTeachBox(Response_ChangePlanner, cfd, sendObj);
        break;

    case Ask_SlaveConnect:
        // 没有buf
        sendObj["connect"] = controller->connect;
        sendToTeachBox(Response_SlaveConnect, cfd, sendObj);
        break;
    default:
        wdlog_e("cmdParsing", "parse error\n");
        break;
    }
}

void teachBoxCommunication(const int &lfd)
{
    static int cfd = 0;
    static struct sockaddr_in clientAddr;
    static socklen_t clientAddrLen = sizeof(clientAddr);
    static TcpMessage tcpRecvMessage;
    // wdlog_d("teachBoxComTask", "---------\n");

    if (cfd == 0 || cfd == -1)
    {
        if ((cfd = accept(lfd, (sockaddr *)&clientAddr, &clientAddrLen)) == -1)
        {
            if (errno == EWOULDBLOCK || errno == EAGAIN)
                return;
            else
                wdlog_e("teachBoxComTask", "accept error\n");
        }
        int flags = fcntl(cfd, F_GETFL, 0);
        if (flags == -1)
            wdlog_e("teachBoxComTask", "fcntl error\n");
        if (fcntl(cfd, F_SETFL, flags | O_NONBLOCK) == -1)
            wdlog_e("teachBoxComTask", "fcntl error\n");

        char IP_buf[128] = {0};
        wdlog_i("teachBoxComTask", "示教器已连接 客户端ip: %s port: %d\n", inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, IP_buf, sizeof(IP_buf)), ntohs(clientAddr.sin_port));
    }
    int ret = recv(cfd, &tcpRecvMessage, sizeof(tcpRecvMessage), 0);

    if (ret == 0)
    {
        char IP_buf[128] = {0};
        wdlog_i("teachBoxComTask", "示教器已断开 客户端ip: %s port: %d\n", inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, IP_buf, sizeof(IP_buf)), ntohs(clientAddr.sin_port));
        cfd = 0;
        return;
    }

    if (ret < 0 && errno == EAGAIN) // 没有数据可读
        return;
    cmdParsing(tcpRecvMessage, cfd);
    memset(tcpRecvMessage.buf, 0, sizeof(tcpRecvMessage.buf)); // 清除缓存
    // wdlog_i("teachBoxComTask", "num %d buf: %s  ret: %d strlen: %d \n", tcpRecvMessage.num, tcpRecvMessage.buf, ret, strlen(tcpRecvMessage.buf));
}

void *teachBoxComTask(void *arg)
{
    wdlog_i("teachBoxComTask", "创建示教器通信任务\n");

    int lfd = 0;
    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
        wdlog_e("teachBoxComTask", "socket error\n");

    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_port = htons(9527);              // 端口
    serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); // IP
    int flags = fcntl(lfd, F_GETFL, 0);
    if (flags == -1)
        wdlog_e("teachBoxComTask", "fcntl error\n");
    if (fcntl(lfd, F_SETFL, flags | O_NONBLOCK) == -1)
        wdlog_e("teachBoxComTask", "fcntl error\n");

    int opt = 1;
    if (setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) != 0)
        wdlog_e("teachBoxComTask", "Server setsockopt failed");

    if (bind(lfd, (sockaddr *)&serverAddr, sizeof(serverAddr)) == -1)
        wdlog_e("teachBoxComTask", "bind error\n");

    listen(lfd, 128);

    while (1)
    {
        teachBoxCommunication(lfd);
        usleep(5 * 1000);
    }
    return nullptr;
}