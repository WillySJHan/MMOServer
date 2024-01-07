#include "pch.h"
#include "TaskTimer.h"
#include "TaskQueue.h"



void TaskTimer::Reserve(unsigned __int64 tickAfter, std::weak_ptr<TaskQueue> owner, std::shared_ptr<Task> task)
{
	const unsigned __int64 executeTick = ::GetTickCount64() + tickAfter;
	/////////////////////////////////////////////////////////////////////////////////////
	TaskData* task_data = new TaskData(owner, task);

	std::lock_guard<std::recursive_mutex> lock(_rMutex);

	_datas.push(TimerData{ executeTick, task_data });
}

void TaskTimer::Distribute(unsigned __int64 now)
{
	
	if (_distributing.exchange(true) == true)
		return;

	std::vector<TimerData> datas;

	{
		std::lock_guard<std::recursive_mutex> lock(_rMutex);

		while (_datas.empty() == false)
		{
			const TimerData& timerItem = _datas.top();
			if (now < timerItem.executeTick)
				break;

			datas.push_back(timerItem);
			_datas.pop();
		}
	}

	for (TimerData& data : datas)
	{
		if (std::shared_ptr<TaskQueue> owner = data.taskData->owner.lock())
			owner->Push(data.taskData->task);

		delete(data.taskData);
	}

	
	_distributing.store(false);
}

void TaskTimer::Clear()
{
	std::lock_guard<std::recursive_mutex> lock(_rMutex);

	while (_datas.empty() == false)
	{
		const TimerData& timerData = _datas.top();
		delete(timerData.taskData);
		_datas.pop();
	}
}