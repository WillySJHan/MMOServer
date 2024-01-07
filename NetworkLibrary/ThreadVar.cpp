#include "pch.h"
#include "ThreadVar.h"

thread_local unsigned __int32 LThreadId = 0;
thread_local unsigned __int64				LEndTickCount = 0;
thread_local TaskQueue* LCurrentTaskQueue = nullptr;