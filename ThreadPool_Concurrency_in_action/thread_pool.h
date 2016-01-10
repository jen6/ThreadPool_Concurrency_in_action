#pragma once
#include <atomic>
#include <vector>
#include <functional>
#include <thread>
#include <chrono>

#include "thread_safe_queue.h"
#include "join_threads.h"

class thread_pool
{
	std::atomic_bool done;
	thread_safe_queue<std::function<void()> > work_queue;
	std::vector<std::thread> threads;
	join_threads joiner;

	std::chrono::time_point<std::chrono::system_clock> time;

	void worker_thread()
	{
		while (!done)
		{
			std::function<void()> task;
			if (work_queue.try_pop(task))
			{
				task();
			}
			else
			{
				std::this_thread::yield();
			}
		}
	}
public:
	thread_pool() :
		done(false), joiner(threads), time(std::chrono::system_clock::now())
	{
		//unsigned const thread_count = std::thread::hardware_concurrency() - 1;
		unsigned const thread_count = std::thread::hardware_concurrency();
		try
		{
			for (unsigned i = 0; i<thread_count; ++i)
			{
				threads.push_back(
					std::thread(&thread_pool::worker_thread, this));
			}
		}
		catch (...)
		{
			done = true;
			throw;
		}
	}
	~thread_pool()
	{
		while (!work_queue.empty()) {}
		done = true;

		auto end = std::chrono::system_clock::now();
		std::chrono::duration<double> elapsed = end - time;
		std::cout << "time elapsed : " << elapsed.count() << std::endl;
	}
	template<typename FunctionType>
	void submit(FunctionType f)
	{
		work_queue.push(std::function<void()>(f));
	}
};