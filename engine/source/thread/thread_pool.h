#pragma once
#include <functional>
#include <optional>
#include <queue>
#include <condition_variable>

namespace core
{
	class Task
	{
	public:
		class Callback
		{
		public:
			virtual void OnFinished() = 0;
		};

	private:
		std::function<void()> function_;
		std::optional<Callback*> callback_;

	public:
		Task() = default;
		Task(std::function<void()> _function_, std::optional<Callback*> _callback = std::nullopt);

		void Execute();
	};

	class ThreadPool
	{
	private:
		std::vector<std::thread> workers_;
		std::queue<Task> tasks_;

		std::mutex mutex_;
		std::condition_variable condition_;
		bool stop_;

	public:
		ThreadPool(size_t _numThreads);
		~ThreadPool();

		void EnqueueTask(Task _task);

	private:
		void WorkerThread();
	};
}