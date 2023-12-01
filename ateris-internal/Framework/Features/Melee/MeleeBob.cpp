#include "MeleeBob.h"
#include "../../../Globals.h"
#include "..\Projectile\Movement\Movement.h"

void CMeleeBob::FindSuitableTargets( )
{
	Targets.clear( );

	if ( !gLocalWeapon )
		return;

	int WarpPredictTicks = 0;
	if ( ( ( GetAsyncKeyState( Config->Misc->DT->WarpKey ) & 0x8000 )
		&& Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) )
	{
		if ( Config->Aimbot->Melee->WarpPrediction )
			WarpPredictTicks = G->Doubletap.ToSend;
	}

	auto Extrapolate = [ ] ( Vector& Pos, const Vector& vVel, float flTime, bool bGravity ) -> void
	{
		if ( bGravity )
			Pos += ( vVel * flTime ) - Vector( 0.0f, 0.0f, 800.f ) * 0.5f * flTime * flTime;

		else Pos += ( vVel * flTime );
	};

	Vector LocalPos = gLocalPlayer->EyePosition( );
	if ( gLocalPlayer->Class( ) != CLASS_SPY )
	{
		bool DoGravity = !gLocalPlayer->IsOnGround( ) && gLocalPlayer->MoveType( ) == MOVETYPE_WALK;
		int Ticks = gLocalWeapon->ItemDefinitionIndex( ) == Soldier_t_TheMarketGardener ?
			DoGravity ? 5 : 4 : DoGravity ? 4 : 3;

		if ( gLocalPlayer->IsCharging( ) )
			Ticks = 2;

		for ( float t = 0; t < TICKS_TO_TIME( Ticks + WarpPredictTicks ); t += gGlobals->interval_per_tick )
			Extrapolate( LocalPos, gLocalPlayer->Velocity( ), t, DoGravity );
	}

	for (auto& Entity : gEntities)
	{
		auto Player = reinterpret_cast<CBasePlayer*>(Entity);
		if (!Player || Player == gLocalPlayer || Player->IsDormant())
			continue;

		if (!Player->IsAlive())
			continue;

		if (Player->IsPlayer() || Player->IsBuilding())
		{
			if ( gLocalWeapon->ItemDefinitionIndex( ) != Soldier_t_TheDisciplinaryAction )
			{
				if ( !Player->IsEnemy( ) )
					continue;
			}
			else
			{
				if ( Player->PlayerCond( ) & TF_COND_SPEED_BOOST )
					continue;
			}
		}
		else
			continue;

		float Distance = gLocalPlayer->WorldSpaceCenter().DistTo(Player->WorldSpaceCenter());
		if (Player->IsPlayer())
		{
			Vector WorldPos = Player->WorldSpaceCenter();
			if ( Player->IsEnemy( ) )
			{
				const auto& LagCompters = LagComp->GetClosestTick( Entity->GetIndex( ) );
				if ( !LagComp->IsTickValid( LagCompters ) )
					continue;

				WorldPos = LagCompters.WorldSpaceCenter;

				if ( gLocalPlayer->Class( ) != CLASS_SPY )
				{
					Distance = LocalPos.DistTo( WorldPos );

					if ( G->Aimbot.SavedWeaponID == TF_WEAPON_SWORD )
					{
						if ( Distance > ( gLocalPlayer->IsCharging( ) ? 80.f : 110.f ) )
							continue;
					}
					else
					{
						if ( Distance > 80.f )
							continue;
					}
				}
				else
				{
					Distance = LocalPos.DistTo( WorldPos );
					if ( Distance > 82.f )
						continue;
				}
			}
			else
			{
				Distance = LocalPos.DistTo( WorldPos );
				if ( Distance > 82.f )
					continue;
			}
			
		}
		else
		{
			if (Distance > 80.f)
				continue;
		}

		if (Player->IsPlayer())
		{
			if (Player->PlayerCond() & TFCond_Bonked && !Config->Aimbot->Targetting->TargetBonk)
				continue;

			if (Player->IsInvulnerable( ) && !Config->Aimbot->Targetting->TargetUber )
				continue;

			if (Player->IsSteamFriend() && !Config->Aimbot->Targetting->TargetFriends && 
				gLocalWeapon->ItemDefinitionIndex( ) != Soldier_t_TheDisciplinaryAction )
				continue;
		}

		Target_t Tamget;
		Tamget.Distance = Distance;
		Tamget.Player = Player;
		Targets.emplace_back(Tamget); // i thimk i drank tu mahc hehe
	}

	// cheese
	std::sort(Targets.begin(), Targets.end(), [](const Target_t& t1, const Target_t& t2) {
		return t1.Distance < t2.Distance;
	});
}

bool CMeleeBob::CanBackstab(const Vector& Source, const Vector& Destination, Vector Delta)
{
	Delta.z = 0.f;
	Delta.NormalizeInPlace();

	auto Forward = Vector();
	gMath->AngleVectors(Source, &Forward);

	Forward.z = 0;
	Forward.NormalizeInPlace();

	auto DestinationForward = Vector();
	gMath->AngleVectors(Destination, &DestinationForward);

	DestinationForward.z = 0;
	DestinationForward.NormalizeInPlace();

	if (Delta.DotProduct(Forward) <= 0.5f)
		return false;

	if (Delta.DotProduct(DestinationForward) <= 0.01f)
		return false;

	if (Forward.DotProduct(DestinationForward) <= -0.3f)
		return false;

	return true;
}

void CMeleeBob::Run( CUserCmd* pCmd )
{
	if ( !gLocalPlayer || !gLocalWeapon || !gLocalPlayer->IsAlive( ) || !Config->Aimbot->Melee->Enabled )
		return;

	if ( gLocalWeapon->Slot( ) != SLOT_MELEE || !Config->Aimbot->Global->Enabled )
		return;

	G->Aimbot.AimPoint = Vector();

	FindSuitableTargets( );
	if ( Targets.empty( ) )
		return;

	G->Aimbot.ClosestToCrosshair = Targets.front( ).Player;

	if ( !( ( GetAsyncKeyState( Config->Aimbot->Global->AimKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) && Config->Aimbot->Melee->RequireAimKey )
		return;

	CBasePlayer* BestTarget = nullptr;
	float SimulationTime = 0.f;
	auto BestTick = CLagComp::CLagCompRecord();
	Vector AimPoint = {};

	for (auto& Target : Targets)
	{
		auto Entity = Target.Player;
		if ( !Entity->IsAlive() )
			continue;

		if (!Entity->IsPlayer())
		{
			if (!TF2->IsVisible(gLocalPlayer, Entity, gLocalPlayer->EyePosition(), Entity->WorldSpaceCenter(), MASK_SHOT | CONTENTS_GRATE))
				continue;

			SimulationTime = -1.f;
			BestTarget = Entity;
			AimPoint = Entity->WorldSpaceCenter( );
		}
		else
		{
			if ( !Entity->IsEnemy( ) )
			{
				SimulationTime = Entity->SimulationTime();
				BestTarget = Entity;
				AimPoint = Entity->WorldSpaceCenter();
			}
			else
			{

				/*
					pretend theres an image of a troll face here
				*/
				const auto& LagCompters = LagComp->GetClosestTick( Entity->GetIndex( ) );

				if ( !LagComp->IsTickValid( LagCompters ) )
					continue;

				if ( !TF2->IsVisible( gLocalPlayer, Entity, gLocalPlayer->EyePosition( ), LagCompters.WorldSpaceCenter, MASK_SHOT | CONTENTS_GRATE ) )
					continue;

				if ( gLocalPlayer->Class( ) == CLASS_SPY )
				{
					if ( !CanBackstab( gMath->CalcAngle( gLocalPlayer->EyePosition( ), LagCompters.WorldSpaceCenter ),
						Entity->EyeAngles( ), LagCompters.WorldSpaceCenter - gLocalPlayer->EyePosition( ) ) )
						continue;
				}

				BestTarget = Entity;
				BestTick = LagCompters;
				AimPoint = LagCompters.WorldSpaceCenter;
			}
		}
	}

	if (BestTarget && BestTarget->IsAlive())
	{
		if ( Config->Misc->CritHack->ForceOnMelee && gLocalPlayer->Class() != CLASS_SPY && 
			BestTarget->IsEnemy( ) )
			G->CritHack->Force = true;

		if ( !Config->Misc->CritHack->ForceOnMelee && BestTarget->IsPlayer( ) 
			&& gLocalPlayer->Class( ) != CLASS_SPY && BestTarget->IsEnemy( ) )
		{
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

		pCmd->buttons |= IN_ATTACK;

		if (TF2->IsAttacking(pCmd))
		{
			G->Aimbot.AimPoint = AimPoint;
			pCmd->viewangles = gMath->CalcAngle( gLocalPlayer->EyePosition( ), AimPoint );

			if ( BestTarget->IsPlayer( ) && !BestTarget->IsEnemy( ) )
			{
				pCmd->tick_count = TIME_TO_TICKS( SimulationTime );
				return;
			}

			if (BestTarget->IsPlayer() && SimulationTime > -1.f)
			{
				LagComp->Restore( pCmd, BestTick, BestTarget );
			}
		}
	}

}