
#ifndef __FISHJOY_LOG_H__
#define __FISHJOY_LOG_H__

#include <cstdarg>

#include <cstdint>
#include <fstream>
#include <iostream>
#include <list>
#include <map>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include "singleton.hpp"
#include "util.hpp"

#define FISHJOY_LOG_LEVEL(logger, level)                                                                                    \
  if (logger->getLevel() <= level)                                                                                          \
  fishjoy::LogEventWrap(fishjoy::LogEvent::ptr(new fishjoy::LogEvent(                                                       \
                            logger, level, __FILE__, __LINE__, 0, fishjoy::GetThreadId(), fishjoy::GetFiberId(), time(0)))) \
      .getSS()

#define FISHJOY_LOG_DEBUG(logger) FISHJOY_LOG_LEVEL(logger, fishjoy::LogLevel::DEBUG)
#define FISHJOY_LOG_INFO(logger)  FISHJOY_LOG_LEVEL(logger, fishjoy::LogLevel::INFO)
#define FISHJOY_LOG_WARN(logger)  FISHJOY_LOG_LEVEL(logger, fishjoy::LogLevel::WARN)
#define FISHJOY_LOG_ERROR(logger) FISHJOY_LOG_LEVEL(logger, fishjoy::LogLevel::ERROR)
#define FISHJOY_LOG_FATAL(logger) FISHJOY_LOG_LEVEL(logger, fishjoy::LogLevel::FATAL)

#define FISHJOY_LOG_FMT_LEVEL(logger, level, fmt, ...)                                                                      \
  if (logger->getLevel() <= level)                                                                                          \
  fishjoy::LogEventWrap(fishjoy::LogEvent::ptr(new fishjoy::LogEvent(                                                       \
                            logger, level, __FILE__, __LINE__, 0, fishjoy::GetThreadId(), fishjoy::GetFiberId(), time(0)))) \
      .getEvent()                                                                                                           \
      ->format(fmt, __VA_ARGS__)

#define FISHJOY_LOG_FMT_DEBUG(logger, fmt, ...) FISHJOY_LOG_FMT_LEVEL(logger, fishjoy::LogLevel::DEBUG, fmt, __VA_ARGS__)
#define FISHJOY_LOG_FMT_INFO(logger, fmt, ...)  FISHJOY_LOG_FMT_LEVEL(logger, fishjoy::LogLevel::INFO, fmt, __VA_ARGS__)
#define FISHJOY_LOG_FMT_WARN(logger, fmt, ...)  FISHJOY_LOG_FMT_LEVEL(logger, fishjoy::LogLevel::WARN, fmt, __VA_ARGS__)
#define FISHJOY_LOG_FMT_ERROR(logger, fmt, ...) FISHJOY_LOG_FMT_LEVEL(logger, fishjoy::LogLevel::ERROR, fmt, __VA_ARGS__)
#define FISHJOY_LOG_FMT_FATAL(logger, fmt, ...) FISHJOY_LOG_FMT_LEVEL(logger, fishjoy::LogLevel::FATAL, fmt, __VA_ARGS__)

#define FISHJOY_LOG_ROOT()     fishjoy::LoggerMgr::GetInstance()->getRoot()
#define FISHJOY_LOG_NAME(name) fishjoy::LoggerMgr::GetInstance()->getLogger(name)


namespace fishjoy
{

  class Logger;
  class LoggerManager;

  //日志级别
  class LogLevel
  {
   public:
    enum Level
    {
      UNKNOWN = 0,
      DEBUG = 1,  //调试信息
      INFO = 2,   //一般信息
      WARN = 3,   //警告
      ERROR = 4,  //严重错误
      FATAL = 5,  //致命情况，系统不可用
    };

    static const char* ToString(LogLevel::Level level);
  };

  //日志事件
  class LogEvent
  {
   public:
    using ptr = std::shared_ptr<LogEvent>;
    LogEvent(
        std::shared_ptr<Logger> logger,
        LogLevel::Level level,
        const char* file,
        int32_t line,
        uint32_t elapse,
        uint32_t thread_id,
        uint32_t fiber_id,
        uint64_t time);

    const char* getFile() const
    {
      return m_file;
    }
    int32_t getLine() const
    {
      return m_line;
    }
    uint32_t getElapse() const
    {
      return m_elapse;
    }
    uint32_t getThreadId() const
    {
      return m_threadId;
    }
    uint32_t getFiberId() const
    {
      return m_fiberId;
    }
    uint64_t getTime() const
    {
      return m_time;
    }
    std::string getContent() const
    {
      return m_ss.str();
    }

    std::stringstream& getSS()
    {
      return m_ss;
    }
    std::shared_ptr<Logger> getLogger() const
    {
      return m_logger;
    }
    LogLevel::Level getLevel() const
    {
      return m_level;
    }


    void format(const char* fmt, ...);
    void format(const char* fmt, va_list al);

   private:
    const char* m_file = nullptr;  //文件名
    int32_t m_line = 0;            //行号
    uint32_t m_elapse = 0;         //程序启动开始到现在的毫秒数
    uint32_t m_threadId = 0;       //线程id
    uint32_t m_fiberId = 0;        //协程id
    uint64_t m_time = 0;           //时间戳
    std::stringstream m_ss;        //内容

    std::shared_ptr<Logger> m_logger;
    LogLevel::Level m_level;
  };

  //日志封装
  class LogEventWrap
  {
   public:
    explicit LogEventWrap(LogEvent::ptr event);
    ~LogEventWrap();
    LogEvent::ptr getEvent() const
    {
      return m_event;
    }
    std::stringstream& getSS();

   private:
    LogEvent::ptr m_event;
  };

  //日志格式器
  class LogFormatter
  {
   public:
    using ptr = std::shared_ptr<LogFormatter>;
    explicit LogFormatter(const std::string& pattern);

    //将 LogEvent 格式化为字符串
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);

   public:
    // 具体日志格式项
    class LogFormatItem
    {
     public:
      using ptr = std::shared_ptr<LogFormatItem>;
      virtual ~LogFormatItem() = default;

      virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void init();

   private:
    //日志格式
    std::string m_pattern;
    //通过日志格式解析出来的 FormatItem, 支持扩展
    std::vector<LogFormatItem::ptr> m_items;
  };

  //日志输出位置
  class LogAppender
  {
   public:
    using ptr = std::shared_ptr<LogAppender>;

    virtual ~LogAppender() = default;

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;

    //获取格式 m_formatter
    LogFormatter::ptr getFormatter() const
    {
      return m_formatter;
    }

    //设置格式 m_formatter
    void setFormatter(LogFormatter::ptr fmt)
    {
      m_formatter = fmt;
    }

    void setLevel(LogLevel::Level level)
    {
      m_level = level;
    }

   protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    //日志格式器
    LogFormatter::ptr m_formatter;
  };

  //日志器
  class Logger : public std::enable_shared_from_this<Logger>
  {
    friend class LoggerManager;
   public:
    using ptr = std::shared_ptr<Logger>;

    explicit Logger(const std::string& name = "root");

    //工具函数
    void log(LogLevel::Level level, const LogEvent::ptr event);

    //根据不同级别调用 log 工具函数写日志

    // DEBUG级别日志
    void debug(LogEvent::ptr event);

    // INFO级别日志
    void info(LogEvent::ptr event);

    // WARN级别日志
    void warn(LogEvent::ptr event);

    // ERROR级别日志
    void error(LogEvent::ptr event);

    // FATAL级别日志
    void fatal(LogEvent::ptr event);

    //对 Appender集合 m_appenders 操作

    //添加 Appender
    void addAppender(LogAppender::ptr appender);

    //删除 Appender
    void deleteAppender(LogAppender::ptr appender);

    void clearAppender();

    // 日志级别
    LogLevel::Level getLevel() const
    {
      return m_level;
    }

    // 设置日志级别
    void setLevel(LogLevel::Level level)
    {
      m_level = level;
    }

    // 获取日志名称
    const std::string& getName() const
    {
      return m_name;
    }

   private:
    std::string m_name;                       //日志名称
    LogLevel::Level m_level;                  //日志级别
    std::list<LogAppender::ptr> m_appenders;  //Appender集合
    LogFormatter::ptr m_formatter;            //日志器格式
    Logger::ptr m_root;                       //主日志器
  };

  //输出到控制台的Appender
  class StdoutLogAppender : public LogAppender
  {
   public:
    using ptr = std::shared_ptr<StdoutLogAppender>;
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
  };

  //定义输出到文件的Appender
  class FileLogAppender : public LogAppender
  {
   public:
    using ptr = std::shared_ptr<FileLogAppender>;
    explicit FileLogAppender(const std::string& filename);
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

    //重新打开文件，文件打开成功返回true
    bool reopen();

   private:
    std::string m_filename;
    std::ofstream m_filestream;
  };

  //日志管理器
  class LoggerManager
  {
   public:
    LoggerManager();
    Logger::ptr getLogger(const std::string& name);

    void init();
    Logger::ptr getRoot() const
    {
      return m_root;
    }

   private:
    std::map<std::string, Logger::ptr> m_loggers;

    Logger::ptr m_root;
  };

  using LoggerMgr = fishjoy::Singleton<LoggerManager>;
}  // namespace fishjoy
#endif