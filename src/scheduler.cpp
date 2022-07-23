//
// Created by zsl on 6/11/22.
//
#include "fishjoy/scheduler.hpp"

#include "fishjoy/fiber.hpp"
#include "fishjoy/hook.hpp"
#include "fishjoy/log.hpp"
#include "fishjoy/utils/macro.hpp"

namespace fishjoy {
  static fishjoy::Logger::ptr g_logger = FISHJOY_LOG_NAME("system");
  
  /// 当前线程的调度器，同一个调度器下的所有线程共享同一个实例
  static thread_local Scheduler *t_scheduler = nullptr;
  /// 当前线程的调度协程，每个线程都独有一份
  static thread_local Fiber *t_scheduler_fiber = nullptr;

  Scheduler::Scheduler(size_t threads, bool use_caller, const std::string &name) {
    FISHJOY_ASSERT(threads > 0)

    m_useCaller = use_caller;
    m_name      = name;

    if (use_caller) {
      --threads;
      fishjoy::Fiber::GetThis();
      FISHJOY_ASSERT(GetThis() == nullptr)
      t_scheduler = this;

      /**
         * caller线程的主协程不会被线程的调度协程run进行调度，而且，线程的调度协程停止时，应该返回caller线程的主协程
         * 在user caller情况下，把caller线程的主协程暂时保存起来，等调度协程结束时，再resume caller协程
       */
      m_rootFiber.reset(new Fiber(std::bind(&Scheduler::run, this), 0, false));

      fishjoy::Thread::SetName(m_name);
      t_scheduler_fiber = m_rootFiber.get();
      m_rootThread      = fishjoy::GetThreadId();
      m_threadIds.push_back(m_rootThread);
    } else {
      m_rootThread = -1;
    }
    m_threadCount = threads;
  }

  Scheduler *Scheduler::GetThis() { 
    return t_scheduler; 
  }

  Fiber *Scheduler::GetMainFiber() { 
    return t_scheduler_fiber;
  }

  void Scheduler::setThis() {
    t_scheduler = this;
  }

  Scheduler::~Scheduler() {
    FISHJOY_LOG_DEBUG(g_logger) << "Scheduler::~Scheduler()";
    FISHJOY_ASSERT(m_stopping)

    if (GetThis() == this) {
      t_scheduler = nullptr;
    }
  }

  void Scheduler::start() {
    FISHJOY_LOG_DEBUG(g_logger) << "start";
    MutexType::Lock lock(m_mutex);
    if (m_stopping) {
      FISHJOY_LOG_ERROR(g_logger) << "Scheduler is stopped";
      return;
    }
    FISHJOY_ASSERT(m_threads.empty())

    m_threads.resize(m_threadCount);
    for (size_t i = 0; i < m_threadCount; i++) {
      m_threads[i].reset(new Thread(std::bind(&Scheduler::run, this),
                                    m_name + "_" + std::to_string(i)));
      m_threadIds.push_back(m_threads[i]->getId());
    }
  }

  bool Scheduler::stopping() {
    MutexType::Lock lock(m_mutex);
    return m_stopping && m_tasks.empty() && m_activeThreadCount == 0;
  }

  void Scheduler::tickle() { 
    FISHJOY_LOG_DEBUG(g_logger) << "ticlke"; 
  }

  void Scheduler::idle() {
    FISHJOY_LOG_DEBUG(g_logger) << "idle";
    while (!stopping()) {
      fishjoy::Fiber::GetThis()->yield();
    }
  }

  void Scheduler::stop() {
    FISHJOY_LOG_DEBUG(g_logger) << "stop";
    if (stopping()) {
      return;
    }
    m_stopping = true;

    /// 如果use caller，那只能由caller线程发起stop
    if (m_useCaller) {
      FISHJOY_ASSERT(GetThis() == this)
    } else {
      FISHJOY_ASSERT(GetThis() != this)
    }

    for (size_t i = 0; i < m_threadCount; i++) {
      tickle();
    }

    if (m_rootFiber) {
      tickle();
    }

    /// 在use caller情况下，调度器协程结束时，应该返回caller协程
    if (m_rootFiber) {
      m_rootFiber->resume();
      FISHJOY_LOG_DEBUG(g_logger) << "m_rootFiber end";
    }

    std::vector<Thread::ptr> thrs;
    {
      MutexType::Lock lock(m_mutex);
      thrs.swap(m_threads);
    }
    for (auto &i : thrs) {
      i->join();
    }
  }

  void Scheduler::run() {
    FISHJOY_LOG_DEBUG(g_logger) << "run";
    set_hook_enable(true);
    setThis();
    if (fishjoy::GetThreadId() != m_rootThread) {
      t_scheduler_fiber = fishjoy::Fiber::GetThis().get();
    }

    Fiber::ptr idle_fiber(new Fiber(std::bind(&Scheduler::idle, this)));
    Fiber::ptr cb_fiber;

    ScheduleTask task;
    while (true) {
      task.reset();
      bool tickle_me = false; // 是否 tickle 其他线程进行任务调度
      {
        MutexType::Lock lock(m_mutex);
        auto it = m_tasks.begin();
        // 遍历所有调度任务
        while (it != m_tasks.end()) {
          if (it->thread != -1 && it->thread != fishjoy::GetThreadId()) {
            // 指定了调度线程，但不是在当前线程上调度，标记一下需要通知其他线程进行调度，然后跳过这个任务，继续下一个
            ++it;
            tickle_me = true;
            continue;
          }

          // 找到一个未指定线程，或是指定了当前线程的任务
          FISHJOY_ASSERT(it->fiber || it->callback)

          // if (it->fiber) {
          //     // 任务队列时的协程一定是READY状态，谁会把RUNNING或TERM状态的协程加入调度呢？
          //     FISHJOY_ASSERT(it->fiber->getState() == Fiber::READY);
          // }

          // [BUG FIX]: hook IO相关的系统调用时，在检测到IO未就绪的情况下，会先添加对应的读写事件，再yield当前协程，等IO就绪后再resume当前协程
          // 多线程高并发情境下，有可能发生刚添加事件就被触发的情况，如果此时当前协程还未来得及yield，则这里就有可能出现协程状态仍为RUNNING的情况
          // 这里简单地跳过这种情况，以损失一点性能为代价，否则整个协程框架都要大改
          if(it->fiber && it->fiber->getState() == Fiber::RUNNING) {
            ++it;
            continue;
          }
                
          // 当前调度线程找到一个任务，准备开始调度，将其从任务队列中剔除，活动线程数加1
          task = *it;
          m_tasks.erase(it++);
          ++m_activeThreadCount;
          break;
        }
        // 当前线程拿完一个任务后，发现任务队列还有剩余，那么tickle一下其他线程
        tickle_me |= (it != m_tasks.end());
      }

      if (tickle_me) {
        tickle();
      }

      if (task.fiber) {
        // resume协程，resume返回时，协程要么执行完了，要么半路yield了，总之这个任务就算完成了，活跃线程数减一
        task.fiber->resume();
        --m_activeThreadCount;
        task.reset();
      } else if (task.callback) {
        if (cb_fiber) {
          cb_fiber->reset(task.callback);
        } else {
          cb_fiber.reset(new Fiber(task.callback));
        }
        task.reset();

        cb_fiber->resume();
        --m_activeThreadCount;
        cb_fiber.reset();
      } else {
        // 进到这个分支情况一定是任务队列空了，调度idle协程即可
        if (idle_fiber->getState() == Fiber::TERM) {
          // 如果调度器没有调度任务，那么idle协程会不停地resume/yield，不会结束，如果idle协程结束了，那一定是调度器停止了
          FISHJOY_LOG_DEBUG(g_logger) << "idle fiber term";
          break;
        }
        ++m_idleThreadCount;
        idle_fiber->resume();
        --m_idleThreadCount;
      }
    }
    FISHJOY_LOG_DEBUG(g_logger) << "Scheduler::run() exit";
  }  
}