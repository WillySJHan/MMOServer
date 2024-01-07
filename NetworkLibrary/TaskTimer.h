#pragma once

struct TaskData
{
	TaskData(std::weak_ptr<TaskQueue> owner, std::shared_ptr<Task> task) : owner(owner), task(task)
	{

	}

	std::weak_ptr<TaskQueue>	owner;
	std::shared_ptr<Task>				task;
};

struct TimerData
{
	bool operator<(const TimerData& other) const
	{
		return executeTick > other.executeTick;
	}

	unsigned __int64 executeTick = 0;
	TaskData* taskData = nullptr;
};



class TaskTimer
{
public:
	void			Reserve(unsigned __int64 tickAfter, std::weak_ptr<TaskQueue> owner, std::shared_ptr<Task> task);
	void			Distribute(unsigned __int64 now);
	void			Clear();

private:
	std::recursive_mutex _rMutex;
	std::priority_queue<TimerData>	_datas;
	std::atomic<bool>				_distributing = false;
};

