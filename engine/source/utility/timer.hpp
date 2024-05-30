#pragma once
#include <chrono>

namespace utility
{
	template <typename T, typename Ratio = std::ratio<1>>
	class Timer
	{
	private:
		using Clock = std::chrono::high_resolution_clock;
		using Duration = std::chrono::duration<T, Ratio>;

	private:
		Clock::time_point timePoint_;

	public:
		Timer()
		{
			timePoint_ = Clock::now();
		}
		virtual ~Timer() = default;

	public:
		T Peek() const
		{
			return std::chrono::duration_cast<Duration>(Clock::now() - timePoint_).count();
		}

		T Mark()
		{
			const auto now = Clock::now();
			T delta = std::chrono::duration_cast<Duration>(Clock::now() - timePoint_).count();
			timePoint_ = now;

			return delta;
		}
	};
}