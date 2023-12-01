#pragma once
#include "../Utils/MinHook/MinHook.h"
#include "../Framework.h"
#include "../Utils/Memory/Memory.h"
#include "../SDK/Interfaces/Interfaces.h"

#define HOOK_HEADER( name, fn, type, callconvo, ... ) namespace name\
{\
	using FN = fn;\
	inline FN Target;\
	__declspec( dllexport ) inline FN OriginalFunction;\
	__declspec( dllexport ) type callconvo Function(__VA_ARGS__);\
	void Init();\
	void Uninit();\
}

#define HOOK_VTABLE( name, table, index, type, callconvo, ... )\
void name::Init()\
{\
	Target = Memory->GetVFunc<FN>( table, index );\
	MH_CreateHook( Target, &Function, reinterpret_cast<LPVOID*>( &OriginalFunction ));\
	std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );\
}\
void name::Uninit()\
{\
	MH_RemoveHook(Target);\
	std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );\
}\
__declspec( dllexport ) type callconvo name::Function(__VA_ARGS__)

#define HOOK_POINTER(name, dll, pattern, type, callconvo, ...)\
void name::Init()\
{\
	Target = Memory->FindSignature<FN>(dll, pattern);\
	MH_CreateHook( Target, &Function, reinterpret_cast<LPVOID*>( &OriginalFunction ));\
	std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );\
}\
void name::Uninit()\
{\
	MH_RemoveHook(Target);\
	std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );\
}\
__declspec( dllexport ) type callconvo name::Function(__VA_ARGS__)

#define HOOK_NETVAR( classname, netvar )\
namespace classname##__##netvar{\
	RecvProp* Prop;\
	RecvVarProxyFn Original;\
	void Init();\
	void Restore();\
	void Function(const CRecvProxyData* data, void* ECX, void* out);\
}\
void classname##__##netvar::Init() {\
	Prop = NetVars->FindNetVarProp(#classname, #netvar);\
	Original = Prop->ProxyFn;\
	Prop->ProxyFn = Function;\
}\
void classname##__##netvar::Restore(){\
	Prop->ProxyFn = Original;\
}\
void classname##__##netvar::Function(const CRecvProxyData* data, void* ECX, void* out)