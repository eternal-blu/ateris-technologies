#include "AutoMedic.h"

struct CPotentialHealingTarget
{
	float HealthRatio = 0.f;
	int Priority = 0;
	CBasePlayer* Ent = nullptr;
};

std::vector<CPotentialHealingTarget> Targets = {};

CBasePlayer* CAutoMedic::ScanTargets()
{
	if ( !Targets.empty( ) )
		Targets.clear( );

	if (!gLocalPlayer || !gLocalWeapon || gLocalWeapon->Slot() != 1 || gLocalPlayer->Class() != CLASS_MEDIC)
		return nullptr;

	for (auto& Player : gPlayers)
	{
		if (Player && Player->IsAlive() && !Player->IsEnemy() && Player != gLocalPlayer && !Player->IsDormant())
		{
			// early out
			if ( gLocalWeapon->HealingTarget( ) && 
				gLocalWeapon->HealingTarget( )->GetIndex( ) == LockedTarget )
				return gLocalWeapon->HealingTarget( );

			float Distance = Player->WorldSpaceCenter( ).DistTo( gLocalPlayer->WorldSpaceCenter( ) );
			if (Distance > 500.f )
				continue;

			if ( !TF2->IsVisible( gLocalPlayer, Player, gLocalPlayer->EyePosition( ), Player->WorldSpaceCenter( ) ) )
				continue;

			CPotentialHealingTarget Pot;
			Pot.Ent = Player;

			if ( Player->GetIndex( ) == LockedTarget )
			{
				Pot.Priority = 20;
				Pot.HealthRatio = 0.f;
				Targets.push_back( Pot );
				break;
			}

			if ( Player->IsSteamFriend( ) )
			{
				Pot.Priority = 2;
				Pot.HealthRatio = Player->Health( ) / Player->GetMaxHealth( );
				Targets.push_back( Pot );
				continue;
			}

			Pot.Priority = 1;
			Pot.HealthRatio = Player->Health( ) / Player->GetMaxHealth( );
			Targets.push_back( Pot );
		}
	}

	if ( Targets.empty( ) )
		return nullptr;

	std::sort( Targets.begin( ), Targets.end( ), [ ] ( CPotentialHealingTarget t1, CPotentialHealingTarget t2 )
	{
		return t1.HealthRatio < t2.HealthRatio;
	} );

	std::sort( Targets.begin( ), Targets.end( ), [ ] ( CPotentialHealingTarget t1, CPotentialHealingTarget t2 )
	{
		return t1.Priority > t2.Priority;
	} );

	return Targets.front().Ent;
}

void CAutoMedic::Run(CUserCmd* pCmd)
{
	if (!gLocalPlayer || !gLocalWeapon || gLocalWeapon->Slot() != 1 || gLocalPlayer->Class() != CLASS_MEDIC)
		return;

	if (!Config->AutoMedic->Enabled)
		return;

	if ((GetAsyncKeyState(Config->AutoMedic->LockTarget) & 0x8000) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
	{
		if (gLocalWeapon->HealingTarget())
		{
			if (gLocalWeapon->HealingTarget()->GetIndex( ) != LockedTarget)
				LockedTarget = gLocalWeapon->HealingTarget()->GetIndex();
		}
		else
			LockedTarget = -1;
	}

	if ( auto Player = ScanTargets( ) )
	{
		if ( gLocalWeapon->HealingTarget( ) != Player )
		{
			if ( Player && Player->IsAlive( ) )
			{
				Vector angle = gMath->CalcAngle( gLocalPlayer->EyePosition( ), Player->WorldSpaceCenter( ) );
				pCmd->viewangles = angle;

				if ( pCmd->command_number % 6 == 0 )
					pCmd->buttons |= IN_ATTACK;
				else
					pCmd->buttons &= ~IN_ATTACK;
			}
		}
	}
}