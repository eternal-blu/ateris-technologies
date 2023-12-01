#include "Anti-Aimbot.h"
#include "../../../Globals.h"
#include "../Models/Models.h"
#include "../Outlines/Outlines.h"
#include "../Misc/Misc.h"

std::pair<float, float> CAntiAim::GetRagePitch( )
{
	std::pair<float, float> FinalPitch = { 0.f, 0.f };

	switch ( Config->Misc->AntiAim->Pitch )
	{
		case 1: //automatic
		{
			switch ( gLocalPlayer->Class( ) )
			{
				case CLASS_SCOUT:
				{
					FinalPitch.first = -91.0f;
					FinalPitch.second = 91.0f;
					break;
				}
				case CLASS_SOLDIER:
				case CLASS_PYRO:
				case CLASS_DEMOMAN:
				case CLASS_ENGINEER:
				case CLASS_MEDIC:
				{
					FinalPitch.first = 91.0f;
					FinalPitch.second = 91.0f;
					break;
				}
				case CLASS_HEAVY:
				{
					if ( gLocalWeapon->Slot( ) == 1 )
					{
						FinalPitch.first = 91.0f;
						FinalPitch.second = 91.0f;
					}
					else
					{
						FinalPitch.first = -91.0f;
						FinalPitch.second = -91.0f;
					}
					break;
				}
				case CLASS_SNIPER:
				case CLASS_SPY:
				{
					FinalPitch.first = -91.0f;
					FinalPitch.second = -91.0f;
					break;
				}
				default: break;
			}
			break;
		}

		case 2: //manual
		{
			switch ( Config->Misc->AntiAim->RealPitch )
			{
				case 0: //up
				{
					if ( Config->Misc->AntiAim->FakePitch )
					{
						FinalPitch.first = 271.0f;
					}
					else
					{
						FinalPitch.first = -91.0f;
					}
					FinalPitch.second = -91.0f;
					break;
				}

				case 1: //down
				{
					if ( Config->Misc->AntiAim->FakePitch )
					{
						FinalPitch.first = -271.0f; // use prime numbers.
					}
					else
					{
						FinalPitch.first = 91.0f;
					}
					FinalPitch.second = 91.0f;
					break;
				}

				case 2: //jitter
				{
					if ( IsPitchJittering )
					{
						FinalPitch.first = -271.0f;
						FinalPitch.second = 91.0f;
					}
					else
					{
						FinalPitch.first = 271.0f;
						FinalPitch.second = -91.0f;
					}
					break;
				}
				default: break;
			}
			break;
		}
		default: break;
	}
	return FinalPitch;
}

float CAntiAim::GetRealRageYaw( )
{
	float& LastAngle = LastRealAngleUpdate;
	float& LastAngleRotate = LastRealAngleUpdate;
	float FinalAngle = 0.f;
	switch ( Config->Misc->AntiAim->RealMode )
	{
		case 0: //static
		{
			FinalAngle = Config->Misc->AntiAim->RealYaw;
			if ( FinalAngle < 1 && FinalAngle > -1 )
				FinalAngle = 0.1f;
			if ( /*IsRealFlipped*/ IsFlipped )
			{
				if ( FinalAngle > 0.f ) { FinalAngle -= 180.f; }
				else if ( FinalAngle < 0.f ) { FinalAngle += 180.f; }
			}
			break;
		}
		case 1: //jitter
		{
			if ( IsRealJittering )
			{
				if ( /*IsRealFlipped*/ IsFlipped )
				{
					FinalAngle += Config->Misc->AntiAim->RealDelta;
				}
				else
				{
					FinalAngle -= Config->Misc->AntiAim->RealDelta;
				}
			}
			else
			{
				if ( /*IsRealFlipped*/ IsFlipped )
				{
					FinalAngle -= Config->Misc->AntiAim->RealDelta;
				}
				else
				{
					FinalAngle += Config->Misc->AntiAim->RealDelta;
				}
			}
			FinalAngle += Config->Misc->AntiAim->RealYaw;
			if ( /*IsRealFlipped*/ IsFlipped )
			{
				if ( FinalAngle > 0.f ) { FinalAngle -= 180.f; }
				else if ( FinalAngle < 0.f ) { FinalAngle += 180.f; }
			}
			if ( FinalAngle > 180.f ) { FinalAngle -= 360.f; }
			if ( FinalAngle < -180.f ) { FinalAngle += 360.f; }
			break;
		}
		case 2: //spin
		{
			LastAngle += 1 * Config->Misc->AntiAim->RealSpin;
			if ( LastAngle > 180.f ) { LastAngle -= 360.f; }
			if ( LastAngle < -180.f ) { LastAngle += 360.f; }
			FinalAngle = LastAngle;
			break;
		}
		case 3: //rotate
		{
			static bool flip = false;

			if ( Config->Misc->AntiAim->RealAngle1 < Config->Misc->AntiAim->RealAngle2 )
			{
				if ( LastAngleRotate <= Config->Misc->AntiAim->RealAngle1 )
					flip = true;

				if ( LastAngleRotate >= Config->Misc->AntiAim->RealAngle2 )
					flip = false;
			}
			else
			{
				if ( LastAngleRotate <= Config->Misc->AntiAim->RealAngle2 )
					flip = true;

				if ( LastAngleRotate >= Config->Misc->AntiAim->RealAngle1 )
					flip = false;
			}

			if ( flip )
			{
				LastAngleRotate += 1 * Config->Misc->AntiAim->RealRotate;
			}
			else
			{
				LastAngleRotate -= 1 * Config->Misc->AntiAim->RealRotate;
			}

			if ( LastAngleRotate > 180.f ) { LastAngleRotate -= 360.f; }
			if ( LastAngleRotate < -180.f ) { LastAngleRotate += 360.f; }
			FinalAngle = LastAngleRotate;
			if ( /*IsRealFlipped*/ IsFlipped )
			{
				if ( FinalAngle > 0.f ) { FinalAngle -= 180.f; }
				else if ( FinalAngle < 0.f ) { FinalAngle += 180.f; }
			}
			FinalAngle += Config->Misc->AntiAim->RealYaw;
			break;
		}
		default: break;
	}
	if ( FinalAngle < 1 && FinalAngle > -1 )
		FinalAngle = 0.1f;

	return FinalAngle;
}

float CAntiAim::GetFakeRageYaw( )
{
	float& LastAngle = LastFakeAngleUpdate;
	float& LastAngleRotate = LastFakeAngleUpdate;
	float FinalAngle = 0.f;
	switch ( Config->Misc->AntiAim->FakeMode )
	{
		case 0: //static
		{
			FinalAngle = Config->Misc->AntiAim->FakeYaw;
			if ( FinalAngle < 1 && FinalAngle > -1 )
				FinalAngle = 0.1f;
			if ( /*IsFakeFlipped*/ IsFlipped )
			{
				if ( FinalAngle > 0.f ) { FinalAngle -= 180.f; }
				else if ( FinalAngle < 0.f ) { FinalAngle += 180.f; }
			}
			break;
		}
		case 1: //jitter
		{
			if ( IsFakeJittering )
			{
				if ( /*IsFakeFlipped*/ IsFlipped )
				{
					FinalAngle += Config->Misc->AntiAim->FakeDelta;
				}
				else
				{
					FinalAngle -= Config->Misc->AntiAim->FakeDelta;
				}
			}
			else
			{
				if ( /*IsFakeFlipped*/ IsFlipped )
				{
					FinalAngle -= Config->Misc->AntiAim->FakeDelta;
				}
				else
				{
					FinalAngle += Config->Misc->AntiAim->FakeDelta;
				}
			}
			FinalAngle += Config->Misc->AntiAim->FakeYaw;
			if ( /*IsFakeFlipped*/ IsFlipped )
			{
				if ( FinalAngle > 0.f ) { FinalAngle -= 180.f; }
				else if ( FinalAngle < 0.f ) { FinalAngle += 180.f; }
			}
			if ( FinalAngle > 180.f ) { FinalAngle -= 360.f; }
			if ( FinalAngle < -180.f ) { FinalAngle += 360.f; }
			break;
		}
		case 2: //spin
		{
			LastAngle += 1 * Config->Misc->AntiAim->FakeSpin;
			if ( LastAngle > 180.f ) { LastAngle -= 360.f; }
			if ( LastAngle < -180.f ) { LastAngle += 360.f; }
			FinalAngle = LastAngle;
			break;
		}
		case 3: //rotate
		{
			static bool flip = false;

			if ( Config->Misc->AntiAim->FakeAngle1 < Config->Misc->AntiAim->FakeAngle2 )
			{
				if ( LastAngleRotate <= Config->Misc->AntiAim->FakeAngle1 )
					flip = true;

				if ( LastAngleRotate >= Config->Misc->AntiAim->FakeAngle2 )
					flip = false;
			}
			else
			{
				if ( LastAngleRotate <= Config->Misc->AntiAim->FakeAngle2 )
					flip = true;

				if ( LastAngleRotate >= Config->Misc->AntiAim->FakeAngle1 )
					flip = false;
			}

			if ( flip )
			{
				LastAngleRotate += 1 * Config->Misc->AntiAim->FakeRotate;
			}
			else
			{
				LastAngleRotate -= 1 * Config->Misc->AntiAim->FakeRotate;
			}

			if ( LastAngleRotate > 180.f ) { LastAngleRotate -= 360.f; }
			if ( LastAngleRotate < -180.f ) { LastAngleRotate += 360.f; }
			FinalAngle = LastAngleRotate;
			if ( /*IsFakeFlipped*/ IsFlipped )
			{
				if ( FinalAngle > 0.f ) { FinalAngle -= 180.f; }
				else if ( FinalAngle < 0.f ) { FinalAngle += 180.f; }
			}
			FinalAngle += Config->Misc->AntiAim->FakeYaw;
			break;
		}
		default: break;
	}
	if ( FinalAngle < 1 && FinalAngle > -1 )
		FinalAngle = 0.1f;

	return FinalAngle;
}

float CAntiAim::GetLegitYaw( )
{
	float FinalAngle = 0.f;

	FinalAngle = Config->Misc->AntiAim->LegitYaw;
	if ( FinalAngle < 1 && FinalAngle > -1 )
		FinalAngle = 0.1f;
	if ( /*IsRealFlipped*/ IsFlipped )
	{
		if ( FinalAngle > 0.f ) { FinalAngle -= 180.f; }
		else if ( FinalAngle < 0.f ) { FinalAngle += 180.f; }
	}

	return FinalAngle;
}

void CAntiAim::RealAngles( CUserCmd* Cmd )
{
	G->AntiAim->AASet = { false, false };

	if ( !Config->Misc->AntiAim->Enabled ) { return; }

	if ( !gLocalPlayer->IsAlive( )
		 || gLocalPlayer->IsTaunting( )
		 || gLocalPlayer->IsGhost( )
		 || gLocalPlayer->IsInKart( ) ) {
		return;
	}

	if ( gLocalPlayer->MoveType( ) == MOVETYPE_NOCLIP
		 || gLocalPlayer->MoveType( ) == MOVETYPE_LADDER
		 || gLocalPlayer->MoveType( ) == MOVETYPE_OBSERVER )
	{
		return;
	}

	if ( gLocalPlayer->IsCharging( ) ) {
		return;
	}

	if ( IsToggled )
	{
		if ( Config->Misc->AntiAim->RageActive )
		{
			/*if ( ( GetAsyncKeyState( Config->Misc->AntiAim->CycleKey ) & 0x1 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
			{
				IsRealFlipped = !IsRealFlipped;
			}*/

			IsPitchJittering = !IsPitchJittering;
			if ( Config->Misc->AntiAim->Pitch ) {
				const float RealPitch = GetRagePitch( ).second;
				Cmd->viewangles.x = RealPitch;
			}

			IsRealJittering = !IsRealJittering;
			Cmd->viewangles.y += GetRealRageYaw( );
			G->AntiAim->AASet.first = true;
		}
		else
		{
			/*if ( ( GetAsyncKeyState( Config->Misc->AntiAim->CycleKey ) & 0x1 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
			{
				IsRealFlipped = !IsRealFlipped;
			}*/

			Cmd->viewangles.y += GetLegitYaw( );
			G->AntiAim->AASet.first = true;
		}
	}
}

void CAntiAim::FakeAngles( CUserCmd* Cmd )
{
	G->AntiAim->AASet = { false, false };

	if ( !Config->Misc->AntiAim->Enabled ) { return; }

	if ( !gLocalPlayer->IsAlive( )
		 || gLocalPlayer->IsTaunting( )
		 || gLocalPlayer->IsGhost( )
		 || gLocalPlayer->IsInKart( ) ) {
		return;
	}

	if ( gLocalPlayer->MoveType( ) == MOVETYPE_NOCLIP
		 || gLocalPlayer->MoveType( ) == MOVETYPE_LADDER
		 || gLocalPlayer->MoveType( ) == MOVETYPE_OBSERVER )
	{
		return;
	}

	if ( gLocalPlayer->IsCharging( ) )
	{
		return;
	}

	if ( IsToggled )
	{
		if ( Config->Misc->AntiAim->RageActive )
		{
			/*if ( ( GetAsyncKeyState( Config->Misc->AntiAim->CycleKey ) & 0x1 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
			{
				IsFakeFlipped = !IsFakeFlipped;
			}*/

			if ( Config->Misc->AntiAim->Pitch ) {
				const float FakePitch = GetRagePitch( ).first;
				Cmd->viewangles.x = FakePitch;
			}

			IsFakeJittering = !IsFakeJittering;
			Cmd->viewangles.y += GetFakeRageYaw( );
			G->AntiAim->AASet.second = true;
		}
		else
		{
			Cmd->viewangles.y += 0.1f;
			G->AntiAim->AASet.second = true;
		}
	}
}

void CAntiAim::HandleChoke( CUserCmd* Cmd, bool* SendPacket )
{
	// scout typically leaves 1 tick behind
	// todo: charge 24, shift 21? for charge speed reasons?
	const int MaxAllowedChoke = G->Doubletap.Charged > 2 ? 1 : 4;

	if ( Config->Misc->FakeLag->Enabled && Misc->IsFakeLagToggled )
	{
		if ( gClientState->chokedcommands == G->FakeLag->Ticks )
		{
			AntiAim->FakeAngles( Cmd );
			G->AntiAim->FakeAngles = Cmd->viewangles;
		}
		else
		{
			AntiAim->RealAngles( Cmd );
			G->AntiAim->RealAngles = Cmd->viewangles;
		}
	}
	else
	{
		// ensure tick choke
		if ( gClientState->chokedcommands < MaxAllowedChoke )
		{
			*SendPacket = false;
			AntiAim->RealAngles( Cmd );
			G->AntiAim->RealAngles = Cmd->viewangles;
		}
		else
		{
			*SendPacket = true;
			AntiAim->FakeAngles( Cmd );
			G->AntiAim->FakeAngles = Cmd->viewangles;
		}
	}
}

void CAntiAim::CreateMove( CUserCmd* Cmd, bool* SendPacket )
{
	if ( !gLocalPlayer || !gLocalWeapon || !Config->Misc->AntiAim->Enabled || ( gLocalWeapon && gLocalWeapon->IsAttacking( Cmd ) ) 
		 || G->Doubletap.QueueRecharge || G->Doubletap.Recharge || G->FakeLag->FakeDucking )
		return;

	if ( TF2->IsAttacking( Cmd ) || ( ( Cmd->buttons & IN_ATTACK2 ) 
		&& G->Aimbot.SavedWeaponID == TF_WEAPON_FLAMETHROWER 
		&& gLocalWeapon->ItemDefinitionIndex( ) != Pyro_m_ThePhlogistinator ) )
	{
		return;
	}

	if ( gLocalPlayer->IsCharging( ) )
		return;

	//todo:
	//legit aa: limit angles to 89.f and -89.f difference between real and fake, choke 1-2 cmds
	//rage aa: if we have dt charged, choke the same amount as before, else choke 3-4 cmds or however many fakelag ticks we choke
	//HandleChoke( Cmd, SendPacket );

	if ( Config->Misc->FakeLag->Enabled && Misc->IsFakeLagToggled )
	{
		// what the fuck is this nigger
		if ( G->Doubletap.Charged < 22 - G->FakeLag->Ticks )
		{
			if ( gClientState->chokedcommands == G->FakeLag->Ticks )
			{
				AntiAim->FakeAngles( Cmd );
				G->AntiAim->FakeAngles = Cmd->viewangles;
			}
			else
			{
				AntiAim->RealAngles( Cmd );
				G->AntiAim->RealAngles = Cmd->viewangles;
			}
		}
		else if ( G->Doubletap.ToSend <= 0 && G->Doubletap.Charged > ( gLocalWeapon->IsForceANature( ) ? 2 : 0 ) )
		{
			if ( gGlobals->tickcount % 3 != 0 )
			{
				if ( IsToggled )
				{
					*SendPacket = false;
				}
				AntiAim->RealAngles( Cmd );
				G->AntiAim->RealAngles = Cmd->viewangles;
			}
			else
			{
				if ( IsToggled )
				{
					*SendPacket = true;
				}
				AntiAim->FakeAngles( Cmd );
				G->AntiAim->FakeAngles = Cmd->viewangles;
			}
		}
	}
	else
	{
		if ( G->Doubletap.ToSend > 0 && G->Doubletap.Charged > ( gLocalWeapon->IsForceANature( ) ? 2 : 0 ) )
			return;

		if ( gGlobals->tickcount % 3 != 0 )
		{
			if ( IsToggled )
			{
				*SendPacket = false;
			}
			AntiAim->RealAngles( Cmd );
			G->AntiAim->RealAngles = Cmd->viewangles;
		}
		else
		{
			if ( IsToggled )
			{
				*SendPacket = true;
			}
			AntiAim->FakeAngles( Cmd );
			G->AntiAim->FakeAngles = Cmd->viewangles;
		}
	}
}

void CAntiAim::DrawFakeModel( void* ECX, void* EDX, const DrawModelState_t& State, const ModelRenderInfo_t& Info, std::function< void( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4* ) > f )
{
	auto Entity = gEntityList->GetClientEntity( Info.entity_index )->As<CBaseEntity>( );
	if ( !gLocalPlayer || !Entity || Entity != gLocalPlayer || Entity == gLocalWeapon )
		return;

	if ( !Config->Misc->AntiAim->Enabled || !Config->Misc->AntiAim->DrawFake || !AntiAim->IsToggled || G->Doubletap.QueueRecharge || G->Doubletap.Recharge || G->FakeLag->FakeDucking )
		return;

	if ( ( G->AntiAim->RealAngles.y - G->AntiAim->FakeAngles.y < 1.f && G->AntiAim->RealAngles.y - G->AntiAim->FakeAngles.y > -1.f ) )
		return;

	if ( G->FakeTaunt->FakeTauntTime > gEngineClient->Time( ) )
		return;

	if ( !Config->Misc->AntiAim->FakeModel && !Config->Misc->AntiAim->FakeOutline )
		return;

	if ( Outlines->IsRendering( ) && !Models->IsRendering( ) && Config->Misc->AntiAim->FakeOutline && Config->Outlines->Global->Enabled && Config->Models->Global->Enabled && Config->Models->Players->Enabled )
	{
		gRenderView->SetColorModulation( Config->Colors->FakeGlow );

		G->AnimationFix->DrawingFakeAngles = true;

		if ( G->AnimationFix->FakeMatrixBuilt )
			f( ECX, EDX, State, Info, G->AnimationFix->FakeMatrix );

		G->AnimationFix->DrawingFakeAngles = false;
	}

	if ( Models->IsRendering( ) && !Outlines->IsRendering( ) && Config->Models->Global->Enabled )
	{
		if ( Models->FlatMaterial )
			gModelRender->ForcedMaterialOverride( Models->FlatMaterial );

		gRenderView->SetColorModulation( Config->Colors->FakeModel );

		if ( Config->Misc->AntiAim->FakeModel )
		{
			gRenderView->SetBlend( Config->Misc->AntiAim->FakeAlpha );
		}
		else
		{
			gRenderView->SetBlend( 0.f );
		}

		G->AnimationFix->DrawingFakeAngles = true;

		if ( G->AnimationFix->FakeMatrixBuilt )
			f( ECX, EDX, State, Info, G->AnimationFix->FakeMatrix );

		G->AnimationFix->DrawingFakeAngles = false;
	}
}

void CAntiAim::DrawLagModel( void* ECX, void* EDX, const DrawModelState_t& State, const ModelRenderInfo_t& Info, std::function< void( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4* ) > f )
{
	auto Entity = gEntityList->GetClientEntity( Info.entity_index )->As<CBaseEntity>( );
	if ( !gLocalPlayer || !Entity || Entity != gLocalPlayer || Entity == gLocalWeapon || gClientState->chokedcommands < 1 )
		return;

	if ( !Config->Misc->FakeLag->Enabled || !Config->Misc->FakeLag->DrawLag )
		return;

	if ( !Config->Misc->FakeLag->LagModel && !Config->Misc->FakeLag->LagOutline )
		return;

	if ( Outlines->IsRendering( ) && Config->Misc->FakeLag->LagOutline && Config->Outlines->Global->Enabled )
	{
		if ( gLocalPlayer->Velocity( ).Length2D( ) > 2.f )
		{
			gRenderView->SetColorModulation( Config->Colors->LagGlow );

			if (G->AnimationFix->FakeLagMatrixBuilt )
				f( ECX, EDX, State, Info, G->AnimationFix->NonInterpolatedBones );
		}
	}

	if ( Models->IsRendering( ) && Config->Models->Global->Enabled )
	{
		if ( Models->FlatMaterial )
			gModelRender->ForcedMaterialOverride( Models->FlatMaterial );

		gRenderView->SetColorModulation( Config->Colors->LagModel );

		if ( Config->Misc->FakeLag->LagModel )
		{
			gRenderView->SetBlend( Config->Misc->FakeLag->ModelAlpha );
		}
		else
		{
			gRenderView->SetBlend( 0.f );
		}

		if ( gLocalPlayer->Velocity( ).Length2D( ) > 2.f )
		{
			if (G->AnimationFix->FakeLagMatrixBuilt )
				f( ECX, EDX, State, Info, G->AnimationFix->NonInterpolatedBones );
		}
	}
}