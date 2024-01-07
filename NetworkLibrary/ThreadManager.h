#pragma once

#include <thread>
#include <functional>
#include <mutex>

class ThreadManager
{
public:
	ThreadManager();
	~ThreadManager();

	void Launch(std::function<void(void)> task);
	void Join();

	static void InitThreadVar();
	static void DestroyThreadVar();

	static void WorkGlobalQueueTask();
	static void PushReservedTasks();

private:
	std::mutex					_mutex;
	std::vector<std::thread>	_threads;
};

