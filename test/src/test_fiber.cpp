//
// Created by zsl on 6/11/22.
//
#include "fishjoy/fishjoy.hpp"
#include <string>
#include <vector>

fishjoy::Logger::ptr test_logger = FISHJOY_LOG_ROOT();

void run_in_fiber2() {
  FISHJOY_LOG_INFO(test_logger) << "run_in_fiber2 begin";
  FISHJOY_LOG_INFO(test_logger) << "run_in_fiber2 end";
}

void run_in_fiber() {
  FISHJOY_LOG_INFO(test_logger) << "run_in_fiber begin";

  FISHJOY_LOG_INFO(test_logger) << "before run_in_fiber yield";
  fishjoy::Fiber::GetThis()->yield();
  FISHJOY_LOG_INFO(test_logger) << "after run_in_fiber yield";

  FISHJOY_LOG_INFO(test_logger) << "run_in_fiber end";
  // fiber结束之后会自动返回主协程运行
}

void test_fiber() {
  FISHJOY_LOG_INFO(test_logger) << "test_fiber begin";

  // 初始化线程主协程
  fishjoy::Fiber::GetThis();

  fishjoy::Fiber::ptr fiber(new fishjoy::Fiber(run_in_fiber, 0, false));
  FISHJOY_LOG_INFO(test_logger) << "use_count:" << fiber.use_count(); // 1

  FISHJOY_LOG_INFO(test_logger) << "before test_fiber resume";
  fiber->resume();
  FISHJOY_LOG_INFO(test_logger) << "after test_fiber resume";

  /** 
     * 关于fiber智能指针的引用计数为3的说明：
     * 一份在当前函数的fiber指针，一份在MainFunc的cur指针
     * 还有一份在在run_in_fiber的GetThis()结果的临时变量里
   */
  FISHJOY_LOG_INFO(test_logger) << "use_count:" << fiber.use_count(); // 3

  FISHJOY_LOG_INFO(test_logger) << "fiber status: " << fiber->getState(); // READY

  FISHJOY_LOG_INFO(test_logger) << "before test_fiber resume again";
  fiber->resume();
  FISHJOY_LOG_INFO(test_logger) << "after test_fiber resume again";

  FISHJOY_LOG_INFO(test_logger) << "use_count:" << fiber.use_count(); // 1
  FISHJOY_LOG_INFO(test_logger) << "fiber status: " << fiber->getState(); // TERM

  fiber->reset(run_in_fiber2); // 上一个协程结束之后，复用其栈空间再创建一个新协程
  fiber->resume();

  FISHJOY_LOG_INFO(test_logger) << "use_count:" << fiber.use_count(); // 1
  FISHJOY_LOG_INFO(test_logger) << "test_fiber end";
}

int main(int argc, char *argv[]) {
//  fishjoy::EnvMgr::GetInstance()->init(argc, argv);
//  fishjoy::Config::LoadFromConfDir(fishjoy::EnvMgr::GetInstance()->getConfigPath());

  fishjoy::Thread::SetName("main_thread");
  FISHJOY_LOG_INFO(test_logger) << "main begin";

  std::vector<fishjoy::Thread::ptr> thrs;
  for (int i = 0; i < 2; i++) {
    thrs.push_back(fishjoy::Thread::ptr(
        new fishjoy::Thread(&test_fiber, "thread_" + std::to_string(i))));
  }

  for (auto i : thrs) {
    i->join();
  }

  FISHJOY_LOG_INFO(test_logger) << "main end";
  return 0;
}