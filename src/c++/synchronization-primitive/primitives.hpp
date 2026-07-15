#ifndef PRIMITIVES_HPP
#define PRIMITIVES_HPP
#include <mutex>
#include <condition_variable>
#include <atomic>
#include <chrono>
#include <thread>

class spinlock 
{
private:
    std::atomic_flag flag;
public:
    void lock();
    void unlock();

    spinlock();

    void lock();
    void unlock();
};

class barrier 
{
private:    
    std::mutex mtx;
    std::condition_variable cv;
    int count;
public:
    barrier(int count);
    void arrive_and_wait();
};

class semaphore 
{
private:
    std::mutex mtx;
    std::condition_variable cv;
	int cnt;
public:
    semaphore(int cnt);
    void acquire();
    void release(); 
};

#endif