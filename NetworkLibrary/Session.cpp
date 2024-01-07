#include "pch.h"
#include "Session.h"
#include "SocketManager.h"
#include "Service.h"



Session::Session() : _recvBuffer(BUFFER_SIZE)
{
	_socket = SocketManager::CreateSocket();
}

Session::~Session()
{
	SocketManager::Close(_socket);
	std::cout << "~ Session" << std::endl;
}

void Session::Send(std::shared_ptr<SendBuffer> sendBuffer)
{
	if (IsConnected() == false)
		return;



	bool registerSend = false;

	
	{
		std::lock_guard<std::recursive_mutex> lock(_rMutex);

		_sendQueue.push(sendBuffer);



		if (_sendRegistered.exchange(true) == false)
			registerSend = true;
	}

	if (registerSend)
		RegisterSend();
}

bool Session::Connect()
{
	return RegisterConnect();
}

void Session::Disconnect(const WCHAR* cause)
{
	if (_connected.exchange(false) == false)
		return;


	std::wcout << "Disconnect : " << cause << std::endl;

	RegisterDisconnect();
}

HANDLE Session::GetHandle()
{
	return reinterpret_cast<HANDLE>(_socket);
}

void Session::Dispatch(IocpEvent* iocpEvent, __int32 numOfBytes)
{
	switch (iocpEvent->eventType)
	{
	case EventType::Connect:
		ProcessConnect();
		break;
	case EventType::Disconnect:
		ProcessDisconnect();
		break;
	case EventType::Recv:
		ProcessRecv(numOfBytes);
		break;
	case EventType::Send:
		ProcessSend(numOfBytes);
		break;
	default:
		break;
	}
}

bool Session::RegisterConnect()
{
	if (IsConnected())
		return false;

	if (GetService()->GetServiceType() != ServiceType::Client)
		return false;

	if (SocketManager::SetReuseAddress(_socket, true) == false)
		return false;

	if (SocketManager::BindAnyAddress(_socket, 0) == false)
		return false;

	_connectEvent.Init();
	_connectEvent.owner = shared_from_this(); 

	DWORD numOfBytes = 0;
	SOCKADDR_IN sockAddr = GetService()->GetSocketAddr().GetSocketAddress();
	if (false == SocketManager::ConnectEx(_socket, reinterpret_cast<SOCKADDR*>(&sockAddr), sizeof(sockAddr), nullptr, 0, &numOfBytes, &_connectEvent))
	{
		__int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_connectEvent.owner = nullptr; 
			return false;
		}
	}

	return true;
}

bool Session::RegisterDisconnect()
{
	_disconnectEvent.Init();
	_disconnectEvent.owner = shared_from_this(); 

	if (false == SocketManager::DisconnectEx(_socket, &_disconnectEvent, TF_REUSE_SOCKET, 0))
	{
		__int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			_disconnectEvent.owner = nullptr; 
			return false;
		}
	}

	return true;
}

void Session::RegisterRecv()
{
	if (IsConnected() == false)
		return;

	_recvEvent.Init();
	_recvEvent.owner = shared_from_this(); 

	WSABUF wsaBuf;
	wsaBuf.buf = reinterpret_cast<char*>(_recvBuffer.WritePos());
	wsaBuf.len = _recvBuffer.FreeSize();

	DWORD numOfBytes = 0;
	DWORD flags = 0;
	if (SOCKET_ERROR == ::WSARecv(_socket, &wsaBuf, 1, &numOfBytes, &flags, &_recvEvent, nullptr))
	{
		__int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_recvEvent.owner = nullptr; 
		}
	}
}

void Session::RegisterSend()
{
	if (IsConnected() == false)
		return;

	_sendEvent.Init();
	_sendEvent.owner = shared_from_this(); 


	{
		std::lock_guard<std::recursive_mutex> lock(_rMutex);

		__int32 writeSize = 0;
		while (_sendQueue.empty() == false)
		{
			std::shared_ptr<SendBuffer> sendBuffer = _sendQueue.front();

			writeSize += sendBuffer->WriteSize();
			// TODO : 예외 체크

			_sendQueue.pop();
			_sendEvent.sendBuffers.push_back(sendBuffer);
		}
	}

	
	std::vector<WSABUF> wsaBufs;
	wsaBufs.reserve(_sendEvent.sendBuffers.size());
	for (std::shared_ptr<SendBuffer> sendBuffer : _sendEvent.sendBuffers)
	{
		WSABUF wsaBuf;
		wsaBuf.buf = reinterpret_cast<char*>(sendBuffer->Buffer());
		wsaBuf.len = static_cast<LONG>(sendBuffer->WriteSize());
		wsaBufs.push_back(wsaBuf);
	}

	DWORD numOfBytes = 0;
	if (SOCKET_ERROR == ::WSASend(_socket, wsaBufs.data(), static_cast<DWORD>(wsaBufs.size()), OUT & numOfBytes, 0, &_sendEvent, nullptr))
	{
		__int32 errorCode = ::WSAGetLastError();
		if (errorCode != WSA_IO_PENDING)
		{
			HandleError(errorCode);
			_sendEvent.owner = nullptr; 
			_sendEvent.sendBuffers.clear(); 
			_sendRegistered.store(false);
		}
	}
}

void Session::ProcessConnect()
{
	_connectEvent.owner = nullptr;

	_connected.store(true);


	GetService()->AddSession(GetSessionPtr());


	OnConnected();


	RegisterRecv();
}

void Session::ProcessDisconnect()
{
	_disconnectEvent.owner = nullptr; 

	OnDisconnected(); 
	GetService()->ReleaseSession(GetSessionPtr());
}

void Session::ProcessRecv(__int32 numOfBytes)
{
	_recvEvent.owner = nullptr; 

	if (numOfBytes == 0)
	{
		Disconnect(L"Recv 0");
		return;
	}

	if (_recvBuffer.OnWrite(numOfBytes) == false)
	{
		Disconnect(L"OnWrite Overflow");
		return;
	}

	__int32 dataSize = _recvBuffer.DataSize();
	__int32 processLen = OnRecv(_recvBuffer.ReadPos(), dataSize); 
	if (processLen < 0 || dataSize < processLen || _recvBuffer.OnRead(processLen) == false)
	{
		Disconnect(L"OnRead Overflow");
		return;
	}


	_recvBuffer.Clean();

	
	RegisterRecv();
}

void Session::ProcessSend(__int32 numOfBytes)
{
	_sendEvent.owner = nullptr; 
	_sendEvent.sendBuffers.clear(); 

	if (numOfBytes == 0)
	{
		Disconnect(L"Send 0");
		return;
	}


	OnSend(numOfBytes);

	std::lock_guard<std::recursive_mutex> lock(_rMutex);
	if (_sendQueue.empty())
		_sendRegistered.store(false);
	else
		RegisterSend();
}

void Session::HandleError(__int32 errorCode)
{
	switch (errorCode)
	{
	case WSAECONNRESET:
	case WSAECONNABORTED:
		Disconnect(L"HandleError");
		break;
	default:
		std::cout << "Handle Error : " << errorCode << std::endl;
		break;
	}
}



PacketSession::PacketSession()
{
}

PacketSession::~PacketSession()
{
}


__int32 PacketSession::OnRecv(unsigned char* buffer, __int32 len)
{
	__int32 processLen = 0;

	while (true)
	{
		__int32 dataSize = len - processLen;
		
		if (dataSize < sizeof(PacketHeader))
			break;

		PacketHeader header = *(reinterpret_cast<PacketHeader*>(&buffer[processLen]));
		
		if (dataSize < header.size)
			break;

		
		OnRecvPacket(&buffer[processLen], header.size);

		processLen += header.size;
	}

	return processLen;
}
