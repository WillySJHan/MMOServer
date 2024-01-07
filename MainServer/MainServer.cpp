#include "stdafx.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "ClientSession.h"
#include "ClientSessionManager.h"
#include "BufferWriter.h"
#include "ClientProtocolManager.h"
//#include <tchar.h>
#include "Proto.pb.h"
#include "Room.h"
#include "RoomManager.h"
#include "Task.h"
#include "ConfigManager.h"
#include "DataManager.h"

enum
{
	WORKER_TICK = 64
};

void DoWorkerTask(std::shared_ptr<ServerService>& service)
{
	while (true)
	{
		LEndTickCount = ::GetTickCount64() + WORKER_TICK;

		service->GetIocpBase()->Dispatch(10);

		ThreadManager::PushReservedTasks();

		ThreadManager::WorkGlobalQueueTask();
	}
}

int main()
{
	ConfigManager::LoadConfig(); 
	DataManager::LoadData();

	RoomManager::Instance()->Add(1);


	ClientProtocolManager::Init();

	std::shared_ptr<ServerService> service = std::make_shared<ServerService>(
		SocketAddress(L"127.0.0.2", 9797),
		std::make_shared<IocpBase>(),
		std::make_shared<ClientSession>, 
		110);


	service->Start();

	for (__int32 i = 0; i < 20; i++)
	{
		GThreadManager->Launch([&service]()
			{
				while (true)
				{
					DoWorkerTask(service);
				}
			});
	}
	std::cout << "Listening..." << std::endl;

	// Main Thread
	DoWorkerTask(service);
	
	GThreadManager->Join();
	
}
