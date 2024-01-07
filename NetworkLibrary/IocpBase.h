#pragma once


class IocpObject : public std::enable_shared_from_this<IocpObject>
{
public:
	virtual HANDLE GetHandle() abstract;
	virtual void Dispatch(class IocpEvent* iocpEvent, __int32 numOfBytes = 0) abstract;
};


class IocpBase
{
public:
	IocpBase();
	~IocpBase();

	HANDLE		GetHandle() { return _iocpHandle; }

	bool		Register(std::shared_ptr<IocpObject> iocpObject);
	bool		Dispatch(unsigned __int32 timeoutMs = INFINITE);

private:
	HANDLE		_iocpHandle;
};

