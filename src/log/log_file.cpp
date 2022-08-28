//
// Created by zsl on 8/24/22.
//

#include "fishjoy/log/log_file.hpp"
#include<fcntl.h>
#include <sys/mman.h>
#include <cstring>

namespace fishjoy {
  MMapFileWriter::MMapFileWriter(const std::string& basename, uint32_t mem_size) {
    m_memSize = mem_size;
    m_written = 0;
    if(m_fd >= 0) {
      close(m_fd);
    }
    m_fd = open(basename.c_str(), O_RDWR | O_CREAT,
                S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH);
    if (m_fd < 0) {
      fprintf(stderr, "open new file failed,errno=%d", errno);
    } else {
      int n = ftruncate(m_fd, mem_size);
      (void)n;
      m_buffer = (char *)mmap(NULL, m_memSize, PROT_READ | PROT_WRITE,
                            MAP_SHARED, m_fd, 0);
      if (m_buffer == MAP_FAILED) {
        fprintf(stderr, "mmap file failed,errno=%d", errno);
      }
    }
  }

  MMapFileWriter::~MMapFileWriter() {
    if(m_fd >= 0) {
      close(m_fd);
      m_fd = -1;
    }
    if(m_buffer != MAP_FAILED) {
      munmap(m_buffer, m_memSize);
    }
  }

  void MMapFileWriter::append(const char *msg, int32_t len) {
    if (len > m_memSize - m_written) {
      fprintf(stderr, "mmap memory overflow ,errno=%d", errno);
      return;
    }
    memcpy(m_buffer + m_written, msg, len);
    m_written += len;
  }

  void MMapFileWriter::flush() {
    if (m_buffer != MAP_FAILED) {
      msync(m_buffer, m_memSize, MS_ASYNC);
    }
  }

  AppendFileWriter::AppendFileWriter(const std::string &filename)
      : m_fp(::fopen(filename.c_str(), "ae")) {
    ::setbuffer(m_fp, m_buffer, sizeof m_buffer);
  }
  AppendFileWriter::~AppendFileWriter() {
    if (m_fp) {
      ::fclose(m_fp);
    }
  }
  void AppendFileWriter::append(const char *msg, int32_t len) {
    size_t n = ::fwrite_unlocked(msg, 1, len, m_fp);
    size_t remain = len - n;
    while (remain > 0) {
      size_t x = ::fwrite_unlocked(msg + n, 1, remain, m_fp);
      if (x == 0) {
        int err = ferror(m_fp);
        if (err) {
          fprintf(stderr, "AppendFile::append() failed %s\n", strerror(err));
        }
        break;
      }
      n += x;
      remain = len - n; // remain -= x
    }

    m_written += len;
  }



  void LogFile::append(const char *msg, int32_t len) {
    m_file->append(msg, len);
    if (m_file->writtenBytes() > m_rollSize) {
      rollFile();
    } else {
      ++m_count;
      //隔多久需要检查一下
      if (m_count >= m_checkFreqCount) {
        m_count = 0;
        time_t now = ::time(NULL);
        time_t this_period = now / kRollPerSeconds * kRollPerSeconds;
        if (this_period != m_startOfPeriod) {
          rollFile();
        } else if (now - m_lastFlush > m_flushInterval) {
          m_lastFlush = now;
          m_file->flush();
        }
      }
    }
  }

  void LogFile::flush() { m_file->flush(); }

  bool LogFile::rollFile() {
    time_t now = 0;
    std::string filename = getLogFileName(m_basename, &now);
    time_t start = now / kRollPerSeconds * kRollPerSeconds;

    if (now > m_lastRoll) {
      m_lastRoll = now;
      m_lastFlush = now;
      m_startOfPeriod = start;
      if (m_fileWriterType == FileWriterType::MMAPFILE) {
        m_file.reset(new MMapFileWriter(filename, m_rollSize));
      } else {
        m_file.reset(new AppendFileWriter(filename));
      }
    }
    return true;
  }


}