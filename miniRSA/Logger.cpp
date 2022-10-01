#include "Logger.h"
#include "Timestamp.h"
#include <functional>
#include <cstdio>
#include<iostream>

using std::function;
using std::cout;
using std::endl;

Logger &Logger::getInstance()
{
    static Logger logger;
    return logger;
}

void Logger::setLogLevel(LogLevel level)
{
    logLevel_ = level;
}

void Logger::log(const string &msg,const string &header)
{
    cout << levelName_[logLevel_]
         << " "
         << Timestamp::now().toString()
         << " : "
         << header <<"  "
         << msg << endl;
}

Logger::Logger()
{
    levelName_.insert({INFO, "[INFO]"});
    levelName_.insert({ERROR, "[ERROR]"});
    levelName_.insert({FATAL, "[FATAL]"});
    levelName_.insert({DEBUG, "[DEBUG]"});
}