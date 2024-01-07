#pragma once
#include <functional>



using CallbackType = std::function<void(void)>;

class Task
{
public:
	Task(CallbackType&& callback) : _callback(std::move(callback))
	{
	}

	template<typename T, typename Ret, typename... Args>
	Task(std::shared_ptr<T> owner, Ret(T::* memFunc)(Args...), Args&&... args)
	{
		_callback = [owner, memFunc, args...]()
			{
				(owner.get()->*memFunc)(args...);
			};
	}

	void Execute()
	{
 		if ( _cancel == false)
			_callback();
	}

private:
	CallbackType _callback;

public:
	bool _cancel = false;
};

