#include "cspch.h"
#include "log.h"
#include "util.h"

#include <iostream>

int main(int argc, char** argv) {
    CppServer::Logger::ptr logger(new CppServer::Logger);
    logger->addAppender(CppServer::LogAppender::ptr(new CppServer::StdoutLogAppender));

    CppServer::FileLogAppender::ptr file_appender(new CppServer::FileLogAppender("./log.txt"));
    CppServer::LogFormatter::ptr fmt(new CppServer::LogFormatter("%d%T%p%T%m%n"));
    file_appender->setFormatter(fmt);
    file_appender->setLevel(CppServer::LogLevel::ERROR);

    logger->addAppender(file_appender);

    //CppServer::LogEvent::ptr event(new CppServer::LogEvent(__FILE__, __LINE__, 0, CppServer::GetThreadId(), CppServer::GetFiberId(), time(0)));
    //event->getSS() << "hello CppServer log";
    //logger->log(CppServer::LogLevel::DEBUG, event);
    std::cout << "hello CppServer log" << std::endl;

    CS_LOG_INFO(logger) << "test macro";
    CS_LOG_ERROR(logger) << "test macro error";

    CS_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

    auto l = CppServer::LoggerMgr::GetInstance()->getLogger("xx");
    CS_LOG_INFO(l) << "xxx";
    return 0;
}