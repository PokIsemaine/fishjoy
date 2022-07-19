//
// Created by zsl on 7/19/22.
//

#ifndef FISHJOY_HTTP_SESSION_HPP
#define FISHJOY_HTTP_SESSION_HPP

#include "fishjoy/streams/socket_stream.hpp"
#include "http.hpp"

namespace fishjoy {
  namespace http {

    /**
 * @brief HTTPSession封装
     */
    class HttpSession : public SocketStream {
     public:
      /// 智能指针类型定义
      using ptr = std::shared_ptr<HttpSession>;

      /**
     * @brief 构造函数
     * @param[in] sock Socket类型
     * @param[in] owner 是否托管
       */
      HttpSession(Socket::ptr sock, bool owner = true);

      /**
     * @brief 接收HTTP请求
       */
      HttpRequest::ptr recvRequest();

      /**
     * @brief 发送HTTP响应
     * @param[in] rsp HTTP响应
     * @return >0 发送成功
     *         =0 对方关闭
     *         <0 Socket异常
       */
      int sendResponse(HttpResponse::ptr rsp);
    };

  }
}

#endif  // FISHJOY_HTTP_SESSION_HPP
