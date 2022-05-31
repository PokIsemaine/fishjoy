#include "fishjoy/log.hpp"
#include "fishjoy/thread.hpp"
#include "fishjoy/util.hpp"
#include <unistd.h>

fishjoy::Logger::ptr g_logger = FISHJOY_LOG_ROOT();

int count = 0;
fishjoy::RWMutex s_mutex;

void fun1() {
  FISHJOY_LOG_INFO(g_logger) << "name: " << fishjoy::Thread::GetName()
                           << " this.name: " << fishjoy::Thread::GetThis()->getName()
                           << " id: " << fishjoy::GetThreadId()
                           << " this.id: " << fishjoy::Thread::GetThis()->getId();
  for (int i = 0; i < 10000; ++i) {
    fishjoy::RWMutex::WriteLock lock(s_mutex);
    ++count;
  }
}

void fun2() {
}

int main(int argc, char** argv) {
  FISHJOY_LOG_INFO(g_logger) << "thread test begin";

  std::vector<fishjoy::Thread::ptr> threads;
  for(int i = 0; i < 5; ++i) {
    fishjoy::Thread::ptr thread(new fishjoy::Thread(&fun1, "name_" + std::to_string(i)));
    threads.push_back(thread);
  }

  for(int i = 0; i < 5; ++i) {
    threads[i]->join();
  }
  FISHJOY_LOG_INFO(g_logger) << "thread test end";
  FISHJOY_LOG_INFO(g_logger) << "count=" << count;

  return 0;
}