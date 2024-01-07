#include "pch.h"
#include "TaskQueue.h"
#include "TaskQueueManager.h"



void TaskQueue::Push(std::shared_ptr<Task> task, bool pushOnly)
{
	const __int32 prevCount = _taskCount.fetch_add(1);
	_tasks.Push(task); 


	if (prevCount == 0)
	{
		
		if (LCurrentTaskQueue == nullptr)
		{
			Execute();
		}
		else
		{
			
			GTaskQueueManager->Push(shared_from_this());
		}
	}
}


void TaskQueue::Execute()
{
	LCurrentTaskQueue = this;

	while (true)
	{
		std::vector<std::shared_ptr<Task>> tasks;
		_tasks.PopAll(tasks);

		const __int32 taskCount = static_cast<__int32>(tasks.size());
		for (__int32 i = 0; i < taskCount; i++)
			tasks[i]->Execute();


		if (_taskCount.fetch_sub(taskCount) == taskCount)
		{
			LCurrentTaskQueue = nullptr;
			return;
		}

		const unsigned __int64 now = ::GetTickCount64();
		if (now >= LEndTickCount)
		{
			LCurrentTaskQueue = nullptr;
			
			GTaskQueueManager->Push(shared_from_this());
			break;
		}
	}
}
