//
// Created by zsl on 8/21/22.
//

#ifndef FISHJOY_THREADPOOL_HPP
#define FISHJOY_THREADPOOL_HPP


#include <utility>
#include <vector>
#include <memory>
#include <queue>
#include <atomic>
#include <mutex>
#include <condition_variable>
#include <functional>
#include <thread>
#include <future>
#include <iostream>

namespace fishjoy {
  namespace threadpool {
    const int TASK_MAX_THRESHOLD = INT32_MAX;
    const int THREAD_MAX_THRESHOLD = 10;
    const int THREAD_MAX_IDLE_TIME = 60; //seconds
    /**
      * @brief 线程池支持的模式
     */
    enum class PoolMode {
      MODE_FIXED,     // 固定数量的线程
      MODE_CACHED,    // 线程数量可动态增长
    };
    //TODO 和原来的 Thread 类做个融合
    /**
      * @brief 线程类型
     */
    class Thread
    {
     public:
      //线程函数对象类型
      using ThreadFunc = std::function<void(int)>;
      //线程构造函数
      explicit Thread(ThreadFunc func)
          :func_(std::move(func))
            ,threadId_(generatedId_++)
      {}
      //线程析构函数
      ~Thread() = default;
      //启动线程
      void start() {
        //创建一个线程来执行线程函数
        //ThreadPool::threadFunc() -> Thread类中绑定接收 -> func -> func_
        std::thread t(func_, threadId_);//c++11来将，线程对象t   和  线程执行函数func_
        t.detach(); //设置分离线程    线程对象和线程执行函数分开，，，线程对象t离开作用域会被析构，detach之后不影响线程函数的执行
      }
      //获取线程id
      int getId() const {
        return threadId_;
      }
     private:

      ThreadFunc func_;
      int threadId_;//保存线程id
      static int generatedId_;
    };

    int Thread::generatedId_ = 0;

    /**
 * @brief 线程池类型
     */
    class ThreadPool {
     public:
      /// 线程池构造
      ThreadPool()
          : initThreadSize_(0)
            , taskSize_(0)
            , taskQueMaxSizeThreshold_(TASK_MAX_THRESHOLD)
            , poolMode_(PoolMode::MODE_FIXED)
            , isPoolRunning_(false)
            , idleThreadSize_(0)
            , threadSizeThreshold_(THREAD_MAX_THRESHOLD)
            , curThreadSize_(0) {
      }


      /// 线程池析构 用户的线程（需要线程通信）
      ~ThreadPool() {
        isPoolRunning_ = false;
        // 等待线程池所有的线程返回 两种状态：阻塞 & 正在执行任务中
        std::unique_lock<std::mutex> lock(taskQueMtx_);  // 都给唤醒了
        notEmpty_.notify_all();// 等待 ==> 阻塞
        // 等待容器里的线程对象都清空了才析构,每个线程回收的时候会唤醒 exitCond_ 来检查是否满足条件
        exitCond_.wait(lock,[&]()->bool {return threads_.empty();});
      }

      /// 开启线程池
      void start(unsigned int initThreadSize = std::thread::hardware_concurrency())
      {
        //设置线程池运行状态
        isPoolRunning_ = true;

        //记录初始线程个数
        initThreadSize_ = initThreadSize;
        //记录当前线程个数
        curThreadSize_ = initThreadSize;

        //集中创建线程对象
        for (int i = 0; i < initThreadSize_; i++)
        {
          //创建thread线程对象的时候，把线程函数给到thread线程对象
          auto ptr = std::make_unique<Thread>([this](auto && PH1) { threadFunc(std::forward<decltype(PH1)>(PH1)); });
          int threadId = ptr->getId();
          threads_.emplace(threadId, std::move(ptr));
        }

        //启动所有线程
        for (int i = 0; i < initThreadSize_; i++)
        {
          threads_[i]->start();
          idleThreadSize_++;//记录空闲线程数量
        }
      }

      /// 设置线程池模式
      void setMode(PoolMode mode) {
        if(checkRunningState()) return;
        poolMode_ = mode;
      }

      /// 设置任务队列上限阈值
      void setTaskQueMaxThreshHold(int threshold) {
        if(checkRunningState()) return;
        taskQueMaxSizeThreshold_ = threshold;
      }

      /// 设置线程池 cached 模式下线程阈值
      void setThreadSizeThreshHold(int threshold) {
        if(checkRunningState()) return;
        if(poolMode_ == PoolMode::MODE_CACHED) {
          threadSizeThreshold_ = threshold;
        }
      }


      /// 设置初始的线程数量
      void setInitThreadSize(int size) {
        if(checkRunningState()) return;
        initThreadSize_ = size;
      }

      /// 给线程池提交任务
      /// 使用可变参模板编程，让 submitTask 可以接收任意任务函数和任意数量的参数
      /// pool.submitTask(sum1, 10, 20);
      /// 返回值 future<>  用 decltype 推
      template<typename Func, typename... Args>
      auto submitTask(Func&& func, Args&&...args) -> std::future<decltype(func(args...))> {
        //打包任务 放到任务队列中
        using RType = decltype(func(args...));
        auto task = std::make_shared<std::packaged_task<RType()>>(
            std::bind(std::forward<Func>(func), std::forward<Args>(args)...));
        std::future<RType> result = task->get_future();

        // 获取锁
        std::unique_lock<std::mutex> lock(taskQueMtx_);
        // 用户提交任务，最长不能阻塞超过 1s, 否则提交任务失败
        if (!notFull_.wait_for(lock, std::chrono::seconds(1),
                               [&]()->bool {return taskQue_.size() < (size_t)taskQueMaxSizeThreshold_; }))
        {
          //表示notFull_等待1s，条件依然没有满足
          std::cerr << "task queue is full, submit task fail." << std::endl;
          auto taskEmpty = std::make_shared<std::packaged_task<RType()>>(
              []()->RType { return RType(); });
          (*taskEmpty)();
          return taskEmpty->get_future();
        }

        // 如果有空余，把任务放到任务队列中
        // using Task = std::function<void()>;
        taskQue_.emplace([task]() {(*task)();});
        ++taskSize_;

        // 因为新放了任务，任务队列肯定不空了，在 notEmpty_ 上进行通知, 赶快分配线程执行任务 （消费）
        notEmpty_.notify_all();

        // cached模式 任务处理比较紧急 场景：小而快的任务
        // 需要根据任务数量和空闲线程数量，判断是否需要创建新的线程出来
        if(poolMode_ == PoolMode::MODE_CACHED
            && taskSize_ > idleThreadSize_
            &&  curThreadSize_ < threadSizeThreshold_) {
          std::cout << "create new thread..." << std::endl;
          // 创建新线程对象
          auto ptr = std::make_unique<Thread>([this](auto && PH1) { threadFunc(std::forward<decltype(PH1)>(PH1)); });
          int threadId = ptr->getId();
          threads_.emplace(threadId, std::move(ptr));

          // 启动线程
          threads_[threadId]->start();

          // 修改线程个数相关变量
          curThreadSize_++;
          idleThreadSize_++;
        }

        return result;
      }

      /// 禁止拷贝构造和赋值
      ThreadPool(const ThreadPool&) = delete;
      ThreadPool& operator=(const ThreadPool&) = delete;

     private:
      /// 定义线程函数 线程池的所有线程从任务队列里 消费任务
      void threadFunc(int threadid) {
        auto lastTime = std::chrono::high_resolution_clock::now();
        while(isPoolRunning_){
          Task task;
          {
            // 先取锁
            std::unique_lock<std::mutex> lock(taskQueMtx_);
            std::cout << "tid:"<< std::this_thread::get_id() << "尝试获取任务..." << std::endl;

            // 锁 + 双重判断
            while(isPoolRunning_ && taskQue_.empty()) {   // 任务队列没任务，就等 1s 看看情况；有任务就直接出去消费
              if (poolMode_ == PoolMode::MODE_CACHED) {
                // cached 模式下，有可能已经创建了很多的线程，但是空闲时间超过 60s
                // 超过 initThreadSize_ 数量的线程要进行回收
                // 当前时间 - 上次线程执行的时间 > 60s
                // 每 1s 中返回一次 怎么区分超时返回还是有任务待执行返回？

                if (std::cv_status::timeout ==
                    notEmpty_.wait_for(lock, std::chrono::seconds(1))) {
                  // 1s 后还是没任务 条件变量超时返回 检查如果线程空闲了 60s 那就回收掉
                  auto now = std::chrono::high_resolution_clock::now();
                  auto dur = std::chrono::duration_cast<std::chrono::seconds>(now - lastTime);
                  if (dur.count() >= THREAD_MAX_IDLE_TIME
                      && curThreadSize_ > initThreadSize_) {
                    // 开始回收当前线程
                    // 记录线程数量的相关变量的值修改
                    // 把线程对象从线程列表容器中回收 没有办法匹配 threadFunc 是哪一个 Thread 对象
                    // thread id => thread 对象 => 删除
                    threads_.erase(threadid);   // 不要 std::this_thread::get_id()
                    curThreadSize_--;
                    idleThreadSize_--;

                    std::cout << "threadid:" << std::this_thread::get_id() << "exit" << std::endl;
                    // 通知一下主线程
                    exitCond_.notify_all();
                    return;
                  }
                }
              } else {
                notEmpty_.wait(lock);
              }
            }

            if(!isPoolRunning_) {
              break;
            }


            std::cout << "tid:"<< std::this_thread::get_id() << "获取任务成功..." << std::endl;
            // 取到任务减少空闲线程数量
            idleThreadSize_--;

            // 从任务队列中取一个任务出来
            task = taskQue_.front();
            taskQue_.pop();
            --taskSize_;

            // 如果依然有剩余任务，继续通知其他的线程执行任务
            if(!taskQue_.empty()) {
              notEmpty_.notify_all();
            }

            // 取出一个任务，进行通知,通知可以继续提交生产任务
            notFull_.notify_all();
          }   // 取完任务就释放锁了，执行任务的时候不应该拿着锁占时间

          // 当前线程负责执行这个任务
          if(task != nullptr) {
            task(); // 执行function<void()>
          }
          // 执行完任务空闲了
          idleThreadSize_++;
          // 更新线程执行完任务的时间
          lastTime = std::chrono::high_resolution_clock::now();

        }

        // 结束线程池的时候正在执行任务，回来发现 isPoolRunning_ == false,就跳到这里
        threads_.erase(threadid);
        std::cout << "threadid:" << std::this_thread::get_id() << "exit" << std::endl;
        // 通知一下主线程
        exitCond_.notify_all();
      }

      /// 检查 pool 运行状态
      bool checkRunningState() const {
        return isPoolRunning_;
      }

     private:
      std::unordered_map<int,std::unique_ptr<Thread>> threads_;   // 线程列表
      unsigned int initThreadSize_;                               // 初始线程数量
      std::atomic_uint curThreadSize_;                            // 记录当前线程池里面的线程数量
      unsigned int threadSizeThreshold_;                          // 线程数量上限阈值
      std::atomic_uint idleThreadSize_;                           // 记录空闲线程数量


      using Task = std::function<void()>;                         // Task任务 => 函数对象
      std::queue<Task> taskQue_;	                                // 任务队列
      std::atomic_uint taskSize_;                                 // 任务数量
      int taskQueMaxSizeThreshold_;                               // 任务队列数量上限的阈值


      std::mutex taskQueMtx_;                                     // 保证任务队列线程安全
      std::condition_variable notFull_;                           // 任务队列不满
      std::condition_variable notEmpty_;                          // 任务队列不空
      std::condition_variable exitCond_;                          // 等待线程资源全部回收


      PoolMode poolMode_;                                         // 当前线程池模式
      std::atomic_bool isPoolRunning_;                            // 表示线程池是否正在运行

    };
  }
}

#endif  // FISHJOY_THREADPOOL_HPP
