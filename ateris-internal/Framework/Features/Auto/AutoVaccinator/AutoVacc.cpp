#include "AutoVacc.h"

void CAutoVacc::Run( CUserCmd* pCmd )
{
	if ( !Config->AutoVacc->Enabled )
		return;

	if ( !gThreats.empty( ) )
		gThreats.clear( );

	if ( !gLocalPlayer || !gLocalWeapon || !gLocalPlayer->IsAlive( ) )
	{
		if ( !gThreats.empty( ) )
			gThreats.clear( );
		return;
	}

	if ( gLocalWeapon->ItemDefinitionIndex( ) != Medic_s_TheVaccinator )
	{
		if ( !gThreats.empty( ) )
			gThreats.clear( );
		return;
	}

	FindThreats( );
	if ( gThreats.empty( ) )
		return;

	auto HealingTarget = gLocalWeapon->HealingTarget( );
	int WantedResistance = RESISTANCE_BULLET;
	bool ShouldPop = false;
	CBasePlayer* BestThreat = nullptr;
	CPriority* BestPriorityInstance = nullptr;
	std::string reason = "Unknown";

	bool ShouldScanPlayers = true;
	if ( gLocalPlayer->IsOnFire( ) || ( HealingTarget && HealingTarget->IsOnFire( ) ) )
	{
		float Ratio = 0.f;
		float LocalRatio = 0.f;
		if ( HealingTarget )
		{
			Ratio = static_cast< float >( HealingTarget->Health( ) ) /
				static_cast< float >( HealingTarget->GetMaxHealth( ) );
		}

		LocalRatio = static_cast< float >( gLocalPlayer->Health( ) ) /
			static_cast< float >( gLocalPlayer->GetMaxHealth( ) );

		if ( Ratio <= 0.85f || LocalRatio <= 0.85f )
		{
			ShouldPop = true;
			WantedResistance = RESISTANCE_FIRE;
			reason = "Lethal fire damage";
		}
	}

	for ( auto& gThreat : gThreats )
	{
		auto Threat = gThreat->Entity;
		if ( !Threat )
			continue;

		// We need to update this at all times
		gThreat->UpdateDistance( );

		if ( !gThreat->IsPlayer )
		{
			WantedResistance = GetResistanceForProjectile( Threat );
			CBasePlayer* Thrower = Threat->GetThrower( );
			if ( !Thrower )
				Thrower = Threat->GetOwnerEntity( )->As<CBasePlayer>( );

			if ( !Thrower )
				continue;

			if ( Thrower->Class( ) == CLASS_SNIPER )
			{
				if ( HealingTarget && HealingTarget->Velocity( ).Length( ) > 80.f )
				{
					if ( gThreat->Distance <= 255.f || gThreat->LocalDistance <= 255.f )
					{
						reason = "Projectile too close";
						ShouldPop = true;
					}
				}
				else
				{
					if ( gThreat->Distance <= 65.f || gThreat->LocalDistance <= 65.f )
					{
						reason = "Projectile too close";
						ShouldPop = true;
					}
				}
			}
			else
			{
				if ( Thrower->Class( ) == CLASS_MEDIC && !Config->AutoVacc->AutoPopCrossbow )
					continue;

				if ( gThreat->Distance <= 350.f || gThreat->LocalDistance <= 350.f )
				{
					reason = "Projectile too close";
					ShouldPop = true;
				}
			}

			if ( ShouldPop )
			{
				BestThreat = Threat->As<CBasePlayer>( );
				BestPriorityInstance = gThreat;
				ShouldScanPlayers = false;
				break;
			}
		}
		else if ( ShouldScanPlayers )
		{
			auto Player = Threat->As<CBasePlayer>( );

			if ( gThreat->Distance > 850.f && Player->Class( ) != CLASS_SNIPER )
				continue;

			WantedResistance = GetResistanceForClass( Player );
			if ( Player->Class( ) == CLASS_SNIPER && Player->IsZoomed( ) )
			{
				reason = "Headshot threat";
				ShouldPop = true;
				break;
			}

			switch ( Player->Class( ) )
			{
			case CLASS_PYRO:
			{
				if ( gThreat->Distance > 550.f )
					continue;

				if ( Config->AutoVacc->AutoPopShotgun && WantedResistance == RESISTANCE_BULLET &&
					gThreat->Distance < 145.f )
				{
					reason = "Shotgun too close";
					ShouldPop = true;
				}
				break;
			}
			case CLASS_SCOUT:
			case CLASS_ENGINEER:
			{
				if ( gThreat->Distance > 550.f )
					continue;

				if ( Config->AutoVacc->AutoPopShotgun && Player->ActiveWeapon( ) &&
					Player->ActiveWeapon( )->Slot( ) == 0 )
				{
					if ( Player->ActiveWeapon( )->ItemDefinitionIndex( ) == Scout_m_ForceANature ||
						Player->ActiveWeapon( )->ItemDefinitionIndex( ) == Scout_m_FestiveForceANature )
					{
						if ( gThreat->Distance < 225.f || gThreat->LocalDistance < 225.f )
						{
							reason = "Lethal DT distance";
							ShouldPop = true;
						}
					}
					else
					{
						if ( gThreat->Distance < 145.f || gThreat->LocalDistance < 145.f )
						{
							reason = "Shotgun too close";
							ShouldPop = true;
						}
					}
				}

				WantedResistance = RESISTANCE_BULLET;
				break;
			}
			case CLASS_HEAVY:
			{
				PlayerInfo_t info;
				gEngineClient->GetPlayerInfo( Threat->GetIndex( ), &info );

				if ( Config->AutoVacc->AutoPopHeavies )
				{
					if ( Config->AutoVacc->IgnoreFriendHeavies && 
						( Threat->As<CBasePlayer>( )->IsSteamFriend( ) 
							|| gPriorities[ info.friendsID ].Priority == 3 ) )
						continue;

					if ( gThreat->Distance > Config->AutoVacc->HeavyDistance )
						continue;

					reason = "Heavy too close";
					ShouldPop = true;
					break;
				}

				if ( Config->AutoVacc->AutoPopHigherHealthHeavies )
				{
					if ( Config->AutoVacc->IgnoreFriendHeavies && ( Threat->As<CBasePlayer>( )->IsSteamFriend( ) || gPriorities[ info.friendsID ].Priority == 3 ) )
						continue;

					float Health = 0.f;
					float EnemyHealth = ( float ) Player->Health( ) / ( float ) Player->GetMaxHealth( );

					if ( HealingTarget )
						Health = ( float ) HealingTarget->Health( ) / ( float ) HealingTarget->GetMaxHealth( );
					else
						Health = ( float ) gLocalPlayer->Health( ) / ( float ) gLocalPlayer->GetMaxHealth( );


					float Delta = Health - EnemyHealth;

					if ( Delta >= Config->AutoVacc->AutoPopMinHealth / 100.f )
					{
						reason = "Heavy with higher health";
						ShouldPop = true;
					}
					break;
				}

				break;
			}
			case CLASS_SOLDIER:
			{
				if ( gThreat->Distance > 650.f )
					continue;

				if ( Config->AutoVacc->AutoPopShotgun && ( gThreat->LocalDistance < 145.f ||
					gThreat->Distance < 145.f ) && WantedResistance == RESISTANCE_BULLET )
				{
					reason = "Shotgun too close";
					ShouldPop = true;
				}
				break;
			}
			}

			if ( ShouldPop )
			{
				BestThreat = Player;
				BestPriorityInstance = gThreat;
				break;
			}
		}
	}

	SwapResistance( pCmd, WantedResistance );
	
	if ( !BestThreat || !BestPriorityInstance )
		return;

	if ( ShouldPop )
		HandlePop( pCmd, BestThreat, WantedResistance, !BestPriorityInstance->IsPlayer, reason );
}

void CAutoVacc::HandlePop( CUserCmd* pCmd, CBasePlayer* BestThreat, int Resistance, bool IsProjectile, const std::string& Reason )
{
	if ( !BestThreat )
		return;

	auto HealingTarget = gLocalWeapon->HealingTarget( );
	bool ShouldPop = false;
	bool Projectile = IsProjectile;

	if ( !Projectile )
		if ( !BestThreat->IsAlive( ) || BestThreat->IsDormant( ) )
			return;

	if ( Projectile )
	{
		if ( BestThreat->WorldSpaceCenter( ).DistTo( gLocalPlayer->EyePosition( ) ) < 370.f )
			ShouldPop = true;

		if ( HealingTarget &&
			BestThreat->WorldSpaceCenter( ).DistTo( HealingTarget->EyePosition( ) ) < 370.f )
			ShouldPop = true;
	}

	if ( !ShouldPop )
		return;

	bool WasVisible = false;

	if ( TF2->IsVisible( gLocalPlayer, BestThreat, gLocalPlayer->EyePosition( ), BestThreat->WorldSpaceCenter( ) ) )
		WasVisible = true;

	if ( !WasVisible && HealingTarget )
	{
		if ( TF2->IsVisible( HealingTarget, BestThreat,
			BestThreat->WorldSpaceCenter( ), HealingTarget->WorldSpaceCenter( ) ) )
		{
			WasVisible = true;
		}
	}

	if ( !WasVisible )
		return;

	/*
		No point in popping if we are alr charged
	*/
	if ( Resistance == RESISTANCE_BULLET )
	{
		if ( ( HealingTarget && HealingTarget->IsBulletCharged( ) ) ||
			gLocalPlayer->IsBulletCharged( ) )
			return;
	}

	if ( Resistance == RESISTANCE_EXPLOSIVE && Config->AutoVacc->AutoPopExplosive )
	{
		if ( ( HealingTarget && HealingTarget->IsExplosiveCharged( ) ) ||
			gLocalPlayer->IsExplosiveCharged( ) )
			return;
	}

	if ( Resistance == RESISTANCE_FIRE )
	{
		if ( ( HealingTarget && HealingTarget->IsFireCharged( ) ) ||
			gLocalPlayer->IsFireCharged( ) )
			return;
	}

	static int PopDelay = 6;
	if ( gLocalWeapon->ChargeResistType( ) == Resistance && 
		gLocalWeapon->ChargeLevel() > 0.25f )
	{
		if ( PopDelay++ >= 6 )
		{
			// print reason
			// done
			std::string str = White + "|" + Logo + "Ateris" + White + "| - |" + Feature + "AV" + White + "| Popped because of " + Threat + Reason + White + ".";
			gClientMode->m_pChatElement->ChatPrintf( 0, 0, str.c_str( ) );

			pCmd->buttons |= IN_ATTACK2;
			PopDelay = 0;
		}
	}
}

void CAutoVacc::FindThreats( )
{
	CBasePlayer* Check = ( gLocalWeapon && gLocalWeapon->HealingTarget( ) ) ?
		gLocalWeapon->HealingTarget( ) : gLocalPlayer;

	if ( !Check )
		return;

	for ( auto& Entity : gPlayers )
	{
		if ( !Entity )
			continue;

		CPriority* Prior = new CPriority( );

		float Distance = Entity->WorldSpaceCenter( ).DistTo( Check->WorldSpaceCenter( ) );

		if ( Distance > 800.f && Entity->Class( ) != CLASS_SNIPER )
			continue;

		auto Player = Entity->As<CBasePlayer>( );
		if ( !Player->IsAlive( ) || !Player->IsEnemy( ) || Player->IsLocalPlayer( ) || Player->IsDormant( ) )
			continue;

		Prior->Distance = Distance;
		Prior->Entity = Entity;
		Prior->Threat = Entity->Class( ) != CLASS_SNIPER ? 10 : Player->IsZoomed() ? 11 : 10;
		Prior->IsPlayer = true;
		gThreats.emplace_back( Prior );
	}

	std::sort( gThreats.begin( ), gThreats.end( ), [ ] ( CPriority* t1, CPriority* t2 )
	{
		return t1->Distance < t2->Distance;
	} );

	// limit players
	if ( gThreats.size( ) > 4 )
		gThreats.erase( gThreats.begin( ) + 4, gThreats.end( ) );

	for ( auto& Entity : gProjectiles )
	{
		CBasePlayer* Thrower = Entity->GetThrower( );
		if ( !Thrower )
			Thrower = Entity->GetOwnerEntity( )->As<CBasePlayer>( );

		if ( !Thrower )
			continue;

		CPriority* Prior = new CPriority( );

		if ( Entity->IsDormant( ) || !Thrower->IsEnemy( ) 
			|| (Thrower->Class() != CLASS_DEMOMAN && Entity->EstimateAbsVelocity( ).Length( ) <= 10.f ))
			continue;

		float Distance = Entity->WorldSpaceCenter( ).DistTo( Check->WorldSpaceCenter( ) );
		float LocalDistance = Entity->WorldSpaceCenter( ).DistTo( gLocalPlayer->WorldSpaceCenter( ) );

		if ( Distance > 650.f )
			continue;

		Prior->Distance = Distance;
		Prior->LocalDistance = LocalDistance;
		Prior->Entity = Entity;
		Prior->IsPlayer = false;
		Prior->Threat = 25;
		gThreats.emplace_back( Prior );
	}

	std::sort( gThreats.begin( ), gThreats.end( ), [ ] ( CPriority* t1, CPriority* t2 )
	{
		return t1->Threat > t2->Threat;
	} );

	std::sort( gThreats.begin( ), gThreats.end( ), [ ] ( CPriority* t1, CPriority* t2 )
	{
		return t1->Distance < t2->Distance;
	} );
}

void CAutoVacc::SwapResistance( CUserCmd* pCmd, int Wanted )
{
	if ( FindSwapsNeededFor( Wanted ) > 0 )
	{
		if (pCmd->command_number % 6 == 0 )
			pCmd->buttons |= IN_RELOAD;
		else
			pCmd->buttons &= ~IN_RELOAD;
	}
}

int CAutoVacc::FindSwapsNeededFor( int WantedResistance )
{
	int StartResistance = gLocalWeapon->ChargeResistType( );

	int Resistance = 0;
	for ( int i = 0; i <= 6; i++ )
	{
		if ( Resistance >= 3 )
			Resistance = 0;

		if ( StartResistance + Resistance == WantedResistance )
			break;

		Resistance++;
	}

	return Resistance;
}

ResistanceTypes CAutoVacc::GetResistanceForClass( CBasePlayer* Player )
{
	if ( !Player || !Player->IsAlive( ) )
		return RESISTANCE_BULLET;

	CBaseCombatWeapon* Weapon = Player->ActiveWeapon( );
	if ( !Weapon )
		return RESISTANCE_BULLET;

	switch ( Player->Class( ) )
	{
	case CLASS_SCOUT:
	case CLASS_ENGINEER:
	case CLASS_MEDIC:
	case CLASS_HEAVY:
	case CLASS_SPY:
		return RESISTANCE_BULLET;
	case CLASS_SOLDIER:
		if ( Player->WorldSpaceCenter( ).DistTo( gLocalPlayer->WorldSpaceCenter( ) ) > 125.f )
			return RESISTANCE_BULLET;

		if ( Weapon->WeaponID( ) == TF_WEAPON_SHOTGUN_SOLDIER )
			return RESISTANCE_BULLET;

		return RESISTANCE_EXPLOSIVE;
	case CLASS_DEMOMAN:
		return RESISTANCE_EXPLOSIVE;
	case CLASS_PYRO:
		if ( Player->WorldSpaceCenter( ).DistTo( gLocalPlayer->WorldSpaceCenter( ) ) > 175.f )
			return RESISTANCE_BULLET;

		if ( Weapon->WeaponID( ) == TF_WEAPON_SHOTGUN_PYRO )
			return RESISTANCE_BULLET;

		return RESISTANCE_FIRE;
	default: return RESISTANCE_BULLET;
	}
}

ResistanceTypes CAutoVacc::GetResistanceForProjectile( CBaseEntity* Projectile )
{
	if ( !Projectile )
		return RESISTANCE_BULLET;

	CBasePlayer* Thrower = Projectile->GetThrower( );
	if ( !Thrower )
		Thrower = Projectile->GetOwnerEntity( )->As<CBasePlayer>( );

	if ( !Thrower )
		return RESISTANCE_BULLET;

	if ( ( DWORD ) Projectile <= 0x8000 )
		return RESISTANCE_BULLET;

	switch ( Thrower->Class() )
	{
	case CLASS_SCOUT:
	case CLASS_SNIPER:
		return RESISTANCE_BULLET;
	case CLASS_MEDIC:
		return RESISTANCE_BULLET;
	case CLASS_PYRO:
		return RESISTANCE_FIRE;
	case CLASS_SOLDIER:
	case CLASS_DEMOMAN:
		return RESISTANCE_EXPLOSIVE;
	default: 
		if ( Thrower->IsBuilding( ) )
			return RESISTANCE_EXPLOSIVE;
		return RESISTANCE_BULLET;
		break;
	}

}

void CPriority::UpdateDistance( )
{
	if ( !Entity )
		return;

	if ( gLocalPlayer && gLocalWeapon )
	{
		LocalDistance = Entity->WorldSpaceCenter( ).DistTo( gLocalPlayer->WorldSpaceCenter( ) );
		if ( gLocalWeapon->HealingTarget( ) )
			Distance = gLocalWeapon->HealingTarget( )->WorldSpaceCenter( ).DistTo( Entity->WorldSpaceCenter( ) );
	}
}