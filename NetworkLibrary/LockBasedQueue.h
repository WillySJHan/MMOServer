#pragma once

template<typename T>
class LockBasedQueue
{
public:
	void Push(T data)
	{
		std::lock_guard<std::recursive_mutex> lock(_rMutex);
		_datas.push(data);
	}

	T Pop()
	{
		std::lock_guard<std::recursive_mutex> lock(_rMutex);
		if (_datas.empty())
			return T();

		T ret = _datas.front();
		_datas.pop();
		return ret;
	}

	void PopAll(std::vector<T>& datas)
	{
		std::lock_guard<std::recursive_mutex> lock(_rMutex);
		while (T data = Pop())
			datas.push_back(data);
	}

	void Clear()
	{
		std::lock_guard<std::recursive_mutex> lock(_rMutex);
		_datas = std::queue<T>();
	}


private:
	std::recursive_mutex _rMutex;
	std::queue<T> _datas;
};