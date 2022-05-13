
#ifndef __FISHJOY_LOG_H__
#define __FISHJOY_LOG_H__

#include <string>
#include <cstdint>
#include <memory>
#include <list>
#include <sstream>
#include <fstream>
#include <iostream>
#include <vector>

namespace fishjoy{

  class Logger;
  //日志事件
  class LogEvent{
   public:
    typedef std::shared_ptr<LogEvent> ptr;
    LogEvent();

    const char* getFile() const { return m_file;}
    int32_t getLine() const { return m_line;}
    uint32_t getThreadId() const { return m_threadId;}
    uint32_t getElapse() const { return m_elapse;}
    uint32_t getFiberId() const { return m_fiberId;}
    uint32_t getTime() const { return m_time;}
    const std::string& getContent() const { return m_content;}



   private:
    const char* m_file = nullptr;	//文件名
    int32_t m_line = 0;				//行号
    uint32_t m_threadId = 0;		//线程Id
    uint32_t m_elapse =0;			//程序启动开始到现在的毫秒数
    uint32_t m_fiberId = 0;			//协程Id
    time_t m_time;					//时间戳
    std::string m_content;

  };

  //日志级别
  class LogLevel{
   public:
    enum Level{
      UNKNOWN = 0,
      DEBUG = 1,	//调试信息
      INFO = 2,	//一般信息
      WARN = 3,	//警告
      ERROR = 4,	//严重错误
      FATAL = 5,	//致命情况，系统不可用
    };

    static const char* ToString(LogLevel::Level level);
  };


  //日志格式器
  class LogFormatter{
   public:
    typedef std::shared_ptr<LogFormatter> ptr;
    LogFormatter(const std::string& pattern);

    //将 LogEvent 格式化为字符串
    std::string format(std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event);
   public:

    // 具体日志格式项
    class FormatItem{
     public:
      typedef std::shared_ptr<FormatItem> ptr;
      virtual ~FormatItem() {};
      virtual void format(std::ostream& os, std::shared_ptr<Logger> logger, LogLevel::Level level, LogEvent::ptr event) = 0;
    };

    void init();
   private:
    //日志格式
    std::string m_pattern;
    //通过日志格式解析出来的 FormatItem, 支持扩展
    std::vector<FormatItem::ptr> m_items;
  };


  //日志输出位置
  class LogAppender{
   public:
    typedef std::shared_ptr<LogAppender> ptr;

    virtual ~LogAppender(){}

    virtual void log(std::shared_ptr<Logger> logger, LogLevel::Level level,LogEvent::ptr event) = 0;


    //获取格式 m_formmater
    LogFormatter::ptr getFormatter() const { return m_formatter; }

    //设置格式 m_formmater
    void setFormatter(LogFormatter::ptr fmt){ m_formatter = fmt; }

   protected:
    LogLevel::Level m_level = LogLevel::DEBUG;
    //日志格式器
    LogFormatter::ptr m_formatter;
  };

  //日志器
  class Logger : public  std::enable_shared_from_this<Logger> {
   public:
    typedef std::shared_ptr<Logger> ptr;


    Logger(const std::string& name = "root");
	
    //根据不同级别调用 log 工具函数写日志

    //DEBUG级别日志
    void debug(LogEvent::ptr event);

    //INFO级别日志
    void info(LogEvent::ptr event);

    //WARN级别日志
    void warn(LogEvent::ptr event);
	
    //ERROR级别日志
    void error(LogEvent::ptr event);

    //FATAL级别日志
    void fatal(LogEvent::ptr event);

    //对 Appender集合 m_appenders 操作

    //添加 Appender
    void addAppender(LogAppender::ptr appender);

    //删除 Appender
    void deleteAppender(LogAppender::ptr appender);

    //清空 m_appenders
    void clearAppender();

    // 日志级别
    LogLevel::Level getLevel() const {
      return m_level;
    }

    // 设置日志级别
    void setLevel(LogLevel::Level level){
      m_level = level;
    }

    // 获取日志名称
    const std::string& getName() const {
      return m_name;
    }

    //工具函数
    void log(LogLevel::Level level, const LogEvent::ptr event);

   private:
    std::string m_name;								//日志名称
    LogLevel::Level m_level;						//日志级别
    std::list<LogAppender::ptr> m_appenders;		//Appender集合
    LogFormatter::ptr m_formatter;
  };

  //输出到控制台的Appender
  class StdoutLogAppender : public LogAppender {
   public:
    typedef std::shared_ptr<StdoutLogAppender> ptr;
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;
  };

  //定义输出到文件的Appender
  class FileLogAppender : public LogAppender {
   public:
    typedef std::shared_ptr<FileLogAppender> ptr;
    FileLogAppender(const std::string& filename);
    void log(Logger::ptr logger, LogLevel::Level level, LogEvent::ptr event) override;

    //重新打开文件，文件打开成功返回true
    bool reopen();
   private:
    std::string m_filename;
    std::ofstream m_filestream;
  };

}
#endif