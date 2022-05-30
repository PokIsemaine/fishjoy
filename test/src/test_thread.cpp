#include "fishjoy/log.hpp"
#include "fishjoy/thread.hpp"
#include "fishjoy/util.hpp"
#include <unistd.h>

fishjoy::Logger::ptr g_logger = FISHJOY_LOG_ROOT();

void fun1() {
  FISHJOY_LOG_INFO(g_logger) << "name: " << fishjoy::Thread::GetName()
                           << " this.name: " << fishjoy::Thread::GetThis()->getName()
                           << " id: " << fishjoy::GetThreadId()
                           << " this.id: " << fishjoy::Thread::GetThis()->getId();
  sleep(20);
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

  return 0;
}