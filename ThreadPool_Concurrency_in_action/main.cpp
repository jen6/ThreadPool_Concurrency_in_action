#include <iostream>
#include <thread>
#include <chrono>

#include "thread_pool.h"

volatile int var = 0;
int google = 0;
int main()
{
	//std::this_thread::sleep_for(std::chrono::seconds(5));
	thread_pool p;
	for (auto i = 0; i < 500; ++i)
	{
		p.submit(
			[]() 
			{
				for (auto i = 0; i < 100000000; ++i)
				{
					var++;
				}
			}
		);
	}
}