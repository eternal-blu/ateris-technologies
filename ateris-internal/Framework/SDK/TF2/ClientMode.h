#pragma once
#include "../../Utils/Memory/Memory.h"

class CBaseHudChat
{
public:
	void ChatPrintf( int Index, int Filter, const char* Fmt, ... )
	{
		return Memory->GetVFunc< void( __cdecl* )( CBaseHudChat*, int, int, const char*, ... ) >( this, 19u )( this, Index, Filter, Fmt );
	}

	void StartMessageMode( int MessageModeType )
	{
		return Memory->GetVFunc< void( __thiscall* )( CBaseHudChat*, int ) >( this, 20u )( this, MessageModeType );
	}
};

class IClientModeShared
{
private:
	char m_szPad[ 28 ];
public:
	CBaseHudChat* m_pChatElement;
	bool IsChatPanelOutOfFocus( )
	{
		void* CHudChat = Memory->GetVFunc< void* ( __thiscall* )( void* ) >( this, 19 )( this );
		return ( CHudChat && !*( float* )( ( DWORD )CHudChat + 0xFC ) );
	}
};