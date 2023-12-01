#include "Misc.h"
#include "../../Hooks/HookList.h"
#include "../../../Globals.h"
#include "../Menu/Input/Input.h"
#include "../Anti-Aimbot/Anti-Aimbot.h"
#include <filesystem>
#include "../Menu/Menu.h"
#include "Wave/Wave.h"

void CMisc::RemoveInputDelay( bool FinalTick )
{
	if ( gClientState->m_NetChannel && gClientState->m_NetChannel->IsLoopback( ) )
		return;

	if ( !gEngineClient->IsInGame( ) )
		return;

	GlobalsBackup_t Backup;

	Backup.Store( );
	{
		CL_ReadPackets::OriginalFunction( FinalTick );
		State.Store( );
	}
	Backup.Restore( );
}

bool CMisc::ShouldReadPackets( )
{
	if ( gClientState->m_NetChannel && gClientState->m_NetChannel->IsLoopback( ) )
		return true;

	if ( !gEngineClient->IsInGame( ) )
		return true;

	State.Restore( );
	return false;
}

void CMisc::CalcLatencyDifference( )
{
	if ( !gClientState || !gClientState->m_NetChannel )
	{
		return;
	}

	int CurrentLatency = ( int )( gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ) * 1000.f );

	/*//detect high volatily and fluctuations in client's latency to the server (internet dropping/server issues)
	IsClientLagging = std::abs( LastLatency - CurrentLatency ) > 20;

	//fps below tickrate = client is falling behind server's tickrate
	IsClientLagging = gClientState->m_NetChannel->GetLatency( FLOW_INCOMING ) > gGlobals->interval_per_tick;*/

	if ( std::abs( LastLatency - CurrentLatency ) > 20 || gClientState->m_NetChannel->GetLatency( FLOW_INCOMING ) > gGlobals->interval_per_tick )
	{
		//should probably be 0.25f since the latency is updated every ~15-16 ticks
		TimeSinceLag = gEngineClient->Time( ) + 0.5f;
	}

	if ( LastLatency != CurrentLatency )
	{
		LastLatency = CurrentLatency;
	}
}

void CMisc::BunnyHoppenz(CUserCmd* Cmd)
{
	if (!Config->Misc->BHop
		 || !Input->InFocus( )
		 || !gLocalPlayer->IsAlive( )
		 || gLocalPlayer->WaterLevel( ) > 1
		 || gLocalPlayer->IsGhost( )
		 || gLocalPlayer->IsInKart( ) )
	{
		return;
	}

	if ( gLocalPlayer->MoveType( ) == MOVETYPE_NOCLIP
		 || gLocalPlayer->MoveType( ) == MOVETYPE_LADDER
		 || gLocalPlayer->MoveType( ) == MOVETYPE_OBSERVER )
	{
		return;
	}

	// TODO: checkbox
	if (gLocalWeapon && gLocalWeapon->ItemDefinitionIndex() == Scout_m_BabyFacesBlaster )
	{
		if ( !( gLocalPlayer->Flags( ) & FL_ONGROUND ) )
		{
			Cmd->buttons &= ~IN_JUMP;
			return;
		}
	}

	static bool State = false;

	if (Cmd->buttons & IN_JUMP) 
	{
		if (!State && !(gLocalPlayer->Flags() & FL_ONGROUND))
		{
			Cmd->buttons &= ~IN_JUMP;
			G->FakeLag->PreservingBlastJump = true;
		}

		else if (State)
			State = false;
	}

	else if (!State)
		State = true;
}

void CMisc::AirStrafer( CUserCmd* Cmd )
{
	if ( !Config->Misc->AutoStrafer )
		return;

	static bool WasJumping = false;
	bool IsJumping = Cmd->buttons & IN_JUMP;

	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || gLocalPlayer->WaterLevel( ) > 1 )
		return;

	if ( gLocalPlayer->MoveType( ) == MOVETYPE_NOCLIP
		|| gLocalPlayer->MoveType( ) == MOVETYPE_LADDER
		|| gLocalPlayer->MoveType( ) == MOVETYPE_OBSERVER )
	{
		return;
	}

	if ( !( gLocalPlayer->Flags( ) & FL_ONGROUND ) && ( !IsJumping || WasJumping ) ) {
		if ( gLocalPlayer->Flags( ) & ( 1 << 11 ) )
			return;

		float SideMove = Cmd->sidemove;
		float ForwardMove = Cmd->forwardmove;

		Vector Forward = {}, Right = {};
		gMath->AngleVectors( Cmd->viewangles, &Forward, &Right, nullptr );

		Forward.z = Right.z = 0.0f;

		Forward.Normalize( );
		Right.Normalize( );

		Vector WishDir = {};
		gMath->VectorAngles(
			{ ( Forward.x * ForwardMove ) + ( Right.x * SideMove ),
			( Forward.y * ForwardMove ) +
			( Right.y * SideMove ), 0.0f },
			WishDir );

		Vector CurDir = {};
		gMath->VectorAngles( gLocalPlayer->Velocity( ), CurDir );

		float DirDelta = gMath->NormalizeAngle( WishDir.y - CurDir.y );
		float TurnScale = gMath->ReMapValClamped( ( Config->Misc->StraferScale / 100.f ), 0.0f, 1.0f, 0.9f, 1.0f );
		float Rotation = DEG2RAD( ( DirDelta > 0.0f ? -90.0f : 90.f ) + ( DirDelta * TurnScale ) );

		float CosRot = cosf( Rotation );
		float SinRot = sinf( Rotation );

		Cmd->forwardmove = ( CosRot * ForwardMove ) - ( SinRot * SideMove );
		Cmd->sidemove = ( SinRot * ForwardMove ) + ( CosRot * SideMove );
	}

	WasJumping = IsJumping;
}

void CMisc::EdgeJump( CUserCmd* Cmd, bool WasOnGround )
{
	if ( !Config->Misc->EdgeJump
		 || !Input->InFocus( )
		 || !gLocalPlayer->IsAlive( )
		 || gLocalPlayer->WaterLevel( ) > 1
		 || gLocalPlayer->IsGhost( )
		 || gLocalPlayer->IsInKart( ) )
	{
		return;
	}

	if ( gLocalPlayer->MoveType( ) == MOVETYPE_NOCLIP
		 || gLocalPlayer->MoveType( ) == MOVETYPE_LADDER
		 || gLocalPlayer->MoveType( ) == MOVETYPE_OBSERVER )
	{
		return;
	}

	if ( Config->Misc->EdgeJumpKey && !( GetAsyncKeyState( Config->Misc->EdgeJumpKey ) & 0x8000 ) )
	{
		return;
	}

	if ( WasOnGround && !gLocalPlayer->IsOnGround( ) )
	{
		Cmd->buttons |= IN_JUMP;
	}
}

void CMisc::AntiWarp( CUserCmd* Cmd )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) 
		|| !Config->Misc->DT->AntiWarp || !G->Doubletap.ToSend )
		return;

	Vector Velocity = gLocalPlayer->EstimateAbsVelocity( );
	float Speed = Velocity.Length( );

	if ( !gEngineClient->GetNetChannelInfo( ) || !gClientState ||
		Speed < 23.5f )
		return;

	Vector Angles, Forward;
	gMath->VectorAngles( Velocity, Angles );
	Angles.y = gEngineClient->GetViewAngles( ).y - Angles.y;
	gMath->AngleVectors( Angles, &Forward );
	Forward *= Speed;

	float Scale = ( gLocalPlayer->MaxSpeed( ) / Speed ) +
		gEngineClient->GetNetChannelInfo( )->GetLatency( 0 );

	Cmd->forwardmove = -Forward.x * Scale;
	Cmd->sidemove = -Forward.y * Scale;
}

void CMisc::HandleDelayDT()
{
	if (!gLocalPlayer || !gLocalPlayer->IsAlive() || !Config->Misc->DT->Enabled || G->Doubletap.ToSend > 0 || !gLocalWeapon || ( gLocalPlayer->Class( ) == CLASS_DEMOMAN && gLocalWeapon->Slot( ) == SLOT_SECONDARY ) )
		return;

	auto Weapon = gLocalWeapon;
	static auto OldWeapon = Weapon;

	if (OldWeapon != Weapon)
	{
		G->Doubletap.Wait = 26;
		OldWeapon = Weapon;
	}

	if (!gLocalWeapon->CanFire())
		G->Doubletap.Wait = 23;

	if ( G->Aimbot.SavedWeaponID == TF_WEAPON_MINIGUN && gLocalWeapon->MinigunState() != 3)
		G->Doubletap.Wait = 24;
}

bool CMisc::Warp( float extra_sample, bool final_tick, std::function< void( float, bool ) > f )
{
	// idk what i was doing here nfgl
	// fixed by pasting the shit you already made from jengaware
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !Config->Misc->DT->Enabled || G->Doubletap.ToSend > 0 || !gLocalWeapon )
		return false;

	int ticks_per_call = Config->Misc->DT->WarpTicks;

	static float LastWarp = 0.f;

	switch ( Config->Misc->DT->WarpMode )
	{
		case 0: break;
		case 1:
		{
			float velocity_per_tick = gLocalPlayer->Velocity( ).Length2D( ) * gGlobals->interval_per_tick;
			if ( velocity_per_tick <= 0.f )
				velocity_per_tick = 1.f;

			int lag_comp_required = std::clamp( ( int )ceilf( 64.f / velocity_per_tick ), 0, 22 );
			ticks_per_call = lag_comp_required / 2;

			if ( G->Doubletap.Charged < lag_comp_required )
			{
				ticks_per_call = 1;
			}
			break;
		}
		case 2:
		{
			ticks_per_call = 8;
			break;
		}
		case 3:
		{
			// this seems better team
			ticks_per_call = TF2->RandomInt( 0, 4 );
			break;
		}
	}
	if ( ticks_per_call < 1 )
		ticks_per_call = 1;

	if ( Config->Misc->DT->WarpMode == 2 )
	{
		if ( LastWarp > gEngineClient->Time( ) )
		{
			G->Doubletap.Warp = false;
			return false;
		}
	}

	if ( ( ( GetAsyncKeyState( Config->Misc->DT->WarpKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) && G->Doubletap.Charged >= ticks_per_call ) {
		G->Doubletap.Warp = true;
		for ( int i = 0; i < ticks_per_call; i++ ) {
			f( extra_sample, final_tick ); // cl_move call
			G->Doubletap.Charged--;
			G->Doubletap.Sent++;
			
			G->Doubletap.Wait = 26;
			*G->Doubletap.SendPacket = ( i == ticks_per_call - 1 );

			if ( Config->Misc->DT->WarpMode == 2 )
			{
				LastWarp = gEngineClient->Time( ) + 0.074f;
				G->Doubletap.Wait = TIME_TO_TICKS( 0.027f );
			}
		}
		G->Doubletap.Warp = false;
	}

	return false;
}

void CMisc::FastStop(CUserCmd* Cmd)
{
	if (!gLocalPlayer || !gLocalPlayer->IsAlive() || !Config->Misc->FastStop || G->Doubletap.ToSend )
		return;

	if (Cmd->buttons & IN_MOVELEFT || Cmd->buttons & IN_MOVERIGHT || Cmd->buttons & IN_FORWARD || Cmd->buttons & IN_BACK)
		return;

	if (gLocalPlayer->Velocity().Length() <= 42.5f || !(gLocalPlayer->Flags() & FL_ONGROUND) || Cmd->buttons & IN_JUMP)
		return;

	Vector Angles, Forward;
	gMath->VectorAngles(gLocalPlayer->Velocity(), Angles);
	Angles.y = gEngineClient->GetViewAngles().y - Angles.y;
	gMath->AngleVectors(Angles, &Forward);
	Forward *= gLocalPlayer->Velocity().Length();

	float Scale = (gLocalPlayer->Velocity().Length() / gLocalPlayer->MaxSpeed());
	float BaseScale = 12.5f * Scale;

	Cmd->forwardmove = -Forward.x * BaseScale;
	Cmd->sidemove = -Forward.y * BaseScale;
}

void CMisc::ScoutJump( CUserCmd* Cmd )
{
	bool shot = false;
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || 
		!gLocalWeapon || gLocalPlayer->WaterLevel( ) > 1 || !Config->Misc->HighJump )
		return;

	if ( !gLocalWeapon->IsForceANature( ) )
		return;

	if ( gLocalPlayer->MoveType( ) == MOVETYPE_NOCLIP || 
		gLocalPlayer->MoveType( ) == MOVETYPE_LADDER || gLocalPlayer->MoveType( ) == MOVETYPE_OBSERVER )
		return;

	if ( ( GetAsyncKeyState( Config->Misc->HighJumpKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
	{
		if ( gLocalPlayer && gLocalPlayer->IsAlive( ) &&
			gLocalPlayer->Class( ) == CLASS_SCOUT && gLocalPlayer->IsOnGround( ) )
		{
			Vector Angles;
			gMath->VectorAngles( gLocalPlayer->Velocity( ), Angles );
			Vector angle = { 37.f, Angles.y, 0.0f };
			if ( !shot )
			{
				Cmd->buttons |= IN_JUMP;
				Cmd->buttons |= IN_ATTACK;
				shot = true;
			}

			Cmd->viewangles = angle;
			TF2->FixMovement( Cmd, angle );
		}
	}
}

void CMisc::JitterMove( CUserCmd* Cmd )
{
	if ( G->Doubletap.ToSend )
		return;

	static bool pos = false;
	if ( Cmd->forwardmove == 0.f && Cmd->sidemove == 0.f && gLocalPlayer->Velocity( ).Length2D( ) < 10.f && gGlobals->curtime )
	{
		if ( Config->Misc->AntiAim->Enabled && AntiAim->IsToggled )
		{
			if ( gLocalPlayer->IsDucked( ) )
			{
				const float yaw = DEG2RAD( fmodf( gGlobals->curtime * 360.f * 1.95f, 360.f ) );
				const float velDir = yaw;
				const float wishAng = atan2f( -Cmd->sidemove, Cmd->forwardmove );
				const float delta = gMath->AngleDiffRadians( velDir, wishAng );
				const float moveDir = delta < 0.0f ? velDir : velDir;
				Cmd->forwardmove = cosf( moveDir ) * 3.75f;
				Cmd->sidemove = -sinf( moveDir ) * 3.75f;
				pos = !pos;
			}
			else
			{
				const float yaw = DEG2RAD( fmodf( gGlobals->curtime * 360.f * 1.95f, 360.f ) );
				const float velDir = yaw;
				const float wishAng = atan2f( -Cmd->sidemove, Cmd->forwardmove );
				const float delta = gMath->AngleDiffRadians( velDir, wishAng );
				const float moveDir = delta < 0.0f ? velDir : velDir;
				// todo? lower the speed so its not that noticable?
				Cmd->forwardmove = cosf( moveDir ) * 1.25f;
				Cmd->sidemove = -sinf( moveDir ) * 1.25f;
				pos = !pos;
			}
		}
	}
}

void CMisc::FakeLag( CUserCmd* Cmd, bool* SendPacket )
{
	if ( !gLocalPlayer->IsAlive( )
		 || gLocalPlayer->WaterLevel( ) > 1
		 || gLocalPlayer->IsGhost( )
		 || gLocalPlayer->IsInKart( ) 
		|| !gLocalWeapon )
	{
		G->FakeLag->Active = false;
		G->FakeLag->Choking = false;
		return;
	}

	if ( gLocalPlayer->MoveType( ) == MOVETYPE_NOCLIP
		 || gLocalPlayer->MoveType( ) == MOVETYPE_LADDER
		 || gLocalPlayer->MoveType( ) == MOVETYPE_OBSERVER )
	{
		G->FakeLag->Active = false;
		G->FakeLag->Choking = false;
		return;
	}

	if ( G->FakeLag->FakeDucking )
	{
		G->FakeLag->Active = false;
		return;
	}

	if ( !Config->Misc->FakeLag->Enabled )
	{
		G->FakeLag->Active = false;

		if ( gLocalPlayer->Class( ) == CLASS_SOLDIER )
		{
			if ( G->FakeLag->PreservingBlastJump && gLocalPlayer->IsOnGround( ) )
			{
				*G->Doubletap.SendPacket = false;
				G->FakeLag->Choking = true;
				G->FakeLag->PreservingBlastJump = false;
			}
			else if ( !G->FakeLag->PreservingBlastJump && G->FakeLag->Choking )
			{
				*G->Doubletap.SendPacket = true;
				G->FakeLag->Choking = false;
			}
		}
		return;
	}

	//int allowed = 21 - G->Doubletap.Charged == -1 ? 0 : 21 - G->Doubletap.Charged;
	if ( G->Doubletap.Charged > ( gLocalWeapon->IsForceANature( ) ? 2 : 0 ) || G->Doubletap.QueueRecharge || G->Doubletap.Recharge || G->Doubletap.ToSend > 0 )
	{
		G->FakeLag->Active = false;
		G->FakeLag->Choking = false;
		return;
	}

	if ( gLocalWeapon->IsAttacking( Cmd ) && Config->Misc->FakeLag->UnchokeOnAttack && !G->Doubletap.Charged)
	{
		G->FakeLag->Active = false;
		G->FakeLag->Choking = false;
		*SendPacket = true;
		return;
	}

	/*if ( Config->Misc->FakeLag->FakeLagKey && !( (GetAsyncKeyState( Config->Misc->FakeLag->FakeLagKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) )
	{
		return;
	}*/

	if ( !IsFakeLagToggled && Config->Misc->FakeLag->FakeLagKey )
	{
		G->FakeLag->Active = false;
		G->FakeLag->Choking = false;
		return;
	}

	G->FakeLag->Active = true;

	bool ShouldBreakLC = false;
	G->FakeLag->BreakingLagComp = false;
	if ( Config->Misc->FakeLag->BreakLagComp )
	{
		if ( Config->Misc->FakeLag->BreakMoving && gLocalPlayer->Velocity( ).Length2D( ) > 1.f && gLocalPlayer->IsOnGround( ) )
		{
			ShouldBreakLC = true;
			G->FakeLag->BreakingLagComp = true;
		}

		if ( Config->Misc->FakeLag->BreakInAir && !gLocalPlayer->IsOnGround( ) )
		{
			ShouldBreakLC = true;
			G->FakeLag->BreakingLagComp = true;
		}
	}
	else
	{
		G->FakeLag->BreakingLagComp = false;
		ShouldBreakLC = false;
	}

	float VelocityPerTick = gLocalPlayer->Velocity( ).Length2D( ) * gGlobals->interval_per_tick;
	if ( VelocityPerTick <= 0.f )
		VelocityPerTick = 1.f;

	int LagCompRequired = std::clamp( ( int )ceilf( 64.f / VelocityPerTick ), 0, 22 );
	int MinTicks = ShouldBreakLC ? ( LagCompRequired > Config->Misc->FakeLag->Value ? Config->Misc->FakeLag->Value : LagCompRequired ) : Config->Misc->FakeLag->Value;
	if ( MinTicks < 1 )
		MinTicks = 1;

	if ( MinTicks < 2 && Config->Misc->AntiAim->Enabled && AntiAim->IsToggled )
		MinTicks = 2;

	if ( G->Doubletap.Charged > ( gLocalWeapon->IsForceANature( ) ? 2 : 0 ) )
		MinTicks = Config->Misc->AntiAim->Enabled && AntiAim->IsToggled ? 3 : 0;

	G->FakeLag->Ticks = MinTicks;
	if ( gClientState->chokedcommands >= MinTicks ) {
		G->FakeLag->Choking = false;
		*SendPacket = true;
	}
	else {
		G->FakeLag->Choking = true;
		*SendPacket = false;
	}
}

void CMisc::FakeDuck( CUserCmd* Cmd, bool* SendPacket )
{
	G->FakeLag->FakeDucking = false;
	if ( !Config->Misc->FakeDuck->Enabled
		 || !( (GetAsyncKeyState( Config->Misc->FakeDuck->FakeDuckKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
		 || !Input->InFocus( )
		 || !gLocalPlayer->IsAlive( )
		 || gLocalPlayer->WaterLevel( ) > 1
		 || gLocalPlayer->IsGhost( )
		 || gLocalPlayer->IsInKart( )
		 || G->Doubletap.Charged > ( gLocalWeapon->IsForceANature( ) ? 2 : 0 ) )
	{
		G->FakeLag->Choking = false;
		return;
	}

	if ( gLocalPlayer->MoveType( ) == MOVETYPE_NOCLIP
		 || gLocalPlayer->MoveType( ) == MOVETYPE_LADDER
		 || gLocalPlayer->MoveType( ) == MOVETYPE_OBSERVER )
	{
		G->FakeLag->Choking = false;
		return;
	}

	G->FakeLag->FakeDucking = true;

	if ( gClientState->chokedcommands < 22 && gLocalPlayer->Ducktime( ) == 0.f )
	{
		Cmd->forwardmove = 0.f;
		Cmd->sidemove = 0.f;
	}

	/*bool ShouldAttack;
	if ( gLocalWeapon->WeaponID( ) == TF_WEAPON_SNIPERRIFLE || gLocalWeapon->WeaponID( ) == TF_WEAPON_SNIPERRIFLE_DECAP || gLocalWeapon->WeaponID( ) == TF_WEAPON_SNIPERRIFLE_CLASSIC )
	{
		ShouldAttack = ( Cmd->buttons & IN_ATTACK );
	}
	else
	{
		ShouldAttack = gLocalWeapon->IsAttacking( Cmd );
	}*/

	if ( gClientState->chokedcommands < 1 || ( Cmd->buttons & IN_ATTACK ) )
	{
		Cmd->buttons &= ~IN_DUCK;
	}
	else
	{
		Cmd->buttons |= IN_DUCK;
	}

	if ( gClientState->chokedcommands < 22 && ( gLocalPlayer->Ducktime( ) >= 790.f || gLocalPlayer->Ducktime( ) == 0.f ) && !( Cmd->buttons & IN_ATTACK ) )
	{
		G->FakeLag->Choking = true;
		*SendPacket = false;
	}
	else
	{
		G->FakeLag->Choking = false;
		*SendPacket = true;
	}
}

void CMisc::RocketJump( CUserCmd* Cmd )
{
	if ( !Config->Misc->HighJump
		 || !( (GetAsyncKeyState( Config->Misc->HighJumpKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
		 || !Input->InFocus( )
		 || !gLocalPlayer->IsAlive( )
		 || gLocalPlayer->WaterLevel( ) > 1
		 || gLocalPlayer->IsGhost( )
		 || gLocalPlayer->IsInKart( )
		 || gLocalPlayer->Class( ) != CLASS_SOLDIER
		 || !gLocalPlayer->IsOnGround( )
		 || gLocalWeapon->Clip1( ) == 0 )
	{
		return;
	}

	if ( gLocalPlayer->MoveType( ) == MOVETYPE_NOCLIP
		 || gLocalPlayer->MoveType( ) == MOVETYPE_LADDER
		 || gLocalPlayer->MoveType( ) == MOVETYPE_OBSERVER )
	{
		return;
	}

	float pitch{ gMath->ReMapValClamped( gEngineClient->GetViewAngles( ).x, -89.0f, 0.0f, 89.0f, 60.0f ) };
	float yaw{ gMath->NormalizeAngle( gMath->VelocityToAngles( gLocalPlayer->Velocity( ) ).y + 180.0f ) };

	if ( gLocalPlayer->Velocity( ).IsZero( ) )
	{
		yaw = gMath->NormalizeAngle( gEngineClient->GetViewAngles( ).y + 180.0f );
	}

	if ( gLocalPlayer->ViewOffset( ).z > 47.15f )
	{
		Cmd->buttons |= IN_DUCK;
	}
	else if ( gLocalPlayer->ViewOffset( ).z > 45.65f && gLocalPlayer->ViewOffset( ).z < 47.15f )
	{
		Cmd->viewangles = { pitch, yaw, gLocalWeapon->ItemDefinitionIndex( ) != Soldier_m_TheOriginal ? 90.0f : 0.0f };
		gLocalWeapon->AbortReload( );
		Cmd->buttons |= IN_ATTACK;
		Cmd->buttons &= ~IN_ATTACK2;
	}
	else
	{
		Cmd->viewangles = { pitch, yaw, gLocalWeapon->ItemDefinitionIndex( ) != Soldier_m_TheOriginal ? 90.0f : 0.0f };
		Cmd->buttons |= IN_ATTACK | IN_DUCK | IN_JUMP;
		Cmd->buttons &= ~IN_ATTACK2;
	}
}

bool CMisc::AllowedToDoubleTap( )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) )
		return false;

	if ( !Config->Misc->DT->ActiveInAir )
		if ( !( gLocalPlayer->Flags( ) & FL_ONGROUND ) )
			return false;

	if ( !Config->Misc->DT->ActiveOnlyAimbot && !G->Aimbot.Active && !( GetAsyncKeyState( Config->Aimbot->Projectile->GazaBombingKey ) & 0x8000 ) )
		return false;

	if ( Config->Misc->DT->ActivateOnlyLethal && !G->Aimbot.LethalDTRange && G->Aimbot.Active && !( GetAsyncKeyState( Config->Aimbot->Projectile->GazaBombingKey ) & 0x8000 ) )
		return false;

	if ( G->Doubletap.Warp )
		return false;

	if ( G->Aimbot.Sticky )
		return false;

	if ( G->Doubletap.Wait > 0 )
		return false;

	if ( Config->Misc->DT->DTKey && !( GetAsyncKeyState( Config->Misc->DT->DTKey ) & 0x8000 ) && !( GetAsyncKeyState( Config->Aimbot->Projectile->GazaBombingKey ) & 0x8000 ) )
		return false;

	return true;
}

void CMisc::FastBackup(CUserCmd* pCmd)
{
	if (!gLocalPlayer || !gLocalPlayer->IsAlive() || !gLocalPlayer->IsOnGround( ) )
		return;

	if ( gLocalPlayer->WaterLevel( ) > 1
		|| gLocalPlayer->IsGhost( )
		|| gLocalPlayer->IsInKart( ) 
		|| gLocalPlayer->IsCharging( ) )
		return;

	if (!Config->Misc->FastBackup)
		return;

	if ( Config->Misc->AntiAim->Enabled && AntiAim->IsToggled )
		return;

	if (pCmd->buttons & IN_BACK &&
		!(TF2->IsAttacking(pCmd)) &&
		!(pCmd->buttons & IN_FORWARD) &&
		!(pCmd->buttons & IN_MOVELEFT) &&
		!(pCmd->buttons & IN_MOVERIGHT))
	{
		pCmd->viewangles.z = -90.f;
	}
}

void CMisc::KillSay( IGameEvent* Event )
{
	if ( !gLocalPlayer || !gLocalWeapon || Config->Misc->VoiceSpamMode )
		return;

	const auto attacked = gEntityList->GetClientEntity( gEngineClient->GetPlayerForUserID( Event->GetInt( "userid" ) ) );
	const auto attacker = gEntityList->GetClientEntity( Event->GetInt( "inflictor_entindex" ) );
	if ( !attacked || !attacker || attacker != gLocalPlayer || attacked == gLocalPlayer )
		return;

	bool ShouldRun = Config->Misc->VoiceSpam;

	PlayerInfo_t info;
	if ( gEngineClient->GetPlayerInfo( attacked->GetIndex( ), &info ) )
		if ( gPriorities[ info.friendsID ].VoiceSay )
			ShouldRun = true;

	if ( ShouldRun )
		VoiceSay( );
}

void CMisc::VoiceSay( )
{
	if ( Menu->SelectedSound.empty( ) )
		return;

	const char* file = Menu->SelectedSound.c_str( );

	/*std::string path = std::filesystem::current_path( ).parent_path( ).parent_path( ).parent_path( ).string( ) + "\\resource\\camera1.wav";
	std::transform( path.begin( ), path.end( ), path.begin( ), tolower );
	gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "found steam screenshot path: %s", path.c_str( ) );
	file = path.c_str( );*/

	std::ifstream fstream( Menu->SoundFolder + "\\" + file + ".wav" );
	if ( file )
	{
		WaveFileHeader_t* Header;
		char buffer[ 44 ];

		fstream.read( buffer, 44 );
		Header = reinterpret_cast< WaveFileHeader_t* >( buffer );

		float duration = std::filesystem::file_size( Menu->SoundFolder + "\\" + file + ".wav" ) / ( float )Header->ByteRate;
		G->KillSay->Duration = duration;
		G->KillSay->Time = gEngineClient->Time( ) + duration + 0.1f;

		Play( Menu->SelectedSound );
	}
	else
	{
		gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Unable to play file\n" );
	}
}

void CMisc::StopRecording( )
{
	Reset( );

	if ( G->KillSay->Stopped )
	{
		gEngineClient->ClientCmd_Unrestricted( "voice_loopback 0" );
		gEngineClient->ClientCmd_Unrestricted( "-voicerecord" );
		gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "stopped\n" );
		G->KillSay->Stopped = false;
	}
}

void CMisc::Reset( )
{
	delete[ ] current_file_data;
	current_file_data = NULL;
	current_file_bytes = 0;

	waveOut.close( );
}

void CMisc::InitializeSound( )
{
	Reset( );

	hwnd = GetActiveWindow( );
	deviceID = WAVE_MAPPER;

	for ( size_t i = 0; i < waveOutGetNumDevs( ); i++ )
	{
		tagWAVEOUTCAPSA info;
		waveOutGetDevCapsA( i, &info, sizeof( struct tagWAVEOUTCAPSA ) );

		std::string deviceName = info.szPname;

		if ( deviceName.find( "C" ) != std::string::npos )
		{
			gCvar->ConsoleColorPrintf( { 0, 240, 140, 255 }, "Using audio device [%i] - %s\n", i, deviceName.c_str( ) );
			deviceID = i;
			break;
		}
	}

	//LoadSoundFiles( );
}

/*void CMisc::LoadSoundFiles( )
{
	WIN32_FIND_DATAA data;
	auto file_handle = FindFirstFileA( ( Menu->SoundFolder + "*.wav" ).c_str( ), &data );
	if ( file_handle == INVALID_HANDLE_VALUE )
		return;

	do
	{
		if ( data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			continue;

		if ( strstr( data.cFileName, Menu->SelectedSound.c_str( ) ) )
		{

		}

	} while ( FindNextFileA( file_handle, &data ) );
}*/

void CMisc::Play( std::string filename )
{
	if ( !gEngineClient->IsInGame( ) || Menu->SelectedSound.empty( ) )
		return;

	Reset( );

	gEngineClient->ClientCmd_Unrestricted( "voice_loopback 1" );
	gEngineClient->ClientCmd_Unrestricted( "+voicerecord" );

	auto file = Menu->SoundFolder + "\\" + filename + ".wav";

	std::transform( file.begin( ), file.end( ), file.begin( ), ::tolower );
	PlayFile( file.c_str( ) );
}

void CMisc::PlayFile( const char* file )
{
	//55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 85 F6
	typedef bool( __cdecl* fn )( const char* pFilename, char*& pData, int& nDataBytes, int& wBitsPerSample, int& nChannels, int& nSamplesPerSec );
	static auto ReadWaveFile = Memory->FindSignature<fn>( "engine.dll", "55 8B EC 51 56 68 ? ? ? ? FF 75 ? E8 ? ? ? ? 8B F0 83 C4 ? 85 F6 75 ? 32 C0 5E 8B E5 5D C3" );

	int nBitrate, nChannels, nFreq;

	if ( ReadWaveFile( file, current_file_data, current_file_bytes, nBitrate, nChannels, nFreq ) )
		gCvar->ConsoleColorPrintf( { 0, 240, 140, 255 }, "Loaded '%s'\nSize: %i KB\nBitrate: %i\nFrequency: %i\nChannels: %i\n", file, current_file_bytes, nBitrate, nFreq, nChannels );
	else
	{
		gCvar->ConsoleColorPrintf( { 255, 140, 0, 255 }, "Failed to load '%s'\n", file );
		return;
	}

	waveOut.open( nChannels, nFreq, nBitrate, hwnd, deviceID );
	waveOut.play( current_file_data, current_file_bytes );
}

void CMisc::AntiBackstab(CUserCmd* pCmd)
{
	if (!gLocalPlayer || !gLocalWeapon || !pCmd || !Config->Misc->AntiBackstab || gLocalPlayer->IsCharging( ) )
		return;

	Vector Point = { };

	for (auto& ent : gPlayers)
	{
		if (!ent || !ent->IsAlive() || !ent->IsPlayer() || !ent->IsEnemy( ))
			continue;

		if (ent->Class() != CLASS_SPY)
			continue;

		if (ent->WorldSpaceCenter().DistTo(gLocalPlayer->WorldSpaceCenter()) > 160.f)
			continue;

		if ( ent->IsCloaked( ) )
			continue;

		if ( !TF2->IsVisible( gLocalPlayer, ent, gLocalPlayer->WorldSpaceCenter( ), ent->WorldSpaceCenter( ) ) )
			continue;

		Point = ent->WorldSpaceCenter( );
		break;
	}

	if ( !Point.IsZero( ) )
		if ( !TF2->IsAttacking( pCmd ) )
			pCmd->viewangles = gMath->CalcAngle( gLocalPlayer->EyePosition( ), Point );
}

//pasted yucky romanian code
void CMisc::ShootStickyAtCharge( CUserCmd* Cmd )
{
	if ( !gLocalPlayer || !gLocalWeapon || !( gLocalPlayer->Class( ) == CLASS_DEMOMAN && gLocalWeapon->Slot( ) == SLOT_SECONDARY ) ||
		 ( Config->Aimbot->Global->Enabled && ( (GetAsyncKeyState( Config->Aimbot->Global->AimKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) ) ||
		 !Config->Aimbot->Projectile->MaxStickyCharge || !( Cmd->buttons & IN_ATTACK ) )
		return;

	float get_current_charge = gGlobals->curtime - gLocalWeapon->ChargeBeginTime( ); // ever heard of remap_val_clamped
	float get_max_charge = Config->Aimbot->Projectile->MaxStickyCharge / 27.0f;
	float get_latency = gClientState->m_NetChannel->GetLatency( 0 );

	if ( gLocalWeapon->ItemDefinitionIndex( ) == Demoman_s_TheQuickiebombLauncher )
		get_max_charge = Config->Aimbot->Projectile->MaxStickyCharge / 70.0f;

	if ( get_current_charge > 5.0f )
		return;

	if ( get_current_charge >= get_max_charge )
		Cmd->buttons &= ~IN_ATTACK;
}

void CMisc::NetStore( )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) )
	{
		NetReset( );
		return;
	}

	int TickBase = gLocalPlayer->TickBase( );
	StoredData_t* Data = &NetData[ TickBase % MULTIPLAYER_BACKUP ];

	Data->TickBase = TickBase;
	Data->PunchAngle = gLocalPlayer->PunchAngle( );
	Data->PunchVelocity = gLocalPlayer->PunchAngleVel( );
	Data->ViewOffset = gLocalPlayer->ViewOffset( );
	Data->Origin = gLocalPlayer->Origin( );
	if (gLocalWeapon )
	{
		Data->m_flLastFireTime = gLocalWeapon->LastFireTime( );
		Data->m_flNextPrimaryAttack = gLocalWeapon->NextPrimaryAttack( );
		Data->m_flTimeWeaponIdle = gLocalWeapon->TimeWeaponIdle( );
	}

	for ( int i = 0; i < gGlobals->maxClients; i++ )
	{
		const auto& Player = gEntityList->GetClientEntity( i )->As<CBasePlayer>( );

		if ( !Player || Player->IsDormant( ) || !Player->IsAlive( ) || !Player->IsPlayer( ) || Player->GetIndex( ) == gLocalPlayer->GetIndex( ) )
		{
			continue;
		}

		Data->Origins[ Player->GetIndex( ) ] = Player->Origin( );
		Data->SimTimes[ Player->GetIndex( ) ] = Player->SimulationTime( );
		Data->OldSimTimes[ Player->GetIndex( ) ] = Player->OldSimulationTime( );
	}
}

void CMisc::NetApply( )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) )
	{
		NetReset( );
		return;
	}

	int TickBase = gLocalPlayer->TickBase( );
	StoredData_t* Data = &NetData[ TickBase % MULTIPLAYER_BACKUP ];
	Vector PunchAngleDelta = gLocalPlayer->PunchAngle( ) - Data->PunchAngle,
		PunchVelDelta = gLocalPlayer->PunchAngleVel( ) - Data->PunchVelocity,
		ViewOffsetDelta = gLocalPlayer->ViewOffset( ) - Data->ViewOffset,
		OriginDelta = gLocalPlayer->Origin( ) - Data->Origin;

	if ( std::abs( PunchAngleDelta.x ) < 0.03125f && std::abs( PunchAngleDelta.y ) < 0.03125f && std::abs( PunchAngleDelta.z ) < 0.03125f )
	{
		gLocalPlayer->PunchAngle( ) = Data->PunchAngle;
	}

	if ( std::abs( PunchVelDelta.x ) < 0.03125f && std::abs( PunchVelDelta.y ) < 0.03125f && std::abs( PunchVelDelta.z ) < 0.03125f )
	{
		gLocalPlayer->PunchAngleVel( ) = Data->PunchVelocity;
	}

	if ( std::abs( ViewOffsetDelta.x ) < 0.03125f && std::abs( ViewOffsetDelta.y ) < 0.03125f && std::abs( ViewOffsetDelta.z ) < 0.03125f )
	{
		gLocalPlayer->ViewOffset( ) = Data->ViewOffset;
	}

	/*if ( gLocalWeapon )
	{
		float LastFireTimeDelta = gLocalWeapon->LastFireTime( ) - Data->m_flLastFireTime,
			NextPrimaryAttackDelta = gLocalWeapon->NextPrimaryAttack( ) - Data->m_flNextPrimaryAttack,
			TimeWeaponIdleDelta = gLocalWeapon->TimeWeaponIdle( ) - Data->m_flTimeWeaponIdle;

		if ( abs( LastFireTimeDelta ) > 0.315002f )
			gLocalWeapon->LastFireTime( ) += LastFireTimeDelta;

		if ( abs( NextPrimaryAttackDelta ) > 0.315002f )
			gLocalWeapon->NextPrimaryAttack( ) += NextPrimaryAttackDelta;

		if ( abs( TimeWeaponIdleDelta ) > 0.315002f )
			gLocalWeapon->TimeWeaponIdle( ) += TimeWeaponIdleDelta;
	}*/

	/*if ( abs( gLocalPlayer->TickBase( ) - Data->TickBase ) >= 1 )
	{
		gLocalPlayer->TickBase( ) = Data->TickBase;
	}

	if ( std::abs( OriginDelta.x ) < 0.03125f && std::abs( OriginDelta.y ) < 0.03125f && std::abs( OriginDelta.z ) < 0.03125f )
	{
		gLocalPlayer->Origin( ) = Data->Origin;
	}*/

	for ( const auto& Player : gPlayers )
	{
		if ( !Player || Player->IsDormant( ) || !Player->IsAlive( ) || Player->GetIndex( ) == gLocalPlayer->GetIndex( ) )
		{
			continue;
		}

		Vector PlayerOriginDelta = Player->Origin( ) - Data->Origins[ Player->GetIndex( ) ];

		if ( std::abs( PlayerOriginDelta.x ) < 0.03125f && std::abs( PlayerOriginDelta.y ) < 0.03125f 
			&& std::abs( PlayerOriginDelta.z ) < 0.03125f )
		{
			Player->Origin( ) = Data->Origins[ Player->GetIndex( ) ];
		}

		if ( std::abs( Player->SimulationTime( ) - Data->SimTimes[ Player->GetIndex( ) ] ) < 0.03125f )
		{
			Player->SimulationTime( ) = Data->SimTimes[ Player->GetIndex( ) ];
		}

		if ( std::abs( Player->OldSimulationTime( ) - Data->OldSimTimes[ Player->GetIndex( ) ] ) < 0.03125f )
		{
			Player->OldSimulationTime( ) = Data->OldSimTimes[ Player->GetIndex( ) ];
		}
	}
}

void CMisc::NetReset( )
{
	NetData.fill( StoredData_t( ) );
}

void CMisc::OnEvent( IGameEvent* Event )
{
	const auto Attacked = gEntityList->GetClientEntity( gEngineClient->GetPlayerForUserID( Event->GetInt( "userid" ) ) );
	const auto Attacker = gEntityList->GetClientEntity( gEngineClient->GetPlayerForUserID( Event->GetInt( "attacker" ) ) );
	if ( !Attacked || !Attacker || Attacked == Attacker )
		return;

	if ( !Attacker->IsPlayer( ) )
		return;

	if ( Attacker != gLocalPlayer )
		return;

	KilledByLocal[ Attacked->GetIndex( ) ] = true;
}

bool CMisc::CanDoRagdollEffects( CTFRagdoll* ECX )
{
	if ( !ECX || !Config->Visuals->RagdollEffects )
		return false;

	CBasePlayer* Owner = gEntityList->GetClientEntityFromHandle( ECX->Player( ) )->As<CBasePlayer>( );
	if ( Owner )
	{
		if ( Config->Visuals->RagdollLocal && KilledByLocal[ Owner->GetIndex( ) ] )
		{
			KilledByLocal[ Owner->GetIndex( ) ] = false;
			return true;
		}
	}

	if ( Config->Visuals->RagdollEnemy && ECX->IsEnemy( ) )
		return true;

	if ( Config->Visuals->RagdollTeam && !ECX->IsEnemy( ) )
		return true;

	return false;
}

class CAttribute
{
public:
	void* pad = 0;
	uint16_t m_iAttributeDefinitionIndex;
	float m_flValue;
	unsigned int pad2 = 0;

	inline CAttribute( uint16_t iAttributeDefinitionIndex, float flValue )
	{
		m_iAttributeDefinitionIndex = iAttributeDefinitionIndex;
		m_flValue = flValue;
	}
};

class CAttributeList
{
public:
	void* pad;
	CUtlVector<CAttribute> m_Attributes;

	inline void AddAttribute( int iIndex, float flValue )
	{
		if ( m_Attributes.Count( ) > 14 )
			return;

		CAttribute attr( iIndex, flValue );

		m_Attributes.AddToTail( attr );
	}

	using GetItemSchemaFN = void* ( __cdecl* )( );
	using GetAttributeDefinitionFN = void* ( __thiscall* )( void*, int );
	using SetRuntimeAttributeValueFN = void( __thiscall* )( CAttributeList*, void*, float );

	void SetAttribute( int index, float value )
	{
		static auto GetItemSchema = Memory->FindSignature<void*>( "client.dll", "E8 ? ? ? ? 83 C0 ? C3 CC" );
		static auto CEconItemSchema_GetAttributeDefinition = Memory->FindSignature<void*>( "client.dll", "55 8B EC 83 EC ? 53 56 8B D9 8D 4D ? 57 E8 ? ? ? ? 8B 45" );
		static auto CAttributeList_SetRuntimeAttributeValue = Memory->FindSignature<void*>( "client.dll", "55 8B EC 83 EC ? 33 C0 53 8B D9 56 57 8B 7D" );
		auto schema = reinterpret_cast< GetItemSchemaFN >( GetItemSchema )( );

		auto attributeDefinition = reinterpret_cast< GetAttributeDefinitionFN >( CEconItemSchema_GetAttributeDefinition )( schema, index );
		if ( !attributeDefinition )
			return;

		reinterpret_cast< SetRuntimeAttributeValueFN >( CAttributeList_SetRuntimeAttributeValue )( this, attributeDefinition, value );
	}
};

void SwapItemDef( CBaseCombatWeapon* Weapon )
{
	switch ( Weapon->ItemDefinitionIndex( ) )
	{
	case Spy_m_Revolver: Weapon->ItemDefinitionIndex( ) = Spy_m_RevolverR; break;
	case Spy_t_Knife: Weapon->ItemDefinitionIndex( ) = Spy_t_KnifeR; break;
	case Sniper_m_SniperRifle: Weapon->ItemDefinitionIndex( ) = Sniper_m_SniperRifleR; break;
	case Heavy_m_Minigun: Weapon->ItemDefinitionIndex( ) = Heavy_m_MinigunR; break;
	case Medic_s_MediGun: Weapon->ItemDefinitionIndex( ) = Medic_s_MediGunR; break;
	case Scout_m_Scattergun: Weapon->ItemDefinitionIndex( ) = Scout_m_ScattergunR; break;
	}
}

void CMisc::RunTest( )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !gLocalWeapon )
		return;

	if ( !Config->SkinChanger->Enabled )
		return;

	constexpr uint16_t KillStreakActive = 2013;
	constexpr uint16_t KillStreakTier = 2025;
	constexpr uint16_t KillStreakEffect = 2013;
	constexpr uint16_t KillStreakIdleEffect = 2014;
	constexpr uint16_t SetAttachedParticle = 134;

	constexpr int Hot = 701;
	constexpr int Isotope = 702;
	constexpr int Cool = 703;
	constexpr int EnergyOrb = 704;

	auto& m_hMyWeapons = gLocalPlayer->hMyWeapons( );
	for ( int i = 0; m_hMyWeapons[ i ].IsValid( ); i++ )
	{
		auto Weapon = m_hMyWeapons[ i ].Get( );
		if ( Weapon)
		{
			SwapItemDef( Weapon );
			auto AttribList = reinterpret_cast< CAttributeList* >( Weapon + 0x9C4 );
			if ( Config->SkinChanger->Skins[ Weapon->GetIndex( ) ].Aussie )
			{
				AttribList->SetAttribute( 2027, true );
				AttribList->SetAttribute( 2022, true );
				AttribList->SetAttribute( 542, true );
				AttribList->SetAttribute( 273, true );
			}

			if ( Config->SkinChanger->Skins[ Weapon->GetIndex( ) ].SwapModel )
			{
				auto Entry = Config->SkinChanger->Skins[ Weapon->GetIndex( ) ].Model;
				switch ( Entry )
				{
				case 0: Weapon->ItemDefinitionIndex( ) = Sniper_m_SniperRifleR; break;
				case 1: Weapon->ItemDefinitionIndex( ) = Sniper_m_ShootingStar; break;
				case 2: Weapon->ItemDefinitionIndex( ) = Sniper_m_TheMachina; break;
				case 3: Weapon->ItemDefinitionIndex( ) = Soldier_m_RocketLauncherR; break;
				case 4: Weapon->ItemDefinitionIndex( ) = Demoman_m_GrenadeLauncherR; break;
				case 5: Weapon->ItemDefinitionIndex( ) = Heavy_m_MinigunR; break;
				case 6: Weapon->ItemDefinitionIndex( ) = Soldier_m_TheDirectHit; break;
				case 7: Weapon->ItemDefinitionIndex( ) = Soldier_m_TheOriginal; break;
				case 8: Weapon->ItemDefinitionIndex( ) = Soldier_m_TheCowMangler5000; break;
				}
			}
		}
	}
}

void CMisc::AutoRetreat( CUserCmd* pCmd )
{
	static Vector StartPosition = {};
	static bool HasFiredShot = false;

	if ( !Config->Misc->AutoRetreat || !gLocalPlayer || !gLocalPlayer->IsAlive( ) )
	{
		StartPosition = Vector( );
		HasFiredShot = false;
		return;
	}

	if ( ( ( GetAsyncKeyState( Config->Misc->AutoRetreatKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) )
	{
		if ( StartPosition.IsZero( ) )
		{
			StartPosition = gLocalPlayer->Origin( );
		}
		else
		{
			// TODO: Draw the start position

			if ( TF2->IsAttacking( pCmd ) )
			{
				HasFiredShot = true;
				return;
			}

			if ( HasFiredShot )
			{
				Vector Current = gLocalPlayer->GetAbsOrigin( );
				Vector Diffrence = Current - StartPosition;
				if ( Diffrence.Length2D( ) > 5.f )
				{
					float Yaw = gEngineClient->GetViewAngles( ).y;

					Vector Velocity = Vector( 
						Diffrence.x * cos( Yaw / 180.0f * PI ) + Diffrence.y * sin( Yaw / 180.0f * PI ),
						Diffrence.y * cos( Yaw / 180.0f * PI ) - Diffrence.x * sin( Yaw / 180.0f * PI ),
						Diffrence.z );

					pCmd->forwardmove = -Velocity.x * 20.f;
					pCmd->sidemove = Velocity.y * 20.f;
				}
				else
				{
					StartPosition = Vector( );
					HasFiredShot = false;
				}
			}
		}
	}
	else
	{
		StartPosition = Vector( );
		HasFiredShot = false;
	}
}