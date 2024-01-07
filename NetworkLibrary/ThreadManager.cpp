#include "pch.h"
#include "ThreadManager.h"
#include "ThreadVar.h"
#include "GlobalVar.h"
#include "TaskQueueManager.h"

ThreadManager::ThreadManager()
{
	InitThreadVar();
}

ThreadManager::~ThreadManager()
{
	Join();
}

void ThreadManager::Launch(std::function<void()> task)
{
	std::lock_guard<std::mutex> guard(_mutex);
	_threads.push_back(std::thread([=]()
		{
			InitThreadVar();
			task();
			DestroyThreadVar();
		}));
}

void ThreadManager::Join()
{
	for (std::thread& t: _threads)
	{
		if (t.joinable())
			t.join();
	}

	_threads.clear();

}

void ThreadManager::InitThreadVar()
{
	static std::atomic<__int32> SThreadId = 1;
	LThreadId = SThreadId.fetch_add(1);
}

void ThreadManager::DestroyThreadVar()
{

}

void ThreadManager::WorkGlobalQueueTask()
{
	while (true)
	{
		unsigned __int64 now = ::GetTickCount64();
		if (now > LEndTickCount)
			break;

		std::shared_ptr<TaskQueue> taskQueue = GTaskQueueManager->Pop();
		if (taskQueue == nullptr)
			break;

		taskQueue->Execute();
	}
}

void ThreadManager::PushReservedTasks()
{
	const unsigned __int64 now = ::GetTickCount64();

	GTaskTimer->Distribute(now);
}
