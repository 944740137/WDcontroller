#include "protocol.h"
#include "teachBoxCom.h"
#include "libjsoncpp/json.h"
#include "wdLog/log.h"
#include "arpa/inet.h"
#include <unistd.h>
#include <fcntl.h>
#include <iostream>
int lfd = 0, cfd = 0;
struct TcpMessage
{
    uint16_t num;
    char buf[256] = {0};
};
TcpMessage tcpSendMessage, tcpRecvMessage;
char IP_buf[128];
struct sockaddr_in serverAddr, clientAddr;
socklen_t clientAddrLen;
void sendToTeachBox(int num, int cfd_in, Json::Value &root)
{
    Json::StyledWriter styleWriter;
    std::string str;
    tcpSendMessage.num = num;
    str = styleWriter.write(root);
    strcpy(tcpSendMessage.buf, str.c_str());
    send(cfd_in, &tcpSendMessage, sizeof(tcpSendMessage), 0);
}
void cmdParsing(TcpMessage &tcpMessage, int cfd_in)
{
    Json::Reader reader;
    Json::Value recvObj;
    std::string str;

    Json::Value sendObj;
    switch (tcpMessage.num)
    {
    case Request_ChangeController:
        str = tcpMessage.buf;
        if (reader.parse(str, recvObj))
        {
            wdlog_d("cmdParsing", "con: %d\n", recvObj["Controller"].asInt());
        }
        sendObj["result"] = recvObj["Controller"].asInt();
        sendObj["error"] = "error";
        sendToTeachBox(Response_ChangeController, cfd_in, sendObj);
        break;
    case 2:
        tcpSendMessage.num = 22;
        tcpSendMessage.buf[0] = 'c';
        tcpSendMessage.buf[1] = 'd';
        send(cfd_in, &tcpSendMessage, sizeof(tcpSendMessage), 0);
        break;
    default:
        break;
    }
    memset(tcpSendMessage.buf, 0, sizeof(tcpSendMessage.buf)); // 清除缓存
    memset(tcpMessage.buf, 0, sizeof(tcpMessage.buf));         // 清除缓存
}
void teachBoxCommunication()
{
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

        wdlog_i("teachBoxComTask", "client ip: %s port: %d\n", inet_ntop(AF_INET, &clientAddr.sin_addr.s_addr, IP_buf, sizeof(IP_buf)), ntohs(clientAddr.sin_port));
    }
    int ret = recv(cfd, &tcpRecvMessage, sizeof(tcpRecvMessage), 0);

    if (ret == 0)
    {
        cfd = 0;
        return;
    }

    if (ret < 0 && errno == EAGAIN)
        return;
    cmdParsing(tcpRecvMessage, cfd);
    // wdlog_i("teachBoxComTask", "num %d buf: %s  ret: %d strlen: %d \n", tcpRecvMessage.num, tcpRecvMessage.buf, ret, strlen(tcpRecvMessage.buf));
}
void *teachBoxComTask(void *arg)
{
    wdlog_i("teachBoxComTask", "创建示教器通信任务\n");

    lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1)
        wdlog_e("teachBoxComTask", "socket error\n");

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
    clientAddrLen = sizeof(clientAddr);
    while (1)
    {
        teachBoxCommunication();
        usleep(500 * 1000);
    }
    return nullptr;
}