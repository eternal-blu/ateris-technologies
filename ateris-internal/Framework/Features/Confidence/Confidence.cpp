#include "Confidence.h"
#include "../../../Globals.h"
#include "../../Utils/DrawTools/DrawTools.h"
#include "..\Menu\Menu.h"

std::unordered_map<int, CWatchedPlayer*> Players;

bool IsInList( int friends_id )
{
	if ( friends_id <= 0 )
		return false;

	static bool sorted = false;
	if ( !sorted )
	{
		std::sort( TF2->GlobalPlayerList.begin( ), TF2->GlobalPlayerList.end( ) );
		sorted = true;
	}

	return std::binary_search( TF2->GlobalPlayerList.begin( ), TF2->GlobalPlayerList.end( ), friends_id );
}

void CWatchdog::WD_StartData()
{
	for ( auto& Player : gPlayers )
	{
		if ( Player == gLocalPlayer || 
			gPlayerResource->GetTeam(Player->GetIndex()) == gLocalPlayer->TeamNum( ) )
			continue;

		PlayerInfo_t info;
		gEngineClient->GetPlayerInfo( Player->GetIndex( ), &info );

		if ( info.fakeplayer )
		{
			gPriorities[ info.friendsID ].Priority = 0;
			continue;
		}

		if ( info.friendsID <= 0 )
			continue;

		auto Paster = WarningPasterDetected( info.friendsID );
		if ( Paster.first )
		{
			for ( int i = 0; i < 2; i++ )
				SendInfo( info.name, Paster.second.Name.c_str( ), Paster.second.Group.c_str( ) );

			gPriorities[ info.friendsID ].Priority = 10;
			gPriorities[ info.friendsID ].CustomFlag = Paster.second.Name.c_str();
			continue;
		}

		// Already marked, leave him alone
		if ( gPriorities[ info.friendsID ].Priority > 0 )
			continue;

		if ( Players.find( info.friendsID ) == Players.end( ) )
		{
			// Not found, create new.
			Players[ info.friendsID ] = new CWatchedPlayer( false, 0.f, info, Player->GetIndex( ), new CPlayerStats(), false );
			continue;
		}

		// https://youtu.be/DGVEvfZRYGw
		CWatchedPlayer* Instance = Players[ info.friendsID ];
		
		if ( Instance->HasBeenChecked && Instance->LastCheckTime + 2.5f < gEngineClient->Time( ) )
			Instance->HasBeenChecked = false;

		if ( !Instance->HasBeenCheckedList )
		{
			if ( Player->IsSteamFriend( ) )
			{
				// team set him as friend omgomgomg
				//3 is friend idot, 4 is important
				// om real
				gPriorities[ Instance->Info.friendsID ].Priority = 3; // HACK
				Menu->SavePlayer( Instance->Info.friendsID, gPriorities[ Instance->Info.friendsID ] );
				Instance->HasBeenCheckedList = true;
				Instance->HasBeenChecked = true;
				continue;
			}

			if ( IsInList( info.friendsID ) )
			{
				// todo: check if theres even stuff in the list
				gPriorities[ Instance->Info.friendsID ].Priority = 2; // HACK
				SendInfo( "Detection through global playerlist", Instance->Info.name );
				Menu->SavePlayer( Instance->Info.friendsID, gPriorities[ Instance->Info.friendsID ] );
				Instance->HasBeenCheckedList = true;
				Instance->HasBeenChecked = true;
			}
		}

		if ( !Instance->HasBeenChecked )
		{
			WD_Calc( Instance );

			Instance->HasBeenChecked = true;
			Instance->LastCheckTime = gEngineClient->Time( );
		}
	}
}

void CWatchdog::WD_EndData()
{
	// clean this shit up
	if ( !Players.empty( ) )
		Players.clear( );
}

void CWatchdog::WD_Events( IGameEvent* Event )
{
	const auto Attacked = gEntityList->GetClientEntity( gEngineClient->GetPlayerForUserID( Event->GetInt( "userid" ) ) );
	const auto Attacker = gEntityList->GetClientEntity( Event->GetInt( "inflictor_entindex" ) );
	if ( !Event || !gLocalPlayer || !gLocalWeapon || !Attacked || !Attacker || Attacked == Attacker )
		return;

	if ( !Attacker->IsPlayer( ) )
		return;

	//  (0: None, 1: Mini, 2: Full) 
	auto CritType = Event->GetInt( "crit_type" );

	PlayerInfo_t AttackerInfo {};
	gEngineClient->GetPlayerInfo( Attacker->GetIndex( ), &AttackerInfo );

	if ( CritType == 2 )
		if ( Attacker->As< CBasePlayer >( )->IsCritBoosted( ) )
			return;

	if ( Attacker->As<CBasePlayer>( )->ActiveWeapon( ) && Attacker->As<CBasePlayer>( )->ActiveWeapon( )->Slot( ) == 2 )
	{
		if ( Players.find( AttackerInfo.friendsID ) != Players.end( ) )
		{
			if ( CritType == 2 )
				Players[ AttackerInfo.friendsID ]->PlayerStats->CritMeleeKills++;
			else
				Players[ AttackerInfo.friendsID ]->PlayerStats->MeleeKills++;
		}
		return;
	}

	if ( Attacked == gLocalPlayer )
	{
		if ( CritType == 2 && !Attacker->As<CBasePlayer>( )->IsCritBoosted( ) )
		{
			if ( Players.find( AttackerInfo.friendsID ) != Players.end( ) )
			{
				Players[ AttackerInfo.friendsID ]->PlayerStats->CritKillsOnLocal++;
			}
		}
		else
		{
			if ( Players.find( AttackerInfo.friendsID ) != Players.end( ) )
			{
				Players[ AttackerInfo.friendsID ]->PlayerStats->NormalKillsOnLocal++;
			}
		}
	}
	else
	{
		if ( CritType == 2 )
		{
			if ( Players.find( AttackerInfo.friendsID ) != Players.end( ) )
			{
				Players[ AttackerInfo.friendsID ]->PlayerStats->CriticalKills++;
			}
		}
	}
}

void CWatchdog::SendInfo( const char* WhatHappen, const char* OnWho )
{
	std::string str = White + "|" + Logo + "Ateris" + White + "| - |" + Feature + "WD40" + White + "| Marked " + Player + OnWho + White + " due to " + Reason + WhatHappen + White + ".";
	ChatPrintf::OriginalFunction( gClientMode->m_pChatElement, 0, 0, str.c_str( ) );
}

void CWatchdog::SendInfo( const char* WhatHappen, const char* OnWho, int data )
{
	std::string str = White + "|" + Logo + "Ateris" + White + "| - |" + Feature + "WD40" + White + "| Marked " + Player + OnWho + White + " due to " + Reason + WhatHappen + "%i" + White + ".";
	ChatPrintf::OriginalFunction( gClientMode->m_pChatElement, 0, 0, str.c_str( ), data );
}

void CWatchdog::SendInfo( const char* WhatHappen, const char* OnWho, float data, const char* format )
{
	std::string str = White + "|" + Logo + "Ateris" + White + "| - |" + Feature + "WD40" + White + "| Marked " + Player + OnWho + White + " due to " + Reason + WhatHappen + format + White + ".";
	ChatPrintf::OriginalFunction( gClientMode->m_pChatElement, 0, 0, str.c_str( ), data );
}

void CWatchdog::SendInfo( const char* OnWho, const char* Name, const char* Group )
{
	std::string str = White + "|" + Logo + "Ateris" + White + "| - |" + 
		Feature + "WD40" + White + "| Marked " + Player + OnWho + White + " because he is known as " + 
		Reason + Name + White + " and he belongs to a group known as " + Reason + Group + White + ".";

	ChatPrintf::OriginalFunction( gClientMode->m_pChatElement, 0, 0, str.c_str( ) );
}

void CWatchdog::WD_Calc( CWatchedPlayer* Instance )
{
	if ( !gPlayerResource )
		return;

	CBasePlayer* Player = ( CBasePlayer* ) ( gEntityList->GetClientEntity( Instance->EntityIndex ) );
	if ( !Player )
		return;

	if ( !Player || Player->IsDormant( ) )
	{
		return;
	}

	PlayerInfo_t Info;
	gEngineClient->GetPlayerInfo( Player->GetIndex( ), &Info );

	if ( Info.fakeplayer )
	{
		gPriorities[ Instance->Info.friendsID ].Priority = 0;
		return;
	}

	if ( Player->IsSteamFriend( ) )
	{
		gPriorities[ Instance->Info.friendsID ].Priority = 3;
		return;
	}

	if ( Player->EyeAngles( ).x >= 90.f || Player->EyeAngles( ).x <= -90.f )
	{
		gPriorities[ Instance->Info.friendsID ].Priority = 2; // HACK
		SendInfo( "Invalid pitch angle", Instance->Info.name );
		Menu->SavePlayer( Instance->Info.friendsID, gPriorities[ Instance->Info.friendsID ] );
		return;
	}

	if ( gPlayerResource->GetPing( Player->GetIndex( ) ) > 500 )
	{
		gPriorities[ Instance->Info.friendsID ].Priority = 1;
		SendInfo( "Extremely high latency: ", Instance->Info.name, gPlayerResource->GetPing( Player->GetIndex( ) ) );
		Menu->SavePlayer( Instance->Info.friendsID, gPriorities[ Instance->Info.friendsID ] );
		return;
	}

	auto& Stats = Instance->PlayerStats;
	Stats->CriticalKills = Stats->CriticalKills - Stats->CritMeleeKills;
	Stats->NormalKills = gPlayerResource->GetKills( Instance->EntityIndex ) - 
		Stats->CriticalKills - Stats->MeleeKills;

	Stats->Deaths = gPlayerResource->GetDeaths( Instance->EntityIndex );
	Stats->KDRatio = gPlayerResource->GetKills( Instance->EntityIndex ) /
		( Stats->Deaths == 0 ? 1 : Stats->Deaths ); // :troll:

	// eg 2 normal kills - 3 crit kills
	int Delta = Stats->NormalKills - Stats->CriticalKills;
	if ( Delta <= -2 )
	{
		if ( Player->Class( ) == CLASS_SNIPER )
		{
			if ( Delta <= -6 )
			{
				gPriorities[ Instance->Info.friendsID ].Priority = 1;
				SendInfo( "Overall crit/normal kill ratio too high (Sniper)", Instance->Info.name );
				Menu->SavePlayer( Instance->Info.friendsID, gPriorities[ Instance->Info.friendsID ] );
			}
		}
		else
		{
			gPriorities[ Instance->Info.friendsID ].Priority = 1;
			SendInfo( "Overall crit/normal kill ratio too high", Instance->Info.name );
			Menu->SavePlayer( Instance->Info.friendsID, gPriorities[ Instance->Info.friendsID ] );
		}
		return;
	}

	int LocalDelta = Stats->NormalKillsOnLocal - Stats->CritKillsOnLocal;
	if ( LocalDelta <= -2 )
	{
		gPriorities[ Instance->Info.friendsID ].Priority = 1;
		SendInfo( "Overall crit/normal kill ratio on localplayer too high", Instance->Info.name );
		Menu->SavePlayer( Instance->Info.friendsID, gPriorities[ Instance->Info.friendsID ] );
		return;
	}

	if ( Stats->KDRatio > 6.f )
	{
		if ( Stats->Deaths > 0 && Stats->KDRatio > 8.f )
		{
			SendInfo( "K/D ratio too high (Case #2)", Instance->Info.name );
			gPriorities[ Instance->Info.friendsID ].Priority = 1; // Among Us
			Menu->SavePlayer( Instance->Info.friendsID, gPriorities[ Instance->Info.friendsID ] );
			return;
		}
		else if ( Stats->Deaths <= 0 )
		{
			SendInfo( "K/D ratio too high (Case #1)", Instance->Info.name );
			gPriorities[ Instance->Info.friendsID ].Priority = 1; // Among Us
			Menu->SavePlayer( Instance->Info.friendsID, gPriorities[ Instance->Info.friendsID ] );
			return;
		}
	}
}

std::pair<bool, CWatchdog::CPaster> CWatchdog::WarningPasterDetected( int FriendsID )
{
	switch ( FriendsID )
	{
	case 934709457:
		return { true, CPaster( "Wallet Stealer", "Pentagon Pasters" ) };
		break;
	}

	return { false, CPaster( ) };
}