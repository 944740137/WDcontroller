
#define LOG_DIR "/home/wd/log/franka/main/"
#define LOG_NAME "wdlog"
#define LOG_SUFFIX "log"
#define MAX_NAME_LENGTH 64
#define MAX_lOG_SIZE 1024 // KB
#define MAX_INFO_SIZE 256
#define ERROR "Error"
#define WARN "Warn"
#define DEBUG "Debug"
#define INFO "Info"

enum Level
{
    info = 1,
    debug = 2,
    warn = 3,
    error = 4,
};
void startLog();
void logWrite(Level level, const char *tag, const char *format, ...);

#define wdlog_error(tag, ...) \
    logWrite(Level::error, tag, __VA_ARGS__)
#define wdlog_info(tag, ...) \
    logWrite(Level::info, tag, __VA_ARGS__)
#define wdlog_debug(tag, ...) \
    logWrite(Level::debug, tag, __VA_ARGS__)
#define wdlog_warn(tag, ...) \
    logWrite(Level::warn, tag, __VA_ARGS__)

#define wdlog_e(tag, ...) wdlog_error(tag, __VA_ARGS__)
#define wdlog_i(tag, ...) wdlog_info(tag, __VA_ARGS__)
#define wdlog_d(tag, ...) wdlog_debug(tag, __VA_ARGS__)
#define wdlog_w(tag, ...) wdlog_warn(tag, __VA_ARGS__)