//
// Created by zsl on 7/17/22.
//
#include "fishjoy/net/tcp_server.hpp"

#include "fishjoy/config.hpp"
#include "fishjoy/log.hpp"

namespace fishjoy {
  static fishjoy::Logger::ptr g_logger = FISHJOY_LOG_NAME("system");

  static fishjoy::ConfigVar<uint64_t>::ptr g_tcp_server_read_timeout =
      fishjoy::Config::Lookup("tcp_server.read_timeout", (uint64_t)(60 * 1000 * 2),
                            "tcp server read timeout");
  TcpServer::TcpServer(fishjoy::IOManager* io_worker,
                       fishjoy::IOManager* accept_worker)
      :m_ioWorker(io_worker)
        ,m_acceptWorker(accept_worker)
        ,m_recvTimeout(g_tcp_server_read_timeout->getValue())
        ,m_name("fishjoy/1.0.0")
        ,m_type("tcp")
        ,m_isStop(true) {
  }
  
  TcpServer::~TcpServer() {
    for(auto& i : m_socks) {
      i->close();
    }
    m_socks.clear();
  }
  
  bool TcpServer::bind(fishjoy::Address::ptr addr) {
    std::vector<Address::ptr> addrs;
    std::vector<Address::ptr> fails;
    addrs.push_back(addr);
    return bind(addrs, fails);
  }

  bool TcpServer::bind(const std::vector<Address::ptr>& addrs
                       ,std::vector<Address::ptr>& fails ) {
    for(auto& addr : addrs) {
      Socket::ptr sock = Socket::CreateTCP(addr);
      if(!sock->bind(addr)) {
        FISHJOY_LOG_ERROR(g_logger) << "bind fail errno="
                                  << errno << " errstr=" << strerror(errno)
                                  << " addr=[" << addr->toString() << "]";
        fails.push_back(addr);
        continue;
      }
      if(!sock->listen()) {
        FISHJOY_LOG_ERROR(g_logger) << "listen fail errno="
                                  << errno << " errstr=" << strerror(errno)
                                  << " addr=[" << addr->toString() << "]";
        fails.push_back(addr);
        continue;
      }
      m_socks.push_back(sock);
    }

    if(!fails.empty()) {
      m_socks.clear();
      return false;
    }

    for(auto& i : m_socks) {
      FISHJOY_LOG_INFO(g_logger) << "type=" << m_type
                               << " name=" << m_name
                               << " server bind success: " << *i;
    }
    return true;
  }

  void TcpServer::startAccept(Socket::ptr sock) {
    while(!m_isStop) {
      Socket::ptr client = sock->accept();
      if(client) {
        client->setRecvTimeout(m_recvTimeout);
        m_ioWorker->schedule(std::bind(&TcpServer::handleClient,
                                       shared_from_this(), client));
      } else {
        FISHJOY_LOG_ERROR(g_logger) << "accept errno=" << errno
                                  << " errstr=" << strerror(errno);
      }
    }
  }

  bool TcpServer::start() {
    if(!m_isStop) {
      return true;
    }
    m_isStop = false;
    for(auto& sock : m_socks) {
      m_acceptWorker->schedule(std::bind(&TcpServer::startAccept,
                                         shared_from_this(), sock));
    }
    return true;
  }

  void TcpServer::stop() {
    m_isStop = true;
    auto self = shared_from_this();
    m_acceptWorker->schedule([this, self]() {
      for(auto& sock : m_socks) {
        sock->cancelAll();
        sock->close();
      }
      m_socks.clear();
    });
  }

  void TcpServer::handleClient(Socket::ptr client) {
    FISHJOY_LOG_INFO(g_logger) << "handleClient: " << *client;
  }

  std::string TcpServer::toString(const std::string& prefix) {
    std::stringstream ss;
    ss << prefix << "[type=" << m_type
       << " name=" << m_name
       << " io_worker=" << (m_ioWorker ? m_ioWorker->getName() : "")
       << " accept=" << (m_acceptWorker ? m_acceptWorker->getName() : "")
       << " recv_timeout=" << m_recvTimeout << "]" << std::endl;
    std::string pfx = prefix.empty() ? "    " : prefix;
    for(auto& i : m_socks) {
      ss << pfx << pfx << *i << std::endl;
    }
    return ss.str();
  }
}