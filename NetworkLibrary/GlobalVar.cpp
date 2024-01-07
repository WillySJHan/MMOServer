#include "pch.h"
#include "GlobalVar.h"
#include "ThreadManager.h"
#include "SocketManager.h"
#include "TaskQueueManager.h"
#include "TaskTimer.h"


ThreadManager* GThreadManager = nullptr;
TaskQueueManager* GTaskQueueManager = nullptr;
TaskTimer* GTaskTimer = nullptr;

class GlobalVar
{
public:
	GlobalVar()
	{
		GThreadManager = new ThreadManager();
		GTaskQueueManager = new TaskQueueManager();
		GTaskTimer = new TaskTimer();
		SocketManager::Init();
	}

	~GlobalVar()
	{
		delete GThreadManager;
		delete GTaskQueueManager;
		delete GTaskTimer;
		SocketManager::Clear();
	}
} GGlobalVar;

