#pragma once
#include <queue>
#include <condition_variable>
#include <functional>
#include "utility/forward_declaration.h"

namespace thread
{
	class ThreadPool
	{
		friend class window::Application;

	private:
		inline static std::vector<std::thread> workers_;
		inline static std::queue<std::function<void()>> tasks_;
		inline static std::mutex mutex_;
		inline static std::condition_variable condition_;
		inline static bool stop_ = false;

	public:
		static void EnqueueTask(std::function<void()> _task);

	private:
		static void Initialize(size_t _numThreads = 4);
		static void Deinitialize();
		static void WorkerThread();
	};
}