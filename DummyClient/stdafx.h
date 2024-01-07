#pragma once
//#ifndef PCH_H
//#define PCH_H

// add headers that you want to pre-compile here
#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers

#ifdef _DEBUG
#pragma comment(lib, "Debug\\NetworkLibrary.lib")
#pragma comment(lib, "Debug\\libprotobufd.lib")
#else
#pragma comment(lib, "Release\\NetworkLibrary.lib")
#pragma comment(lib, "Release\\libprotobuf.lib")
#endif

#include "NetworkHeader.h"

//#endif //PCH_H