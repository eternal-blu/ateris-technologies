#pragma once
#include "../Memory/Memory.h"

class RecvTable;
class RecvProp;
class CNetVars
{
public:
	int GetOffset(RecvTable* Table, const char* Name);
	int GetNetVar(const char* Class, const char* NetVar);

	RecvProp* GetNetVarProp( RecvTable* table, const char* name );
	RecvProp* FindNetVarProp( const char* classname, const char* netvar );
};

inline CNetVars* NetVars = new CNetVars;

#define NETVAR(_name, type, table, name) inline type &_name() \
{ \
	static int offset = NetVars->GetNetVar(table, name); \
	return *reinterpret_cast<type *>(reinterpret_cast<DWORD>(this) + offset); \
}

#define OFFSET_FROM(_name, type, table, name, addition) inline type &_name() \
{ \
	static int offset = NetVars->GetNetVar(table, name); \
	return *reinterpret_cast<type *>(reinterpret_cast<DWORD>(this) + offset + addition); \
}

#define OFFSET_PURE(_name, type, offset) inline type &_name() \
{ \
	return *reinterpret_cast<type *>(reinterpret_cast<DWORD>(this) + offset); \
}