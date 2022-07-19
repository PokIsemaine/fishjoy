//
// Created by zsl on 7/19/22.
//

#ifndef FISHJOY_HTTP_SERVER_HPP
#define FISHJOY_HTTP_SERVER_HPP
#include "fishjoy/tcp_server.hpp"
#include "http_session.hpp"
#include "servlet.hpp"

namespace fishjoy {
  namespace http {

    /**
 * @brief HTTP服务器类
     */
    class HttpServer : public TcpServer {
     public:
      /// 智能指针类型
      using ptr = std::shared_ptr<HttpServer>;

      /**
     * @brief 构造函数
     * @param[in] keepalive 是否长连接
     * @param[in] worker 工作调度器
     * @param[in] accept_worker 接收连接调度器
       */
      HttpServer(bool keepalive = false
                 ,fishjoy::IOManager* worker = fishjoy::IOManager::GetThis()
                     ,fishjoy::IOManager* io_worker = fishjoy::IOManager::GetThis()
                     ,fishjoy::IOManager* accept_worker = fishjoy::IOManager::GetThis());

      /**
     * @brief 获取ServletDispatch
       */
      ServletDispatch::ptr getServletDispatch() const { return m_dispatch;}

      /**
     * @brief 设置ServletDispatch
       */
      void setServletDispatch(ServletDispatch::ptr v) { m_dispatch = v;}

      virtual void setName(const std::string& v) override;
     protected:
      virtual void handleClient(Socket::ptr client) override;
     private:
      /// 是否支持长连接
      bool m_isKeepalive;
      /// Servlet分发器
      ServletDispatch::ptr m_dispatch;
    };

  }
}

#endif  // FISHJOY_HTTP_SERVER_HPP
