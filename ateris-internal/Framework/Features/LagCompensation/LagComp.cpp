#include "LagComp.h"
#include "../../../Globals.h"
#include "../Anti-Aimbot/Anti-Aimbot.h"
#include "../Misc/Misc.h"

static std::string Local({ '\x7', '0', '0', '9', '5', 'F', 'F' });
static std::string Hit({ '\x7', '8', '0', 'f', 'f', '8', '0' });
static std::string Miss({ '\x7', 'F', 'F', '3', '2', '3', '2' });

bool CLagComp::IsTickValid( const CLagCompRecord Rec )
{
	if ( !Rec.Valid )
		return false;

	float Correct = 0.f;
	// both flows are needed
	Correct += gEngineClient->GetNetChannelInfo( )->GetLatency( FLOW_INCOMING );
	Correct += gEngineClient->GetNetChannelInfo( )->GetLatency( FLOW_OUTGOING );
	Correct = std::clamp( Correct, 0.f, 1.f );

	// todo: actual server time
	// tickbase wont work here
	int ArrivalTick = TF2->ServerTick( );
	float Delta = Correct - ( ( TICKS_TO_TIME( ArrivalTick ) + TF2->LerpTime( ) ) - Rec.SimTime );
	return std::fabsf( Delta ) < ( 0.2f - gGlobals->interval_per_tick ); //2 lagcomp ticks
}

void CLagComp::Restore( CUserCmd* pCmd, CLagCompRecord& Record, CBasePlayer* Target )
{
	pCmd->tick_count = TIME_TO_TICKS( Record.SimTime + TF2->LerpTime( ) ) 
		+ ( Record.Choked > 11 && G->Doubletap.ToSend <= 0 ? Record.Choked : 0 );

	Target->Maxs( ) = Record.Maxs;
	Target->Mins( ) = Record.Mins;
	Target->EyeAngles( ) = Record.Angles;
	Target->Origin( ) = Record.Origin;
	Target->SimulationTime( ) = Record.SimTime;
	Target->AnimationTime( ) = Record.AnimTime;
	Target->OldSimulationTime( ) = Record.OldSimTime;
	Target->Velocity( ) = Record.Velocity;
	Target->Flags( ) = Record.Flags;

	const auto& AnimState = Target->AnimState( );
	if ( AnimState )
	{
		for ( int i = 0; i < GESTURE_SLOT_COUNT; i++ )
		{
			if ( !Record.GestureSlots.empty( ) )
			{
				const auto& GestureSlot = Record.GestureSlots[ i ];
				AnimState->m_aGestureSlots[ i ] = GestureSlot;

				if ( !Record.AnimationLayers.empty( ) )
				{
					const auto& AnimLayer = Record.AnimationLayers[ i ];
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_nSequence = AnimLayer.m_nSequence;
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_flPrevCycle = AnimLayer.m_flPrevCycle;
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_flWeight = AnimLayer.m_flWeight;
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_nOrder = AnimLayer.m_nOrder;
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_flPlaybackRate = AnimLayer.m_flPlaybackRate;
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_flCycle = AnimLayer.m_flCycle;
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_flLayerAnimtime = AnimLayer.m_flLayerAnimtime;
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_flLayerFadeOuttime = AnimLayer.m_flLayerFadeOuttime;
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_flBlendIn = AnimLayer.m_flBlendIn;
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_flBlendOut = AnimLayer.m_flBlendOut;
					AnimState->m_aGestureSlots[ i ].m_pAnimLayer->m_bClientBlend = AnimLayer.m_bClientBlend;
				}
			}
		}
	}
}

void CLagComp::Record( CBasePlayer* Player )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !gLocalWeapon )
	{
		if ( !Records->empty( ) )
			Records->clear( );

		return;
	}

	const auto Index = Player->GetIndex( );

	auto Network = gEngineClient->GetNetChannelInfo( );
	if ( !Network )
		return;

	// for (int i = 1; i < gGlobals->maxClients; i++)
	//{
	//	auto Player = gEntityList->GetClientEntity(i);
	//}
	if ( !Player->IsAlive( ) || Player->IsDormant( ) || !Player->IsEnemy( ) )
	{
		if ( !Records[ Index ].empty( ) )
			Records[ Index ].clear( );

		if ( Player->IsDormant( ) )
			Resolverer[ Index ].MissedShots = -1;

		return;
	}

	if ( !Records[ Index ].empty( ) &&
		Records[ Index ].front( ).SimTime >= Player->SimulationTime( ) )
		return;

	bool DoNotRecord = false;
	HandleResolving( Player );

	CLagCompRecord Record;
	Record.Player = Player;
	Record.Maxs = Player->Maxs( );
	Record.Mins = Player->Mins( );
	Record.Model = Player->GetModel( );
	Record.OldSimTime = Player->OldSimulationTime( );
	Record.SimTime = Player->SimulationTime( );
	Record.AnimTime = Player->AnimationTime( );
	Record.Origin = Player->Origin( );
	Record.Velocity = Player->Velocity( );
	Record.Flags = Player->Flags( );
	Record.WorldSpaceCenter = Player->WorldSpaceCenter( );
	Record.Angles = Player->EyeAngles( );

	const int Needed = std::clamp( ( int ) ceilf( 64.f / ( Player->Velocity( ).Length( ) * 
		gGlobals->interval_per_tick ) ), 0, 22 );
	const float Difference = Record.SimTime - Record.OldSimTime;
	const int Ticks = std::clamp( TIME_TO_TICKS( Difference ), 0, 22 );
	Record.Choked = Ticks;

	Record.Valid = true;

	if ( Ticks >= Needed && Player->Velocity().Length2D() > 125.f )
	{
		// do not continue
		Record.Valid = false;
	}

	Record.UpdateRecord( );
	Record.SetupBoner = Player->SetupBones( Record.Boner, 128, BONE_USED_BY_ANYTHING, -1.f );

	const auto& AnimState = Player->AnimState( );
	if ( AnimState )
	{
		for ( int i = 0; i < GESTURE_SLOT_COUNT; i++ )
		{
			const auto& GestureSlot = AnimState->m_aGestureSlots[ i ];
			Record.GestureSlots[ i ] = GestureSlot;

			const auto& AnimLayer = GestureSlot.m_pAnimLayer;
			if ( AnimLayer )
			{
				Record.AnimationLayers[ i ].m_nSequence = AnimLayer->m_nSequence;
				Record.AnimationLayers[ i ].m_flPrevCycle = AnimLayer->m_flPrevCycle;
				Record.AnimationLayers[ i ].m_flWeight = AnimLayer->m_flWeight;
				Record.AnimationLayers[ i ].m_nOrder = AnimLayer->m_nOrder;
				Record.AnimationLayers[ i ].m_flPlaybackRate = AnimLayer->m_flPlaybackRate;
				Record.AnimationLayers[ i ].m_flCycle = AnimLayer->m_flCycle;
				Record.AnimationLayers[ i ].m_flLayerAnimtime = AnimLayer->m_flLayerAnimtime;
				Record.AnimationLayers[ i ].m_flLayerFadeOuttime = AnimLayer->m_flLayerFadeOuttime;
				Record.AnimationLayers[ i ].m_flBlendIn = AnimLayer->m_flBlendIn;
				Record.AnimationLayers[ i ].m_flBlendOut = AnimLayer->m_flBlendOut;
				Record.AnimationLayers[ i ].m_bClientBlend = AnimLayer->m_bClientBlend;
			}
		}
	}

	Records[ Index ].push_front( Record );

	size_t CorrectWindowSize = static_cast< size_t >( TIME_TO_TICKS( ( 0.2f
		+ Network->GetLatency( FLOW_OUTGOING ) ) + Network->GetLatency( FLOW_INCOMING ) ) );

	CorrectWindowSize = std::clamp( CorrectWindowSize, 0u, 65u );
	while ( Records[ Index ].size( ) > CorrectWindowSize )
		Records[ Index ].pop_back( );
}

CLagComp::CLagCompRecord CLagComp::GetLastTick( int Player )
{
	if ( LagComp->Records[ Player ].empty( ) || LagComp->Records[ Player ].size( ) < 2 )
		return {};

	for ( size_t i = LagComp->Records[ Player ].size( ) - 1; i > 0; i-- )
	{
		LagComp->Records[ Player ][ i ].UpdateRecord( );

		if ( LagComp->IsTickValid( LagComp->Records[ Player ][ i ] ) )
			return LagComp->Records[ Player ][ i ];
	}

	return {};
}

CLagComp::CLagCompRecord CLagComp::GetClosestTick(int Player)
{
	// make sure u dont modify the records, otherwise the clearing gets fcked up
	std::deque<CLagCompRecord> OldRecords = GetValidTicks( Player );
	std::sort( OldRecords.begin(), OldRecords.end(), [](CLagComp::CLagCompRecord& t1, CLagComp::CLagCompRecord& t2) {
		t1.UpdateRecord( );
		t2.UpdateRecord( );

		return t1.DistanceToLocal < t2.DistanceToLocal;
	});

	if ( !OldRecords.empty( ) )
	{
		for ( size_t i = 0; i < OldRecords.size( ); i++ )
		{
			//first record always misses for some reason
			if ( i == 0 )
				continue;

			auto& RecordTemp = OldRecords[ i ];
			return RecordTemp;
		}
	}

	return {};
}

#define MAX_TELEPORT_DIST 64 * 64
std::deque<CLagComp::CLagCompRecord> CLagComp::GetValidTicks( int Player )
{
	if ( LagComp->Records[ Player ].empty( ) || LagComp->Records[ Player ].size( ) < 2 )
		return {};

	std::deque<CLagCompRecord> OldRecords{};
	CLagCompRecord Previous{};

	for ( size_t i = 0; i < LagComp->Records[ Player ].size( ); i++ )
	{
		if ( !LagComp->IsTickValid( LagComp->Records[ Player ][ i ] ) )
		{
			continue;
		}

		if ( i > 0 && Previous.Valid )
		{
			Vector Difference = LagComp->Records[ Player ][ i ].Origin - Previous.Origin;
			if ( Difference.Length2DSqr( ) > MAX_TELEPORT_DIST )
			{
				LagComp->Records[ Player ].clear( );
				return {};
			}
		}

		Previous = LagComp->Records[ Player ][ i ];
		OldRecords.push_back( LagComp->Records[ Player ][ i ] );
	}

	return OldRecords;
}

void CLagComp::CLagCompRecord::UpdateRecord( )
{
	if ( !Player || !gLocalPlayer )
		return;

	// this always changes, make sure to keep it up2date
	DistanceToLocal = WorldSpaceCenter.DistTo( gLocalPlayer->WorldSpaceCenter( ) );
}

void CLagComp::HandleResolving( CBasePlayer* Entity )
{
	if ( !Entity )
		return;

	int Idx = Entity->GetIndex( );
	bool HasMissed = false;

	PlayerInfo_t info;
	gEngineClient->GetPlayerInfo( Idx, &info );

	if ( !gPriorities[ info.friendsID ].Resolver )
	{
		Resolverer[ Idx ].AhHesLegitWhoCouldHaveThought = true;
		return;
	}

	//not fully working yet i think
	if ( Entity->Class( ) == CLASS_SNIPER )
	{
		const auto& SniperDot = gSniperDots[ Idx ];
		if ( SniperDot )
		{
			Vector Angle;
			gMath->VectorAngles( SniperDot->Origin( ) - Entity->EyePosition( ), Angle );
			Entity->EyeAngles( ).x = std::clamp( Angle.x, -89.f, 89.f );
		}

		//this edge handling is dogshit, rather go without it
		/*if ( Resolverer[ Idx ].MissedShots == -1 && !Resolverer[ Idx ].Resolved )
		{
			Vector Angle = gMath->CalcAngle( Entity->WorldSpaceCenter( ), gLocalPlayer->WorldSpaceCenter( ) );

			Vector Left, Right;
			gMath->AngleVectors( Angle - Vector( 0.0f, 90.f, 0.0f ), &Left );
			gMath->AngleVectors( Angle + Vector( 0.0f, 90.f, 0.0f ), &Right );

			Left = Entity->WorldSpaceCenter( ) + Left * 25;
			Right = Entity->WorldSpaceCenter( ) + Right * 25;

			if ( TF2->IsVisible( gLocalPlayer, Entity, gLocalPlayer->EyePosition( ), Left ) )
			{
				Entity->EyeAngles( ).y = Angle.y - 90.f;
			}
			else if ( TF2->IsVisible( gLocalPlayer, Entity, gLocalPlayer->EyePosition( ), Right ) )
			{
				Entity->EyeAngles( ).y = Angle.y + 90.f;
			}
		}

		//test for anti-bruteforce :alien:
		if ( G->AntiAim->FiredShotHit[ Entity->GetIndex( ) ] )
		{
			G->AntiAim->FiredShotHit[ Entity->GetIndex( ) ] = false;
			return;
		}

		if ( G->AntiAim->FiredShotsTimes[ Entity->GetIndex( ) ] != 0.0f && !G->AntiAim->FiredShotHit[ Entity->GetIndex( ) ] )
		{
			gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Miss registered\n" );
			G->AntiAim->FiredShotsTimes[ Entity->GetIndex( ) ] = 0.0f;
			AntiAim->IsFlipped = !AntiAim->IsFlipped;
		}*/
	}

	//should we resolve based on our hitbox choice
	//if targetting head, resolve on miss and bodyshot
	//if targetting body, resolve on miss only
	if ( G->Aimbot.SentShot > 0.f && gEngineClient->Time( ) >= G->Aimbot.SentShot + gEngineClient->GetNetChannelInfo( )->GetLatency( 2 )
		&& ( ( !G->Aimbot.ConfirmedHeadshot && G->Aimbot.TargetHead ) || ( !G->Aimbot.ConfirmedHit && !G->Aimbot.TargetHead ) ) )
	{
		// mark shot as missed
		G->Aimbot.SentShot = 0.f;
		G->Aimbot.ConfirmedHit = false;
		G->Aimbot.ConfirmedHeadshot = false;

		//set parameters for our entity
		Resolverer[ Idx ].MissedShots++;
		Resolverer[ Idx ].CycleYaw = true;
		Resolverer[ Idx ].Resolved = false;
		Resolverer[ Idx ].AhHesLegitWhoCouldHaveThought = false;
		Resolverer[ Idx ].ChangeAngles = true;

		if ( Entity->EyeAngles( ).x < 88.f && Entity->EyeAngles( ).x > -88.f )
		{
			Resolverer[ Idx ].LegitAA = true;
		}
		else
		{
			Resolverer[ Idx ].LegitAA = false;
		}
	}

	//constantly set angles to the angles we last hit
	if ( Resolverer[ Idx ].MissedShots > -1 && Resolverer[ Idx ].Resolved )
	{
		Entity->EyeAngles( ).y = Resolverer[ Idx ].Yaw;
		if ( Entity->Class( ) != CLASS_SNIPER && !Resolverer[ Idx ].LegitAA )
		{
			Entity->EyeAngles( ).x = Resolverer[ Idx ].Pitch;
		}
		return;
	}

	if ( Entity != G->Aimbot.Target )
	{
		return;
	}

	if ( Resolverer.find( Idx ) == Resolverer.end( ) )
	{
		return;
	}

	if ( Resolverer[ Idx ].AhHesLegitWhoCouldHaveThought )
	{
		return;
	}

	if ( Resolverer[ Idx ].ChangeAngles )
	{
		Vector Ang = gMath->CalcAngle( Entity->WorldSpaceCenter( ), gLocalPlayer->WorldSpaceCenter( ) );
		if ( Resolverer[ Idx ].CycleYaw )
		{
			if ( Resolverer[ Idx ].LegitAA && gPriorities[ Idx ].Priority != 2 )
			{
				switch ( Resolverer[ Idx ].MissedShots % 3 )
				{
				case 0: Resolverer[ Idx ].Yaw = Ang.y + 90.f; break;
				case 1: Resolverer[ Idx ].Yaw = Ang.y - 90.f; break;
				case 2: Resolverer[ Idx ].Yaw = Ang.y + 180.f; break;
				case 3: Resolverer[ Idx ].Yaw = Ang.y; break;
				}
				std::string str = White + "|" + Logo + "Ateris" + White + "| - |" + Feature + "Resolver" + White + "| Cycling legit angles on " + Player + info.name + White + ".";
				gClientMode->m_pChatElement->ChatPrintf( 0, 0, str.c_str( ) );
			}
			else
			{
				if ( Entity->Class( ) == CLASS_SNIPER )
				{
					switch ( Resolverer[ Idx ].MissedShots % 7 )
					{
					case 0: Resolverer[ Idx ].Yaw = Ang.y - 90.f; break;
					case 1: Resolverer[ Idx ].Yaw = Ang.y + 90.f; break;
					case 2: Resolverer[ Idx ].Yaw = Ang.y + 180.f; break;
						// 125 -125 is alr checked by -90 and 90, bc of overlap
					case 3: Resolverer[ Idx ].Yaw = Ang.y - 125.f; break;
					case 4: Resolverer[ Idx ].Yaw = Ang.y + 125.f; break;
					case 5: Resolverer[ Idx ].Yaw = Ang.y - 25.f; break;
					case 6: Resolverer[ Idx ].Yaw = Ang.y + 25.f; break;
					}
				}
				else
				{
					switch ( Resolverer[ Idx ].MissedShots % 14 )
					{
					case 0: Resolverer[ Idx ].Yaw = Ang.y + 90.f; Resolverer[ Idx ].Pitch = -89.f; break;
					case 1: Resolverer[ Idx ].Yaw = Ang.y + 90.f; Resolverer[ Idx ].Pitch = 89.f; break;
					case 2: Resolverer[ Idx ].Yaw = Ang.y - 90.f; Resolverer[ Idx ].Pitch = -89.f; break;
					case 3: Resolverer[ Idx ].Yaw = Ang.y - 90.f; Resolverer[ Idx ].Pitch = 89.f; break;
					case 4: Resolverer[ Idx ].Yaw = Ang.y + 125.f; Resolverer[ Idx ].Pitch = -89.f; break;
					case 5: Resolverer[ Idx ].Yaw = Ang.y + 125.f; Resolverer[ Idx ].Pitch = 89.f; break;
					case 6: Resolverer[ Idx ].Yaw = Ang.y - 125.f; Resolverer[ Idx ].Pitch = -89.f; break;
					case 7: Resolverer[ Idx ].Yaw = Ang.y - 125.f; Resolverer[ Idx ].Pitch = 89.f; break;
					case 8: Resolverer[ Idx ].Yaw = Ang.y + 180.f; Resolverer[ Idx ].Pitch = -89.f; break;
					case 9: Resolverer[ Idx ].Yaw = Ang.y + 180.f; Resolverer[ Idx ].Pitch = 89.f; break;
					case 10: Resolverer[ Idx ].Yaw = Ang.y - 25.f; Resolverer[ Idx ].Pitch = -89.f; break;
					case 11: Resolverer[ Idx ].Yaw = Ang.y - 25.f; Resolverer[ Idx ].Pitch = 89.f; break;
					case 12: Resolverer[ Idx ].Yaw = Ang.y + 25.f; Resolverer[ Idx ].Pitch = -89.f; break;
					case 13: Resolverer[ Idx ].Yaw = Ang.y + 25.f; Resolverer[ Idx ].Pitch = 89.f; break;
					}
				}
				std::string str = White + "|" + Logo + "Ateris" + White + "| - |" + Feature + "Resolver" + White + "| Cycling angles on " + Player + info.name + White + ".";
				gClientMode->m_pChatElement->ChatPrintf( 0, 0, str.c_str( ) );
			}
			Resolverer[ Idx ].CycleYaw = false;
		}

		Entity->EyeAngles( ).y = Resolverer[ Idx ].Yaw;

		if ( Entity->Class( ) != CLASS_SNIPER && !Resolverer[ Idx ].LegitAA )
		{
			Entity->EyeAngles( ).x = Resolverer[ Idx ].Pitch;
		}
	}
}

void CLagComp::ResolverHurtEvent( IGameEvent* Event )
{
	if ( !gLocalPlayer || !gLocalWeapon || gLocalPlayer->Class( ) != CLASS_SNIPER )
		return;

	const auto attacked = gEntityList->GetClientEntity( gEngineClient->GetPlayerForUserID( Event->GetInt( "userid" ) ) );
	const auto attacker = gEntityList->GetClientEntity( gEngineClient->GetPlayerForUserID( Event->GetInt( "attacker" ) ) );
	if ( !attacked || !attacker || attacker != gLocalPlayer )
		return;

	const auto crit = Event->GetBool( "crit" );
	if ( crit )
	{
		// most likely killed a player.
		G->Aimbot.ConfirmedHit = true;
		G->Aimbot.ConfirmedHeadshot = true;
		G->Aimbot.SentShot = 0.f;
		Resolverer[ attacked->GetIndex( ) ].Resolved = true;
		if ( G->AntiAim->FiredShotsTimes[ attacker->GetIndex( ) ] != 0.0f && attacked == gLocalPlayer )
		{
			//gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Hit registered.\n" );
			G->AntiAim->FiredShotHit[ attacker->GetIndex( ) ] = true;
			G->AntiAim->FiredShotsTimes[ attacker->GetIndex( ) ] = 0.0f;
		}

		/*if ( Resolverer.find( attacked->GetIndex( ) ) == Resolverer.end( ) )
			return;

		Resolverer[attacked->GetIndex()].Reset();*/
	}
	else
	{
		// hit registered
		G->Aimbot.ConfirmedHit = true;
		G->Aimbot.ConfirmedHeadshot = false;
		//G->Aimbot.SentShot = 0.f;
		Resolverer[ attacked->GetIndex( ) ].HitShots++;
	}

}