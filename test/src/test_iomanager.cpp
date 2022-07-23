/**
* @file test_iomanager.cc 
* @brief IO协程调度器测试
* @details 通过IO协程调度器实现一个简单的TCP客户端，这个客户端会不停地判断是否可读，并把读到的消息打印出来
*          当服务器关闭连接时客户端也退出
*/
#include "fishjoy/fishjoy.hpp"
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <fcntl.h>

fishjoy::Logger::ptr test_logger = FISHJOY_LOG_ROOT();

int sockfd;
void watch_io_read();

// 写事件回调，只执行一次，用于判断非阻塞套接字connect成功
void do_io_write() {
 FISHJOY_LOG_INFO(test_logger) << "write callback";
 int so_err;
 socklen_t len = size_t(so_err);
 getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &so_err, &len);
 if(so_err) {
   FISHJOY_LOG_INFO(test_logger) << "connect fail";
   return;
 } 
 FISHJOY_LOG_INFO(test_logger) << "connect success";
}

// 读事件回调，每次读取之后如果套接字未关闭，需要重新添加
void do_io_read() {
 FISHJOY_LOG_INFO(test_logger) << "read callback";
 char buf[1024] = {0};
 int readlen = 0;
 readlen = read(sockfd, buf, sizeof(buf));
 if(readlen > 0) {
   buf[readlen] = '\0';
   FISHJOY_LOG_INFO(test_logger) << "read " << readlen << " bytes, read: " << buf;
 } else if(readlen == 0) {
   FISHJOY_LOG_INFO(test_logger) << "peer closed";
   close(sockfd);
   return;
 } else {
   FISHJOY_LOG_ERROR(test_logger) << "err, errno=" << errno << ", errstr=" << strerror(errno);
   close(sockfd);
   return;
 }
 // read之后重新添加读事件回调，这里不能直接调用addEvent，因为在当前位置fd的读事件上下文还是有效的，直接调用addEvent相当于重复添加相同事件
 fishjoy::IOManager::GetThis()->schedule(watch_io_read);
}

void watch_io_read() {
 FISHJOY_LOG_INFO(test_logger) << "watch_io_read";
 fishjoy::IOManager::GetThis()->addEvent(sockfd, fishjoy::IOManager::READ, do_io_read);
}

void test_io() {
 sockfd = socket(AF_INET, SOCK_STREAM, 0);
 FISHJOY_ASSERT(sockfd > 0);
 fcntl(sockfd, F_SETFL, O_NONBLOCK);

 sockaddr_in servaddr;
 memset(&servaddr, 0, sizeof(servaddr));
 servaddr.sin_family = AF_INET;
 servaddr.sin_port = htons(1234);
 inet_pton(AF_INET, "10.10.19.159", &servaddr.sin_addr.s_addr);

 int rt = connect(sockfd, (const sockaddr*)&servaddr, sizeof(servaddr));
 if(rt != 0) {
   if(errno == EINPROGRESS) {
     FISHJOY_LOG_INFO(test_logger) << "EINPROGRESS";
     // 注册写事件回调，只用于判断connect是否成功
     // 非阻塞的TCP套接字connect一般无法立即建立连接，要通过套接字可写来判断connect是否已经成功
     fishjoy::IOManager::GetThis()->addEvent(sockfd, fishjoy::IOManager::WRITE, do_io_write);
     // 注册读事件回调，注意事件是一次性的
     fishjoy::IOManager::GetThis()->addEvent(sockfd, fishjoy::IOManager::READ, do_io_read);
   } else {
     FISHJOY_LOG_ERROR(test_logger) << "connect error, errno:" << errno << ", errstr:" << strerror(errno);
   }
 } else {
   FISHJOY_LOG_ERROR(test_logger) << "else, errno:" << errno << ", errstr:" << strerror(errno);
 }
}

void test_iomanager() {
 fishjoy::IOManager iom;
 // fishjoy::IOManager iom(10); // 演示多线程下IO协程在不同线程之间切换
 iom.schedule(test_io);
}

int main(int argc, char *argv[]) {
 fishjoy::EnvMgr::GetInstance()->init(argc, argv);
 fishjoy::Config::LoadFromConfDir(fishjoy::EnvMgr::GetInstance()->getConfigPath());
   
 test_iomanager();

 return 0;
}