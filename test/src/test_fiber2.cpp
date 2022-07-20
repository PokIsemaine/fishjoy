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

  /**
     * 非对称协程，子协程不能创建并运行新的子协程，下面的操作是有问题的，
     * 子协程再创建子协程，原来的主协程就跑飞了
   */
  fishjoy::Fiber::ptr fiber(new fishjoy::Fiber(run_in_fiber2, 0, false));
  fiber->resume();

  FISHJOY_LOG_INFO(test_logger) << "run_in_fiber end";
}

int main(int argc, char *argv[]) {
  fishjoy::EnvMgr::GetInstance()->init(argc, argv);
  fishjoy::Config::LoadFromDir(fishjoy::EnvMgr::GetInstance()->getConfigPath());

  FISHJOY_LOG_INFO(test_logger) << "main begin";

  fishjoy::Fiber::GetThis();

  fishjoy::Fiber::ptr fiber(new fishjoy::Fiber(run_in_fiber, 0, false));
  fiber->resume();

  FISHJOY_LOG_INFO(test_logger) << "main end";
  return 0;
}