#include "Aimbot.h"
#include "../../../Globals.h"
#include "../Menu/Input/Input.h"
#include "..\NoSpread\NoSpread.h"
#include "../Auto/AutoDetonate/AutoDetonate.h"
#include "../Misc/Misc.h"

bool IsEntityAValidTarget(CBasePlayer* Entity)
{
	if ( Entity->IsBomb( ) && Config->Aimbot->Targetting->TargetLethalBombs )
		return true;

	if ( Entity->IsTank( ) || Entity->IsSentryBuster( ) )
		return true;

	return (Entity->IsPlayer() && Entity->IsAlive() && Config->Aimbot->Targetting->TargetPlayers) || 
		(Entity->IsBuilding() && Config->Aimbot->Targetting->TargetBuildings) ||
		(Entity->IsProjectile() && Entity->GetThrower() && Entity->GetThrower()->IsEnemy() &&
			Entity->PipebombType() == TYPE_STICKY && Entity->EstimateAbsVelocity().Length() <= 20.f && Config->Aimbot->Targetting->TargetStickies);
}

void CAimbot::FindSuitableTargets()
{
	Targets.clear();

	Vector ViewAngles = gEngineClient->GetViewAngles();
	if (G->Doubletap.ToSend > 0 )
	{
		if ( !G->Aimbot.LockedAngles.IsZero( ) )
			ViewAngles = G->Aimbot.LockedAngles;
	}
	else
	{
		G->Aimbot.LockedAngles = Vector();
	}

	if (!gLocalPlayer || !gLocalPlayer->IsAlive())
		return;

	for (auto& Entity : gEntities)
	{
		auto Player = reinterpret_cast<CBasePlayer*>(Entity);
		if ( !Player || Player->IsLocalPlayer( ) )
			continue;

		if (!IsEntityAValidTarget(Player))
			continue;

		if (Player->IsPlayer() || Player->IsBuilding())
			if (!Player->IsEnemy() || Player->IsDormant( ) )
				continue;

		if (Player->IsPlayer())
		{
			if (Player->PlayerCond() & TFCond_Bonked && !Config->Aimbot->Targetting->TargetBonk)
				continue;

			if (Player->PlayerCondEx() & TFCondEx_BulletCharge && !Config->Aimbot->Targetting->TargetVacc)
				continue;

			if (Player->IsInvulnerable( ) && !Config->Aimbot->Targetting->TargetUber )
				continue;

			if ( Player->IsSteamFriend( ) && !Config->Aimbot->Targetting->TargetFriends )
				continue;
		}

		float CurrentFov = gMath->CalcFov(ViewAngles, gMath->CalcAngle(gLocalPlayer->EyePosition(), Player->WorldSpaceCenter()));
		float Distance = gLocalPlayer->WorldSpaceCenter().DistTo(Player->WorldSpaceCenter());

		if (CurrentFov > Config->Aimbot->Global->FOV)
		{
			continue;
		}

		if ( !Entity->IsEntityOnScreen( ) && CurrentFov < 1.f )
		{
			continue;
		}

		Target_t Target(CurrentFov, Distance, Entity->IsBuilding() ? 21 : 20, Player, false);
		
		int Priority = 0;

		if (Player->IsPlayer())
		{
			if (gLocalPlayer->Class() == CLASS_HEAVY && Player->Class() == CLASS_MEDIC && Config->Aimbot->Hitscan->PreventBait)
				if (Player->ActiveWeapon() && Player->ActiveWeapon()->WeaponID() == TF_WEAPON_MEDIGUN)
					if (Player->ActiveWeapon()->HealingTarget() != nullptr)
						if (Player->ActiveWeapon()->HealingTarget()->Class() == CLASS_HEAVY)
							continue;

			switch (Player->Class())
			{
			case CLASS_SCOUT: Priority = Config->Aimbot->Priorities->Scout; break;
			case CLASS_SOLDIER: Priority = Config->Aimbot->Priorities->Soldier; break;
			case CLASS_PYRO: Priority = Config->Aimbot->Priorities->Pyro; break;
			case CLASS_DEMOMAN: Priority = Config->Aimbot->Priorities->Demoman; break;
			case CLASS_HEAVY: Priority = Config->Aimbot->Priorities->Heavy; break;
			case CLASS_ENGINEER: Priority = Config->Aimbot->Priorities->Engineer; break;
			case CLASS_MEDIC: Priority = Config->Aimbot->Priorities->Medic; break;
			case CLASS_SNIPER: Priority = Config->Aimbot->Priorities->Sniper; break;
			case CLASS_SPY: Priority = Config->Aimbot->Priorities->Spy; break;
			}

			if (Player->Class() == CLASS_HEAVY && Player->PlayerCond() & TFCond_Slowed &&
				gLocalPlayer->Class() == CLASS_HEAVY)
				Priority = Config->Aimbot->Priorities->Revved;

			if (Player->PlayerCond() & TFCond_Zoomed && 
				gLocalPlayer->Class() == CLASS_SNIPER)
				Priority = Config->Aimbot->Priorities->Scoped;

			if (Player->PlayerCond() & TFCond_Cloaked)
				Priority = Config->Aimbot->Priorities->Cloaked;

			if (Player->IsSteamFriend())
			{
				if ( !Config->Aimbot->Targetting->TargetFriends )
					continue;

				Priority = Config->Aimbot->Priorities->Friends;
			}

		}
		else if ( Player->IsProjectile( ) )
		{
			if ( Player->EstimateAbsVelocity( ).Length( ) > 0.05f )
				continue;

			Priority = Config->Aimbot->Priorities->Stickies;

			if ( Player->GetThrower( ) && Player->GetThrower( )->IsSteamFriend( ) && !Config->Aimbot->Targetting->TargetFriends )
				continue;
		}
		else if ( Player->IsTank( ) || Player->IsSentryBuster( ) )
		{
			Priority = Config->Aimbot->Priorities->Other;
		}
		else
		{
			if ( Player->IsBomb( ) )
			{
				bool FoundDet = false;
				for ( CEntitySphereQuery Sphere( Player->WorldSpaceCenter( ), 145.f ); ( Entity = Sphere.CurrentEntity( ) ) != nullptr; Sphere.NextEntity( ) )
				{
					// do not explode urself b0z0
					if ( Entity == gLocalPlayer )
						break;

					if ( !Entity || !Entity->IsPlayer( ) || !Entity->IsEnemy( ) )
						continue;

					auto Plr = Entity->As<CBasePlayer>( );
					if ( !Config->Aimbot->Targetting->TargetFriends )
					{
						PlayerInfo_t info;
						gEngineClient->GetPlayerInfo( Player->GetIndex( ), &info );

						if ( Plr->IsSteamFriend( ) || Config->Aimbot->Priorities->Friends <= 0 || 
							gPriorities[ info.friendsID ].Priority == 4 )
							continue;
					}

					CTraceFilterWorldAndPropsOnly Filter = {};
					CGameTrace Trace = {};

					if ( !TF2->IsVisible( Player, Plr, Player->WorldSpaceCenter( ),
						Plr->WorldSpaceCenter( ), MASK_SOLID ) )
						continue;

					Target.Priority = 20000; // :troll:
					Targets.emplace_back( Target );
					FoundDet = true;
					break;
				}
				
				if ( FoundDet )
					break;
			}

			switch (Player->GetClientClass()->ClassID)
			{
			case EClassID::CObjectSentrygun: Priority = Config->Aimbot->Priorities->Sentry; break;
			case EClassID::CObjectTeleporter: Priority = Config->Aimbot->Priorities->OtherBuildings; break;
			case EClassID::CObjectDispenser: Priority = Config->Aimbot->Priorities->OtherBuildings; break;
			}
		}

		if ( Player->IsPlayer( ) )
		{
			PlayerInfo_t info;
			gEngineClient->GetPlayerInfo( Player->GetIndex( ), &info );

			switch ( gPriorities[ info.friendsID ].Priority )
			{
			case 0: break;
			case 1: break;
			case 2: Priority = Config->Aimbot->Priorities->Cheaters; break;
			case 3: Priority = Config->Aimbot->Targetting->TargetFriends ? Config->Aimbot->Priorities->Friends : 0; break;
			case 4: Priority = 5; break;
			default: break;
			}
		}

		Target.Priority = Priority;
		if (Target.Priority <= 0)
			continue;

		Targets.emplace_back(Target);
	}

	std::sort(Targets.begin(), Targets.end(), [](const Target_t& t1, const Target_t& t2) {
		return t1.Fov < t2.Fov;
	});

	std::sort(Targets.begin(), Targets.end(), [](const Target_t& t1, const Target_t& t2) {
		return t1.Priority > t2.Priority;
	});

	while ( Targets.size( ) > 4 )
	{
		Targets.erase( Targets.begin( ) + 4, Targets.end( ) );
	}
}

std::vector<Vector> CAimbot::GeneratedPointsForHitbox(CBasePlayer* Player, int Hitbox, Matrix3x4* Matrix, const model_t* Model)
{
	if (!Model)
		return { };

	studiohdr_t* hdr = gModelInfo->GetStudioModel(Model);

	if (!hdr)
		return { };

	if (!Matrix)
		return { };

	mstudiohitboxset_t* set = hdr->GetHitboxSet(Player->HitboxSet());
	if (!set)
		return { };

	mstudiobbox_t* box = set->hitbox(Hitbox);
	if (!box)
		return { };

	Vector min = box->bbmin;
	Vector max = box->bbmax;

	float scale = Config->Aimbot->Hitscan->PointScale / 100.f;
	if (Player->Class() == CLASS_HEAVY)
		scale = Config->Aimbot->Hitscan->HeavyPointScale / 100.f;

	std::vector< Vector > Untransofmed = {};

	if ( Hitbox == HITBOX_UPPERARM_L || Hitbox == HITBOX_UPPERARM_R  || Hitbox == HITBOX_HEAD )
	{
		Untransofmed = { Vector( ( min.x + max.x ) * 0.5f,
			( min.y + max.y ) * 0.5f, ( max.z + min.z ) * 0.5f ) };
	}
	else
	{
		Untransofmed = {
			Vector( ( min.x * -scale ), ( min.y * -scale ), ( min.z + max.z ) * 0.5f ), // left
			Vector( ( min.x * scale ), ( min.y * scale ), ( min.z + max.z ) * 0.5f ), // right
			Vector( ( min.x + max.x ) * 0.5f, ( min.y + max.y ) * 0.5f, ( max.z + min.z ) * 0.5f ) // center
		};
	}

	std::vector< Vector > Points = { };
	for (const Vector& point : Untransofmed) {
		Vector Transformed = {};
		gMath->VectorTransform(point, Matrix[box->bone], Transformed);

		if (!TF2->IsVisible(Player, NULL, Player->WorldSpaceCenter(), Transformed, MASK_SHOT | CONTENTS_GRATE))
			continue;

		Points.emplace_back(Transformed);
	}

	return Points;
}

static std::vector<int> Hitboxes = { HITBOX_HEAD, HITBOX_PELVIS, HITBOX_SPINE_0, HITBOX_SPINE_1, 
	HITBOX_SPINE_2, HITBOX_SPINE_3, HITBOX_UPPERARM_L, HITBOX_UPPERARM_R };
Vector CAimbot::FindPoint(CBasePlayer* Player, float& Simtime, Vector& point, CLagComp::CLagCompRecord& Record)
{
	Matrix3x4 Matrix[128] = {};
	Player->SetupBones(Matrix, 128, 0x77FF00, gGlobals->curtime);

	if (!LagComp->Records[Player->GetIndex()].empty() && Config->Aimbot->Hitscan->LagCompMethod )
	{
		auto RecordTemp = LagComp->GetClosestTick(Player->GetIndex());

		if (RecordTemp.SetupBoner)
		{
			if (Config->Aimbot->Hitscan->Preference == 0 && HandleHeadshot() && Player->Health() > 50)
			{
				G->Aimbot.TargetHead = true;
				auto Point = Player->GetHitboxPos(HITBOX_HEAD, RecordTemp.Boner, RecordTemp.Model) + Vector(0, 0, 2);
				if (TF2->IsVisible(gLocalPlayer, Player, gLocalPlayer->EyePosition(), Point, MASK_SHOT | CONTENTS_GRATE))
				{
					point = Point;
					Simtime = RecordTemp.SimTime;
					Record = RecordTemp;
					return point;
				}
			}

			if ( Config->Aimbot->Hitscan->Preference == 1 )
			{
				G->Aimbot.TargetHead = false;
				for ( auto& Hitbox : Hitboxes )
				{
					if ( !CanTargetHitbox( Hitbox, Player ) )
						continue;

					auto Point = Player->GetHitboxPos( Hitbox, RecordTemp.Boner, RecordTemp.Model );
					if ( TF2->IsVisible( gLocalPlayer, Player, gLocalPlayer->EyePosition( ), Point, MASK_SHOT | CONTENTS_GRATE ) )
					{
						point = Point;
						Simtime = RecordTemp.SimTime;
						Record = RecordTemp;
						return point;
					}
				}
			}

			if ( Config->Aimbot->Hitscan->Preference == 2 )
			{
				if ( HandleHeadshot( ) && Player->Health( ) > 50 )
				{
					G->Aimbot.TargetHead = true;
					auto Point = Player->GetHitboxPos( HITBOX_HEAD, RecordTemp.Boner, RecordTemp.Model ) + Vector( 0, 0, 3 );
					if ( TF2->IsVisible( gLocalPlayer, Player, gLocalPlayer->EyePosition( ), Point, MASK_SHOT | CONTENTS_GRATE ) )
					{
						point = Point;
						Simtime = RecordTemp.SimTime;
						Record = RecordTemp;
						return point;
					}
				}
				else
				{
					G->Aimbot.TargetHead = false;
					for ( auto& Hitbox : Hitboxes )
					{
						if ( !CanTargetHitbox( Hitbox, Player ) )
							continue;

						auto Point = Player->GetHitboxPos( Hitbox, RecordTemp.Boner, RecordTemp.Model );
						if ( TF2->IsVisible( gLocalPlayer, Player, gLocalPlayer->EyePosition( ), Point, MASK_SHOT | CONTENTS_GRATE ) )
						{
							point = Point;
							Simtime = RecordTemp.SimTime;
							Record = RecordTemp;
							return point;
						}
					}
				}
			}
		}
	}

	auto Channel = gEngineClient->GetNetChannelInfo( );
	if ( Config->Aimbot->Hitscan->FakeLatency && Misc->IsFakeLatencyToggled && ( ( Channel &&
		Config->Aimbot->Hitscan->FakeLatencyAmount >= ( int )( 200.f - Channel->GetLatency( FLOW_OUTGOING ) * 1000.f ) ) || Config->Aimbot->Hitscan->DynamicLatency ) )
		return {};

	if ( Config->Aimbot->Hitscan->Preference == 0 && HandleHeadshot( ) )
	{
		G->Aimbot.TargetHead = true;
		if (TF2->IsVisible(gLocalPlayer, Player, gLocalPlayer->EyePosition(), Player->GetHitboxPos(0) + Vector(0, 0, 3), MASK_SHOT | CONTENTS_GRATE))
		{
			point = Player->GetHitboxPos(HITBOX_HEAD) + Vector(0, 0, 3);
			Simtime = 0.f;
			return point;
		}
	}

	if ( Config->Aimbot->Hitscan->Preference == 1 )
	{
		G->Aimbot.TargetHead = false;
		for ( auto& Hitbox : Hitboxes )
		{
			if ( !CanTargetHitbox( Hitbox, Player ) )
				continue;

			auto Points = GeneratedPointsForHitbox( Player, Hitbox, Matrix, Player->GetModel( ) );
			for ( auto& Point : Points )
			{
				if ( TF2->IsVisible( gLocalPlayer, Player, gLocalPlayer->EyePosition( ), Point, MASK_SHOT | CONTENTS_GRATE ) )
				{
					point = Point;
					Simtime = 0.f;
					return point;
				}
			}
		}
	}

	if ( Config->Aimbot->Hitscan->Preference == 2 )
	{
		if ( HandleHeadshot( ) && Player->Health( ) > 50 )
		{
			G->Aimbot.TargetHead = true;
			if ( TF2->IsVisible( gLocalPlayer, Player, gLocalPlayer->EyePosition( ), Player->GetHitboxPos( 0 ) + Vector( 0, 0, 3 ), MASK_SHOT | CONTENTS_GRATE ) )
			{
				point = Player->GetHitboxPos( HITBOX_HEAD ) + Vector( 0, 0, 3 );
				Simtime = 0.f;
				return point;
			}
		}
		else
		{
			G->Aimbot.TargetHead = false;
			for ( auto& Hitbox : Hitboxes )
			{
				if ( !CanTargetHitbox( Hitbox, Player ) )
					continue;

				auto Points = GeneratedPointsForHitbox( Player, Hitbox, Matrix, Player->GetModel( ) );
				for ( auto& Point : Points )
				{
					if ( TF2->IsVisible( gLocalPlayer, Player, gLocalPlayer->EyePosition( ), Point, MASK_SHOT | CONTENTS_GRATE ) )
					{
						point = Point;
						Simtime = 0.f;
						return point;
					}
				}
			}
		}
	}

	return {};
}

bool CAimbot::HandleHeadshotStuff(CUserCmd* pCmd, CBasePlayer* BestTarget)
{
	if (!gLocalPlayer || !gLocalWeapon)
		return false;

	if (gLocalPlayer->Class() == CLASS_SNIPER && gLocalWeapon->Slot() == SLOT_PRIMARY && !(gLocalPlayer->PlayerCond() & TF_COND_ZOOMED))
	{
		pCmd->buttons |= IN_ATTACK2;
		return false;
	}

	if (gLocalPlayer->Class() == CLASS_SNIPER && gLocalWeapon->ItemDefinitionIndex() != Sniper_m_TheSydneySleeper && Config->Aimbot->Hitscan->WaitForHeadshot)
		if (!gLocalWeapon->CanFireCritical(true))
			return false;

	if (gLocalPlayer->Class() == CLASS_SNIPER && gLocalWeapon->Slot() == SLOT_PRIMARY && Config->Aimbot->Hitscan->WaitForCharge)
	{
		if (BestTarget)
		{
			if (BestTarget->Health() <= 150.f)
				return true;

			float Damage = gMath->ReMapValClamped(gLocalWeapon->ChargedDamage(), 0.f, 150.f, 0.f, 450.f);
			if (Damage >= 450.f) // early out
				return true;

			if (Damage < BestTarget->Health())
				return false;

			return true;
		}
	}

	if ((gLocalWeapon->ItemDefinitionIndex() == Spy_m_TheAmbassador || gLocalWeapon->ItemDefinitionIndex() == Spy_m_FestiveAmbassador) && 
		Config->Aimbot->Hitscan->WaitForHeadshot)
	{
		float LastShot = (gLocalPlayer->TickBase() * gGlobals->interval_per_tick) - gLocalWeapon->LastFireTime();
		if ((BestTarget && BestTarget->Health() > 70) && Config->Aimbot->Hitscan->Preference != 1)
		{
			if (LastShot <= 0.990f)
				return false;
		}
		else
		{
			if (LastShot <= 0.250f) // we need some accuracy
				return false;
		}
	}

	return true;
}

void CAimbot::Run( CUserCmd* pCmd )
{
	G->Aimbot.Active = false;

	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !gLocalWeapon || !Config->Aimbot->Global->Enabled || Config->Aimbot->Hitscan->LegitBob->Enabled )
		return;

	G->Aimbot.Sticky = false;
	G->Aimbot.LethalDTRange = false;

	if ( Config->Aimbot->Global->AutoSecondarySwitch && gLocalWeapon->Clip1( ) < 1 && gLocalWeapon->Slot( ) == SLOT_PRIMARY && G->Aimbot.SavedWeaponID != TF_WEAPON_SNIPERRIFLE
		&& G->Aimbot.SavedWeaponID != TF_WEAPON_SNIPERRIFLE_DECAP && G->Aimbot.SavedWeaponID != TF_WEAPON_SNIPERRIFLE_CLASSIC
		&& gLocalWeapon->ItemDefinitionIndex( ) != Engi_m_TheWidowmaker && gLocalWeapon->ItemDefinitionIndex( ) != Sniper_m_TheMachina && ( ( GetAsyncKeyState( Config->Aimbot->Global->AimKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) )
	{
		gEngineClient->ClientCmd_Unrestricted( "slot2" );
	}

	if ( !TF2->IsHitscanWeapon( gLocalWeapon ) || gLocalWeapon->Slot( ) == SLOT_MELEE )
		return;

	FindSuitableTargets( );

	static bool HasShotAtTarget = false;
	static CBasePlayer* OldTarget = { };

	if ( Targets.empty( ) )
		return;

	if ( Targets.front( ).This->IsPlayer( ) )
		G->Aimbot.ClosestToCrosshair = Targets.front( ).This;

	if ( !( ( GetAsyncKeyState( Config->Aimbot->Global->AimKey ) & 0x8000 ) && !gSurface->IsCursorVisible( ) ) )
		return;

	float SimulationTime = -1.f;
	bool Found = false;
	Vector Point = {};
	CLagComp::CLagCompRecord Record = {};

	CBasePlayer* BestTarget = nullptr;
	for ( auto& Target : Targets )
	{
		auto& Player = Target.This;
		if ( !Player->IsPlayer( ) )
		{
			// wait for charge on buildings
			if ( G->Aimbot.SavedWeaponID == TF_WEAPON_SNIPERRIFLE && ( gLocalWeapon->ChargedDamage( ) >= Player->Health( ) || gLocalWeapon->ChargedDamage( ) >= 150.f ) )
			{
				if ( TF2->IsVisible( gLocalPlayer, Player, gLocalPlayer->EyePosition( ), Player->WorldSpaceCenter( ), MASK_SHOT | CONTENTS_GRATE ) )
				{
					SimulationTime = -1.f;
					Found = true;
					Point = Player->WorldSpaceCenter( );
					BestTarget = Player;
					break;
				}
			}
			else if ( G->Aimbot.SavedWeaponID != TF_WEAPON_SNIPERRIFLE )
			{
				if ( TF2->IsVisible( gLocalPlayer, Player, gLocalPlayer->EyePosition( ), Player->WorldSpaceCenter( ), MASK_SHOT | CONTENTS_GRATE ) )
				{
					SimulationTime = -1.f;
					Found = true;
					Point = Player->WorldSpaceCenter( );
					BestTarget = Player;
					break;
				}
			}
		}
		else
		{
			if ( Config->Aimbot->Hitscan->Preference == 2 )
			{
				bool CanHeadshot = HandleHeadshot( );
				if ( CanHeadshot )
					Hitboxes = { HITBOX_HEAD, HITBOX_PELVIS, HITBOX_SPINE_0, HITBOX_SPINE_1, HITBOX_SPINE_2, HITBOX_SPINE_3, HITBOX_UPPERARM_L, HITBOX_UPPERARM_R };
				else
					Hitboxes = { HITBOX_PELVIS, HITBOX_SPINE_0, HITBOX_SPINE_1, HITBOX_SPINE_2, HITBOX_SPINE_3, HITBOX_UPPERARM_L, HITBOX_UPPERARM_R };
			}

			Found = !FindPoint( Player, SimulationTime, Point, Record ).IsZero( );
			if ( Found )
			{
				BestTarget = Player;
				break;
			}
		}
	}

	G->Aimbot.AimPoint = Point;
	if ( BestTarget )
		if ( !HandleHeadshotStuff( pCmd, BestTarget ) )
			return;

	if ( Point.IsZero( ) )
		return;

	if ( OldTarget != BestTarget )
	{
		OldTarget = BestTarget;
		HasShotAtTarget = false;
	}

	if ( !HandleDelayShot( BestTarget, HasShotAtTarget, Point ) )
		return;

	if ( BestTarget )
	{
		if ( BestTarget->IsPlayer( ) )
		{
			G->Aimbot.LethalDTRange = TF2->IsInLethalDTRange( BestTarget );
			G->Aimbot.Sticky = false;
		}
		else
			G->Aimbot.Sticky = BestTarget && ( BestTarget->IsProjectile( ) || BestTarget->IsBomb( ) );
	}

	if ( Found && BestTarget )
	{
		if ( BestTarget->IsPlayer( ) )
		{
			/* 
			* If you even find a way to detect warping
			* Check for it here
			* And use this code
			
			float Delta = BestTarget->SimulationTime( ) - BestTarget->OldSimulationTime( );
			if ( Delta < TICKS_TO_TIME( 2 ) )
				if ( Delta < gGlobals->interval_per_tick || Delta > gGlobals->interval_per_tick * 1.4f )
					return;

			*/

			PlayerInfo_t info;
			gEngineClient->GetPlayerInfo( BestTarget->GetIndex( ), &info );

			switch ( gPriorities[ info.friendsID ].Priority )
			{
			case 0: break;
			case 1:
				if ( Config->Misc->CritHack->ForceOnSuspicious )
					G->CritHack->Force = true;
				break;
			case 2:
				if ( Config->Misc->CritHack->ForceOnCheater )
					G->CritHack->Force = true;
				break;
			case 3:
				if ( Config->Misc->CritHack->ForceOnFriends )
					G->CritHack->Force = true;
				break;
			case 4:
				if ( Config->Misc->CritHack->ForceOnStreamers )
					G->CritHack->Force = true;
				break;
			default: break;
			}
		}

		if ( Input->InFocus( ) && ( ( GetAsyncKeyState( Config->Misc->FakeDuck->FakeDuckKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) )
		{
			if ( gLocalPlayer->Ducktime( ) == 0.f && !gLocalPlayer->Ducking( ) && !gLocalPlayer->IsDucked( ) )
			{
				pCmd->buttons |= IN_ATTACK;
			}
		}
		else
		{
			pCmd->buttons |= IN_ATTACK;
		}

		if ( Config->Aimbot->Hitscan->TapFire && G->Aimbot.SavedWeaponID == TF_WEAPON_MINIGUN )
		{
			if ( Config->Misc->NoSpread->Enabled )
			{
				if ( NoSpread->MantissaStep < 8.f || NoSpread->SyncState != NoSpread->SYNCED )
				{
					if ( gLocalPlayer->Origin( ).DistTo( BestTarget->Origin( ) ) > 950.f )
					{
						float LastShot = ( gLocalPlayer->TickBase( ) * gGlobals->interval_per_tick ) - gLocalWeapon->LastFireTime( );
						if ( LastShot <= 0.25f )
							pCmd->buttons &= ~IN_ATTACK;
					}
				}
			}
			else
			{
				if ( gLocalPlayer->Origin( ).DistTo( BestTarget->Origin( ) ) > 950.f )
				{
					float LastShot = ( gLocalPlayer->TickBase( ) * gGlobals->interval_per_tick ) - gLocalWeapon->LastFireTime( );
					if ( LastShot <= 0.25f )
						pCmd->buttons &= ~IN_ATTACK;
				}
			}
		}

		if ( TF2->IsAttacking( pCmd ) )
		{
			pCmd->viewangles = gMath->CalcAngle( gLocalPlayer->EyePosition( ), Point );
			pCmd->viewangles -= G->Aimbot.PunchAngle;

			if ( G->Doubletap.Charged >= 21 )
				G->Aimbot.LockedAngles = pCmd->viewangles;

			//HasShotAtTarget = true;
			if ( BestTarget->IsPlayer( ) )
			{
				if ( SimulationTime <= 0.f )
					pCmd->tick_count = TIME_TO_TICKS( BestTarget->SimulationTime( ) + TF2->LerpTime( ) );
				else
					LagComp->Restore( pCmd, Record, BestTarget );
			}

			if ( gLocalPlayer->Class( ) == CLASS_SNIPER && gLocalWeapon->Slot( ) == SLOT_PRIMARY )
			{
				G->Aimbot.SentShot = gEngineClient->Time( );
				G->Aimbot.ConfirmedHit = false;
				G->Aimbot.ConfirmedHeadshot = false;
				G->Aimbot.Target = BestTarget;
			}
		}

		G->Aimbot.Active = true;
	}

	gMath->ClampAngles( pCmd->viewangles ); // lets clamp here
}

bool CAimbot::HandleDelayShot( CBasePlayer* BestTarget, bool HasShotAtTarget, const Vector Point )
{
	if ( !Config->Misc->DT->ShootWood && G->Doubletap.Charged > 2 && BestTarget )
	{
		bool Result = false;
		if ( gLocalPlayer->Class( ) == CLASS_SCOUT || gLocalPlayer->Class( ) == CLASS_HEAVY )
		{
			if ( Config->Misc->DT->DelayMode )
			{
				Vector Left = {}, Right = {}, Forward = {};
				Vector Angle = gMath->CalcAngle( gLocalPlayer->WorldSpaceCenter( ), BestTarget->WorldSpaceCenter( ) );

				gMath->AngleVectors( Angle + Vector( 0, 90, 0 ), &Right );
				gMath->AngleVectors( Angle - Vector( 0, 90, 0 ), &Left );
				gMath->AngleVectors( Angle, &Forward );

				Left = gLocalPlayer->EyePosition( ) + Left * ( Config->Misc->DT->DelayTicks + 8 );
				Right = gLocalPlayer->EyePosition( ) + Right * ( Config->Misc->DT->DelayTicks + 8 );

				bool cFound = false;
				Vector cPoint = {};

				CGameTrace Trace;
				CTraceFilterHitscan Filter;
				Filter.Skip = gLocalPlayer;

				if ( !cFound )
				{
					Ray_t Ray;
					Ray.Init( Left, Left + ( Forward * 256.f ), Vector( -2.f, -2.f, -2.f ), Vector( 2.f, 2.f, 2.f ) );
					gEngineTrace->TraceRay( Ray, MASK_SHOT | CONTENTS_GRATE, &Filter, &Trace );
					cFound = !Trace.DidHit( );
					if ( Trace.StartSolid )
						cFound = false;

					cPoint = Right;
				}

				if ( !cFound )
				{
					Ray_t Ray;
					Ray.Init( Right, Right + ( Forward * 256.f ), Vector( -2.f, -2.f, -2.f ), Vector( 2.f, 2.f, 2.f ) );
					gEngineTrace->TraceRay( Ray, MASK_SHOT | CONTENTS_GRATE, &Filter, &Trace );
					cFound = !Trace.DidHit( );
					if ( Trace.StartSolid )
						cFound = false;

					cPoint = Left;
				}

				if ( !HasShotAtTarget && cFound )
				{
					if ( TF2->IsVisible( gLocalPlayer, BestTarget, cPoint, Point ) )
						Result = true;

					return Result;
				}
			}
			else
			{
				static int TargetTicks = 0;
				if ( gLocalPlayer->Class( ) == CLASS_SCOUT || gLocalPlayer->Class( ) == CLASS_HEAVY )
				{
					const int Ticks = Config->Misc->DT->DelayTicks;
					Result = TargetTicks == Ticks;

					if ( G->Doubletap.Sent > 0 )
					{
						TargetTicks = 0;
						Result = true;
					}

					if ( !Result )
					{
						if ( TF2->IsVisible( gLocalPlayer, BestTarget, gLocalPlayer->EyePosition( ), Point, MASK_SHOT | CONTENTS_GRATE ) )
						{
							TargetTicks++;
						}
					}

					return Result;
				}
			}
		}
	}

	return true;
}

bool CAimbot::HandleHeadshot( )
{
	if ( gLocalPlayer->Class( ) == CLASS_SNIPER && gLocalWeapon->Slot( ) == 0 )
		return true;

	if ( ( gLocalWeapon->ItemDefinitionIndex( ) == Spy_m_TheAmbassador || gLocalWeapon->ItemDefinitionIndex( ) == Spy_m_FestiveAmbassador ) )
		return true;

	return false;
}

bool CAimbot::CanTargetArms( CBasePlayer* Player )
{
	if ( !Config->Aimbot->Hitscan->TargetArms )
		return false;

	if ( Player->Class( ) != CLASS_HEAVY )
		return false;

	if ( Player->EyeAngles( ).x >= 89.f || Player->EyeAngles( ).x <= -89.f )
		return false;

	return true;
}

bool CAimbot::CanTargetHitbox( int Hitbox, CBasePlayer* Player )
{
	switch ( Hitbox )
	{
	case HITBOX_HEAD:
	{
		return false;
	}
	case HITBOX_PELVIS:
	{
		return Config->Aimbot->Hitscan->TargetPelvis;
	}
	case HITBOX_SPINE_0:
	case HITBOX_SPINE_1:
	{
		return Config->Aimbot->Hitscan->TargetStomach;
	}
	case HITBOX_SPINE_2:
	case HITBOX_SPINE_3:
	{
		return Config->Aimbot->Hitscan->TargetChest;
	}
	case HITBOX_UPPERARM_L:
	case HITBOX_UPPERARM_R:
	{
		return !CanTargetArms( Player );
	}
	default: return false;
	}

	return false;
}