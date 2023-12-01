#include "Projectile.h"
#include "../../../Globals.h"

bool IsEntityValid(CBasePlayer* Entity)
{
	if ( Entity->IsTank( ) || Entity->IsSentryBuster( ) )
		return true;

	return (Entity->IsPlayer() && Entity->IsAlive() && Config->Aimbot->Targetting->TargetPlayers) ||
		(Entity->IsBuilding() && Config->Aimbot->Targetting->TargetBuildings);
}

std::vector<Vector> Points(CBasePlayer* Player)
{
	if ( !Player )
		return {  };

	switch ( gLocalPlayer->Class( ) )
	{
	case CLASS_SNIPER:
	{
		if ( Player->Origin( ).DistTo( gLocalPlayer->Origin( ) ) > 980.f )
		{
			return { Player->GetHitboxPos( 0 ) - Vector( 0,0,3 ) };
		}

		return { Player->GetHitboxPos( 0 ) };
		break;
	}
	case CLASS_MEDIC:
	case CLASS_PYRO:
	case CLASS_ENGINEER:
	{
		return { Player->WorldSpaceCenter( ) };
		break;
	}
	case CLASS_SCOUT:
	{
		return { Player->Origin( ) + Vector( 0,0,12 ) };// do not question this
		break;
	}
	case CLASS_DEMOMAN:
	{
		if ( gLocalWeapon->Slot( ) == SLOT_SECONDARY )
			return { Player->WorldSpaceCenter( ) };

		return { Player->Origin( ) + Vector( 0,0, 18 ),
			Player->WorldSpaceCenter( ), Player->GetHitboxPos( 0 ) };
		break;
	}
	default:
	{
		if ( gLocalWeapon->ItemDefinitionIndex( ) == Soldier_s_TheRighteousBison )
			return { Player->WorldSpaceCenter( ) };

		return { Player->Origin( ) + Vector( 0,0, 18 ), 
			Player->WorldSpaceCenter( ), Player->GetHitboxPos( 0 ) };
		break;
	}
	}
}

void CProjectiles::HandleGaza( CUserCmd* pCmd )
{
	if ( ( GetAsyncKeyState( Config->Aimbot->Projectile->GazaBombingKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
	{
		float CurrentCharge = gGlobals->curtime - gLocalWeapon->ChargeBeginTime( );
		float MaxCharge = 0.0037f;

		if ( gLocalWeapon->ItemDefinitionIndex( ) == Demoman_s_TheQuickiebombLauncher )
			MaxCharge = 0.014f;

		if ( CurrentCharge > 0.1f )
			return;

		if ( CurrentCharge >= MaxCharge )
			pCmd->buttons &= ~IN_ATTACK;

		G->Doubletap.Recharge = true;
	}
}

void CProjectiles::Run( CUserCmd* pCmd )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !gLocalWeapon || !Config->Aimbot->Global->Enabled || G->Doubletap.ToSend > 0 )
		return;

	MovementSim->OldPositions.clear( );
	MovementSim->PredictedPositions.clear( );
	G->Aimbot.PredictedPos = Vector( );

	static bool HasSwapped = false;
	static bool WasCharging = false;

	if ( HasSwapped )
	{
		gEngineClient->ClientCmd_Unrestricted( "slot2" );
		HasSwapped = false;
		return;
	}

	if ( !TF2->IsProjectileWeapon( gLocalWeapon ) )
		return;

	if ( gLocalWeapon->ItemDefinitionIndex( ) == Soldier_m_RocketJumper 
		|| gLocalWeapon->ItemDefinitionIndex( ) == Demoman_s_StickyJumper )
		return;

	static auto OldWeaponId = 0;
	if ( G->Aimbot.SavedWeaponID != OldWeaponId )
	{
		if ( !GetProjectileInfo( ) )
			return;

		OldWeaponId = G->Aimbot.SavedWeaponID;
	}
	
	HandleGaza( pCmd );

	if ( !( ( GetAsyncKeyState( Config->Aimbot->Global->AimKey ) & 0x8000 ) 
		&& Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) )
		return;

	// UHHHHHHHH todo
	if ( !G->ProjectileCamera->SavedAimPoint.IsZero( ) )
	{

	}

	FindSuitableTargets( );

	if ( Targets.empty( ) )
	{
		if ( WasCharging && G->ProjectileCamera->SavedAimPoint.IsZero( ) )
		{
			if ( ( gLocalWeapon->Slot( ) == 1 && gLocalPlayer->Class( ) == CLASS_DEMOMAN )
				|| G->Aimbot.SavedWeaponID == TF_WEAPON_COMPOUND_BOW )
			{
				pCmd->buttons |= IN_ATTACK;
				HasSwapped = true;
				gEngineClient->ClientCmd_Unrestricted( "slot3" );
				WasCharging = false;
			}
		}
		return;
	}

	if ( ( ( gLocalWeapon->Slot( ) == 1 && gLocalPlayer->Class( ) == CLASS_DEMOMAN ) ||
		( gLocalWeapon->Slot( ) == 0 && gLocalPlayer->Class( ) == CLASS_SNIPER ) ) )
	{
		pCmd->buttons |= IN_ATTACK;
		WasCharging = true;
	}

	CBasePlayer* Current = Targets.front( ).This;
	if ( !Current )
		return;

	Vector Angle {};
	float PredictionTime = 0.f;
	if ( !SolveTarget( Current, Angle, PredictionTime ) )
	{
		if ( gLocalWeapon->Slot( ) == 1 && gLocalPlayer->Class( ) == CLASS_DEMOMAN )
		{
			float charge = gMath->ReMapValClamped( gGlobals->curtime - gLocalWeapon->ChargeBeginTime( ),
				0.0f,
				TF2->AttribHookFloat( 4.0f, "stickybomb_charge_rate", gLocalWeapon, nullptr, false ),
				900.0f, 2400.0f );

			if ( gLocalWeapon->ChargeBeginTime( ) <= 0.0f ) 
				charge = 900.0f;

			if ( charge > 2200.f ) // do not question this
			{
				HasSwapped = true;
				gEngineClient->ClientCmd_Unrestricted( "slot3" );
			}
		}

		return;
	}

	//HandleGaza( pCmd );
	if ( Current->IsPlayer( ) )
	{
		PlayerInfo_t info;
		gEngineClient->GetPlayerInfo( Current->GetIndex( ), &info );

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

	if ( G->Aimbot.SavedWeaponID == TF_WEAPON_COMPOUND_BOW && Config->Aimbot->Projectile->WaitForHeadshot )
	{
		float Charge = gGlobals->curtime - gLocalWeapon->ChargeBeginTime( );
		float ChargeDamage = gMath->ReMapValClamped( Charge, 0.0f, 1.0f, 150.f, 360.f );
		if ( Current->Health( ) <= 360 )
			if ( ChargeDamage < Current->Health( ) )
				return;
	}

	if ( gLocalWeapon->Slot( ) == SLOT_SECONDARY && 
		( ( GetAsyncKeyState( Config->Aimbot->Projectile->GazaBombingKey ) & 0x8000 ) 
			&& Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) )
	{
		if ( !gLocalWeapon->ChargeBeginTime( ) && G->Doubletap.Charged == 22 )
		{
			pCmd->buttons |= IN_ATTACK;
		}

		if ( gLocalWeapon->ChargeBeginTime( ) > 0.f && G->Doubletap.Charged < 22 )
		{
			pCmd->buttons &= ~IN_ATTACK;
		}
	}

	if ( gLocalWeapon->Clip1( ) > 0 )
		gLocalWeapon->AbortReload( );

	static float NextAvaibleShot = -1.f;
	static float InAttackTime = 0.f;
	static CBasePlayer* OldTarget = Current;

	if ( OldTarget != Current )
	{
		NextAvaibleShot = -1.f;
		OldTarget = Current;
	}

	if ( !Config->Aimbot->Projectile->DelayShot ||
		gLocalWeapon->ItemDefinitionIndex() == Soldier_m_TheAirStrike)
	{
		NextAvaibleShot = -1.f;
	}

	if ( gEngineClient->Time( ) > NextAvaibleShot )
	{
		pCmd->buttons |= IN_ATTACK;
		if ( InAttackTime < gEngineClient->Time( ) )
			InAttackTime = gEngineClient->Time( ) + 1.2f + gEngineClient->GetNetChannelInfo( )->GetLatency( 0 );
	}

	if ( ( G->Aimbot.SavedWeaponID == TF_WEAPON_PIPEBOMBLAUNCHER || G->Aimbot.SavedWeaponID == TF_WEAPON_COMPOUND_BOW ) && gLocalWeapon->ChargeBeginTime( ) > 0.0f )
		pCmd->buttons &= ~IN_ATTACK;

	if ( SavedProjectileInfo.Pipes )
	{
		Vector vAngle = {}, Forward = {}, Up = {};
		gMath->AngleVectors( Angle, &Forward, nullptr, &Up );
		Vector vVelocity = ( Forward * SavedProjectileInfo.Speed ) - ( Up * 200.0f );
		gMath->VectorAngles( vVelocity, vAngle );
		Angle.x = vAngle.x;
	}

	bool BypassIsAttacking = false;
	if ( gLocalWeapon->ItemDefinitionIndex( ) == Scout_s_TheFlyingGuillotine ||
		gLocalWeapon->ItemDefinitionIndex( ) == Scout_s_TheFlyingGuillotineG )
	{
		if ( InAttackTime <= gEngineClient->Time( ) )
		{
			BypassIsAttacking = true;
		}
	}

	bool IsAttacking = TF2->IsAttacking( pCmd ) || BypassIsAttacking;

	if ( IsAttacking )
	{
		pCmd->viewangles = Vector( Angle.x, Angle.y, 0.f );
		pCmd->viewangles -= G->Aimbot.PunchAngle;
		gMath->ClampAngles( pCmd->viewangles );

		float Engine = gEngineClient->Time( );
		MovementSim->ShotTime = gEngineClient->Time( ) + PredictionTime
			+ ( ( gLocalPlayer->Class( ) == CLASS_PYRO ) ? 0.45f : 2.55f );
		MovementSim->ClearCache = true;

		float Division = 1.f;
		if ( gLocalPlayer->IsInCondition( ETFCond::TF_COND_BLASTJUMPING ) )
			Division = 2.f;

		if ( !( gLocalPlayer->Class( ) == CLASS_PYRO && gLocalWeapon->Slot( ) == SLOT_PRIMARY ) )
			NextAvaibleShot = gEngineClient->Time( ) + std::clamp( PredictionTime / Division, 0.0f, 1.4f );
		
	}
}

bool CProjectiles::SolveTarget( CBasePlayer* Target, Vector& AngleOut, float& TimeOut )
{
	if ( !Target )
		return false;

	Vector LocalPos = gLocalPlayer->EyePosition();
	if ( SavedProjectileInfo.Pipes )
	{
		const Vector Offset = Vector( 16.f, 8.f, -6.f );
		TF2->GetProjectileFireSetup( gLocalPlayer, gEngineClient->GetViewAngles( ), Offset, &LocalPos );
	}

	if ( Target->IsPlayer( ) )
	{
		if ( !MovementSim->Init( Target ) )
			return false;

		for ( float n = 0.f; n < Config->Aimbot->Projectile->MaxTime; n += gGlobals->interval_per_tick )
		{
			float TimeToTarget = 0.0f;

			MovementSim->OldPositions.push_back( MovementSim->GetOrigin( ) );
			MovementSim->RunTick( );
			const Vector Position = MovementSim->GetOrigin( );
			MovementSim->PredictedPositions.push_back( Position );

			if ( !SolveAngle( LocalPos, Position, AngleOut, TimeToTarget ) )
				continue;

			int TargetTick = TIME_TO_TICKS( TimeToTarget + gEngineClient->GetNetChannelInfo( )->GetLatency( 2 ) );

			if ( G->Aimbot.SavedWeaponID == TF_WEAPON_PIPEBOMBLAUNCHER )
			{
				auto dettime { TF2->AttribHookFloat( 0.8f, "sticky_arm_time", gLocalPlayer, 0 ) };
				if ( TICKS_TO_TIME( TargetTick ) < dettime )
					TargetTick += TIME_TO_TICKS( fabsf( TimeToTarget - dettime ) );
			}

			if ( TargetTick == TIME_TO_TICKS(n) || TargetTick == TIME_TO_TICKS(n) - 1 )
			{
				bool FoundPoint = false;
				if ( !Target->IsOnGround( ) )
				{
					FoundPoint = HandleSplashBot( Target, LocalPos, Position, AngleOut, TimeToTarget );
					if ( FoundPoint )
					{
						TimeOut = TimeToTarget;
						MovementSim->Restore( );
						return true;
					}
				}

				if ( !FoundPoint )
				{
					for ( auto& p : Points( Target ) )
					{
						Vector Point = Position + ( p - Target->Origin( ) );
						if ( !SolveAngle( LocalPos, Point, AngleOut, TimeToTarget ) )
							continue;

						if ( CanSee( LocalPos, Point, Target, AngleOut, TimeToTarget ) )
						{
							TimeOut = TimeToTarget;
							MovementSim->Restore( );
							FoundPoint = true;
							return true;
						}
						else
						{
							if ( SavedProjectileInfo.Pipes )
							{
								MovementSim->Restore( );
								return false;
							}
						}
					}
				}

				if ( !FoundPoint )
				{
					FoundPoint = HandleSplashBot( Target, LocalPos, Position, AngleOut, TimeToTarget );
					if ( FoundPoint )
					{
						TimeOut = TimeToTarget;
						MovementSim->Restore( );
						return true;
					}

					// none found, fuck off then
					MovementSim->Restore( );
					return false;
				}
			}
		}
		
		MovementSim->Restore( );
		return false;
	}
	else
	{
		float TimeToTarget = 0.f;
		Vector Position = Target->WorldSpaceCenter( );
		if ( !SolveAngle( LocalPos, Position, AngleOut, TimeToTarget ) )
			return false;

		bool FoundPoint = false;

		if ( CanSee( LocalPos, Position, Target, AngleOut, TimeToTarget ) )
		{
			TimeOut = TimeToTarget;
			MovementSim->Restore( );
			FoundPoint = true;
			return true;
		}

		FoundPoint = HandleSplashBot( Target, LocalPos, Position, AngleOut, TimeToTarget );
		if ( FoundPoint )
		{
			TimeOut = TimeToTarget;
			MovementSim->Restore( );
			return true;
		}
	}
	return false;
}

bool CProjectiles::CanSee( const Vector From, const Vector To, CBasePlayer* Target, Vector AngleTo, float TimeTo )
{
	if ( !gLocalPlayer || !gLocalWeapon )
		return false;

	Vector Local = From;

	switch ( G->Aimbot.SavedWeaponID )
	{
	case TF_WEAPON_ROCKETLAUNCHER:
	case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
	case TF_WEAPON_FLAREGUN:
	case TF_WEAPON_FLAREGUN_REVENGE:
	case TF_WEAPON_SYRINGEGUN_MEDIC:
	case TF_WEAPON_FLAME_BALL:
	case TF_WEAPON_CROSSBOW:
	case TF_WEAPON_FLAMETHROWER:
	case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
	{
		if ( gLocalWeapon->ItemDefinitionIndex( ) != Soldier_m_TheOriginal )
		{
			Vector Offset = { 23.5f, 12.0f, -3.0f };

			if ( gLocalPlayer->IsDucked( ) )
				Offset.z = 8.0f;

			TF2->GetProjectileFireSetup( gLocalPlayer, AngleTo, Offset, &Local );
		}

		break;
	}
	case TF_WEAPON_COMPOUND_BOW:
	{
		Vector Offset = { 20.5f, 12.0f, -3.0f };

		if ( gLocalPlayer->IsDucked( ) )
			Offset.z = 8.0f;

		TF2->GetProjectileFireSetup( gLocalPlayer, AngleTo, Offset, &Local );

		break;
	}
	default: break;
	}

	if ( SavedProjectileInfo.GravityMod )
	{
		return CanArcReach( From, To, Target, AngleTo, TimeTo );
	}
	else
	{
		return TraceProjectile( Target, Local, To );
	}
}

bool CProjectiles::TraceProjectile( CBasePlayer* Target, const Vector& From, const Vector& To, unsigned int MASK )
{
	Trace_t Trace = { };
	CTraceFilterWorldCustom Filter = {};

	// Do not fucking question this or i will drain ur balls
	if ( Target == gLocalPlayer )
	{
		Filter.m_pTarget = gLocalPlayer;
		Filter.m_pSkip = Target;
	}
	else
	{
		Filter.m_pTarget = Target;
		Filter.m_pSkip = gLocalPlayer;
	}

	Vector Mins = Vector( -4.0f, -4.0f, -4.0f );
	Vector Maxs = Vector( 4.0f, 4.0f, 4.0f );

	TF2->TraceHull( From, To, Mins, Maxs, &Filter, &Trace, MASK );
	return Trace.Ent == Target || ( Trace.Fraction > 0.99f && !Trace.AllSolid && !Trace.StartSolid );
}

bool CProjectiles::CanArcReach( const Vector From, const Vector To, CBasePlayer* Target, Vector AngleTo, float TimeTo )
{
	if ( !Target || !gLocalPlayer || !gLocalWeapon )
		return false;

	ProjectileInfo Info = {};
	if ( !ProjectileSim->GetInfo( AngleTo, Info ) )
		return false;

	Vector Mins { -6.0f, -6.0f, -6.0f };
	Vector Maxs { 6.0f, 6.0f, 6.0f };

	switch ( Info.Type )
	{
	case TF_PROJECTILE_PIPEBOMB:
	case TF_PROJECTILE_PIPEBOMB_REMOTE:
	case TF_PROJECTILE_PIPEBOMB_PRACTICE:
	case TF_PROJECTILE_CANNONBALL:
	{
		Mins = { -8.0f, -8.0f, -8.0f };
		Maxs = { 8.0f, 8.0f, 20.0f };
		break;
	}
	case TF_PROJECTILE_FLARE:
	{
		Mins = { -8.0f, -8.0f, -8.0f };
		Maxs = { 8.0f, 8.0f, 8.0f };
		break;
	}
	default:
	{
		break;
	}
	}

	if ( gLocalWeapon->ItemDefinitionIndex( ) == Demoman_m_TheLochnLoad )
		Info.Speed += 45.0f;

	if ( !ProjectileSim->Init( Info, true ) )
		return false;

	CTraceFilterWorldCustom Filter {};
	Filter.m_pTarget = Target;
	Filter.m_pSkip = gLocalPlayer;

	for ( float n = 0.f; 
		n < Config->Aimbot->Projectile->MaxProjectileTime; 
		n += gGlobals->interval_per_tick )
		// we can extend this
		// for optimization reasons, we dont need it to be fully accurate.
	{
		Vector Pre = ProjectileSim->GetOrigin( );
		ProjectileSim->Simulate( );
		Vector Post = ProjectileSim->GetOrigin( );

		Trace_t Trace {};

		TF2->TraceHull( Pre, Post, Mins, Maxs, &Filter, &Trace, MASK_SOLID );
		if ( Trace.Ent == Target )
			return true;

		if ( Trace.DidHit( ) )
		{
			if ( Info.Position.DistTo( Trace.EndPosition ) > Info.Position.DistTo( To ) )
				return true;

			if ( Trace.EndPosition.DistTo( To ) > 40.0f )
				return false;

			TF2->Trace( Trace.EndPosition, To, &Filter, &Trace, MASK_SOLID );
			return !Trace.DidHit( ) || Trace.Ent == Target;
		}
	}

	return true;
}

bool CProjectiles::HandleSplashBot( CBasePlayer* Target, const Vector Start, const Vector Position, Vector& AngleOut, float& TimeOut )
{
	if ( !Config->Aimbot->Projectile->SplashBot )
		return false;

	if ( !( ( gLocalPlayer->Class( ) == CLASS_SOLDIER && gLocalWeapon->Slot( ) == SLOT_PRIMARY )
		|| ( gLocalPlayer->Class( ) == CLASS_PYRO && gLocalWeapon->ItemDefinitionIndex( ) != Pyro_s_TheScorchShot ) ) )
		return false;

	if ( gLocalWeapon->ItemDefinitionIndex( ) == Soldier_m_TheDirectHit )
		return false;

	Vector Local = Start;
	// make sure the offsetting is correct, unless u like wood
	switch ( G->Aimbot.SavedWeaponID )
	{
	case TF_WEAPON_ROCKETLAUNCHER:
	case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
	case TF_WEAPON_FLAREGUN:
	case TF_WEAPON_FLAREGUN_REVENGE:
	{
		if ( gLocalWeapon->ItemDefinitionIndex( ) != Soldier_m_TheOriginal )
		{
			Vector Offset = { 23.5f, 12.0f, -3.0f };

			if ( gLocalPlayer->IsDucked( ) )
				Offset.z = 8.0f;

			// we have already solved the angle before the call to this func
			TF2->GetProjectileFireSetup( gLocalPlayer, AngleOut, Offset, &Local );
		}
		break;
	}
	default: break;
	}

	const Vector Center = Position + ( Target->WorldSpaceCenter( ) - Target->Origin( ) );
	auto Sphere = ComputeSphere( Config->Aimbot->Projectile->SplashRadius * 1.5f, 
		Config->Aimbot->Projectile->SplashPoints );
	std::sort( Sphere.begin( ), Sphere.end( ), [ & ] ( const Vector& a, const Vector& b ) -> bool
	{
		return ( Center + a ).DistTo( Center ) < ( Center + b ).DistTo( Center );
	} );

	if ( Sphere.empty( ) )
		return false;

	for ( const auto& SpherePoint : Sphere )
	{
		auto Point = SpherePoint + Center;

		CTraceFilterWorldCustom Filter;
		CGameTrace Trace;
		Ray_t Ray;
		Filter.m_pSkip = Target;

		Ray.Init( Center, Point, { -4.0f, -4.0f, -4.0f }, { 4.0f, 4.0f, 4.0f } );
		gEngineTrace->TraceRay( Ray, MASK_SOLID & ~( CONTENTS_GRATE ), &Filter, &Trace );
		if ( !Trace.DidHit( ) )
			continue;

		// we can keep this point
		if ( !TraceProjectile( gLocalPlayer, Center, Trace.EndPosition, MASK_SOLID & ~( CONTENTS_GRATE ) ) )
			continue;

		// check if we cant see the point
		if ( !TraceProjectile( Target, Local, Trace.EndPosition, MASK_SOLID) )
			continue;

		if ( !SolveAngle( Local, Trace.EndPosition, AngleOut, TimeOut ) )
			continue;

		return true;
	}

	return false;
}

bool CProjectiles::SolveAngle( const Vector From, const Vector To, Vector& Out, float& TimeOut )
{
	if ( !gLocalWeapon )
		return false;

	Vector v = To - From;
	float dx = sqrt( v.x * v.x + v.y * v.y );
	float dy = v.z;
	float v0 = G->Aimbot.ReflectVelocity > 0.f ? G->Aimbot.ReflectVelocity : SavedProjectileInfo.Speed;
	float g = 800.f * SavedProjectileInfo.GravityMod; // this will be incorrect, but who cares

	if ( g )
	{
		if ( SavedProjectileInfo.Pipes )
			if ( v0 > 2000.0f )
				v0 = k_flMaxVelocity;

		auto root { v0 * v0 * v0 * v0 - g * ( g * dx * dx + 2.0f * dy * v0 * v0 ) };

		if ( root < 0.0f )
		{
			return false;
		}

		Out = { -RAD2DEG( atanf( ( v0 * v0 - sqrtf( root ) ) / ( g * dx ) ) ), RAD2DEG( atan2f( v.y, v.x ) ), 0.0f };
		TimeOut = dx / ( cosf( -DEG2RAD( Out.x ) ) * v0 );

		if ( SavedProjectileInfo.Pipes )
		{
			float magic = 0.0f;

			if ( G->Aimbot.SavedWeaponID == TF_WEAPON_GRENADELAUNCHER )
				magic = gLocalWeapon->ItemDefinitionIndex( ) == Demoman_m_TheLochnLoad ? 0.07f : 0.11f;

			if ( G->Aimbot.SavedWeaponID == TF_WEAPON_PIPEBOMBLAUNCHER )
				magic = 0.16f;

			if ( G->Aimbot.SavedWeaponID == TF_WEAPON_CANNON )
				magic = 0.35f;

			v0 -= ( v0 * TimeOut ) * magic;

			auto root { v0 * v0 * v0 * v0 - g * ( g * dx * dx + 2.0f * dy * v0 * v0 ) };

			if ( root < 0.0f )
			{
				return false;
			}

			Out = { -RAD2DEG( atanf( ( v0 * v0 - sqrtf( root ) ) / ( g * dx ) ) ), 
				RAD2DEG( atan2f( v.y, v.x ) ), 0.0f };

			TimeOut = dx / ( cosf( -DEG2RAD( Out.x ) ) * v0 );
		}
	}
	else
	{
		Out = gMath->CalcAngle( From, To );
		TimeOut = From.DistTo( To ) / v0;
	}

	if ( G->Aimbot.ReflectVelocity > 0.f )
		TimeOut = G->Aimbot.ReflectVelocity;

	if ( SavedProjectileInfo.Pipes )
	{
		if ( G->Aimbot.SavedWeaponID == TF_WEAPON_CANNON )
		{
			if ( TimeOut > 0.95f )
				return false;
		}
		else
		{
			if ( gLocalWeapon->ItemDefinitionIndex( ) == Demoman_m_TheIronBomber )
			{
				if ( TimeOut > 1.4f )
					return false;
			}
			else
				if ( TimeOut > 2.0f )
					return false;
		}
	}

	if ( G->Aimbot.ReflectVelocity <= 0.f )
	{
		if ( ( G->Aimbot.SavedWeaponID == TF_WEAPON_FLAME_BALL ||
			G->Aimbot.SavedWeaponID == TF_WEAPON_FLAMETHROWER ) && TimeOut > 0.18f )
			return false;
	}

	return true;
}

Vector CProjectiles::GetShootPos( )
{
	if ( !gLocalPlayer || !gLocalWeapon)
		return {};

	Vector out = gLocalPlayer->EyePosition( );

	switch ( G->Aimbot.SavedWeaponID )
	{
	case TF_WEAPON_ROCKETLAUNCHER:
	case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
	case TF_WEAPON_FLAREGUN:
	case TF_WEAPON_FLAREGUN_REVENGE:
	case TF_WEAPON_SYRINGEGUN_MEDIC:
	case TF_WEAPON_FLAME_BALL:
	case TF_WEAPON_CROSSBOW:
	case TF_WEAPON_FLAMETHROWER:
	case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
	{
		if ( gLocalWeapon->ItemDefinitionIndex( ) != Soldier_m_TheOriginal )
		{
			Vector Offset = { 23.5f, 12.0f, -3.0f };

			if ( gLocalPlayer->IsDucked( ) )
				Offset.z = 8.0f;

			TF2->GetProjectileFireSetup( gLocalPlayer, gEngineClient->GetViewAngles( ), Offset, &out );
		}

		break;
	}
	case TF_WEAPON_COMPOUND_BOW:
	{
		Vector Offset = { 20.5f, 12.0f, -3.0f };

		if ( gLocalPlayer->IsDucked( ) )
			Offset.z = 8.0f;

		TF2->GetProjectileFireSetup( gLocalPlayer, gEngineClient->GetViewAngles( ), Offset, &out );

		break;
	}

	default: break;
	}

	return out;
}

bool CProjectiles::GetProjectileInfo( )
{
	SavedProjectileInfo = {};

	if ( !gLocalPlayer || !gLocalWeapon )
		return false;

	switch ( G->Aimbot.SavedWeaponID )
	{
	case TF_WEAPON_ROCKETLAUNCHER:
	case TF_WEAPON_PARTICLE_CANNON:
	case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
	{
		SavedProjectileInfo = { 1100.0f, 0.0f };
		SavedProjectileInfo.Speed = TF2->AttribHookFloat( SavedProjectileInfo.Speed, "mult_projectile_speed", gLocalWeapon, 0 );

		if ( int rocket_specialist { static_cast< int >( TF2->AttribHookFloat( 0.0f, "rocket_specialist", gLocalPlayer, 0 ) ) } )
		{
			SavedProjectileInfo.Speed *= gMath->ReMapValClamped( static_cast< float >( rocket_specialist ), 1.0f, 4.0f, 1.15f, 1.6f );
			SavedProjectileInfo.Speed = std::min( SavedProjectileInfo.Speed, 3000.0f );
		}
		break;
	}
	case TF_WEAPON_GRENADELAUNCHER:
	{
		SavedProjectileInfo = { 1200.0f, 1.0f, true };
		SavedProjectileInfo.Speed = TF2->AttribHookFloat( SavedProjectileInfo.Speed, "mult_projectile_speed", gLocalWeapon, 0 );
		break;
	}
	case TF_WEAPON_PIPEBOMBLAUNCHER:
	{
		float flChargeBeginTime = gLocalWeapon->ChargeBeginTime();
		float flCharge = TICKS_TO_TIME( gLocalPlayer->TickBase( ) ) - flChargeBeginTime;

		if ( flChargeBeginTime )
		{
			SavedProjectileInfo.Speed = gMath->ReMapValClamped
			(
				flCharge,
				0.0f,
				TF2->AttribHookFloat( 4.0f, "stickybomb_charge_rate", gLocalWeapon, 0 ),
				900.0f,
				2400.0f
			);
		}
		else SavedProjectileInfo.Speed = 900.0f;
		SavedProjectileInfo.GravityMod = 1.0f;
		SavedProjectileInfo.Pipes = true;
		break;
	}
	case TF_WEAPON_CANNON:
	{
		SavedProjectileInfo = { 1454.0f, 1.0f, true };
		break;
	}
	case TF_WEAPON_COMPOUND_BOW:
	{
		float flChargeBeginTime = gLocalWeapon->ChargeBeginTime();
		float flCharge = TICKS_TO_TIME( gLocalPlayer->TickBase( ) ) - flChargeBeginTime;

		if ( flChargeBeginTime ) {
			SavedProjectileInfo.Speed = 1800.0f + std::clamp<float>( flCharge, 0.0f, 1.0f ) * 800.0f;
			SavedProjectileInfo.GravityMod = gMath->ReMapValClamped( flCharge, 0.0f, 1.0f, 0.5f, 0.1f );
		}

		else
		{
			SavedProjectileInfo.Speed = 1800.0f;
			SavedProjectileInfo.GravityMod = 0.5f;
		}

		break;
	}
	case TF_WEAPON_CROSSBOW:
	case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
	{
		SavedProjectileInfo = { 2400.0f, 0.2f };
		break;
	}
	case TF_WEAPON_SYRINGEGUN_MEDIC:
	{
		SavedProjectileInfo = { 1000.0f, 0.3f };
		break;
	}
	case TF_WEAPON_FLAREGUN:
	{
		SavedProjectileInfo = { 2000.0f, 0.3f };
		break;
	}
	case TF_WEAPON_FLAREGUN_REVENGE:
	{
		SavedProjectileInfo = { 3000.0f, 0.45f };
		break;
	}
	case TF_WEAPON_FLAME_BALL:
	{
		SavedProjectileInfo = { 3000.0f, 0.0f };
		break;
	}
	case TF_WEAPON_FLAMETHROWER:
	{
		SavedProjectileInfo = { 2000.0f, 0.0f };
		SavedProjectileInfo.Flamethrower = true;
		break;
	}
	case TF_WEAPON_RAYGUN:
	case TF_WEAPON_DRG_POMSON:
	{
		SavedProjectileInfo = { 1200.0f, 0.0f };
		break;
	}
	case TF_PROJECTILE_CLEAVER:
	{
		SavedProjectileInfo = { 2000.f, 1.f };
		break;
	}
	case TF_WEAPON_LUNCHBOX:
	{
		SavedProjectileInfo = { 500.f, 1.0f };
		break;
	}

	default: break;
	}

	if (gLocalWeapon->ItemDefinitionIndex() == Scout_s_TheFlyingGuillotine ||  
		gLocalWeapon->ItemDefinitionIndex( ) == Scout_s_TheFlyingGuillotineG )
		SavedProjectileInfo = { 2000.f, 1.0f };

	return SavedProjectileInfo.Speed > 0.f;
}

bool CTraceFilterWorldCustom::ShouldHitEntity( void* pServerEntity, int contentsMask )
{
	if ( pServerEntity == m_pSkip )
		return false;

	if ( auto pEntity = reinterpret_cast< IClientEntity* >( pServerEntity )->As<CBaseEntity>( ) )
	{
		switch ( pEntity->GetClientClass( )->ClassID )
		{
		case EClassID::CTFPlayer:
		case EClassID::CObjectSentrygun:
		case EClassID::CObjectDispenser:
		case EClassID::CObjectTeleporter: return pEntity == m_pTarget;
		case EClassID::CObjectCartDispenser:
		case EClassID::CBaseDoor:
		case EClassID::CPhysicsProp:
		case EClassID::CDynamicProp:
		case EClassID::CBaseEntity:
		case EClassID::CFuncTrackTrain: return true;

		default: return false;
		}
	}

	return false;
}

TraceType_t CTraceFilterWorldCustom::GetTraceType( ) const
{
	return TRACE_EVERYTHING;
}

void CProjectiles::FindSuitableTargets( )
{
	Targets.clear( );

	Vector ViewAngles = gEngineClient->GetViewAngles( );
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !gLocalWeapon )
		return;

	for ( auto& Entity : gEntities )
	{
		auto Player = reinterpret_cast< CBasePlayer* >( Entity );
		if ( !Player || Player == gLocalPlayer || Player->IsDormant( ) )
			continue;

		if ( !IsEntityValid( Player ) )
			continue;

		if ( Player->IsPlayer( ) || Player->IsBuilding( ) )
			if ( G->Aimbot.SavedWeaponID != TF_WEAPON_CROSSBOW )
				if ( !Player->IsEnemy( ) )
					continue;

		if ( G->Aimbot.SavedWeaponID == TF_WEAPON_CROSSBOW && Player->IsBuilding( ) )
			continue;

		if ( Player->IsPlayer( ) )
		{
			if ( Player->PlayerCond( ) & TFCond_Bonked && !Config->Aimbot->Targetting->TargetBonk )
				continue;

			int Ticks = std::clamp( TIME_TO_TICKS( Player->SimulationTime( ) - Player->OldSimulationTime( ) ), 0, 22 );
			if ( Ticks > 18 )
				continue;

			if ( gLocalPlayer->Class( ) == CLASS_DEMOMAN || gLocalPlayer->Class( ) == CLASS_SOLDIER )
			{
				if ( Player->PlayerCondEx( ) & TFCondEx_ExplosiveCharge && !Config->Aimbot->Targetting->TargetVacc )
					continue;
			}
			else if ( gLocalPlayer->Class( ) == CLASS_PYRO )
				if ( Player->PlayerCondEx( ) & TFCondEx_FireCharge && !Config->Aimbot->Targetting->TargetVacc )
					continue;

			if ( Player->IsInvulnerable( ) && !Config->Aimbot->Targetting->TargetUber )
				continue;
		}

		float CurrentFov = gMath->CalcFov( ViewAngles, gMath->CalcAngle( gLocalPlayer->EyePosition( ), Player->WorldSpaceCenter( ) ) );
		float Distance = gLocalPlayer->WorldSpaceCenter( ).DistTo( Player->WorldSpaceCenter( ) );

		if ( CurrentFov > Config->Aimbot->Global->FOV )
		{
			continue;
		}

		if ( !Entity->IsEntityOnScreen( ) && CurrentFov < 1.f )
		{
			continue;
		}

		Target_t Target( CurrentFov, Distance, Entity->IsBuilding( ) ? 21 : 20, Player, false );

		int Priority = 1;

		if ( Player->IsPlayer( ) )
		{
			switch ( Player->Class( ) )
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

			if ( Player->Class( ) == CLASS_HEAVY && Player->PlayerCond( ) & TFCond_Slowed )
				Priority = Config->Aimbot->Priorities->Revved;

			if ( Player->PlayerCond( ) & TFCond_Zoomed )
				Priority = Config->Aimbot->Priorities->Scoped;

			if ( Player->PlayerCond( ) & TFCond_Cloaked )
				Priority = Config->Aimbot->Priorities->Cloaked;

		}
		else if ( Player->IsTank( ) || Player->IsSentryBuster( ) )
		{
			Priority = Config->Aimbot->Priorities->Other;
		}
		else
		{
			switch ( Player->GetClientClass( )->ClassID )
			{
			case EClassID::CObjectSentrygun: Priority = Config->Aimbot->Priorities->Sentry; break;
			case EClassID::CObjectTeleporter: Priority = Config->Aimbot->Priorities->OtherBuildings; break;
			case EClassID::CObjectDispenser: Priority = Config->Aimbot->Priorities->OtherBuildings; break;
			}
		}

		PlayerInfo_t info;
		gEngineClient->GetPlayerInfo( Player->GetIndex( ), &info );

		bool IsFriend = false;
		if ( Player->IsPlayer( ) )
			IsFriend = ( Player->IsSteamFriend( ) || gPriorities[ info.friendsID ].Priority == 3 );

		switch ( gPriorities[ info.friendsID ].Priority )
		{
		case 0: break;
		case 1: break;
		case 2: Priority = 999; break;
		case 3: Priority = Config->Aimbot->Targetting->TargetFriends ? Config->Aimbot->Priorities->Friends : 0; break;
		case 4: Priority = 10; break;
		default: break;
		}

		Target.Priority = Priority;
		if ( G->Aimbot.SavedWeaponID != TF_WEAPON_CROSSBOW )
		{
			if ( Priority <= 0 )
				continue;
		}
		else
		{
			if ( Priority <= 0 && !IsFriend )
				continue;
		}


		Targets.emplace_back( Target );
	}

	std::sort( Targets.begin( ), Targets.end( ), [ ] ( const Target_t& t1, const Target_t& t2 )
		{
			return t1.Fov < t2.Fov;
		} );
}

std::vector<Vector> CProjectiles::ComputeSphere( float radius, int numSamples )
{
	std::vector<Vector> points;
	points.reserve( numSamples );

	const float GoldenAngle = 2.39996323f;  // precompute the golden angle

	for ( int i = 0; i < numSamples; ++i )
	{
		const float Inclination = acosf( 1.0f - 2.0f * i / numSamples );  // compute the inclination angle
		const float Azimuth = GoldenAngle * i;  // compute the azimuth angle using the golden angle

		// compute the sample point on the unit sphere
		const Vector sample = Vector( cosf( Azimuth ) * sinf( Inclination ), sinf( Azimuth ) * sinf( Inclination ), cosf( Inclination ) ) * radius;

		// translate the point to the desired center
		points.push_back( sample );
	}

	return points;
}