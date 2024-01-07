#include "pch.h"
#include "TaskQueueManager.h"



TaskQueueManager::TaskQueueManager()
{

}

TaskQueueManager::~TaskQueueManager()
{

}

void TaskQueueManager::Push(std::shared_ptr<class TaskQueue> taskQueue)
{
	_taskQueues.Push(taskQueue);
}

std::shared_ptr<class TaskQueue> TaskQueueManager::Pop()
{
	return _taskQueues.Pop();
}
