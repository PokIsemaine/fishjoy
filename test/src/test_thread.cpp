#include "fishjoy/fishjoy.hpp"
#include <unistd.h>

fishjoy::Logger::ptr g_logger = FISHJOY_LOG_ROOT();

int count = 0;
//fishjoy::RWMutex s_mutex;
fishjoy::Mutex s_mutex;

void fun1() {
  FISHJOY_LOG_INFO(g_logger) << "name: " << fishjoy::Thread::GetName()
                           << " this.name: " << fishjoy::Thread::GetThis()->getName()
                           << " id: " << fishjoy::GetThreadId()
                           << " this.id: " << fishjoy::Thread::GetThis()->getId();

  for(int i = 0; i < 100000; ++i) {
    //fishjoy::RWMutex::WriteLock lock(s_mutex);
    fishjoy::Mutex::Lock lock(s_mutex);
    ++count;
  }
}

void fun2() {
  while(true) {
    FISHJOY_LOG_INFO(g_logger) << "xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
  }
}

void fun3() {
  while(true) {
    FISHJOY_LOG_INFO(g_logger) << "========================================";
  }
}

int main(int argc, char** argv) {
  FISHJOY_LOG_INFO(g_logger) << "thread test begin";
  YAML::Node root = YAML::LoadFile("/home/zsl/CLionProjects/fishjoy/conf/log.yml");
  fishjoy::Config::LoadFromYaml(root);

  std::vector<fishjoy::Thread::ptr> thrs;
  for(int i = 0; i < 2; ++i) {
    fishjoy::Thread::ptr thr(new fishjoy::Thread(&fun2, "name_" + std::to_string(i * 2)));
    fishjoy::Thread::ptr thr2(new fishjoy::Thread(&fun3, "name_" + std::to_string(i * 2 + 1)));
    thrs.push_back(thr);
    thrs.push_back(thr2);
  }

  for(size_t i = 0; i < thrs.size(); ++i) {
    thrs[i]->join();
  }
  FISHJOY_LOG_INFO(g_logger) << "thread test end";
  FISHJOY_LOG_INFO(g_logger) << "count=" << count;
  return 0;
}