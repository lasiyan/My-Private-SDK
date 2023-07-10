#include "threadPool.hpp"

#include <algorithm>
#include <exception>
#include <iostream>

namespace rs {

ThreadPool::ThreadPool(size_t min_threads, size_t max_threads)
    : min_threads_(min_threads),
      max_threads_(max_threads),
      active_threads_(0),
      total_threads_(min_threads)
{
  threads_.reserve(max_threads);

  for (size_t i = 0; i < min_threads; ++i) {
    threads_.emplace_back([this]() { this->createWorkerThread(); });
  }
}

ThreadPool::~ThreadPool()
{
  {
    std::unique_lock<std::mutex> lock(job_mutex_);
    threads_stop_ = true;
  }
  job_convar_.notify_all();

  for (auto& thread : threads_) {
    if (thread.joinable())
      thread.join();
  }
}

void ThreadPool::createWorkerThread()
{
  auto id = std::this_thread::get_id();

  while (true) {
    std::unique_lock<std::mutex> ulock(job_mutex_);

    job_convar_.wait(ulock, [this]() {
      return (!this->job_queue_.empty() || this->threads_stop_);
    });

    if (threads_stop_ && job_queue_.empty()) {
      total_threads_--;
      break;
    }

    if (job_queue_.empty() == false) {
      std::function<void()> job = std::move(job_queue_.front());
      job_queue_.pop();
      ulock.unlock();

      active_threads_++;

      try {
        job();
      }
      catch (const std::exception& e) {
        std::cerr << "ThreadPool : std::Exception : " << e.what() << "\n";
      }
      catch (...) {
        std::cerr << "ThreadPool : Unknown Exception : "
                  << "\n";
      }

      active_threads_--;
    }

    if (threads_stop_ == false) {
      if (job_queue_.empty() && total_threads_ > min_threads_) {
        std::unique_lock<std::mutex> ulock(job_mutex_);

        auto it = std::find_if(
            threads_.begin(), threads_.end(),
            [id](const std::thread& t) { return t.get_id() == id; });

        if (it != threads_.end()) {
          it->detach();
          threads_.erase(it);
          total_threads_--;
        }
      }
    }
  }
}
}  // namespace rs