#include <iostream>
#include <memory>
#include "fishjoy/fishjoy.hpp"

#define XX(str) do{printf("\n\nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx  [test] %s \n\n",str);}while(0);


/// 默认 DEBUG 级别
fishjoy::Logger::ptr g_logger = FISHJOY_LOG_ROOT();

void INIT() {
  g_logger->setFormatter("%d{%Y-%m-%d %H:%M:%S}%T%t%T%F%T[%p]%T[%c]%T%f:%l%T%m%n");
  g_logger->clearAppenders();
  g_logger->addAppender(fishjoy::StdoutLogAppender::ptr
                        (new fishjoy::StdoutLogAppender()));
  g_logger->setLevel(fishjoy::LogLevel::DEBUG);
}

void macro_stream_test() {
  INIT();
  FISHJOY_LOG_DEBUG(g_logger) << "macro FISHJOY_LOG_DEBUG";
  FISHJOY_LOG_INFO(g_logger)  << "macro FISHJOY_LOG_INFO";
  FISHJOY_LOG_WARN(g_logger)  << "macro FISHJOY_LOG_WARN";
  FISHJOY_LOG_ERROR(g_logger) << "macro FISHJOY_LOG_ERROR";
  FISHJOY_LOG_FATAL(g_logger) << "macro FISHJOY_LOG_FATAL";
}

void macro_fmt_test() {
  INIT();
  FISHJOY_LOG_FMT_DEBUG(g_logger, "%s","this is a info log") ;
  FISHJOY_LOG_FMT_INFO(g_logger, "this is the %d log", 2);
  FISHJOY_LOG_FMT_WARN(g_logger, "this log can get %d + %d = %d", 1,2,3);
  FISHJOY_LOG_FMT_ERROR(g_logger, "this log level is %d", fishjoy::LogLevel::ERROR);
  FISHJOY_LOG_FMT_FATAL(g_logger, "this log name is %s", g_logger->getName().c_str());
}

void appender_test() {
  INIT();
  fishjoy::FileLogAppender::ptr fileAppender(
      new fishjoy::FileLogAppender("/home/zsl/CLionProjects/fishjoy/test/output/file_test_log.txt"));
  fishjoy::StdoutLogAppender::ptr stdAppender(new fishjoy::StdoutLogAppender());

  g_logger->clearAppenders();
  std::cout << "only fileAppender" << std::endl;
  g_logger->addAppender(fileAppender);
  FISHJOY_LOG_FATAL(g_logger) << "appender_test fatal log";
  FISHJOY_LOG_ERROR(g_logger) << "appender_test error log";
  FISHJOY_LOG_WARN(g_logger) << "appender_test warn log";
  FISHJOY_LOG_INFO(g_logger)  << "appender_test info log";
  FISHJOY_LOG_DEBUG(g_logger) << "appender_test debug log";

  g_logger->delAppender(fileAppender);
  g_logger->addAppender(stdAppender);
}

void level_test() {
  INIT();
  FISHJOY_LOG_LEVEL(g_logger,g_logger->getLevel()) << "now g_logger loglevel = "
                                                  << fishjoy::LogLevel::ToString(g_logger->getLevel());


  g_logger->setLevel(fishjoy::LogLevel::FromString("testUknown"));
  std::cout << "now g_logger loglevel = " << fishjoy::LogLevel::ToString(g_logger->getLevel()) << std::endl;


  g_logger->setLevel(fishjoy::LogLevel::FromString("WARN"));
  fishjoy::FileLogAppender::ptr fileAppender(
      new fishjoy::FileLogAppender("/home/zsl/CLionProjects/fishjoy/test/output/file_test_log.txt"));
  fileAppender->setLevel(fishjoy::LogLevel::ERROR);
  g_logger->addAppender(fileAppender);

  FISHJOY_LOG_DEBUG(g_logger) << "macro FISHJOY_LOG_DEBUG"; //不打印
  FISHJOY_LOG_INFO(g_logger)  << "macro FISHJOY_LOG_INFO";  //不打印
  FISHJOY_LOG_WARN(g_logger)  << "macro FISHJOY_LOG_WARN";
  FISHJOY_LOG_ERROR(g_logger) << "macro FISHJOY_LOG_ERROR";
  FISHJOY_LOG_FATAL(g_logger) << "macro FISHJOY_LOG_FATAL";
}

void format_test() {
  INIT();

  // 时间：启动毫秒数 级别 日志名称 文件名：行号 消息 换行
  fishjoy::LogFormatter::ptr raw_formatter = g_logger->getFormatter();
  fishjoy::LogFormatter::ptr formatter(new fishjoy::LogFormatter("%d:%T%r ms%T%p%T%c%T%f:%l %m%n"));
  g_logger->setFormatter(formatter);
  FISHJOY_LOG_FATAL(g_logger) << "format_test fatal log";
  FISHJOY_LOG_ERROR(g_logger) << "format_test error log";
  FISHJOY_LOG_ERROR(g_logger) << "format_test error log";
  FISHJOY_LOG_INFO(g_logger)  << "format_test info log";
  FISHJOY_LOG_DEBUG(g_logger) << "format_test debug log";


  fishjoy::FileLogAppender::ptr fileAppender(
      new fishjoy::FileLogAppender("/home/zsl/CLionProjects/fishjoy/test/output/file_test_log.txt"));
  fileAppender->setFormatter(raw_formatter);

  FISHJOY_LOG_FATAL(g_logger) << "format_test fatal log";
  FISHJOY_LOG_ERROR(g_logger) << "format_test error log";
  FISHJOY_LOG_ERROR(g_logger) << "format_test error log";
  FISHJOY_LOG_INFO(g_logger)  << "format_test info log";
  FISHJOY_LOG_DEBUG(g_logger) << "format_test debug log";


  g_logger->setFormatter(raw_formatter);
  FISHJOY_LOG_FATAL(g_logger) << "format_test fatal log";
  FISHJOY_LOG_ERROR(g_logger) << "format_test error log";
  FISHJOY_LOG_ERROR(g_logger) << "format_test error log";
  FISHJOY_LOG_INFO(g_logger)  << "format_test info log";
  FISHJOY_LOG_DEBUG(g_logger) << "format_test debug log";
}

void config_test() {
  INIT();
  std::cout << "=============   print fileAppender config" << std::endl;
  fishjoy::FileLogAppender::ptr fileAppender(
      new fishjoy::FileLogAppender("/home/zsl/CLionProjects/fishjoy/test/output/file_test_log.txt"));
  std::cout << fileAppender->toYamlString() << std::endl;

  std::cout << "=============   before" << std::endl;

  std::cout << fishjoy::LoggerMgr::GetInstance()->toYamlString() << std::endl;

  std::cout << "=============   loading" << std::endl;
  YAML::Node root = YAML::LoadFile("/home/zsl/CLionProjects/fishjoy/conf/log.yml");
  if(root["root"].IsDefined()) fishjoy::Config::LoadFromYaml(root);


  std::cout << "=============   print yaml" << std::endl;
  std::cout << root << std::endl;

  std::cout << "=============   after" << std::endl;
  std::cout << fishjoy::LoggerMgr::GetInstance()->toYamlString() << std::endl;

}

void manage_test() {
  INIT();

  FISHJOY_LOG_NAME("system");
  std::cout << fishjoy::LoggerMgr::GetInstance()->toYamlString() << std::endl;

  fishjoy::LoggerMgr::GetInstance()->getRoot()->toYamlString();

  fishjoy::LoggerMgr::GetInstance()->getLogger("system")->toYamlString();

}

void raw_log_test() {
  INIT();

  g_logger->debug( fishjoy::LogEvent::ptr(new fishjoy::LogEvent(g_logger, fishjoy::LogLevel::DEBUG, __FILE__, __LINE__, 0, fishjoy::GetThreadId(), fishjoy::GetFiberId(), time(0), fishjoy::Thread::GetName())));
  g_logger->info( fishjoy::LogEvent::ptr(new fishjoy::LogEvent(g_logger, fishjoy::LogLevel::INFO, __FILE__, __LINE__, 0, fishjoy::GetThreadId(), fishjoy::GetFiberId(), time(0), fishjoy::Thread::GetName())));
  g_logger->fatal( fishjoy::LogEvent::ptr(new fishjoy::LogEvent(g_logger, fishjoy::LogLevel::FATAL, __FILE__, __LINE__, 0, fishjoy::GetThreadId(), fishjoy::GetFiberId(), time(0), fishjoy::Thread::GetName())));
  g_logger->warn( fishjoy::LogEvent::ptr(new fishjoy::LogEvent(g_logger, fishjoy::LogLevel::WARN, __FILE__, __LINE__, 0, fishjoy::GetThreadId(), fishjoy::GetFiberId(), time(0), fishjoy::Thread::GetName())));
  g_logger->error( fishjoy::LogEvent::ptr(new fishjoy::LogEvent(g_logger, fishjoy::LogLevel::ERROR, __FILE__, __LINE__, 0, fishjoy::GetThreadId(), fishjoy::GetFiberId(), time(0), fishjoy::Thread::GetName())));

}

int main() {
  XX("macro_stream_test")
  macro_stream_test();
  XX("macro_fmt_test")
  macro_fmt_test();
  XX("appender_test")
  appender_test();
  XX("level_test")
  level_test();
  XX("format_test")
  format_test();
  XX("config_test")
  config_test();
  XX("manage_test")
  manage_test();
  XX("raw_log_test");
  raw_log_test();
  return 0;
}
