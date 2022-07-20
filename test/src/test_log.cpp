#include <iostream>
#include <memory>
#include "fishjoy/fishjoy.hpp"


/// 默认 DEBUG 级别
fishjoy::Logger::ptr test_logger = FISHJOY_LOG_ROOT();

int main(int argc, char *argv[]) {
  fishjoy::EnvMgr::GetInstance()->init(argc, argv);
  fishjoy::Config::LoadFromDir(fishjoy::EnvMgr::GetInstance()->getConfigPath());
  
  FISHJOY_LOG_FATAL(test_logger) << "fatal msg";
  FISHJOY_LOG_ERROR(test_logger) << "err msg";
  FISHJOY_LOG_INFO(test_logger) << "info msg";
  FISHJOY_LOG_DEBUG(test_logger) << "debug msg";

  FISHJOY_LOG_FMT_FATAL(test_logger, "fatal %s:%d", __FILE__, __LINE__);
  FISHJOY_LOG_FMT_ERROR(test_logger, "err %s:%d", __FILE__, __LINE__);
  FISHJOY_LOG_FMT_INFO(test_logger, "info %s:%d", __FILE__, __LINE__);
  FISHJOY_LOG_FMT_DEBUG(test_logger, "debug %s:%d", __FILE__, __LINE__);
   
  sleep(1);
  fishjoy::SetThreadName("brand_new_thread");

  test_logger->setLevel(fishjoy::LogLevel::WARN);
  FISHJOY_LOG_FATAL(test_logger) << "fatal msg";
  FISHJOY_LOG_ERROR(test_logger) << "err msg";
  FISHJOY_LOG_INFO(test_logger) << "info msg"; // 不打印
  FISHJOY_LOG_DEBUG(test_logger) << "debug msg"; // 不打印


  fishjoy::FileLogAppender::ptr fileAppender(new fishjoy::FileLogAppender("./log.txt"));
  test_logger->addAppender(fileAppender);
  FISHJOY_LOG_FATAL(test_logger) << "fatal msg";
  FISHJOY_LOG_ERROR(test_logger) << "err msg";
  FISHJOY_LOG_INFO(test_logger) << "info msg"; // 不打印
  FISHJOY_LOG_DEBUG(test_logger) << "debug msg"; // 不打印

  fishjoy::Logger::ptr test_logger = FISHJOY_LOG_NAME("test_logger");
  fishjoy::StdoutLogAppender::ptr appender(new fishjoy::StdoutLogAppender);
  fishjoy::LogFormatter::ptr formatter(new fishjoy::LogFormatter("%d:%rms%T%p%T%c%T%f:%l %m%n")); // 时间：启动毫秒数 级别 日志名称 文件名：行号 消息 换行
  appender->setFormatter(formatter);
  test_logger->addAppender(appender);
  test_logger->setLevel(fishjoy::LogLevel::WARN);

  FISHJOY_LOG_ERROR(test_logger) << "err msg";
  FISHJOY_LOG_INFO(test_logger) << "info msg"; // 不打印

  // 输出全部日志器的配置
  test_logger->setLevel(fishjoy::LogLevel::INFO);
  FISHJOY_LOG_INFO(test_logger) << "logger config:" << fishjoy::LoggerMgr::GetInstance()->toYamlString();

  return 0;
}
