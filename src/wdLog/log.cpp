#include <dirent.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdarg.h>
#include <time.h>
#include <map>
#include "log.h"

struct LevelTag
{
    const char *prefix;
    const char *display;
};
typedef std::map<Level, LevelTag> LevelTagMap;
LevelTagMap levelTagMap = {
    {Level::info, {INFO, "32"}},
    {Level::debug, {DEBUG, "36"}},
    {Level::warn, {WARN, "33"}},
    {Level::error, {ERROR, "31"}},
};

#define ZERO_ASCII 48
static FILE *file = nullptr;
static int sign = -1;
static const char *logDir = LOG_DIR;          // 路径名
static const char *logName = LOG_NAME;        // 文件名
static const char *logSuffix = LOG_SUFFIX;    // 后缀名
static char completeLogName[MAX_NAME_LENGTH]; // 完整名

void delOldLog()
{
    char filePath[MAX_NAME_LENGTH];
    // 最大文件数9
    snprintf(filePath, sizeof(filePath), "%s%s9.%s", logDir, logName, logSuffix); // note 格式化字符串
    if (!remove(filePath) == 0)
        printf("[logError] 删除文件失败\n");
}

bool checkLogFile(struct dirent *entry, unsigned int fileNameLen, unsigned int fileSuffixLen)
{
    if (entry->d_name[fileNameLen] > '9' || entry->d_name[fileNameLen] < '0' || entry->d_name[fileNameLen + 1] != '.')
        return false;
    // char*字符串比较前N个字符
    return (strncmp(entry->d_name, logName, fileNameLen) == 0 &&
            strncmp(entry->d_name + fileNameLen + 2, logSuffix, fileSuffixLen) == 0);
}

void checkLogSize(int len)
{
    struct stat st;

    if (!stat(completeLogName, &st) == 0)
        printf("[logError] 获取文件大小失败\n");

    if (len + st.st_size > MAX_lOG_SIZE * 1024) // rename
    {
        fclose(file);

        if (sign >= 9)
        {
            delOldLog();
            sign--;
        }

        char newName[MAX_NAME_LENGTH];
        char oldName[MAX_NAME_LENGTH];
        for (int i = sign; i >= 0; i--)
        {
            snprintf(newName, sizeof(completeLogName), "%s%s%d.%s", logDir, logName, i + 1, logSuffix);
            snprintf(oldName, sizeof(completeLogName), "%s%s%d.%s", logDir, logName, i, logSuffix);
            if (!rename(oldName, newName) == 0)
                printf("[logError] 重命名文件时出错 old: %s, new: %s\n", oldName, newName);
        }
        sign++;

        file = fopen(completeLogName, "w");
        setbuf(file, NULL);
        if (file == nullptr)
            printf("[logError] Failed to open file\n");
    }
}
void startLog()
{
    DIR *dir; // 目录句柄
    if ((dir = opendir(logDir)) == 0)
        printf("[logError] open %s failed!\n", logDir);

    unsigned int logNamelen = strlen(logName); // note char*长度
    unsigned int logSuffixlen = strlen(logSuffix);

    struct dirent *entry; // 文件/目录信息
    while ((entry = readdir(dir)) != NULL)
    {
        if (!checkLogFile(entry, logNamelen, logSuffixlen))
            continue;
        if (sign < (entry->d_name[logNamelen] - ZERO_ASCII))
        {
            sign = (entry->d_name[logNamelen] - ZERO_ASCII);
        }
    }
    if (sign == -1) // 一个日志都没有创建
    {
        sign = 0;
    }
    snprintf(completeLogName, sizeof(completeLogName), "%s%s%d.%s", logDir, logName, 0, logSuffix);
    file = fopen(completeLogName, "a");
    setbuf(file, NULL);
    closedir(dir);
}

void logWrite(Level level, const char *tag, const char *format, ...)
{
    time_t now;
    struct tm *timenow;
    time(&now);
    timenow = localtime(&now);
    char sbuf[MAX_INFO_SIZE];

    sprintf(sbuf, "%s/%s ", levelTagMap[level].prefix, tag); // 前缀

    strftime(sbuf + strlen(sbuf), sizeof(sbuf), "[%Y-%m-%d %H:%M:%S]  ", timenow); // 时间标签
    va_list args;
    va_start(args, format);
    int length = vsnprintf(sbuf + strlen(sbuf), MAX_INFO_SIZE, format, args); // 内容
    va_end(args);

    checkLogSize(length);

    printf("\033[0m\033[1;%sm%s\033[0m", levelTagMap[level].display, sbuf); // 控制台输出

    fprintf(file, "%s", sbuf);
}