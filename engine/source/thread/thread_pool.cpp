#include "thread_pool.h"

namespace core
{
	Task::Task(std::function<void()> _function_, std::optional<Callback*> _callback)
		: function_(_function_)
		, callback_(_callback)
	{}

	void Task::Execute()
	{
		function_();

		if (callback_)
		{
			(*callback_)->OnFinished();
		}
	}

	ThreadPool::ThreadPool(size_t _numThreads)
		: stop_(false)
	{
		workers_.reserve(_numThreads);
		for (size_t i = 0; i < _numThreads; ++i)
		{
			workers_.emplace_back(&ThreadPool::WorkerThread, this);
		}
	}

	ThreadPool::~ThreadPool()
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

	void ThreadPool::EnqueueTask(Task _task)
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
			Task task;
			{
				std::unique_lock<std::mutex> lock(mutex_); // must be locked inside this scope to avoid blocking other workers

				condition_.wait(lock, [this] { return stop_ || !tasks_.empty(); });

				if (stop_)
				{
					return;
				}

				task = tasks_.front();
				tasks_.pop();
			}

			task.Execute();
		}
	}
}