#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <thread>

spinlock::spinlock() {}

void spinlock::lock() 
{
	while(flag.test_and_set(std::memory_order_acquire)) 
	{
		std::this_thread::sleep_for(std::chrono::milliseconds(20));
	}
}

void spinlock::unlock() 
{
	flag.clear();
}

barrier::barrier(int count) 
{
	this->count = count;
}

void barrier::arrive_and_wait() 
{
	std::unique_lock<std::mutex> lock(mtx);
	if (this->count > 0)
	{
		count--;
		cv.notify_all();
	}
		
	while(this->count > 0)
		cv.wait(lock);
}

semaphore::semaphore(int cnt) : cnt(cnt) {}

void semaphore::acquire() 
{
	std::unique_lock<std::mutex> lock(mtx);
	while(cnt == 0)
		cv.wait(lock);
	cnt--;
}

void semaphore::release() 
{
	std::unique_lock<std::mutex> lock(mtx);
	cnt++;
	cv.notify_one();
}

