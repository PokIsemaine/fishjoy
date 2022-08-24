//
// Created by zsl on 8/23/22.
//

#ifndef FISHJOY_LOG_FILE_HPP

#include <cstdint>
#include <string>
#include <memory>
namespace fishjoy {
  enum class FileWriterType : uint8_t {
    MMAPFILE = 0,
    APPENDFILE
  };

  class FileWriter {
   public:
    using ptr = std::shared_ptr<FileWriter>;
    FileWriter() = default;
    virtual ~FileWriter() = default;
    virtual void append(const char* msg, int32_t len) = 0;
    virtual void flush() = 0;
    virtual uint32_t writtenBytes() const = 0;
  };

  class MMapFileWriter : public FileWriter {
   public:
    MMapFileWriter(const std::string &basename, uint32_t mem_size);
    ~MMapFileWriter();
    void append(const char *msg, int32_t len);
    void flush();
    uint32_t writtenBytes() const { return m_written; }

   private:
    int m_fd;
    char * m_buffer;
    int32_t m_memSize;
    int32_t m_written;
  };

  class AppendFileWriter : public FileWriter {
   public:
    AppendFileWriter(const std::string &filename);
    ~AppendFileWriter();
    void append(const char *msg, int32_t len);
    void flush() { ::fflush(m_fp); }
    uint32_t writtenBytes() const { return m_written; }

   private:
    FILE * m_fp;
    char m_buffer[64 * 1024];
    int32_t m_written;

  };

  class LogFile {
   public:
    LogFile(const std::string &basename, int32_t roll_size,
            int32_t flush_interval, int32_t check_interval,
            FileWriterType file_writer_type);
    ~LogFile();

    void append(const char *logline, int32_t len);
    void flush();
    bool rollFile();
   private:
    std::string m_basename;
    uint32_t m_rollSize;
    uint32_t m_flushInterval;
    //多久进行检查一次，因为可能日志没满，但是跨天了
    uint32_t m_checkFreqCount;
    uint32_t m_count;
    time_t m_startOfPeriod;
    time_t m_lastRoll;
    time_t m_lastFlush;
    FileWriter::ptr m_file;
    FileWriterType m_fileWriterType;
    constexpr static int kRollPerSeconds = 60 * 60 * 24;
  };
}

#define FISHJOY_LOG_FILE_HPP

#endif  // FISHJOY_LOG_FILE_HPP
