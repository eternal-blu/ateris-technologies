#include "Events.h"
#include "../CritHacks/CritHacks.h"
#include "../Auto/AutoVaccinator/AutoVacc.h"
#include "../Visuals/Visuals.h"
#include "../Confidence/Confidence.h"
#include "../Misc/Misc.h"
#include "../LagCompensation/LagComp.h"

void CEvents::Setup( const std::deque<const char*>& aEvents )
{
	if ( aEvents.empty( ) )
	{
		return;
	}

	for ( auto event : aEvents )
	{
		gEventManager->AddListener( this, event, false );
	}
}

void CEvents::Destroy( )
{
	gEventManager->RemoveListener( this );
}

void CEvents::FireGameEvent( IGameEvent* Event )
{
	if ( !Event )
		return;

	const auto PlayerHurt = FNV1A::HashConst( "player_hurt" );
	const auto PlayerDeath = FNV1A::HashConst( "player_death" );
	const auto RoundStart = FNV1A::HashConst( "teamplay_round_start" );
	const auto GameNewMap = FNV1A::HashConst( "game_newmap" );
	const auto GameRoundActive = FNV1A::HashConst( "teamplay_round_active" );
	const auto GameRoundRestart = FNV1A::HashConst( "teamplay_restart_round" );
	const auto EventHash = FNV1A::Hash( Event->GetName( ) );

	if ( EventHash == PlayerHurt )
	{
		//CritHack->OnEvent( Event );
		Visuals->HitMarkerEvent( Event );
		LagComp->ResolverHurtEvent(Event);
	}
	else if ( EventHash == RoundStart || EventHash == GameNewMap )
		CritHack->Reset( );
	else if ( EventHash == PlayerDeath )
	{
		Misc->OnEvent( Event );
		gWD->WD_Events( Event );
		Misc->KillSay( Event );
	}
}