#include "stdafx.h"
#include "ThreadManager.h"
#include "Service.h"
#include "Session.h"
#include "ServerProtocolManager.h"
#include "Proto.pb.h"
#include "ServerSession.h"


int main()
{
	ServerProtocolManager::Init();

	std::this_thread::sleep_for(std::chrono::seconds(4));

	std::shared_ptr<ClientService> service = std::make_shared<ClientService>(
		SocketAddress(L"127.0.0.1", 9797),
		std::make_shared<IocpBase>(),
		std::make_shared<ServerSession>, 
		100);
	
	
	service->Start();

	for (__int32 i = 0; i < 2; i++)
	{
		GThreadManager->Launch([=]()
			{
				while (true)
				{
					service->GetIocpBase()->Dispatch();
				}
			});
	}


	GThreadManager->Join();
}