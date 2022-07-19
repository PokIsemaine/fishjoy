//
// Created by zsl on 7/19/22.
//
#include "fishjoy/http/http_server.hpp"
#include "fishjoy/log.hpp"
//#include "servlets/config_servlet.h"
//#include "servlets/status_servlet.h"

namespace fishjoy {
  namespace http {

    static fishjoy::Logger::ptr g_logger = FISHJOY_LOG_NAME("system");

    HttpServer::HttpServer(bool keepalive
                           ,fishjoy::IOManager* worker
                           ,fishjoy::IOManager* io_worker
                           ,fishjoy::IOManager* accept_worker)
        :TcpServer(io_worker, accept_worker)
          ,m_isKeepalive(keepalive) {
      m_dispatch.reset(new ServletDispatch);

      m_type = "http";
      //m_dispatch->addServlet("/_/status", Servlet::ptr(new StatusServlet));
      //m_dispatch->addServlet("/_/config", Servlet::ptr(new ConfigServlet));
    }

    void HttpServer::setName(const std::string& v) {
      TcpServer::setName(v);
      m_dispatch->setDefault(std::make_shared<NotFoundServlet>(v));
    }

    void HttpServer::handleClient(Socket::ptr client) {
      FISHJOY_LOG_DEBUG(g_logger) << "handleClient " << *client;
      HttpSession::ptr session(new HttpSession(client));
      do {
        auto req = session->recvRequest();
        if(!req) {
          FISHJOY_LOG_DEBUG(g_logger) << "recv http request fail, errno="
                                    << errno << " errstr=" << strerror(errno)
                                    << " cliet:" << *client << " keep_alive=" << m_isKeepalive;
          break;
        }

        HttpResponse::ptr rsp(new HttpResponse(req->getVersion()
                                                   ,req->isClose() || !m_isKeepalive));
        rsp->setHeader("Server", getName());
        m_dispatch->handle(req, rsp, session);
        session->sendResponse(rsp);

        if(!m_isKeepalive || req->isClose()) {
          break;
        }
      } while(true);
      session->close();
    }

  }
}