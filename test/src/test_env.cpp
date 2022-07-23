#include "fishjoy/fishjoy.hpp"

fishjoy::Logger::ptr test_logger = FISHJOY_LOG_ROOT();

fishjoy::Env *g_env = fishjoy::EnvMgr::GetInstance();

int main(int argc, char *argv[]) {
  g_env->addHelp("h", "print this help message");

  bool is_print_help = false;
  if(!g_env->init(argc, argv)) {
    is_print_help = true;
  }
  if(g_env->has("h")) {
    is_print_help = true;
  }

  if(is_print_help) {
    g_env->printHelp();
    return false;
  }

  FISHJOY_LOG_INFO(test_logger)<< "exe: " << g_env->getExe();
  FISHJOY_LOG_INFO(test_logger) <<"cwd: " << g_env->getCwd();
  FISHJOY_LOG_INFO(test_logger) << "absoluth path of test: " << g_env->getAbsolutePath("test");
  FISHJOY_LOG_INFO(test_logger) << "conf path:" << g_env->getConfigPath();

  g_env->add("key1", "value1");
  FISHJOY_LOG_INFO(test_logger) << "key1: " << g_env->get("key1");

  g_env->setEnv("key2", "value2");
  FISHJOY_LOG_INFO(test_logger) << "key2: " << g_env->getEnv("key2");

  FISHJOY_LOG_INFO(test_logger) << g_env->getEnv("PATH");

  return 0;
}
