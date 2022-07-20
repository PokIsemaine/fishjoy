#include "fishjoy/fishjoy.hpp"
#include <unistd.h>


fishjoy::Logger::ptr test_logger = FISHJOY_LOG_ROOT();

int count = 0;
fishjoy::Mutex s_mutex;

void func1(void *arg) {
  FISHJOY_LOG_INFO(test_logger) << "name:" << fishjoy::Thread::GetName()
                           << " this.name:" << fishjoy::Thread::GetThis()->getName()
                           << " thread name:" << fishjoy::GetThreadName()
                           << " id:" << fishjoy::GetThreadId()
                           << " this.id:" << fishjoy::Thread::GetThis()->getId();
  FISHJOY_LOG_INFO(test_logger) << "arg: " << *(int*)arg;
  for(int i = 0; i < 10000; i++) {
    fishjoy::Mutex::Lock lock(s_mutex);
    ++count;
  }
}

int main(int argc, char *argv[]) {
  fishjoy::EnvMgr::GetInstance()->init(argc, argv);
  fishjoy::Config::LoadFromDir(fishjoy::EnvMgr::GetInstance()->getConfigPath());

  std::vector<fishjoy::Thread::ptr> thrs;
  int arg = 123456;
  for(int i = 0; i < 3; i++) {
    // 带参数的线程用std::bind进行参数绑定
    fishjoy::Thread::ptr thr(new fishjoy::Thread(std::bind(func1, &arg), "thread_" + std::to_string(i)));
    thrs.push_back(thr);
  }

  for(int i = 0; i < 3; i++) {
    thrs[i]->join();
  }
    
  FISHJOY_LOG_INFO(test_logger) << "count = " << count;
  return 0;
}
