#ifndef MY_MUDUO_LOGGER_H
#define MY_MUDUO_LOGGER_H

#include"noncopyable.h"
#include<unordered_map>
#include<cstdio>
#include<string>

using std::string;
using std::unordered_map;

enum LogLevel
{
    INFO,       //  普通
    ERROR,      //  错误 不影响系统运行
    FATAL,      //  core 影响系统运行
    DEBUG       //  调试
};


//  通过定义宏，使得用户只需要调用宏即可完成日志打印，而无需自己获取Logger，设置level，拼接打印。
#define LOG_INFO(logmsgFormat,...) \
        do \
        { \
           char buf[1024] = {0}; \
           snprintf(buf,1024,logmsgFormat,##__VA_ARGS__); \
           \
           char header[512]={0}; \
           snprintf(header,512,"%s (%d) <%s> ",__FILE__,__LINE__,__FUNCTION__); \
           Logger& logger = Logger::getInstance(); \
           logger.setLogLevel(INFO); \
           logger.log(buf,header); \
        } while (0)
        

#define LOG_ERROR(logmsgFormat,...) \
        do \
        { \
            char buf[512]={0}; \
            snprintf(buf,512,logmsgFormat,##__VA_ARGS__); \
            \
            char header[512]={0}; \
            snprintf(header,512,"%s (%d) <%s> ",__FILE__,__LINE__,__FUNCTION__); \
            Logger &logger = Logger::getInstance(); \
            logger.setLogLevel(ERROR); \
            logger.log(buf,header); \
        }while(0)


#define LOG_FATAL(logmsgFormat,...) \
        do \
        { \
            char buf[1024]={0}; \
            snprintf(buf,1024,logmsgFormat,##__VA_ARGS__); \
            \
            char header[512]={0}; \
            snprintf(header,512,"%s (%d) <%s> ",__FILE__,__LINE__,__FUNCTION__); \
            Logger &logger = Logger::getInstance(); \
            logger.setLogLevel(FATAL); \
            logger.log(buf,header); \
            exit(-1); \
        }while(0)



#ifdef TEST_DEBUG
#define LOG_DEBUG(logmsgFormat,...) \
        do \
        { \
            char buf[1024]={0}; \
            snprintf(buf,1024,logmsgFormat,##__VA_ARGS__); \
            \
            Logger &logger = Logger::getInstance(); \
            logger.setLogLevel(DEBUG); \
            logger.log(buf); \
        }while(0)
#endif



//  Singleton Pattern
class Logger:noncopyable
{
public:
    //  getInstance
    static Logger& getInstance();
    //  set log level
    void setLogLevel(LogLevel);
    //  do the log 
    void log(const string&,const string&);
private:
    LogLevel logLevel_;
    unordered_map<LogLevel,string> levelName_;
    Logger();
};


#endif