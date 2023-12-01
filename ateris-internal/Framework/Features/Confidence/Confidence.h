#pragma once
#include "../../SDK/TF2/TF2.h"

struct CPlayerStats
{
	unsigned int CriticalKills = 0u;
	unsigned int NormalKills = 0u;
	unsigned int CritKillsOnLocal = 0u;
	unsigned int NormalKillsOnLocal = 0u;
	unsigned int MeleeKills = 0u;
	unsigned int CritMeleeKills = 0u;

	unsigned int Deaths = 0u;
	float KDRatio = 0;

	CPlayerStats( unsigned int CriticalKills, 
		unsigned int NormalKills, 
		unsigned int Deaths, 
		float KDRatio, 
		unsigned int NormalKillsOnLocal, 
		unsigned int CritKillsOnLocal,
		unsigned int MeleeKills)
		: CriticalKills( CriticalKills ), 
		NormalKills( NormalKills ), 
		Deaths( Deaths ), 
		KDRatio( KDRatio ), 
		NormalKillsOnLocal( NormalKillsOnLocal ), 
		CritKillsOnLocal( CritKillsOnLocal ), MeleeKills( MeleeKills )
	{
	}

	CPlayerStats( ) = default;
};

struct CWatchedPlayer
{
	// Update Vars
	bool HasBeenChecked = false;
	bool HasBeenCheckedList = false;
	float LastCheckTime = 0.f;

	// Player information
	PlayerInfo_t Info {};
	int EntityIndex = 0;
	CPlayerStats* PlayerStats {};

	CWatchedPlayer( bool HasBeenChecked,
		float LastCheckTime,
		const PlayerInfo_t& Info,
		int EntityIndex,
		CPlayerStats* PlayerStats,
		bool HasBeenCheckedList )
		: HasBeenChecked( HasBeenChecked ),
		LastCheckTime( LastCheckTime ),
		Info( Info ),
		EntityIndex( EntityIndex ),
		PlayerStats( PlayerStats ),
		HasBeenCheckedList( HasBeenCheckedList )
	{
	}

	CWatchedPlayer( ) = default;
};

class CWatchdog
{
public:
	void WD_StartData();
	void WD_EndData();
	void WD_Events( IGameEvent* Event );
	void WD_Calc( CWatchedPlayer* );

	struct CPaster
	{
		std::string Name = {};
		std::string Group = {};

		CPaster( const std::string& Name, const std::string& Group )
			: Name( Name ), Group( Group )
		{
		}

		CPaster( ) = default;
	};

private:
	void SendInfo( const char* WhatHappen, const char* OnWho );
	void SendInfo( const char* WhatHappen, const char* OnWho, int data );
	void SendInfo( const char* WhatHappen, const char* OnWho, float data, const char* format = "%f" );
	void SendInfo( const char* OnWho, const char* Name, const char* Group );
	std::pair<bool, CPaster> WarningPasterDetected( int FriendsID );
};

inline CWatchdog* gWD = new CWatchdog;