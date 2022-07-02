//
// Created by zsl on 6/14/22.
//
#include <unistd.h>     // for pipe
#include <sys/epoll.h>  // for epoll
#include <fcntl.h>      // fcntl()
#include <cstring>
#include "fishjoy/iomanager.hpp"
#include "fishjoy/log.hpp"
#include "fishjoy/macro.hpp"

namespace fishjoy {
  static fishjoy::Logger::ptr g_logger = FISHJOY_LOG_NAME("system");

  enum EpollCtlOp {
  };

  static std::ostream &operator<<(std::ostream &os, const EpollCtlOp &op) {
    switch ((int)op) {
#define XX(ctl) \
    case ctl:   \
        return os << #ctl;
      XX(EPOLL_CTL_ADD);
      XX(EPOLL_CTL_MOD);
      XX(EPOLL_CTL_DEL);
#undef XX
      default:
        return os << (int)op;
    }
  }

  static std::ostream &operator<<(std::ostream &os, EPOLL_EVENTS events) {
    if (!events) {
      return os << "0";
    }
    bool first = true;
#define XX(E)          \
    if (events & E) {  \
        if (!first) {  \
            os << "|"; \
        }              \
        os << #E;      \
        first = false; \
    }
    XX(EPOLLIN);
    XX(EPOLLPRI);
    XX(EPOLLOUT);
    XX(EPOLLRDNORM);
    XX(EPOLLRDBAND);
    XX(EPOLLWRNORM);
    XX(EPOLLWRBAND);
    XX(EPOLLMSG);
    XX(EPOLLERR);
    XX(EPOLLHUP);
    XX(EPOLLRDHUP);
    XX(EPOLLONESHOT);
    XX(EPOLLET);
#undef XX
    return os;
  }

  IOManager::FdContext::EventContext& IOManager::FdContext::getEventContext(Event event) {
    switch (event) {
      case IOManager::READ:
        return read;
      case IOManager::WRITE:
        return write;
      default:
        FISHJOY_ASSERT2(false, "getContext");
    }
    throw std::invalid_argument("getContext invalid event");
  }

  void IOManager::FdContext::resetEventContext(EventContext &ctx) {
    ctx.scheduler = nullptr;
    ctx.fiber.reset();
    ctx.cb = nullptr;
  }

  void IOManager::FdContext::triggerEvent(Event event) {
    // 待触发事件必须已被注册过
    FISHJOY_ASSERT(event & event);

    /**
     * 清除该事件，表示不再关注该事件了
     * 也就是说，注册的 IO 事件是一次性的，如果想持续关注某个 socket fd 的读写事件，那么每次触发事件之后都要重新添加
     */
     event = (Event)(event & ~event);

     // 调度对应的协程
     EventContext &ctx = getEventContext(event);
     if(ctx.cb) {
       ctx.scheduler->schedule(ctx.cb);
     } else {
       ctx.scheduler->schedule(ctx.fiber);
     }
     resetEventContext(ctx);
     return ;
  }

  IOManager::IOManager(size_t threads, bool user_caller, const std::string &name)
    : Scheduler(threads, user_caller, name) {
    m_epfd = epoll_create(5000);
    FISHJOY_ASSERT(m_epfd > 0);

    int rt = pipe(m_tickleFds);
    FISHJOY_ASSERT(!rt);

    // 关注 pipe 读句柄的可读事件， 用于 tickle 协程
    epoll_event event;
    memset(&event, 0 ,sizeof(epoll_event));
    event.events = EPOLLIN | EPOLLET;
    event.data.fd = m_tickleFds[0];

    // 非阻塞方式，配合边缘触发
    rt = fcntl(m_tickleFds[0], F_SETFL, O_NONBLOCK);
    FISHJOY_ASSERT(!rt);

    rt = epoll_ctl(m_epfd, EPOLL_CTL_ADD, m_tickleFds[0], &event);
    FISHJOY_ASSERT(!rt);

    contextResize(32);

    start();

  }

  IOManager::~IOManager() {
    stop();
    close(m_epfd);
    close(m_tickleFds[0]);
    close(m_tickleFds[1]);

    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
      if (m_fdContexts[i]) {
        delete m_fdContexts[i];
      }
    }
  }

  void IOManager::contextResize(size_t size) {
    m_fdContexts.resize(size);

    for (size_t i = 0; i < m_fdContexts.size(); ++i) {
      if (!m_fdContexts[i]) {
        m_fdContexts[i]     = new FdContext;
        m_fdContexts[i]->fd = i;
      }
    }
  }
}