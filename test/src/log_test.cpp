#include <iostream>
#include "fishjoy/log.hpp"
#include "fishjoy/util.hpp"

int main()
{
  fishjoy::Logger::ptr logger(new fishjoy::Logger);
  logger->addAppender(fishjoy::LogAppender::ptr(new fishjoy::StdoutLogAppender));

  fishjoy::FileLogAppender::ptr file_appender(new fishjoy::FileLogAppender("./log.txt"));
  fishjoy::LogFormatter::ptr fmt(new fishjoy::LogFormatter("%d%T%p%T%m%n"));
  file_appender->setFormatter(fmt);
  file_appender->setLevel(fishjoy::LogLevel::ERROR);

  logger->addAppender(file_appender);
  std::cout << "hello fishjoy log" << std::endl;

  FISHJOY_LOG_INFO(logger) << "test macro";
  FISHJOY_LOG_ERROR(logger) << "test macro error";

  FISHJOY_LOG_FMT_ERROR(logger, "test macro fmt error %s", "aa");

  auto logMgr = fishjoy::LoggerMgr::GetInstance()->getLogger("xx");
  FISHJOY_LOG_INFO(logMgr) << "xxx";

//  fishjoy::LogEvent::ptr event(new )
  return 0;
}