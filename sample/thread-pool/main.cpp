#include <iostream>

#include "rowen/core.hpp"
#include "rowen/util/threadPool.hpp"
// #include <future>

std::mutex job_creation_mutex;

void printThreadInfo(rs::ThreadPool* pool)
{
  if (pool == nullptr)
    return;

  logger.info("Working Threads: %lu"
              ", Waiting Jobs: %lu"
              ", Total Threads: %lu"
              ", Maximum Threads: %lu",
              pool->workingCount(), pool->waitingCount(), pool->workerCount(),
              pool->maxThreads());
}

int main()
{
  constexpr size_t min_threads = 1;
  constexpr size_t max_threads = 6;
  constexpr int    total_jobs  = 20;

  std::atomic_bool finished = { false };

  auto pool = std::make_unique<rs::ThreadPool>(min_threads, max_threads);

  // Print thread pool information
  auto logging = std::async(std::launch::async, [&] {
    while (finished.load() == false) {
      {
        std::unique_lock<std::mutex> lock(job_creation_mutex);
        printThreadInfo(pool.get());
      }
      rs::Time::sleep(1s);
    }
  });

  // Create jobs
  auto createJob = std::async(std::launch::async, [&] {
    for (int i = 0; i < total_jobs; ++i) {
      pool->insertJob([i, total_jobs, &finished] {
        // Get Thread ID
        std::ostringstream oss;
        oss << std::this_thread::get_id();
        auto tid = oss.str();

        // Start Job
        {
          std::unique_lock<std::mutex> lock(job_creation_mutex);
          logger.info("Start  Job : %2d on thread %s", i, tid.c_str());
        }

        // Progress Job
        rs::Time::sleep(5s);

        // Finish Job
        {
          std::unique_lock<std::mutex> lock(job_creation_mutex);
          logger.info("Finish Job : %2d on thread %s", i, tid.c_str());

          if (i == total_jobs - 1) {
            finished.store(true);
          }
        }
      });
      rs::Time::sleep(0.5s);
    }
  });

  createJob.wait();
  logging.wait();

  return 0;
}