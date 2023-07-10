#ifndef __ROWEN_SDK_UTIL_THREADPOOL_HPP__
#define __ROWEN_SDK_UTIL_THREADPOOL_HPP__

#include <functional>
#include <future>
#include <queue>

namespace rs {

class ThreadPool {
 public:
  ThreadPool(size_t min_thread, size_t max_thread);
  ~ThreadPool();

  template <typename Callable, typename... Args>
  std::future<typename std::result_of<Callable(Args...)>::type> insertJob(
      Callable&& func, Args&&... args)
  {
    if (threads_stop_) {
      throw std::runtime_error("ThreadPool stoped");
    }

    using return_type = typename std::result_of<Callable(Args...)>::type;

    auto job = std::make_shared<std::packaged_task<return_type()>>(
        std::bind(std::forward<Callable>(func), std::forward<Args>(args)...));

    std::future<return_type> future_job = job->get_future();

    {
      std::unique_lock<std::mutex> lock(job_mutex_);

      bool available_threads = active_threads_ < total_threads_;

      if (!available_threads && total_threads_ < max_threads_) {
        threads_.emplace_back([this]() { this->createWorkerThread(); });
        ++total_threads_;
      }

      job_queue_.push([job]() { (*job)(); });
    }

    job_convar_.notify_one();

    return future_job;
  }

  // return current running threads count
  decltype(auto) workingCount() { return active_threads_.load(); }

  // return total thread count in thread-pool
  decltype(auto) workerCount() { return total_threads_.load(); }

  // return waiting jobs count
  decltype(auto) waitingCount() { return job_queue_.size(); }

  // return maximum available thread count
  decltype(auto) maxThreads() { return max_threads_; }

 private:
  void createWorkerThread();

 private:
  std::vector<std::thread> threads_      = {};
  bool                     threads_stop_ = false;

  size_t              min_threads_;
  size_t              max_threads_;
  std::atomic<size_t> active_threads_;
  std::atomic<size_t> total_threads_;

  std::queue<std::function<void()>> job_queue_;
  std::condition_variable           job_convar_;
  std::mutex                        job_mutex_;
};

}  // namespace rs

#endif  //__ROWEN_SDK_UTIL_THREADPOOL_HPP__