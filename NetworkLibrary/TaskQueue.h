#pragma once
#include "Task.h"
#include "LockBasedQueue.h"
#include "TaskTimer.h"



class TaskQueue : public std::enable_shared_from_this<TaskQueue>
{
public:
	void DoAsync(CallbackType&& callback)
	{
		Push(std::make_shared<Task>(std::move(callback)));
	}

	template<typename T, typename Ret, typename... Args>
	void DoAsync(Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		//std::shared_ptr<T> owner = std::reinterpret_pointer_cast<T>(shared_from_this());
		Push(std::make_shared<Task>(owner, memFunc, std::forward<Args>(args)...));
	}

	std::shared_ptr<Task> DoTimer(unsigned __int64 tickAfter, CallbackType&& callback)
	{
		std::shared_ptr<Task> task = std::make_shared<Task>(std::move(callback));
		GTaskTimer->Reserve(tickAfter, shared_from_this(), task);

		return task;
	}

	template<typename T, typename Ret, typename... Args>
	std::shared_ptr<Task> DoTimer(unsigned __int64 tickAfter, Ret(T::* memFunc)(Args...), Args... args)
	{
		std::shared_ptr<T> owner = std::static_pointer_cast<T>(shared_from_this());
		//std::shared_ptr<T> owner = std::reinterpret_pointer_cast<T>(shared_from_this());
		std::shared_ptr<Task> task = std::make_shared<Task>(owner, memFunc, std::forward<Args>(args)...);
		GTaskTimer->Reserve(tickAfter, shared_from_this(), task);
		return task;
	}

	void				ClearTasks() { _tasks.Clear(); }

public:
	void				Push(std::shared_ptr<Task> task, bool pushOnly = false);

public:
	void				Execute();

protected:
	LockBasedQueue<std::shared_ptr<Task>>	_tasks;
	std::atomic<__int32>		_taskCount = 0;
};
