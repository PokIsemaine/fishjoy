#include <iostream>
#include "fishjoy/log.hpp"

int main(int argc, char** argv)
{
  fishjoy::Logger::ptr logger(new fishjoy::Logger);
  logger->addAppender(fishjoy::LogAppender::ptr(new fishjoy::StdoutLogAppender));

  fishjoy::LogEvent::ptr event(new fishjoy::LogEvent(logger,fishjoy::LogLevel::DEBUG,__FILE__,__LINE__,0, 1, 2,time(0)));

  logger->log(fishjoy::LogLevel::DEBUG, event);

  std::cout << "hello fishjoy log" << std::endl;
  return 0;
}