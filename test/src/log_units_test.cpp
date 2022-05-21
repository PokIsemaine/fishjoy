#include "fishjoy/log.hpp"
#include <yaml-cpp/yaml.h>
#include <gtest/gtest.h>

TEST(StdOut, INFO)
{
  fishjoy::Logger::ptr logger(new fishjoy::Logger);
  logger->addAppender(fishjoy::LogAppender::ptr(new fishjoy::StdoutLogAppender));
  logger->setLevel(fishjoy::LogLevel::INFO);
}
TEST(StdOut, DEBUG)
{

}
TEST(StdOut, WARN)
{

}
TEST(StdOut, ERROR)
{

}
TEST(StdOut, FATAL)
{

}


int main(int argc, char **argv)
{
  ::testing::InitGoogleTest(&argc, argv);
  return RUN_ALL_TESTS();
}
