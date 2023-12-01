#include "ProjectileSim.h"

IPhysicsEnvironment* Env {};
IPhysicsObject* Object {};

bool CProjectileSimulation::Init( const ProjectileInfo& Info, bool NoVecUp )
{
	if ( !Env )
		Env = gPhysics->CreateEnvironment( );

	if ( !Object )
	{
		auto Collision = gPhysicsCollision->BBoxToCollide( { -2.0f, -2.0f, -2.0f }, { 2.0f, 2.0f, 2.0f } );

		auto params = g_PhysDefaultObjectParams;
		params.damping = 0.0f;
		params.rotdamping = 0.0f;
		params.inertia = 0.0f;
		params.rotInertiaLimit = 0.0f;
		params.enableCollisions = false;
		Object = ( CPhysicsObject* ) Env->CreatePolyObject( Collision, 0, Info.Position, Info.Angle, &params );
		Object->Wake( );
	}

	if ( !Env || !Object )
		return false;

	Vector forward {}, up {};

	gMath->AngleVectors( Info.Angle, &forward, nullptr, &up );

	Vector Vel = forward * Info.Speed;
	Vector AngVel {};

	switch ( Info.Type )
	{
	case TF_PROJECTILE_PIPEBOMB:
	case TF_PROJECTILE_PIPEBOMB_REMOTE:
	case TF_PROJECTILE_PIPEBOMB_PRACTICE:
	case TF_PROJECTILE_CANNONBALL:
	{
		if ( !NoVecUp )
			Vel += up * 200.0f;

		AngVel = { 600.0f, -1200.0f, 0.0f };
		break;
	}

	case TF_PROJECTILE_CLEAVER:
	{
		Vel = TF2->GetVelocityVectorForCleaver( forward, up );
		AngVel = { 0.f, 500.f, 0.f };
		break;
	}
	default: break;
	}

	if ( Info.NoSpin )
		AngVel.Zero( );

	Object->SetPosition( Info.Position, Info.Angle, true );
	Object->SetVelocity( &Vel, &AngVel );

	float Drag {};
	Vector DragBasis {};
	Vector AngDragBasis {};

	switch ( Info.Type )
	{
	case TF_PROJECTILE_PIPEBOMB:
	{
		Drag = 1.0f;
		DragBasis = { 0.003902f, 0.009962f, 0.009962f };
		AngDragBasis = { 0.003618f, 0.001514f, 0.001514f };

		break;
	}

	case TF_PROJECTILE_PIPEBOMB_REMOTE:
	case TF_PROJECTILE_PIPEBOMB_PRACTICE:
	{
		Drag = 1.0f;
		DragBasis = { 0.007491f, 0.007491f, 0.007306f };
		AngDragBasis = { 0.002777f, 0.002842f, 0.002812f };

		break;
	}

	case TF_PROJECTILE_CANNONBALL:
	{
		Drag = 1.0f;
		DragBasis = { 0.020971f, 0.019420f, 0.020971f };
		AngDragBasis = { 0.012997f, 0.013496f, 0.013714f };

		break;
	}

	default: break;
	}

	Object->SetDragCoefficient( &Drag, &Drag );
	Object->m_dragBasis = DragBasis;
	Object->m_angDragBasis = AngDragBasis;

	auto MaxVelo { 1000000.0f };
	auto MaxAngleVelo { 1000000.0f };

	switch ( Info.Type )
	{
	case TF_PROJECTILE_PIPEBOMB:
	case TF_PROJECTILE_PIPEBOMB_REMOTE:
	case TF_PROJECTILE_PIPEBOMB_PRACTICE:
	case TF_PROJECTILE_CANNONBALL:
	case TF_PROJECTILE_CLEAVER:
	{
		MaxVelo = k_flMaxVelocity;
		MaxAngleVelo = k_flMaxAngularVelocity;

		break;
	}

	default:
	{
		break;
	}
	}

	physics_performanceparams_t params {};
	params.Defaults( );

	params.maxVelocity = MaxVelo;
	params.maxAngularVelocity = MaxAngleVelo;

	Env->SetPerformanceSettings( &params );
	Env->SetAirDensity( 2.0f );
	Env->SetGravity( { 0.0f, 0.0f, -( 800.0f * Info.Gravity ) } );
	Env->ResetSimulationClock( ); //not needed?
	return true;
}

// credits: mfed
bool CProjectileSimulation::GetInfo( const Vector& Angles, ProjectileInfo& Out )
{
	if ( !gLocalPlayer || !gLocalWeapon )
		return false;

	float CurTime = static_cast< float >( gLocalPlayer->TickBase( ) ) * gGlobals->interval_per_tick;
	bool Ducking = gLocalPlayer->IsDucked( );

	Vector Position {};
	Vector Angle {};

	int WeaponID = gLocalWeapon->WeaponID( );
	int DefIdx = gLocalWeapon->ItemDefinitionIndex( );

	if ( DefIdx == Soldier_s_TheRighteousBison || DefIdx == Engi_m_ThePomson6000 )
	{
		Vector Offset = { 23.5f, 12.0f, -3.0f };

		if ( Ducking )
			Offset.z = 8.0f;

		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, Offset, Position, Angle, false );
		Out = { TF_PROJECTILE_ENERGY_RING, Position, Angle, 1200.f, 0.0f, true };

		return true;
	}

	switch ( WeaponID )
	{
	case TF_WEAPON_LUNCHBOX:
	{
		Vector Source = gLocalPlayer->EyePosition( ) + Vector( 0, 0, -8 );
		QAngle AngForward = Angles + QAngle( -10, 0, 0 );
		Out = { TF_PROJECTILE_NONE, Source, AngForward, 500, 1.0f, false };
		return true;
	}

	case TF_WEAPON_ROCKETLAUNCHER:
	case TF_WEAPON_PARTICLE_CANNON:
	{
		Vector Offset = { 23.5f, 12.0f, -3.0f };
		if ( Ducking )
			Offset.z = 8.0f;
		if ( DefIdx == Soldier_m_TheOriginal )
			Offset = { 0.0f, 0.0f, 0.0f };

		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, Offset, Position, Angle, false );
		Out = { TF_PROJECTILE_ROCKET, Position, Angle, 1100.0f, 0.0f, false };

		return true;
	}
	case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
	{
		Vector Offset = { 23.5f, 12.0f, -3.0f };
		if ( Ducking )
			Offset.z = 8.0f;

		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, Offset, Position, Angle, false );
		Out = { TF_PROJECTILE_ROCKET, Position, Angle, 1980.f, 0.0f, false };

		return true;
	}
	case TF_WEAPON_GRENADELAUNCHER:
	{
		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, { 16.0f, 8.0f, -6.0f }, Position, Angle, true );
		bool IsFaster = gLocalWeapon->ItemDefinitionIndex( ) == Demoman_m_TheLochnLoad;
		float speed = TF2->AttribHookFloat( 1200.0f, "mult_projectile_speed", gLocalWeapon, 0 );

		Out = { TF_PROJECTILE_PIPEBOMB, Position, Angle, speed, 1.0f, IsFaster };

		return true;
	}
	case TF_WEAPON_PIPEBOMBLAUNCHER:
	{
		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, { 16.0f, 8.0f, -6.0f }, Position, Angle, true );
		auto Charge = CurTime - gLocalWeapon->ChargeBeginTime( );
		auto Speed { gMath->ReMapValClamped( Charge, 0.0f, TF2->AttribHookFloat( 4.0f, "stickybomb_charge_rate", gLocalWeapon, 0 ), 900.0f, 2400.0f ) };

		if ( gLocalWeapon->ChargeBeginTime( ) <= 0.0f )
		{
			Speed = 900.0f;
		}

		Out = { TF_PROJECTILE_PIPEBOMB_REMOTE, Position, Angle, Speed, 1.0f, false };
		return true;
	}

	case TF_WEAPON_CANNON:
	{
		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, { 16.0f, 8.0f, -6.0f }, Position, Angle, true );
		Out = { TF_PROJECTILE_CANNONBALL, Position, Angle, 1454.0f, 1.0f, false };
		return true;
	}

	case TF_WEAPON_FLAREGUN:
	{
		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, { 23.5f, 12.0f, Ducking ? 8.0f : -3.0f }, Position, Angle, false );
		Out = { TF_PROJECTILE_FLARE, Position, Angle, 2000.0f, 0.3f, true };

		return true;
	}

	case TF_WEAPON_FLAREGUN_REVENGE:
	{
		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, { 23.5f, 12.0f, Ducking ? 8.0f : -3.0f }, Position, Angle, false );
		Out = { TF_PROJECTILE_FLARE, Position, Angle, 3000.0f, 0.45f, true };
		return true;
	}

	case TF_WEAPON_COMPOUND_BOW:
	{
		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, { 23.5f, 8.0f, -3.0f }, Position, Angle, false );

		float Charge = CurTime - gLocalWeapon->ChargeBeginTime( );
		float Speed = gMath->ReMapValClamped( Charge, 0.0f, 1.0f, 1800.0f, 2600.0f );
		float GravityMod = gMath->ReMapValClamped( Charge, 0.0f, 1.0f, 0.5f, 0.1f );

		if ( gLocalWeapon->ChargeBeginTime( ) <= 0.0f )
		{
			Speed = 1800.0f;
			GravityMod = 0.5f;
		}

		Out = { TF_PROJECTILE_ARROW, Position, Angle, Speed, GravityMod, true };
		return true;
	}

	case TF_WEAPON_CROSSBOW:
	case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
	{
		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, { 23.5f, 8.0f, -3.0f }, Position, Angle, false );
		Out = { TF_PROJECTILE_ARROW, Position, Angle, 2400.0f, 0.2f, true };
		return true;
	}

	case TF_WEAPON_FLAME_BALL:
	{
		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, { 23.5f, 8.0f, -3.0f }, Position, Angle, false );
		Out = { TF_PROJECTILE_BALLOFFIRE, Position, Angle, 3000.f, 0.0f, true };
		return true;
	}

	case TF_WEAPON_SYRINGEGUN_MEDIC:
	{
		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, { 23.5f, 8.0f, -3.0f }, Position, Angle, false );
		Out = { TF_PROJECTILE_SYRINGE, Position, Angle, 1000.0f, 0.0f, true };
		return true;
	}
	case TF_WEAPON_CLEAVER:
	{
		TF2->GetProjectileFireSetup( gLocalPlayer, Angles, { 16.0f, 8.0f, -6.0f }, Position, Angle, false );
		Out = { TF_PROJECTILE_CLEAVER, Position, Angle, 2000.f, 0.2f, true };
		return true;
	}
	}

	return false;
}

Vector CProjectileSimulation::GetOrigin( )
{
	if ( !Object )
		return {};

	Vector Out {};
	Object->GetPosition( &Out, nullptr );
	return Out;
}

void CProjectileSimulation::Simulate( )
{
	if ( !Env ) return;
	Env->Simulate( gGlobals->interval_per_tick );
}