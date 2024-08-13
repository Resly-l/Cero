#include "thread_pool.h"

namespace thread
{
	void ThreadPool::Initialize(size_t _numThreads)
	{
		workers_.reserve(_numThreads);
		for (size_t i = 0; i < _numThreads; ++i)
		{
			workers_.emplace_back(&ThreadPool::WorkerThread);
		}
	}

	void ThreadPool::Deinitialize()
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);
			stop_ = true;
		}

		condition_.notify_all();
		for (std::thread& worker : workers_)
		{
			worker.join();
		}
	}

	void ThreadPool::EnqueueTask(std::function<void()> _task)
	{
		{
			std::unique_lock<std::mutex> lock(mutex_);
			tasks_.push(std::move(_task));
		}
		condition_.notify_one();
	}

	void ThreadPool::WorkerThread()
	{
		while (true)
		{
			std::function<void()> task;
			{
				std::unique_lock<std::mutex> lock(mutex_); // must be locked inside this scope to avoid blocking other workers

				condition_.wait(lock, [] { return stop_ || !tasks_.empty(); });

				if (stop_)
				{
					return;
				}

				task = tasks_.front();
				tasks_.pop();
			}

			task();
		}
	}
}