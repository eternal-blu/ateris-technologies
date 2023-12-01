#pragma once
#include "EngineTrace.h"

class ISpatialLeafEnumerator
{
public:
	virtual bool EnumerateLeaf( int leaf, int context ) = 0;
};

struct PlayerInfo_t
{
	// scoreboard information
	char            name[ 32 ];
	// local server user ID, unique while server is running
	int				userID;
	// global unique player identifer
	char			guid[ 32 + 1 ];
	// friends identification number
	uint32_t		friendsID;
	// friends name
	char			friendsName[ 32 ];
	// true, if player is a bot controlled by game.dll
	bool			fakeplayer;
	// true if player is the HLTV proxy
	bool			ishltv;
	// custom files CRC for this player
	unsigned long	customFiles[ 4 ];
	// this counter increases each time the server downloaded a new file
	unsigned char	filesDownloaded;
};

class ISpatialQuery
{
public:
	virtual int LeafCount( ) const = 0;
	virtual bool EnumerateLeavesAtPoint( Vector const& pt, ISpatialLeafEnumerator* pEnum, int context ) = 0;
	virtual bool EnumerateLeavesInBox( Vector const& mins, Vector const& maxs, ISpatialLeafEnumerator* pEnum, int context ) = 0;
	virtual bool EnumerateLeavesInSphere( Vector const& center, float radius, ISpatialLeafEnumerator* pEnum, int context ) = 0;
	virtual bool EnumerateLeavesAlongRay( Ray_t const& ray, ISpatialLeafEnumerator* pEnum, int context ) = 0;
};

class CEngineClient
{
public:
	void GetScreenSize( int& w, int& h )
	{
		return Memory->GetVFunc< void( __thiscall* )( void*, int&, int& ) >( this, 5u )( this, w, h );
	}

	void ServerCmd( const char* Cmd, bool Reliable )
	{
		return Memory->GetVFunc< void( __thiscall* )( void*, const char*, bool ) >( this, 6u )( this, Cmd, Reliable );
	}

	bool GetPlayerInfo( int EntityIndex, PlayerInfo_t* PlayerInfo )
	{
		return Memory->GetVFunc< bool( __thiscall* )( void*, int, PlayerInfo_t* ) >( this, 8u )( this, EntityIndex, PlayerInfo );
	}

	int GetPlayerForUserID( int userID )
	{
		return Memory->GetVFunc< bool( __thiscall* )( void*, int ) >( this, 9u )( this, userID );
	}

	bool Con_IsVisible( )
	{
		return Memory->GetVFunc< bool( __thiscall* )( void* ) >( this, 11u )( this );
	}

	int GetLocalPlayer( )
	{
		return Memory->GetVFunc< int( __thiscall* )( void* ) >( this, 12u )( this );
	}

	float Time( )
	{
		return Memory->GetVFunc< float( __thiscall* )( void* ) >( this, 14u )( this );
	}

	void GetViewAngles( Vector& va )
	{
		return Memory->GetVFunc< void( __thiscall* )( void*, Vector& va ) >( this, 19u )( this, va );
	}

	void SetViewAngles( Vector& va )
	{
		return Memory->GetVFunc< void( __thiscall* )( void*, Vector & va ) >( this, 20u )( this, va );
	}

	int GetMaxClients( )
	{
		return Memory->GetVFunc< int( __thiscall* )( void* ) >( this, 21u )( this );
	}

	bool IsInGame( )
	{
		return Memory->GetVFunc< bool( __thiscall* )( void* ) >( this, 26u )( this );
	}

	bool IsConnected( )
	{
		return Memory->GetVFunc< bool( __thiscall* )( void* ) >( this, 27u )( this );
	}

	bool IsDrawingLoadingImage( )
	{
		return Memory->GetVFunc< bool( __thiscall* )( void* ) >( this, 28u )( this );
	}

	const Matrix3x4& WorldToScreenMatrix( )
	{
		return Memory->GetVFunc< const Matrix3x4 & ( __thiscall* )( void* ) >( this, 36u )( this );
	}

	ISpatialQuery* GetBSPTreeQuery( )
	{
		return Memory->GetVFunc< ISpatialQuery * ( __thiscall* )( void* ) >( this, 42u )( this );
	}

	char const* GetLevelName( )
	{
		return Memory->GetVFunc< char const* ( __thiscall* )( void* ) >( this, 51u )( this );
	}

	void FireEvents( )
	{
		return Memory->GetVFunc< void( __thiscall* )( void* ) >( this, 56u )( this );
	}

	CNetChannel* GetNetChannelInfo( )
	{
		return Memory->GetVFunc< CNetChannel * ( __thiscall* )( void* ) >( this, 72u )( this );
	}

	bool IsTakingScreenshot( )
	{
		return Memory->GetVFunc< bool( __thiscall* )( void* ) >( this, 85u )( this );
	}

	bool IsHLTV( )
	{
		return Memory->GetVFunc< bool( __thiscall* )( void* ) >( this, 86u )( this );
	}

	void ClientCmd_Unrestricted( const char* szCommandString )
	{
		return Memory->GetVFunc< void( __thiscall* )( void*, const char* ) >( this, 106u )( this, szCommandString );
	}

	void ServerCmdKeyValues( void* pKV )
	{
		return Memory->GetVFunc< void( __thiscall* )( void*, void* ) >( this, 128u )( this, pKV );
	}

	Vector GetViewAngles( )
	{
		Vector out;
		GetViewAngles( out );
		return out;
	}
};