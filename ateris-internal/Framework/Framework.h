#pragma once

//#define WIN32_LEAN_AND_MEAN             // Exclude rarely-used stuff from Windows headers
// Windows Header Files
#include <windows.h>
#include <thread>
#include <chrono>
#include <cstdint>
#include <array>
#include <algorithm>
#include <string>
#include <string_view>
#include <intrin.h>
#include <map>
#include <functional>
#include <sstream>
#include <fstream>
#include <winternl.h>
#include <ImageHlp.h>
#include <vector>
#include <list>
#include <mmsystem.h>
#pragma comment(lib, "ws2_32")
#pragma comment(lib, "winmm")
#pragma comment(lib, "imagehlp.lib")

struct StackFrame
{
    DWORD address;
    std::string name;
    std::string module;
    unsigned int line;
    std::string file;
};

class CFramework
{
public:
	void Attach( );
	void Detach( );
	void PrintOnInject( );
    std::vector<StackFrame> GetStackTrace( PCONTEXT context );
};

inline CFramework* Framework = new CFramework;