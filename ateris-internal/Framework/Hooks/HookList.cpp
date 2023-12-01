#include "HookList.h"
#include "../Features/EnginePrediction/Engine.h"
#include "../Features/ESP/ESP.h"
#include "../Features/Misc/Misc.h"
#include "../Features/LagCompensation/LagComp.h"
#include "../Features/Aimbot/Aimbot.h"
#include "../Features/Projectile/Projectile.h"
#include "../../Globals.h"
#include "../Features/Menu/Menu.h"
#include "../Features/CritHacks/CritHacks.h"
#include "../Features/Visuals/Visuals.h"
#include "../Features/Auto/AutoVaccinator/AutoVacc.h"
#include "../Features/Models/Models.h"
#include "../Features/Legitbob/Legitbob.h"
#include "../Features/Outlines/Outlines.h"
#include "../Features/Anti-Aimbot/Anti-Aimbot.h"
#include "../Features/Melee/MeleeBob.h"
#include "../Features/Confidence/Confidence.h"
#include "../Features/Auto/AutoMedic/AutoMedic.h"
#include "../Features/Auto/AutoDetonate/AutoDetonate.h"
#include "../Features/NoSpread/NoSpread.h"
#include "../Features/Auto/AutoAirblast/AutoAirblast.h"
#include "../Features/AnimFix/AnimFix.h"

HOOK_VTABLE( CreateMove, gClientMode, 21u, bool, __fastcall, void* ECX, void* EDX, float InputSampleTime, CUserCmd* Cmd )
{
	if ( !Cmd || !Cmd->command_number || G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, InputSampleTime, Cmd );

	if ( gLocalPlayer = gEntityList->GetClientEntity( gEngineClient->GetLocalPlayer( ) )->As<CBasePlayer>( ) )
	{
		gLocalWeapon = gEntityList->GetClientEntity( gEngineClient->GetLocalPlayer( ) )->As<CBasePlayer>( )->ActiveWeapon( );
	}
	else
	{
		gLocalWeapon = nullptr;
	}

	if ( !gLocalPlayer || !gEngineClient->IsInGame( ) )
	{
		gLocalWeapon = nullptr;
		return OriginalFunction( ECX, EDX, InputSampleTime, Cmd );
	}

	if (OriginalFunction(ECX, EDX, InputSampleTime, Cmd))
		gEngineClient->SetViewAngles( Cmd->viewangles );
	
	uint32_t _bp = 0; __asm mov _bp, ebp;
	bool* SendPacket = reinterpret_cast<bool*>(***reinterpret_cast<uint32_t***>(_bp) - 0x1);
	G->Doubletap.SendPacket = SendPacket;

	Vector OldAngles = Cmd->viewangles;
	float OldForward = Cmd->forwardmove;
	float OldSide = Cmd->sidemove;
	int OldFlags = gLocalPlayer->Flags( );

	gPrediction->Update( gClientState->m_nDeltaTick,
						 gClientState->m_nDeltaTick > 0,
						 gClientState->last_command_ack,
						 gClientState->lastoutgoingcommand + gClientState->chokedcommands );

	gWD->WD_StartData( );

	if ( gLocalPlayer && gLocalPlayer->IsAlive( ) && gLocalWeapon )
		G->Aimbot.SavedWeaponID = gLocalWeapon->WeaponID( );

	Misc->BunnyHoppenz(Cmd);
	Misc->AirStrafer(Cmd);
	Misc->ScoutJump( Cmd );
	Misc->RocketJump( Cmd );
	Misc->FastStop(Cmd);
	Misc->JitterMove( Cmd );

	if ( G->Doubletap.ToSend > 0 )
	{
		if ( !G->Doubletap.Warp )
			Misc->AntiWarp( Cmd );

		*SendPacket = G->Doubletap.ToSend == 1;
	}


	// TODO:
	// MOVE THIS SHIT
	//  TO MISC FFS
	//update once every half a second
	static auto cl_cmdrate = gCvar->FindVar( "cl_cmdrate" );
	if ( cl_cmdrate && gLocalPlayer && gPlayerResource && gClientState->m_NetChannel 
		&& Config->Aimbot->Hitscan->ReduceScoreboardPing )
	{
		int Ping = gPlayerResource->GetPing( gLocalPlayer->GetIndex( ) );
		NET_SetConVar cmdrate = { "cl_cmdrate", Ping > Config->Aimbot->Hitscan->ScoreboardPingGoal ? "1" : std::to_string( cl_cmdrate->GetInt( ) ).c_str( ) };
		if ( gGlobals->tickcount % 32 == 0 )
		{
			SendNetMessage::OriginalFunction( gClientState->m_NetChannel, nullptr, &cmdrate, false, false );
		}
	}

	//anti afk, should work in theory unless im retarded
	// BRO
	// MAKE THIS SHIT STATIC
	// WHAT THE FUCK
	//oops
	static auto mp_idealmaxtime = gCvar->FindVar( "mp_idlemaxtime" );
	if ( mp_idealmaxtime && mp_idealmaxtime->GetInt( ) )
	{
		auto Ticks = TIME_TO_TICKS( mp_idealmaxtime->GetInt( ) * 60.f );
		auto Interval = ( int )( ( Ticks / 2.f ) - 1.f );

		if ( Cmd->command_number % Interval == 0 )
		{
			Cmd->buttons |= IN_GRENADE1;
		}
	}

	if ( G->KillSay->Time < gEngineClient->Time( ) && G->KillSay->Time > 0.1f )
	{
		G->KillSay->Time = 0.f;
		G->KillSay->Stopped = true;
		Misc->StopRecording( );
	}

	static int OldClass = gLocalPlayer->Class( );
	if ( OldClass != gLocalPlayer->Class( ) )
	{
		CritHack->HasFiredMelee = false;
		OldClass = gLocalPlayer->Class( );
	}

	G->Aimbot.AimPoint = { 0.0f };
	Prediction->Predict(Cmd);
	{
		Aimbot->Run(Cmd);
		LegitBob->Run(Cmd);
		Projectile->Run(Cmd);
		Misc->ShootStickyAtCharge( Cmd );
		MeleeBob->Run( Cmd );
		AutoVaccinator->Run(Cmd);
		AutoMedic->Run(Cmd);
		AutoDetonation->Run(Cmd);
		AutoAirblast->Run( Cmd );

		Misc->FakeLag( Cmd, SendPacket );
		Misc->FakeDuck( Cmd, SendPacket );
		Misc->AntiBackstab(Cmd);
		Misc->AutoRetreat( Cmd );
		Misc->EdgeJump( Cmd, OldFlags & FL_ONGROUND );
		AntiAim->CreateMove( Cmd, SendPacket );

		CritHack->Force( Cmd );
	}
	Prediction->End(Cmd);

	Misc->HandleDelayDT();

	if (G->Doubletap.Warp)
		Cmd->buttons &= ~IN_ATTACK;

	if (TF2->IsAttacking(Cmd) && Config->Misc->DT->Enabled)
	{
		int Required = MAX_COMMANDS - ( gLocalWeapon->IsForceANature() ? 2 : 0 );
		if ( Config->Misc->DT->WarpMode == 2 )
			if ( Required >= G->Doubletap.Charged )
				Required = G->Doubletap.Charged;

		if (G->Doubletap.Charged >= Required && !TF2->IsNonAimable(gLocalWeapon) && Misc->AllowedToDoubleTap())
		{
			G->Doubletap.ToSend = Required;
			if ( gClientState )
				G->Doubletap.ToSendInternal = Required;
			if ( G->Doubletap.Charged > 2 )
			{
				if ( G->Aimbot.SavedWeaponID == TF_WEAPON_PIPEBOMBLAUNCHER )
				{
					G->Doubletap.ToSend = 20;
				}
			}
			*SendPacket = false;
		}
	}

	//gEngineClient->SetViewAngles( Cmd->viewangles );

	Misc->FastBackup(Cmd);

	if (G->Doubletap.ToSend <= 0 && TF2->IsProjectileWeapon(gLocalWeapon))
	{
		static bool WasSet = false;
		if (TF2->IsAttacking(Cmd) || ( G->Aimbot.SavedWeaponID == 
			TF_WEAPON_FLAME_BALL && gLocalWeapon->CanFire()))
		{
			*SendPacket = false;
			WasSet = true;
		}
		else
		{
			if (WasSet)
			{
				*SendPacket = true;
				Cmd->viewangles = OldAngles;
				Cmd->forwardmove = OldForward;
				Cmd->sidemove = OldSide;
				WasSet = false;
			}
		}
	}

	G->AnimationFix->CurrentCmd = Cmd;
	G->AnimationFix->CurrentAngles = Cmd->viewangles;
	//AnimFix->Update( );

	if ( *SendPacket )
	{
		G->AntiAim->RealYaw = G->AntiAim->RealAngles.y;

		if ( Config->Misc->FakeLag->Enabled )
		{
			G->AnimationFix->FakeLagMatrixBuilt = gLocalPlayer->SetupBones( G->AnimationFix->NonInterpolatedBones, 128, BONE_USED_BY_ANYTHING, gGlobals->curtime );
		}
	}

	if ( gClientState && gClientState->chokedcommands > 24 )
	{
		*SendPacket = true;
		gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Safe-guard triggered!\n" );
	}

	NoSpread->WasInAttack = Cmd->buttons & IN_ATTACK;
	NoSpread->CreateMoveHandler( Cmd );

	TF2->FixMovement( Cmd, OldAngles );
	Cmd->viewangles.y = gMath->NormalizeAngle( Cmd->viewangles.y );

	return false;
}

HOOK_VTABLE( Paint, gEngineVGui, 14u, void, __fastcall, void* ECX, void* EDX, PaintMode_t Mode )
{
	OriginalFunction( ECX, EDX, Mode );

	if ( G->DetachSystem->WantsDetach )
		return;

	if (!gEngineClient->IsConnected() || !gEngineClient->IsInGame())
	{
		/*gLocalPlayer = nullptr;
		gLocalWeapon = nullptr;*/
		CritHack->Reset();
	}

	if (gEngineClient->IsTakingScreenshot() )
		return;

	if ( Mode & PAINT_UIPANELS )
	{
		if ( !TF2->ScreenWidth || !TF2->ScreenHeight )
		{
			gEngineClient->GetScreenSize( TF2->ScreenWidth, TF2->ScreenHeight );
		}

		TF2->UpdateWorldToScreen( );

		gSurface->StartDrawing( );
		{
			if ( G->Authentication->IsDebugAllowed && Config->Debug->EnableInfo )
			{
				Draw->String( 5, 50, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "Charged: %i", G->Doubletap.Charged );
				Draw->String( 5, 62, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "Sent: %i", G->Doubletap.Sent );
				Draw->String( 5, 74, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "ToSend: %i", G->Doubletap.ToSend );
			}

			//Draw->String( 5, 110, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "gTurnSpeed: %f", G->Aimbot.TurnSpeed);
			if ( gLocalPlayer && gLocalPlayer->IsAlive( ) && gLocalWeapon )
			{
				if ( G->Authentication->IsDebugAllowed && Config->Debug->EnableInfo )
				{
					//Draw->String( 5, 62, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "Added: %i", CritHack->Data->AddedCritDamageAmt );
					Draw->String( 5, 86, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "correct critchance: %f", CritHack->Data->CorrectCritChance );
					Draw->String( 5, 98, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "cost: %f", CritHack->Data->CritCost );
					//Draw->String( 5, 98, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "crit damage: %f", CritHack->Data->Damage->CritDamage );
					Draw->String( 5, 110, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "unfixed chance: %f", CritHack->Data->UnFixedChance );
					Draw->String( 5, 134, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "Multiplier: %f", CritHack->Data->Multiplier );
					Draw->String( 5, 146, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "CritSeedRequests: %i", gLocalWeapon->CritSeedRequests( ) );
					Draw->String( 5, 158, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "CritChecks: %i", gLocalWeapon->CritChecks( ) );
					Draw->String( 5, 170, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "CanCrit: %i", CritHack->Data->CanCrit );
					Draw->String( 5, 182, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "ObservedCritChance: %f", gLocalWeapon->ObservedCritChance( ) );
				
				/*if ( gLocalPlayer->ModelPtr( ) )
				{
					G->FakeTaunt->Printing = true;
					Draw->String( 5, 266, FONT_ESP, 0, { 255, 255, 255, 255 }, true, "sequence: %i", gLocalPlayer->LookupSequence( gLocalPlayer->ModelPtr( ), "taunt01" ) );
					Draw->String( 5, 278, FONT_ESP, 0, { 255, 255, 255, 255 }, true, "sequence: %i", gLocalPlayer->LookupSequence( gLocalPlayer->ModelPtr( ), "taunt02" ) );
					Draw->String( 5, 290, FONT_ESP, 0, { 255, 255, 255, 255 }, true, "sequence: %i", gLocalPlayer->LookupSequence( gLocalPlayer->ModelPtr( ), "taunt03" ) );
					Draw->String( 5, 302, FONT_ESP, 0, { 255, 255, 255, 255 }, true, "sequence: %i", gLocalPlayer->LookupSequence( gLocalPlayer->ModelPtr( ), "taunt04" ) );
					Draw->String( 5, 314, FONT_ESP, 0, { 255, 255, 255, 255 }, true, "sequence: %i", gLocalPlayer->LookupSequence( gLocalPlayer->ModelPtr( ), "taunt05" ) );
					Draw->String( 5, 326, FONT_ESP, 0, { 255, 255, 255, 255 }, true, "sequence: %i", gLocalPlayer->LookupSequence( gLocalPlayer->ModelPtr( ), "taunt06" ) );
					Draw->String( 5, 338, FONT_ESP, 0, { 255, 255, 255, 255 }, true, "sequence: %i", gLocalPlayer->LookupSequence( gLocalPlayer->ModelPtr( ), "taunt07" ) );
					Draw->String( 5, 350, FONT_ESP, 0, { 255, 255, 255, 255 }, true, "sequence: %i", gLocalPlayer->LookupSequence( gLocalPlayer->ModelPtr( ), "taunt08" ) );
					Draw->String( 5, 362, FONT_ESP, 0, { 255, 255, 255, 255 }, true, "sequence: %i", gLocalPlayer->LookupSequence( gLocalPlayer->ModelPtr( ), "taunt09" ) );
					G->FakeTaunt->Printing = false;
				}*/

					Draw->String( 5, 300, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "ServerTick: %i/%i", gClientState->m_ClockDriftMgr.server_tick
								  , gClientState->m_ClockDriftMgr.server_tick + TIME_TO_TICKS( gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ) ) );

					Draw->String( 5, 312, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "ClientTick: %i/%i", gClientState->m_ClockDriftMgr.client_tick
								  , gClientState->m_ClockDriftMgr.client_tick + TIME_TO_TICKS( gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ) ) );

					Draw->String( 5, 324, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "TickBase: %i - %i", gLocalPlayer->TickBase( ), gClientState->m_ClockDriftMgr.server_tick + TIME_TO_TICKS( gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ) ) );
					Draw->String( 5, 336, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "TickCount: %i", gGlobals->tickcount );
					Draw->String( 5, 348, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "Choked: %i", gClientState->chokedcommands );
					Draw->String( 5, 360, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "TickBase - ClientTick: %i", gLocalPlayer->TickBase( ) - gClientState->m_ClockDriftMgr.client_tick );
					Draw->String( 5, 372, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "TickBase - ServerTick: %i", gLocalPlayer->TickBase( ) - gClientState->m_ClockDriftMgr.server_tick );
					Draw->String( 5, 384, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "TickBase - TickCount: %i", gLocalPlayer->TickBase( ) - gGlobals->tickcount );
					Draw->String( 5, 396, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "ClientTick - ServerTick: %i", gClientState->m_ClockDriftMgr.client_tick - gClientState->m_ClockDriftMgr.server_tick );
					Draw->String( 5, 408, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "FLOW_OUTGOING: %f - %i", gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ), ( int )( gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ) * 1000.f ) );
					Draw->String( 5, 420, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "FLOW_INCOMING: %f - %i", gClientState->m_NetChannel->GetLatency( FLOW_INCOMING ), ( int )( gClientState->m_NetChannel->GetLatency( FLOW_INCOMING ) * 1000.f ) );
					Draw->String( 5, 432, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "MAX_FLOWS: %f - %i", gClientState->m_NetChannel->GetLatency( MAX_FLOWS ), ( int )( gClientState->m_NetChannel->GetLatency( MAX_FLOWS ) * 1000.f ) );
					Draw->String( 5, 444, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "OutSequenceNr: %i", gClientState->m_NetChannel->OutSequenceNr );
					Draw->String( 5, 456, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "InSequenceNr: %i", gClientState->m_NetChannel->InSequenceNr );
					Draw->String( 5, 468, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "OutSequenceNrAck: %i", gClientState->m_NetChannel->OutSequenceNrAck );
					Draw->String( 5, 480, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "lastoutgoingcommand: %i", gClientState->lastoutgoingcommand );
					Draw->String( 5, 492, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "OutReliableState: %i", gClientState->m_NetChannel->OutReliableState );
					Draw->String( 5, 504, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "InReliableState: %i", gClientState->m_NetChannel->InReliableState );
					Draw->String( 5, 516, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "ChokedPackets: %i", gClientState->m_NetChannel->ChokedPackets );
					Draw->String( 5, 528, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "command_ack: %i", gClientState->command_ack );
					if ( gGameRules->GetProxy( ) )
					{
						const char* RoundState = "RoundState: UNKNOWN";
						switch ( gGameRules->GetProxy( )->RoundState( ) )
						{
							case GR_STATE_INIT: RoundState = "RoundState: GR_STATE_INIT"; break;
							case GR_STATE_PREGAME: RoundState = "RoundState: GR_STATE_PREGAME"; break;
							case GR_STATE_STARTGAME: RoundState = "RoundState: GR_STATE_STARTGAME"; break;
							case GR_STATE_PREROUND: RoundState = "RoundState: GR_STATE_PREROUND"; break;
							case GR_STATE_RND_RUNNING: RoundState = "RoundState: GR_STATE_RND_RUNNING"; break;
							case GR_STATE_TEAM_WIN: RoundState = "RoundState: GR_STATE_TEAM_WIN"; break;
							case GR_STATE_RESTART: RoundState = "RoundState: GR_STATE_RESTART"; break;
							case GR_STATE_STALEMATE: RoundState = "RoundState: GR_STATE_STALEMATE"; break;
							case GR_STATE_GAME_OVER: RoundState = "RoundState: GR_STATE_GAME_OVER"; break;
							case GR_STATE_BONUS: RoundState = "RoundState: GR_STATE_BONUS"; break;
							case GR_STATE_BETWEEN_RNDS: RoundState = "RoundState: GR_STATE_BETWEEN_RNDS"; break;
							default: break;
						}
						Draw->String( 5, 540, FONT_MENU, 0, { 255, 255, 255, 255 }, true, RoundState );
					}
				}
			}

			ESP->DrawEntities( );
			Visuals->Spectators( );
			//AutoVaccinator->HandleThreats();

			Visuals->PredictionLine( );
			Visuals->ProjectileLine2( );
			Visuals->HitMarker( );
			Visuals->ScopeVisuals( );

			Visuals->DrawInfo( );
			Visuals->Doubletap( );
			Visuals->Crithack( );
			Visuals->FakeLatency( );
			Visuals->DrawLockedTarget( );
			Visuals->DrawNoSpread( );
			Visuals->ProjectileCam( );
			Input->Update( );
			Menu->Run( ); //menu should always be on top of everything drawn
		}
		gSurface->FinishDrawing( );
	}
}

HOOK_VTABLE( OnScreenSizeChanged, gSurface, 111u, void, __fastcall, void* ECX, void* EDX, int oldwidth, int oldheight )
{
	OriginalFunction( ECX, EDX, oldwidth, oldheight );
	if ( G->DetachSystem->WantsDetach )
		return;

	Draw->InitFonts( );
	gEngineClient->GetScreenSize( TF2->ScreenWidth, TF2->ScreenHeight );
}

HOOK_POINTER( SurfaceWndProc, "vguimatsurface.dll", "55 8B EC 83 EC 14 80 3D ? ? ? ? ?", LRESULT, CALLBACK,
			  HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam )
{
	if ( Menu->IsOpen && !G->DetachSystem->WantsDetach && Input->InFocus( ) )
	{
		gInputSystem->ResetInputState( );
		return 1;
	}

	return OriginalFunction( hWnd, uMsg, wParam, lParam );
}

HOOK_VTABLE( LockCursor, gSurface, 62u, void, __fastcall, void* ECX, void* EDX )
{
	if ( Menu->IsOpen && !G->DetachSystem->WantsDetach )
	{
		return gSurface->UnlockCursor( );
	}

	OriginalFunction( ECX, EDX );
}

HOOK_VTABLE( FrameStageNotify, gClient, 35u, void, __fastcall, void* ECX, void* EDX, ClientFrameStage_t Stage )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, Stage );

	if ( Stage == ClientFrameStage_t::FRAME_START )
	{
		if ( Input->IsPressed( Config->Misc->AntiAim->CycleKey ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
		{
			AntiAim->IsFlipped = !AntiAim->IsFlipped;
		}

		if ( Config->Misc->AntiAim->ToggleKey && Input->IsPressed( Config->Misc->AntiAim->ToggleKey ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
		{
			AntiAim->IsToggled = !AntiAim->IsToggled;
		}

		if ( Config->Misc->FakeLag->FakeLagKey && Input->IsPressed( Config->Misc->FakeLag->FakeLagKey ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
		{
			Misc->IsFakeLagToggled = !Misc->IsFakeLagToggled;
		}

		if ( Config->Aimbot->Hitscan->FakeLatencyKey && Input->IsPressed( Config->Aimbot->Hitscan->FakeLatencyKey ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
		{
			Misc->IsFakeLatencyToggled = !Misc->IsFakeLatencyToggled;
		}
	}
	else if ( Stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_START )
	{
	}
	else if (Stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END)
	{
	}
	else if (Stage == ClientFrameStage_t::FRAME_NET_UPDATE_END)
	{
	}
	else if (Stage == ClientFrameStage_t::FRAME_RENDER_START)
	{
		AnimFix->Update( );
		if ( gLocalPlayer && gLocalPlayer->IsAlive( ) )
		{
			static float time = gEngineClient->Time( );
			static bool command = false;
			float elapsed = gEngineClient->Time( ) - time;

			if ( gLocalWeapon && G->Aimbot.SavedWeaponID == TF_WEAPON_LUNCHBOX && gEngineClient->GetNetChannelInfo( ) )
			{
				if ( gLocalPlayer->IsTaunting( ) )
				{
					if ( elapsed > 0.55f + gEngineClient->GetNetChannelInfo()->GetLatency(2) && !command )
					{
						gEngineClient->ServerCmd( "voicemenu 1 4", false );
						time = gEngineClient->Time( );
						command = true;
					}
				}
				else
				{
					command = false;
					time = gEngineClient->Time( );
				}
			}

			static auto push = gCvar->FindVar("tf_avoidteammates_pushaway");
			if (push)
				push->SetValue(!Config->Misc->AvoidTeamPush);

			G->Aimbot.PunchAngle = gLocalPlayer->PunchAngle();
			if ( Config->Visuals->RemoveViewPunch )
			{
				gLocalPlayer->PunchAngle( ) *= 0.f;
			}

			//gLocalPlayer->TickBase( ) += gLocalPlayer->TickBase( ) - ( gClientState->m_ClockDriftMgr.server_tick + 
			//	TIME_TO_TICKS( gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ) ) );

			if ( G->FakeTaunt->FakeTauntTime > gEngineClient->Time( ) )
			{
				gPrediction->SetLocalViewAngles( G->FakeTaunt->FakeTauntViewAngles );
			}

			if ( G->FakeTaunt->IsFakeTaunting && Config->Visuals->FakeTaunt->Enabled )
			{
				G->FakeTaunt->IsFakeTaunting = false;

				const auto& AnimState = gLocalPlayer->AnimState( );
				if ( AnimState )
				{
					if ( const auto& ModelPtr = gLocalPlayer->ModelPtr( ) )
					{
						if ( G->FakeTaunt->DefaultFakeTaunt )
						{
							AnimState->AddVCDSequenceToGestureSlot( GESTURE_SLOT_CUSTOM, gLocalPlayer->LookupSequence( ModelPtr, gLocalPlayer->SequenceLabel( ) ), 0.0f, true );

							G->FakeTaunt->FakeTauntTime = gLocalPlayer->SequenceDuration( gLocalPlayer->ModelPtr( ), gLocalPlayer->LookupSequence( ModelPtr, gLocalPlayer->SequenceLabel( ) ) ) + gEngineClient->Time( );
							G->FakeTaunt->FakeTauntViewAngles = gLocalPlayer->EyeAngles( );
						}
						else
						{
							const char* sequence = "";
							switch ( G->FakeTaunt->FakeTauntSlot )
							{
								case 1:
								{
									sequence = "taunt_laugh";
									break;
								}

								case 2:
								{
									sequence = "taunt_conga";
									break;
								}

								default: break;
							}

							if ( ( int )sequence > 0 )
							{
								AnimState->AddVCDSequenceToGestureSlot( GESTURE_SLOT_CUSTOM, gLocalPlayer->LookupSequence( ModelPtr, sequence ), 0.0f, true );

								G->FakeTaunt->FakeTauntTime = gLocalPlayer->SequenceDuration( gLocalPlayer->ModelPtr( ), gLocalPlayer->LookupSequence( ModelPtr, sequence ) ) + gEngineClient->Time( );
								G->FakeTaunt->FakeTauntViewAngles = gLocalPlayer->EyeAngles( );
							}
						}
					}
				}
			}
		}
		Visuals->Viewmodel( );
	}

	OriginalFunction( ECX, EDX, Stage );

	if ( Stage == ClientFrameStage_t::FRAME_NET_UPDATE_START )
	{
		EntityCache->Clear( );
	}
	else if ( Stage == ClientFrameStage_t::FRAME_NET_UPDATE_POSTDATAUPDATE_END )
	{
		Misc->RunTest( );
	}
	else if ( Stage == ClientFrameStage_t::FRAME_NET_UPDATE_END )
	{ // https://github.com/OneshotGH/supremacy/blob/master/pred.cpp

		EntityCache->Fill( );
		Misc->NetApply( );

		for ( const auto& Player : gPlayers )
		{
			if ( !Player || Player == gLocalPlayer )
				continue;

			LagComp->Record( Player );
			// Micro optimization part 4
			MovementSim->FillDirections( Player->GetIndex( ) );

			//could be breaking shit idk
			/*if ( Player->SimulationTime( ) < Player->OldSimulationTime( ) && !Player->IsTaunting( ) )
			{
				G->AnimationFix->WarpTimeStamps[ Player->GetIndex( ) ] = gEngineClient->Time( ) + TICKS_TO_TIME( 4 );
			}

			if ( G->AnimationFix->WarpTimeStamps[ Player->GetIndex( ) ] > gEngineClient->Time( ) )
			{
				G->AnimationFix->UpdateAnims = false;
				continue;
			}*/

			if ( auto diff = std::clamp( TIME_TO_TICKS( Player->SimulationTime( ) - Player->OldSimulationTime( ) ), 0, 22 ) )
			{
				float OldFrameTime = gGlobals->frametime;
				gGlobals->frametime = gPrediction->EnginePaused ? 0.0f : gGlobals->interval_per_tick;
				for ( int i = 0; i < diff; i++ )
				{
					G->AnimationFix->UpdateAnims = true;
					Player->UpdateClientSideAnimation( );
					G->AnimationFix->UpdateAnims = false;
				}
				gGlobals->frametime = OldFrameTime;
			}
		}
	}
	else if ( Stage == ClientFrameStage_t::FRAME_RENDER_START )
	{
	}
}

HOOK_POINTER( AnimStateUpdate, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 57 8B F9 8B 9F ? ? ? ? 89 5D E0 85 DB 0F 84", void, __fastcall,
			  CTFPlayerAnimState* ECX, void* EDX, float yaw, float pitch )
{
	if ( !ECX || !gLocalPlayer || G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, yaw, pitch );

	const auto& Player = ECX->GetBasePlayer( );

	if ( !Player )
		return OriginalFunction( ECX, EDX, yaw, pitch );

	/*if ( Player && G->AnimationFix->UpdateAnims )
	{
		if ( G->FakeTaunt->FakeTauntTime > gEngineClient->Time( ) && gLocalPlayer && Player == gLocalPlayer )
		{
			OriginalFunction( ECX, EDX, G->FakeTaunt->FakeTauntViewAngles.y, G->FakeTaunt->FakeTauntViewAngles.x );
		}
		else if ( G->FakeTaunt->FakeTauntTime < gEngineClient->Time( ) && gLocalPlayer && Player == gLocalPlayer && Config->Misc->AntiAim->Enabled )
		{
			float yaw = G->AnimationFix->DrawingFakeAngles ? G->AntiAim->FakeAngles.y : G->AntiAim->RealAngles.y;
			float pitch = G->AnimationFix->DrawingFakeAngles ? G->AntiAim->FakeAngles.x : G->AntiAim->RealAngles.x;
			OriginalFunction( ECX, EDX, yaw, pitch );
		}
		else
		{
			OriginalFunction( ECX, EDX, yaw, pitch );
		}
	}*/

	if ( gLocalPlayer->IsTaunting( ) && gLocalPlayer && ECX == gLocalPlayer->AnimState( ) )
	{
		return OriginalFunction( ECX, EDX, yaw, pitch );
	}

	if ( G->FakeTaunt->FakeTauntTime > gEngineClient->Time( ) && gLocalPlayer && ECX == gLocalPlayer->AnimState( ) )
	{
		return OriginalFunction( ECX, EDX, G->FakeTaunt->FakeTauntViewAngles.y, G->FakeTaunt->FakeTauntViewAngles.x );
	}

	if ( ECX == gLocalPlayer->AnimState( ) )
	{
		if ( G->AnimationFix->UpdateAnims )
		{
			G->AnimationFix->UpdateAnims = false;
			return OriginalFunction( ECX, EDX, yaw, pitch );
		}
		return;
	}

	return OriginalFunction( ECX, EDX, yaw, pitch );
}

HOOK_POINTER( UpdateClientSideAnimation, "client.dll", "55 8B EC 83 EC 0C 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 74 10 8B 17 8B CF 8B 92 ? ? ? ? FF D2", void, __fastcall,
			  void* ECX, void* EDX )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX );

	const auto& Player = reinterpret_cast< CBasePlayer* >( ECX );

	if ( !Player || ( Player == gLocalPlayer && ( gLocalPlayer->IsTaunting( ) || G->FakeTaunt->FakeTauntTime > gEngineClient->Time( ) ) ) )
		return OriginalFunction( ECX, EDX );

	//this shit drops ur fps to like 50-60
	/*if ( Player == gLocalPlayer && !gLocalPlayer->IsInKart( ) && gLocalWeapon )
		gLocalWeapon->UpdateAllViewmodelAddons( );*/

	if ( G->AnimationFix->UpdateAnims )
		OriginalFunction( ECX, EDX );
}

HOOK_POINTER( CL_Move, "engine.dll", "55 8B EC 83 EC ? 83 3D ? ? ? ? ? 0F 8C ? ? ? ? E8",
			  void, __cdecl, float AccumulatedExtraSamples, bool FinalTick )
{
	Misc->RemoveInputDelay(FinalTick); // lets run this
	Misc->CalcLatencyDifference( );

	if (!gEngineClient->IsInGame() || !gLocalPlayer || !gLocalPlayer->IsAlive() || G->DetachSystem->WantsDetach )
	{
		G->Doubletap.Charged = 0;
		G->Doubletap.Sent = MAX_COMMANDS;
		G->Doubletap.Recharge = false;
		return OriginalFunction(AccumulatedExtraSamples, FinalTick);
	}

	if (((GetAsyncKeyState(Config->Misc->DT->RechargeKey) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) 
		&& !G->Doubletap.Recharge && !G->Doubletap.QueueRecharge && !G->Doubletap.ToSend )
	{
		if (G->Doubletap.Charged < MAX_COMMANDS )
		{
			if ( Config->Misc->FakeLag->Enabled || Config->Misc->FakeDuck->Enabled )
			{
				G->Doubletap.QueueRecharge = true;
			}
			else
			{
				G->Doubletap.Recharge = true;
			}
		}
	}

	if ( G->Doubletap.QueueRecharge && !G->FakeLag->Choking )
	{
		G->Doubletap.Recharge = true;
		G->Doubletap.QueueRecharge = false;
	}
	else if (G->Doubletap.Recharge)
	{
		G->Doubletap.Charged++;
		G->Doubletap.Sent--;

		if (G->Doubletap.Charged >= MAX_COMMANDS )
			G->Doubletap.Recharge = false;

		// lets check again
		if (G->Doubletap.Recharge && !( gLocalPlayer->Class( ) == CLASS_DEMOMAN && 
			gLocalWeapon->Slot( ) == SLOT_SECONDARY ) )
			G->Doubletap.Wait = gLocalPlayer->Class() == CLASS_HEAVY ? 3 : 4;

		return;
	}

	OriginalFunction( AccumulatedExtraSamples, FinalTick ); // good game bro

	if (!((GetAsyncKeyState(Config->Misc->DT->RechargeKey) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) ))
	{
		Misc->Warp(AccumulatedExtraSamples, FinalTick, OriginalFunction);
		if (G->Doubletap.Warp)
			return;
	}

	if ( G->Doubletap.ToSendInternal > 0 )
		G->Doubletap.ToSendInternal--;

	if (G->Doubletap.Wait > 0)
	{
		G->Doubletap.Wait--;
		return;
	}

	while (G->Doubletap.ToSend > 0 && G->Doubletap.Charged > 0)
	{
		G->Doubletap.Charged--;
		G->Doubletap.ToSend--;
		G->Doubletap.ToSendInternal--;
		G->Doubletap.Sent++;
		OriginalFunction(AccumulatedExtraSamples, FinalTick );
	}
}

HOOK_POINTER( CL_SendMove, "engine.dll", "55 8B EC 81 EC ? ? ? ? A1 ? ? ? ? 8D",
			  void, __fastcall, void* ECX, void* EDX )
{
	if (!gClientState || !gClientState->m_NetChannel || G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX );

	NoSpread->CL_SendMoveHandler( );

	byte data[4000];
	const int Chocked = ((CNetChannel*)gClientState->m_NetChannel)->ChokedPackets;
	const int NextCommandNumber = gClientState->lastoutgoingcommand + Chocked + 1;

	CLC_Move Message;
	Message.m_DataOut.StartWriting(data, sizeof(data));

	Message.m_nNewCommands = 1 + Chocked;
	Message.m_nNewCommands = std::clamp(Message.m_nNewCommands, 0, MAX_NEW_COMMANDS);
	const int ExtraCommands = (Chocked + 1) - Message.m_nNewCommands;
	const int BackupCommands = std::max(2, ExtraCommands);
	Message.m_nBackupCommands = std::clamp(BackupCommands, 0, MAX_BACKUP_COMMANDS);

	const int numcmds = Message.m_nNewCommands + Message.m_nBackupCommands;

	int from = -1;
	bool bOK = true;
	for (int to = NextCommandNumber - numcmds + 1; to <= NextCommandNumber; to++) {
		const bool isnewcmd = to >= NextCommandNumber - Message.m_nNewCommands + 1;
		bOK = bOK && gClient->WriteUsercmdDeltaToBuffer(&Message.m_DataOut, from, to, isnewcmd);
		from = to;
	}

	if (bOK) {
		if (ExtraCommands)
			((CNetChannel*)gClientState->m_NetChannel)->ChokedPackets -= ExtraCommands;

		Memory->GetVFunc<bool(__thiscall*)(void*, INetMessage*, bool, bool)>(gClientState->m_NetChannel, 37)(gClientState->m_NetChannel, &Message, false, false);
	}

	NoSpread->CL_SendMovePostHandler( );
}

HOOK_POINTER( CL_ReadPackets, "engine.dll", "55 8B EC 83 EC 34 8B 0D ? ? ? ?",
			  void, __cdecl, bool FinalTick )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( FinalTick );

	if ( Misc->ShouldReadPackets( ) )
		OriginalFunction( FinalTick );
}

HOOK_VTABLE( RunCommand, gPrediction, 17u, void, __fastcall, void* ECX, void* EDX, CBasePlayer* Player, CUserCmd* Cmd, IMoveHelper* MoveHelper )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, Player, Cmd, MoveHelper );

	OriginalFunction( ECX, EDX, Player, Cmd, MoveHelper );

	Misc->NetStore( );
	if ( !gMoveHelper )
	{
		gMoveHelper = MoveHelper;
	}
}

HOOK_POINTER( Interpolate, "client.dll", "55 8B EC 83 EC 2C 56 8B F1 83 BE ? ? ? ? ? 0F 85 ? ? ? ? F3 0F 10 86 ? ? ? ?",
			  bool, __fastcall, CBaseEntity* ECX, void* EDX, float CurTime )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, CurTime );

	return true;
}

HOOK_POINTER( InterpolateServerEntities, "client.dll", "55 8B EC 83 EC ? 8B 0D ? ? ? ? 53 33 DB 89 5D ? 89 5D ? 8B 41 ? 89 5D ? 89 5D ? 85 C0 74 ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 53 53 53 53 8D 4D ? 51 50 8B 40 ? FF D0 8B 45 ? 83 C4 ? 8B 0D ? ? ? ? 89 45 ? 8B 45 ? 89 45 ? 8B 59 ? 8B 0D ? ? ? ? 89 5D ? FF 15 ? ? ? ? 88 45 ? 84 C0 74 ? 8B 0D ? ? ? ? 6A ? 6A ? 68 ? ? ? ? 6A ? 68 ? ? ? ? FF 15 ? ? ? ? A1 ? ? ? ? 8B 0D",
			  void, __cdecl )
{
	if (G->Doubletap.Recharge && !G->DetachSystem->WantsDetach )
		return;

	return OriginalFunction( );
}

HOOK_POINTER( CheckForSequenceChange, "client.dll", "55 8B EC 53 8B 5D 08 57 8B F9 85 DB 0F 84 ? ? ? ? 83 7F 0C 00 75 05 E8 ? ? ? ? 6B 4F 0C 2C 0F 57 C0 56 8B 37 83 C6 D4 03 F1 F3 0F 10 4E ? 0F 2E C8 9F F6 C4 44 7B 62 8B 45 0C",
			  void, __fastcall, void* ECX, void* EDX, CStudioHdr* hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate )
{
	return OriginalFunction( ECX, EDX, hdr, nCurSequence, bForceNewSequence, false );
}

// DO NOT HOOK THIS
// VERY UNSTABLE
HOOK_POINTER( AddVar, "client.dll", "55 8B EC 83 EC 0C 53 8B D9 56 33 F6 57 8B 4B 20 8B 7D 0C", void, __fastcall,
			  void* ECX, void* EDX, void* data, IInterpolatedVar* watcher, int type, bool bSetup )
{
	if ( !watcher )
		return OriginalFunction(ECX, EDX, data, watcher, type, bSetup);

	if ( !strstr( watcher->GetDebugName( ), "m_iv_vecOrigin" ) )
	{
		return;
	}
	return OriginalFunction( ECX, EDX, data, watcher, type, bSetup );
}

HOOK_POINTER( BaseInterpolatePart1, "client.dll", "55 8B EC 53 8B 5D ? 56 8B F1 C7 03 ? ? ? ? F6 46 ? ? 74 ? 80 BE ? ? ? ? ? 75 ? 8B 96 ? ? ? ? 83 FA ? 74", int, __fastcall,
			  void* ECX, void* EDX, float& currentTime, Vector& oldOrigin, Vector& oldAngles, Vector& oldVel, int& bNoMoreChanges )
{
	if ( !ECX || !gLocalPlayer || G->DetachSystem->WantsDetach )
	{
		return OriginalFunction( ECX, EDX, currentTime, oldOrigin, oldAngles, oldVel, bNoMoreChanges );
	}

	const auto& entity = reinterpret_cast< CBaseEntity* >( ECX );
	if ( !entity || entity->IsDormant( ) )
	{
		return OriginalFunction( ECX, EDX, currentTime, oldOrigin, oldAngles, oldVel, bNoMoreChanges );
	}

	if ( entity == gLocalPlayer )
	{
		return OriginalFunction( ECX, EDX, currentTime, oldOrigin, oldAngles, oldVel, bNoMoreChanges );
	}
	else
	{
		bNoMoreChanges = 1;
		return 0;
	}
}

HOOK_POINTER(AddToCritBucket, "client.dll", "55 8B EC A1 ? ? ? ? F3 0F 10 81 ? ? ? ? F3 0F 10 48",
	void, __fastcall, void* ECX, void* EDX, float Damage)
{
	if (CritHack->Data->Forcing && !G->DetachSystem->WantsDetach )
		return;

	OriginalFunction(ECX, EDX, Damage);
}

HOOK_POINTER(CalcIsAttackCritical, "client.dll", "53 57 6A ? 68 ? ? ? ? 68 ? ? ? ? 6A ? 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B D8 83 C4 ? 85 DB 0F 84",
	void, __fastcall, void* ECX, void* EDX)
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX );

	auto Weapon = reinterpret_cast<CBaseCombatWeapon*>(ECX);
	if (!Weapon || Weapon != gLocalWeapon)
		return OriginalFunction(ECX, EDX);

	if (!CritHack->IsAttackCriticalHandler())
		return;

	const auto PreviousWeaponMode = Weapon->WeaponMode();
	Weapon->WeaponMode() = 0;
	OriginalFunction(ECX, EDX);
	Weapon->WeaponMode() = PreviousWeaponMode;
}

HOOK_POINTER(CanFireRandomCriticalShot, "client.dll", "55 8B EC F3 0F 10 4D ? F3 0F 58 0D",
	bool, __fastcall, void* ECX, void* EDX, float Chance)
{
	if (gLocalWeapon != nullptr && ECX == gLocalWeapon && !G->DetachSystem->WantsDetach )
		CritHack->HandleCanFireRandomCriticalShot( Chance );

	return OriginalFunction( ECX, EDX, Chance );
}

HOOK_POINTER(IsAllowedToWithdrawFromBucket, "client.dll", "55 8B EC 56 8B F1 0F B7 86 ? ? ? ? FF 86 ? ? ? ? 50 E8 ? ? ? ? 83 C4 04 80 B8 ? ? ? ? ? 74 0A F3 0F 10 15",
	bool, __fastcall, void* ECX, void* EDX, float Damage)
{
	CritHack->Data->Damage->CorrectDamage = Damage;
	return OriginalFunction( ECX, EDX, Damage );
}

HOOK_VTABLE(GetUserCmd, gInput,	8,
	CUserCmd*, __stdcall, int Command)
{
	if (auto Commands = *(CUserCmd**)((uintptr_t)gInput + 0xFC))
		if (CUserCmd* usercmd = &Commands[Command % MULTIPLAYER_BACKUP])
			return usercmd;

	return nullptr;
}

HOOK_POINTER( GetTime, "engine.dll", "55 8B EC 51 80 B9 ? ? ? ? ? F3 0F 2A 81 ? ? ? ? F3 0F 59 05 ? ? ? ?", float, __fastcall,
			  void* ECX, void* EDX )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX );

	static DWORD ReturnAddress = Memory->FindSignature<DWORD>( "engine.dll", "D9 43 ? DF F1" );

	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == ReturnAddress )
		return FLT_MAX;

	return OriginalFunction( ECX, EDX );
}

HOOK_VTABLE( ClientCmd, gEngineClient, 7u, void, __fastcall, void* ECX, void* EDX, const char* cmd )
{
	if ( !Config->Visuals->FakeTaunt->Enabled || G->DetachSystem->WantsDetach )
	{
		return OriginalFunction( ECX, EDX, cmd );
	}

	if ( strstr( cmd, "taunt " ) )
	{
		if ( !strcmp( cmd, "taunt 0" ) )
		{
			if ( gLocalWeapon && gLocalWeapon->IsTauntable( ) && G->FakeTaunt->FakeTauntTime < gEngineClient->Time( ) )
			{
				G->FakeTaunt->IsFakeTaunting = true;
				G->FakeTaunt->DefaultFakeTaunt = true;
			}
			return;
		}
		else
		{
			if ( G->FakeTaunt->FakeTauntTime < gEngineClient->Time( ) )
			{
				char count = cmd[ 6 ];
				int slot = count - 48;
				G->FakeTaunt->IsFakeTaunting = true;
				G->FakeTaunt->DefaultFakeTaunt = false;
				G->FakeTaunt->FakeTauntSlot = slot;
			}
			return;
		}
	}
	else if ( strstr( cmd, "taunt" ) )
	{
		if ( gLocalWeapon && gLocalWeapon->IsTauntable( ) && G->FakeTaunt->FakeTauntTime < gEngineClient->Time( ) )
		{
			G->FakeTaunt->IsFakeTaunting = true;
			G->FakeTaunt->DefaultFakeTaunt = true;
		}
		return;
	}

	OriginalFunction( ECX, EDX, cmd );
}

HOOK_VTABLE( ServerCmd, gEngineClient, 6u, void, __fastcall, void* ECX, void* EDX, const char* cmd, bool reliable )
{
	OriginalFunction( ECX, EDX, cmd, reliable );

	if ( G->DetachSystem->WantsDetach )
		return;

	if (G->Authentication->IsDebugAllowed)
		gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "\nServer executed cmd: %s\n\n", cmd );
}

__declspec( dllexport ) void __cdecl RandomSeed::Function( int Seedenz )
{
	if ( CritHack->Data->Forcing || G->DetachSystem->WantsDetach )
		return OriginalFunction( Seedenz );

	/*static auto ret1 = Memory->FindSignature<DWORD>("client.dll", "83 C4 ? 0F 57 ? 80 7D FF");
	static auto ret2 = Memory->FindSignature<DWORD>("client.dll", "83 C4 ? 68 ? ? ? ? 6A ? FF 15 ? ? ? ? F3 0F ? ? ? 83 C4 ? 0F 28");
	static auto ret3 = Memory->FindSignature<DWORD>("client.dll", "83 C4 ? 68 ? ? ? ? 6A ? FF 15 ? ? ? ? F3 0F ? ? ? 83 C4 ? F3 0F");*/

	// we need to check all of those.
	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == G->RandomSeedRet1Address ||
		 reinterpret_cast< DWORD >( _ReturnAddress( ) ) == G->RandomSeedRet2Address ||
		 reinterpret_cast< DWORD >( _ReturnAddress( ) ) == G->RandomSeedRet3Address )
	{
		if ( gLocalWeapon )
			gLocalWeapon->RandomSeed( ) = Seedenz;
	}

	OriginalFunction( Seedenz );
}

void RandomSeed::Init()
{
	auto Module = GetModuleHandleW(L"vstdlib.dll");
	Target = reinterpret_cast<PVOID>(GetProcAddress(Module, "RandomSeed"));
	MH_CreateHook( Target, RandomSeed::Function, reinterpret_cast<void**>(&OriginalFunction));
}

void RandomSeed::Uninit( )
{
	MH_RemoveHook( Target );
	std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );
}

HOOK_VTABLE( LevelInit, gClient, 6,
			 void, __fastcall, void* ecx, void* edx )
{
	OriginalFunction( ecx, edx );
	if ( G->DetachSystem->WantsDetach )
		return;

	gLocalPlayer = nullptr;
	gLocalWeapon = nullptr;
	CritHack->Reset();
	gEntities.clear();
	gPlayers.clear();
	Visuals->UpdateWorldModulation( );
	G->Sequences.clear( );
	G->HiddenNames.clear( );
	//G->HitMarkers.clear( );
	NoSpread->Reset( );
	gWD->WD_EndData( );
}

HOOK_VTABLE(ShutDown, gClient, 7,
	void, __fastcall, void* ecx, void* edx)
{
	OriginalFunction(ecx, edx);
	if ( G->DetachSystem->WantsDetach )
		return;

	gLocalPlayer = nullptr;
	gLocalWeapon = nullptr;
	CritHack->Reset();
	gEntities.clear();
	gPlayers.clear();
	G->Sequences.clear( );
	G->HiddenNames.clear( );
	//G->HitMarkers.clear( );
	NoSpread->Reset( );
	gWD->WD_EndData( );
}

HOOK_VTABLE(OverrideView, gClientMode, 16, void, __fastcall, void* ecx, void* edx, CViewSetup* ViewSetup)
{
	if (!gLocalPlayer || !ecx || !ViewSetup || G->DetachSystem->WantsDetach )
		return OriginalFunction(ecx, edx, ViewSetup);

	Visuals->ThirdPerson( ViewSetup );

	if ( !Config->Visuals->RemoveScope && gLocalPlayer->IsZoomed( ) )
		return OriginalFunction( ecx, edx, ViewSetup );

	if ( gEngineClient->IsTakingScreenshot( ) )
		return OriginalFunction( ecx, edx, ViewSetup );

	G->ConVars->AspectRatio->SetValue( Config->Visuals->AspectRatio );
	G->ConVars->HdrLevel->SetValue( 0 );
	G->ConVars->BloomScale->SetValue( Config->Visuals->BloomScale );
	G->ConVars->Fullbright->SetValue( Config->Visuals->WorldModulation->Mode == 1 );

	if ( gLocalPlayer && gLocalPlayer->IsAlive( ) && Config->Misc->FakeDuck->Enabled && G->FakeLag->FakeDucking && G->Doubletap.Charged < 1 && Input->InFocus( ) && ( (GetAsyncKeyState( Config->Misc->FakeDuck->FakeDuckKey ) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) )
	{
		int Height = 0;
		switch ( gLocalPlayer->Class( ) )
		{
			case CLASS_SCOUT:
			{
				Height = 65;
				break;
			}

			case CLASS_SOLDIER:
			case CLASS_PYRO:
			case CLASS_DEMOMAN:
			case CLASS_ENGINEER:
			{
				Height = 68;
				break;
			}

			case CLASS_HEAVY:
			case CLASS_MEDIC:
			case CLASS_SNIPER:
			case CLASS_SPY:
			{
				Height = 75;
				break;
			}
			default: break;
		}

		ViewSetup->origin.z = gLocalPlayer->Origin( ).z + Height;
	}

	gLocalPlayer->FOV() = gLocalPlayer->IsZoomed( ) ? Config->Visuals->ScopedFOV : Config->Visuals->FOV;
	ViewSetup->fov = gLocalPlayer->IsZoomed( ) ? Config->Visuals->ScopedFOV : Config->Visuals->FOV;

	OriginalFunction(ecx, edx, ViewSetup);
}

HOOK_POINTER(ViewEffectsFade, "client.dll", "55 8B EC 51 56 57 6A 14", void, __fastcall, void* ecx, void* edx, void* idfk)
{
	if (Config->Visuals->RemoveFade && !G->DetachSystem->WantsDetach )
		return;

	OriginalFunction(ecx, edx, idfk);
}

HOOK_POINTER(ViewEffectsShake, "client.dll", "55 8B EC 56 8B 75 08 57 8B F9 8B 06", void, __fastcall, void* ecx, void* edx, void* idfk)
{
	if (Config->Visuals->RemoveShake && !G->DetachSystem->WantsDetach )
		return;

	OriginalFunction(ecx, edx, idfk);
}

HOOK_VTABLE(FireEventClientSide, gEventManager, 8, bool, __fastcall, void* ECX, void* EDX, IGameEvent* Event )
{
	if (!Event || G->DetachSystem->WantsDetach )
		return OriginalFunction(ECX, EDX, Event);

	constexpr auto party_chat = FNV1A::HashConst("party_chat");
	constexpr auto player_hurt = FNV1A::HashConst("player_hurt");
	const auto EventName = FNV1A::Hash(Event->GetName());
	if (EventName == party_chat)
	{
		const char* Message = Event->GetString("text");
		if (strlen(Message) > 6) // anti rijin party networking crash
		{
			if (Message[0] == '7' && Message[2] == '1' && Message[3] == '7' && Message[4] == 'c')
				return false;
		}

		static float LastTime = 0.f;
		if ( LastTime < gEngineClient->Time( ) )
		{
			char buffer[ 512 ];
			// todo add a checkbox for all of this
			if ( !strcmp( Message, "/help" ) )
			{
				sprintf_s( buffer, "tf_party_chat \"[Ateris]: 3 Commands available:\"" );
				gEngineClient->ClientCmd_Unrestricted( buffer );

				sprintf_s( buffer, "tf_party_chat \" - /help: Shows this message.\"" );
				gEngineClient->ClientCmd_Unrestricted( buffer );

				sprintf_s( buffer, "tf_party_chat \" - /ns: Sends mantissa step of the server\"" );
				gEngineClient->ClientCmd_Unrestricted( buffer );

				sprintf_s( buffer, "tf_party_chat \" - /q: Starts a queue for Casual\"" );
				gEngineClient->ClientCmd_Unrestricted( buffer );

				LastTime = gEngineClient->Time( ) + .25f;
			}

			if ( !strcmp( Message, "/ns" ) )
			{
				if ( gEngineClient->IsInGame( ) && gLocalWeapon && gLocalWeapon->IsSpreadWeapon( ) )
				{
					sprintf_s( buffer, ( NoSpread->MantissaStep < 1.f )
						? "tf_party_chat \"[Ateris]: Current Mantissa: %f.\""
						: "tf_party_chat \"[Ateris]: Current Mantissa: %.0f.\"", NoSpread->MantissaStep );
					gEngineClient->ClientCmd_Unrestricted( buffer );
				}
				else
				{
					sprintf_s( buffer, "tf_party_chat \"[Ateris]: Couldn't find Mantissa.\"" );
					gEngineClient->ClientCmd_Unrestricted( buffer );
				}

				LastTime = gEngineClient->Time( ) + .25f;
			}

			if ( !strcmp( Message, "/q" ) && gPartyClient )
			{
				if ( *reinterpret_cast< bool* >( ( DWORD ) gPartyClient + 40 ) )
				{
					if ( gPartyClient->QueueForCasual( ) )
					{
						sprintf_s( buffer, "tf_party_chat \"[Ateris]: Starting queue for Casual.\"" );
						gEngineClient->ClientCmd_Unrestricted( buffer );
					}
				}

				LastTime = gEngineClient->Time( ) + .25f;
			}

			if ( !strcmp( Message, "/p" ) )
			{
				sprintf_s( buffer, "tf_party_chat \"[Ateris]: Teki is a fucking retard.\"" );
				gEngineClient->ClientCmd_Unrestricted( buffer );

				LastTime = gEngineClient->Time( ) + .25f;
			}
		}		
	}
	else if ( EventName == player_hurt )
	{
		// call PRE event apply
		CritHack->OnEvent( Event );
	}

	return OriginalFunction(ECX, EDX, Event);
}

HOOK_VTABLE( DoPostScreenSpaceEffects, gClientMode, 39u, bool, __fastcall, void* ECX, void* EDX, CViewSetup* View )
{
	if ( !ECX || !View || gEngineClient->IsTakingScreenshot( ) || G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, View );

	Models->Render( );
	Outlines->Render( );
	Visuals->ProjectileLine( );
	return OriginalFunction( ECX, EDX, View );
}

HOOK_VTABLE( DrawModelExecute, gModelRender, 19u, void, __fastcall, void* ECX, void* EDX,
			 const DrawModelState_t& State, const  ModelRenderInfo_t& Info, Matrix3x4* Matrix )
{
	if ( !Matrix || !Info.pModel || gEngineClient->IsTakingScreenshot( ) || G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, State, Info, Matrix );

	const auto& Entity = gEntityList->GetClientEntity( Info.entity_index )->As<CBaseEntity>( );

	if ( ( Models->IsEntityRendered( Entity ) || Outlines->IsEntityRendered( Entity ) ) && !Outlines->IsDrawingGlow && !Models->IsRenderingOverlay( ) )
		return;

	if ( Config->Visuals->StreamerMode && Entity && Entity->GetClientClass( )->ClassID == EClassID::CTFWearable )
	{
		constexpr auto ApparitionsAspect = FNV1A::HashConst( "models/workshop/player/items/pyro/ghost_aspect/ghost_aspect.mdl" );
		if ( Config->Visuals->HideLocal && Entity->GetOwnerEntity( )->As<CBasePlayer>( )->IsLocalPlayer( ) &&
			 FNV1A::Hash( gModelInfo->GetModelName( Info.pModel ) ) != ApparitionsAspect )
		{
			return;
		}

		if ( Config->Visuals->HideFriends && Entity->GetOwnerEntity( )->As<CBasePlayer>( )->IsSteamFriend( ) )
		{
			return;
		}
	}

	//changes players alpha for some reason aswell
	/*if ( Config->Visuals->WorldModulation->Enabled && Config->Visuals->WorldModulation->Mode != 1 && Entity && Entity->GetClientClass( )->ClassID == EClassID::CDynamicProp )
	{
		switch ( Config->Visuals->WorldModulation->Mode )
		{
			case 0:
			{
				gRenderView->SetColorModulation( Color( Config->Visuals->WorldModulation->Amount * 255.f,
												   Config->Visuals->WorldModulation->Amount * 255.f,
												   Config->Visuals->WorldModulation->Amount * 255.f ) );
				gRenderView->SetBlend( Config->Visuals->WorldModulation->Alpha );
				break;
			}

			case 2:
			{
				gRenderView->SetColorModulation( Config->Colors->WorldColor );
				gRenderView->SetBlend( Config->Visuals->WorldModulation->Alpha );
				break;
			}

			default:
			{
				break;
			}
		}
	}*/

	if ( Entity && !Entity->IsDormant( ) && Entity->GetClientClass( )->ClassID == EClassID::CTFViewModel )
	{
		if ( !Config->Visuals->HandsModels )
		{
			OriginalFunction( ECX, EDX, State, Info, Matrix );
		}
	}
	else if ( !Entity && Info.pModel )
	{
		std::string_view szModelName( gModelInfo->GetModelName( Info.pModel ) );

		if (szModelName.find_first_of( ( "arrow" ) ) == std::string_view::npos
			 && szModelName.find_first_of( ( "w_syringe" ) ) == std::string_view::npos
			 && szModelName.find_first_of( ( "nail" ) ) == std::string_view::npos
			 && szModelName.find_first_of( ( "shell" ) ) == std::string_view::npos
			 && szModelName.find_first_of( ( "parachute" ) ) == std::string_view::npos
			 && szModelName.find_first_of( ( "buffbanner" ) ) == std::string_view::npos
			 && szModelName.find_first_of( ( "shogun_warbanner" ) ) == std::string_view::npos
			 && szModelName.find_first_of( ( "targe" ) ) == std::string_view::npos //same as world model, can't filter
			 && szModelName.find_first_of( ( "shield" ) ) == std::string_view::npos //same as world model, can't filter
			 && szModelName.find_first_of( ( "repair_claw" ) ) == std::string_view::npos )
		{
			if ( !Config->Visuals->HandsModels )
			{
				OriginalFunction( ECX, EDX, State, Info, Matrix );
			}
		}
		else
		{
			OriginalFunction( ECX, EDX, State, Info, Matrix );
		}
	}
	else
	{
		OriginalFunction( ECX, EDX, State, Info, Matrix );
	}

	Visuals->DrawViewmodel( ECX, EDX, State, Info, Matrix, OriginalFunction );
	Visuals->DrawBacktrack( ECX, EDX, State, Info, OriginalFunction );
	AntiAim->DrawFakeModel( ECX, EDX, State, Info, OriginalFunction );
	AntiAim->DrawLagModel( ECX, EDX, State, Info, OriginalFunction );
}

HOOK_VTABLE( ForcedMaterialOverride, gModelRender, 1u, void, __fastcall, void* ECX, void* EDX,
			 IMaterial* Material, int OverrideType )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, Material, OverrideType );

	if ( ( Models->IsRendering( ) && !Models->IsUsedMaterial( Material ) ) || ( Outlines->IsRendering( ) && !Outlines->IsUsedMaterial( Material ) ) )
		return;

	OriginalFunction( ECX, EDX, Material, OverrideType );
}

HOOK_VTABLE( PaintTraverse, gPanel, 41u, void, __fastcall, void* ECX, void* EDX,
			 unsigned int VGuiPanel, bool ForceRepaint, bool AllowForce )
{
	const auto PanelName = gPanel->GetName( VGuiPanel );

	constexpr auto HudScope = FNV1A::HashConst( "HudScope" );
	constexpr auto SteamFriendsList = FNV1A::HashConst( "SteamFriendsList" );
	constexpr auto RankPanel = FNV1A::HashConst( "RankPanel" );
	constexpr auto avatar = FNV1A::HashConst( "avatar" );

	if ( Config->Visuals->RemoveScope && FNV1A::Hash( PanelName ) == HudScope && !G->DetachSystem->WantsDetach )
	{
		return;
	}

	//removing local info in main menu
	if ( Config->Visuals->StreamerMode )
	{
		if ( FNV1A::Hash( PanelName ) == SteamFriendsList ||
			 FNV1A::Hash( PanelName ) == RankPanel ||
			 FNV1A::Hash( PanelName ) == avatar )
		{
			return;
		}
	}

	//gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "%s\n", gPanel->GetName( VGuiPanel ) );
	OriginalFunction( ECX, EDX, VGuiPanel, ForceRepaint, AllowForce );
}

//removing local and friend's avatars in the scoreboard
HOOK_POINTER( UpdatePlayerAvatar, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 8B 5D ? 56 8B F1 85 DB 0F 84 ? ? ? ? 8B 06",
			  void, __fastcall, void* ECX, void* EDX, int index, KeyValues* kv )
{
	if ( !Config->Visuals->StreamerMode )
	{
		return OriginalFunction( ECX, EDX, index, kv );
	}

	if ( ( Config->Visuals->HideLocal && index == gEngineClient->GetLocalPlayer( ) ) ||
		 ( Config->Visuals->HideFriends && gEntityList->GetClientEntity( index )->As<CBasePlayer>( )->IsSteamFriend( ) ) )
		return;

	OriginalFunction( ECX, EDX, index, kv );
}

HOOK_POINTER( InCond, "client.dll", "55 8B EC 83 EC 08 56 57 8B 7D 08 8B F1 83 FF 20", bool, __fastcall,
			  void* ECX, void* EDX, ETFCond Cond )
{
	if ( !ECX || G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, Cond );

	const DWORD ReturnAddress = reinterpret_cast< DWORD >( _ReturnAddress( ) );
	
	auto GetOuter = [ &ECX ] ( ) -> CBaseEntity*
	{
		static const auto Shared = NetVars->GetNetVar( ( "CTFPlayer" ), ( "m_Shared" ) );
		static const auto BombHeadStage = NetVars->GetNetVar( ( "CTFPlayer" ), ( "m_nHalloweenBombHeadStage" ) );
		static const auto Off = ( BombHeadStage - Shared ) + 0x4;
		return *reinterpret_cast< CBaseEntity** >( reinterpret_cast< DWORD >( ECX ) + Off );
	};

	if ( Cond == TF_COND_ZOOMED && GetOuter( ) == gLocalPlayer )
	{
		if ( ReturnAddress == G->PlayerDrawAddress || ReturnAddress == G->WearableDrawAddress || ( Config->Visuals->RemoveScope && ReturnAddress == G->HudScopeDrawAddress ) )
		{
			return false;
		}
	}

	if ( GetOuter( ) != gLocalPlayer )
		if ( Cond == TF_COND_DISGUISING || Cond == TF_COND_DISGUISED )
			return false;

	return OriginalFunction( ECX, EDX, Cond );
}

HOOK_VTABLE( RenderView, gViewRender, 6u, void, __fastcall, void* ECX, void* EDX,
			 const CViewSetup& View, int ClearFlags, int WhatToDraw )
{
	OriginalFunction( ECX, EDX, View, ClearFlags, WhatToDraw );
	if ( G->DetachSystem->WantsDetach )
		return;

	CViewSetup projectile_camera;
	bool bView = gClient->GetPlayerView( projectile_camera );
	
	if ( gLocalPlayer && bView && gLocalPlayer->IsAlive( ) && gLocalWeapon && TF2->IsProjectileWeapon( gLocalWeapon ) && Config->Visuals->ProjectileLine && Config->Visuals->ProjectileCamera && !G->ProjectileCamera->ProjectileCameraPos.IsZero( ) ) {
		Visuals->ProjectileCamX = std::clamp( Visuals->ProjectileCamX, 2, TF2->ScreenWidth - Config->Visuals->ProjectileCameraWidth - 2 );
		Visuals->ProjectileCamY = std::clamp( Visuals->ProjectileCamY, 22, TF2->ScreenHeight - Config->Visuals->ProjectileCameraHeight - 2 );

		projectile_camera.x = Visuals->ProjectileCamX;
		projectile_camera.y = Visuals->ProjectileCamY;
		projectile_camera.width = projectile_camera.UnscaledWidth = Config->Visuals->ProjectileCameraWidth;
		projectile_camera.height = projectile_camera.UnscaledHeight = Config->Visuals->ProjectileCameraHeight;
		projectile_camera.AspectRatio = float( projectile_camera.width ) / float( projectile_camera.height );
		projectile_camera.fov = Config->Visuals->ProjectileCameraFOV;

		projectile_camera.origin = G->ProjectileCamera->ProjectileCameraPos;
		Vector forward{}, right{}, up{};
		gMath->AngleVectors( gEngineClient->GetViewAngles( ), &forward, &right, &up );
		float UpwardsOffset = gMath->ReMapValClamped( gLocalPlayer->EyePosition( ).DistTo( G->ProjectileCamera->ProjectileCameraPos ), 0.f, 3000.f, 0.5f, 1.5f );
		float ForwardsOffset = gMath->ReMapValClamped( gLocalPlayer->EyePosition( ).DistTo( G->ProjectileCamera->ProjectileCameraPos ), 0.f, 3000.f, 1.5f, 0.5f );
		float ZDifference = gLocalPlayer->EyePosition( ).z - G->ProjectileCamera->ProjectileCameraPos.z;
		float ZOffset = gMath->ReMapValClamped( ZDifference, -100.f, 500.f, 0.1f, 3.0f );
		Vector offset = ( forward * 50 * ForwardsOffset * ZOffset ) - ( right ) -( up * 120 * UpwardsOffset );
		Vector delta = projectile_camera.origin - offset;
		projectile_camera.origin -= offset;

		Vector angle = gMath->CalcAngle( projectile_camera.origin, G->ProjectileCamera->ProjectileCameraPos );
		projectile_camera.angles = { angle.x, angle.y, 0.f };

		gRenderView->SetBlend( 1.0f );
		gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
		gModelRender->ForcedMaterialOverride( nullptr );

		G->ProjectileCamera->IsDrawingProjectileCamera = true;
		OriginalFunction( ECX, EDX, projectile_camera, VIEW_CLEAR_COLOR | VIEW_CLEAR_DEPTH, 0 );
		G->ProjectileCamera->IsDrawingProjectileCamera = false;
	}
}

HOOK_VTABLE( SceneEnd, gRenderView, 9u, void, __fastcall, void* ECX, void* EDX )
{
	if ( !ECX || gEngineClient->IsTakingScreenshot( ) || G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX );

	Models->Render( );
	Outlines->Render( );

	return OriginalFunction( ECX, EDX );
}

HOOK_POINTER( RenderSpriteCard, "client.dll", "55 8B EC 83 EC 28 56 8B 75 10 57 8B 7D 14 8B C7 99 83 E7 03 83 E2 03",
			  void, __fastcall, void* ECX, void* EDX, void* meshbuilder, void* ctx, SpriteRenderInfo_t& info, int particle, void* sortlist, Vector* camera )
{
	if ( G->DetachSystem->WantsDetach )
	{
		return OriginalFunction( ECX, EDX, meshbuilder, ctx, info, particle, sortlist, camera );
	}

	const auto& Particles = info.m_pParticles;
	if ( !Particles )
	{
		return OriginalFunction( ECX, EDX, meshbuilder, ctx, info, particle, sortlist, camera );
	}

	const auto& Def = Particles->m_pDef;
	if ( !Def )
	{
		return OriginalFunction( ECX, EDX, meshbuilder, ctx, info, particle, sortlist, camera );
	}

	constexpr auto BulletRed = FNV1A::HashConst( "effects\\defense_buff_bullet_red.vmt" );
	constexpr auto BulletBlue = FNV1A::HashConst( "effects\\defense_buff_bullet_blue.vmt" );
	constexpr auto ExplosionRed = FNV1A::HashConst( "effects\\defense_buff_explosion_red.vmt" );
	constexpr auto ExplosionBlue = FNV1A::HashConst( "effects\\defense_buff_explosion_blue.vmt" );
	constexpr auto FireRed = FNV1A::HashConst( "effects\\defense_buff_fire_red.vmt" );
	constexpr auto FireBlue = FNV1A::HashConst( "effects\\defense_buff_fire_blue.vmt" );

	const auto Hash = FNV1A::Hash( Def->m_pszMaterialName );

	bool IsVaccinatorIcon = Hash == BulletRed || Hash == BulletBlue
		|| Hash == ExplosionRed || Hash == ExplosionBlue
		|| Hash == FireRed || Hash == FireBlue;

	if ( IsVaccinatorIcon )
	{
		return;
	}

	/*const auto Crit = FNV1A::HashConst( "effects\\crit.vmt" );
	if ( Hash == Crit )
	{
		return OriginalFunction( ECX, EDX, meshbuilder, ctx, info, particle, sortlist, camera );
	}

	int Group = particle / 4;
	int Offset = particle & 0x3;
	int ColorIndex = Group * info.m_nRGBStride;

	info.m_pRGB[ ColorIndex + 0 ].m128_f32[ Offset ] = 1.0f;
	info.m_pRGB[ ColorIndex + 1 ].m128_f32[ Offset ] = 1.0f;
	info.m_pRGB[ ColorIndex + 2 ].m128_f32[ Offset ] = 1.0f;
	sortlist->m_nAlpha = 10;*/

	/*const auto& RenderContext = gMaterialSystem->GetRenderContext( );
	RenderContext->DepthRange( 0.0f, 0.02f );*/
	return OriginalFunction( ECX, EDX, meshbuilder, ctx, info, particle, sortlist, camera );
	//RenderContext->DepthRange( 0.0f, 1.0f );
}

HOOK_POINTER( RenderSpriteTrail, "client.dll", "53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C 24 ? 8B EC 83 EC ? 56 8B 73 ? 8B",
			  void, __fastcall, void* ECX, void* EDX, void* meshbuilder, SpriteRenderInfo_t& info, int particle, const Vector& CameraPos, float flOODt, ParticleRenderData_t* const sortlist )
{
	const auto& Def = info.m_pParticles->m_pDef;

	int Group = particle / 4;
	int Offset = particle & 0x3;
	int ColorIndex = Group * info.m_nRGBStride;

	info.m_pRGB[ ColorIndex + 0 ].m128_f32[ Offset ] = 1.0f;
	info.m_pRGB[ ColorIndex + 1 ].m128_f32[ Offset ] = 1.0f;
	info.m_pRGB[ ColorIndex + 2 ].m128_f32[ Offset ] = 1.0f;

	return OriginalFunction( ECX, EDX, meshbuilder, info, particle, CameraPos, flOODt, sortlist );
}

HOOK_VTABLE( GetScreenAspectRatio, gEngineClient, 95u, float, __fastcall, void* ECX, void* EDX )
{
	float ratio = Config->Visuals->AspectRatio;
	if ( ratio < 0.01f )
	{
		ratio = ( float )TF2->ScreenWidth / TF2->ScreenHeight;
	}
	return ratio;
}

HOOK_POINTER( FireBullets, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 56 57 8B F9 8B 5D", void, __fastcall,
	void* ECX, void* EDX, CBaseCombatWeapon* weapon, const FireBulletsInfo_t& info, bool doeffects, int damagetype, int customdamagetype )
{
	if ( !gLocalPlayer || G->DetachSystem->WantsDetach )
	{
		return OriginalFunction( ECX, EDX, weapon, info, doeffects, damagetype, customdamagetype );
	}

	const auto& Player = reinterpret_cast< CBasePlayer* >( ECX );
	if ( !Player || Player->IsDormant( ) )
	{
		return OriginalFunction( ECX, EDX, weapon, info, doeffects, damagetype, customdamagetype );
	}

	if ( Config->Visuals->BulletTracers && damagetype & ( 1 << 20 ) )
	{
		doeffects = true;
		damagetype &= ~( 1 << 20 );
	}

	*const_cast<int*>( &info.m_iTracerFreq ) = 1;

	const auto& PlayerWeapon = Player->ActiveWeapon( );
	if ( !PlayerWeapon || PlayerWeapon->IsDormant( ) )
	{
		return OriginalFunction( ECX, EDX, weapon, info, doeffects, damagetype, customdamagetype );
	}

	const Vector start = Player->EyePosition( );
	const Vector end = start + info.m_vecDirShooting * info.m_flDistance;
	Ray_t Ray; CTraceFilterHitscan Filter; trace_t Trace;
	Filter.Skip = Player;

	Ray.Init( start, end );
	gEngineTrace->TraceRay( Ray, MASK_SHOT | CONTENTS_GRATE, &Filter, &Trace );

	if ( !gPrediction->FirstTimePredicted )
	{
		return;
	}

	if ( Config->Visuals->Beams->Enabled )
	{
		/*
		:troll:
		*/
		if ( Player == gLocalPlayer && !Config->Visuals->Beams->Local )
			return OriginalFunction( ECX, EDX, weapon, info, doeffects, damagetype, customdamagetype );
		else if ( Player != gLocalPlayer && Player->IsEnemy( ) && !Config->Visuals->Beams->Enemy )
			return OriginalFunction( ECX, EDX, weapon, info, doeffects, damagetype, customdamagetype );
		else if ( Player != gLocalPlayer && !Player->IsEnemy( ) && !Config->Visuals->Beams->Team )
			return OriginalFunction( ECX, EDX, weapon, info, doeffects, damagetype, customdamagetype );
		else if ( Player != gLocalPlayer && Player->IsSteamFriend( ) && !Config->Visuals->Beams->Friends )
			return OriginalFunction( ECX, EDX, weapon, info, doeffects, damagetype, customdamagetype );

		if ( !PlayerWeapon )
			return OriginalFunction( ECX, EDX, weapon, info, doeffects, damagetype, customdamagetype );

		const int iAttachment = PlayerWeapon->LookupAttachment( "muzzle" );
		PlayerWeapon->GetAttachment( iAttachment, Trace.StartPosition );

		Color BeamCol = Player->GetBeamsColor( Config->Visuals->Beams->ColorScheme );
		if ( Player->IsEnemy( ) )
			G->AntiAim->FiredShotsTimes[ Player->GetIndex( ) ] = gEngineClient->Time( );

		gBeams->DrawBeam( Trace.StartPosition, Trace.EndPosition, BeamCol, 1.f, 1.f );
		return;
	}

	return OriginalFunction( ECX, EDX, weapon, info, doeffects, damagetype, customdamagetype );
}

HOOK_POINTER( GetTracerType, "client.dll", "55 8B EC 51 53 56 57 8B F9 E8 ? ? ? ? 6A 00",
	const char*, __fastcall, void* ECX, void* EDX )
{
	if (!ECX || G->DetachSystem->WantsDetach || ECX != gLocalWeapon )
		return OriginalFunction( ECX, EDX );

	switch ( Config->Visuals->BulletTracers )
	{
	case 1:
		return ( gLocalPlayer->TeamNum( ) == TEAM_RED ? "dxhr_sniper_rail_red" : "dxhr_sniper_rail_blue" );
		break;
	case 2:
		return ( gLocalPlayer->IsCritBoosted( ) ? ( gLocalPlayer->TeamNum( ) == TEAM_RED ?
			"bullet_tracer_raygun_red_crit" : "bullet_tracer_raygun_blue_crit" ) :
			( gLocalPlayer->TeamNum( ) == TEAM_RED ? "bullet_tracer_raygun_red" : 
				"bullet_tracer_raygun_blue" ) );
		break;
	case 3:
		return gLocalPlayer->TeamNum( ) == TEAM_RED ? "dxhr_lightningball_hit_zap_red" : "dxhr_lightningball_hit_zap_blue";
		break;
	case 4:
		return "merasmus_zap";
		break;
	case 5:
		return "merasmus_zap_beam02";
		break;
	case 6:
		return gLocalPlayer->IsCritBoosted( ) ? ( gLocalPlayer->TeamNum( ) == TEAM_RED ?
			"bullet_bignasty_tracer01_red_crit" : "bullet_bignasty_tracer01_blue_crit" )
			: ( gLocalPlayer->TeamNum( ) == TEAM_RED ? "bullet_bignasty_tracer01_blue" : 
			"bullet_bignasty_tracer01_red" );
		break;
	default: break;
	}

	return OriginalFunction( ECX, EDX );
}

HOOK_POINTER( IsUsingStaticPropDebugModes, "engine.dll", "A1 ? ? ? ? 83 78 ? ? 75 ? A1 ? ? ? ? 83 78 ? ? 7D",
			  bool, __cdecl )
{
	return true;
}

HOOK_POINTER( DatatableWarning, "engine.dll", "55 8B EC B8 ? ? ? ? E8 ? ? ? ? 8D 45 0C 50 FF 75 08 8D 85 ? ? ? ? 68 ? ? ? ? 50 E8 ? ? ? ? 8D 85 ? ? ? ? 50 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 18 8B E5 5D C3",
			  void, __cdecl, const char* msg, ... )
{
	return;
}

//hooking this and firing events here (or cl_move) sometimes fucks over player data (like origin)
//because we're calling it too early (before we fully receive server data)
//this pasted supremacy shit doesn't work properly, don't hook, maybe only if we do fakelatency
HOOK_POINTER( ProcessPacket, "engine.dll", "55 8B EC 83 EC ? 53 56 57 8B 7D ? 8B D9 83 7F",
			  void, __fastcall, void* ECX, void* EDX, void* packet, bool header )
{
	auto NetChan = reinterpret_cast< CNetChannel* >( ECX );
	auto Packet = reinterpret_cast< CNetChannel::netpacket_t* >( packet );

	/*bool ShouldGetChokedTicks = NetChan->GetName( )[ 0 ] == 'C' && NetChan->GetName( )[ 5 ] == 'T'; //CLIENT

	float original_last_received = NetChan->last_received;

	if ( ShouldGetChokedTicks )
	{
		TF2->FlowUpdate( NetChan, FLOW_INCOMING, Packet->wiresize + UDP_HEADER_SIZE );
	}*/

	OriginalFunction( ECX, EDX, packet, header );

	//if ( ShouldGetChokedTicks && NetChan->last_received != original_last_received )
	//{
	//	TF2->FlowNewPacket( NetChan, FLOW_INCOMING, NetChan->InSequenceNr, NetChan->OutSequenceNrAck, NetChan->ChokedPackets, NetChan->m_PacketDrop, Packet->wiresize + UDP_HEADER_SIZE );
	//}

	TF2->UpdateIncomingSequences( );

	//useless since event delay isnt applied, look lower
	/*for ( CEventInfo* ei{ gClientState->events }; ei != nullptr; ei = ei->next )
	{
		if ( !ei || !ei->classID)
			continue;

		ei->fire_delay = 0.0f;
	}*/

	gEngineClient->FireEvents( );
}

HOOK_POINTER( SendDatagram, "engine.dll", "55 8B EC B8 ? ? ? ? E8 ? ? ? ? A1 ? ? ? ? 53 56 8B D9",
			  int, __fastcall, void* ECX, void* EDX, bf_write* datagram )
{
	auto NetChan = reinterpret_cast< CNetChannel* >( ECX );
	if ( !NetChan || !gLocalPlayer || datagram || G->DetachSystem->WantsDetach ||
		!Config->Aimbot->Hitscan->FakeLatency || ( Config->Aimbot->Hitscan->FakeLatencyKey && !Misc->IsFakeLatencyToggled ) )
	{
		return OriginalFunction( ECX, EDX, datagram );
	}

	int InSequenceNrBackup = NetChan->InSequenceNr;
	int InReliableStateBackup = NetChan->InReliableState;

	/*static int times = 0;
	static int tickstart = 0;
	static float nexttime = 0;
	static bool isplaying = false;
	int out = NetChan->OutSequenceNr;
	int originalchokedpackets = NetChan->ChokedPackets;
	double originalcleartime = NetChan->m_fClearTime;*/

	TF2->AddLatencyToNetChannel( NetChan, Config->Aimbot->Hitscan->DynamicLatency
								 ? ( 0.2f - NetChan->GetLatency( FLOW_OUTGOING ) - 0.02f ) //:troller:
								 : ( Config->Aimbot->Hitscan->FakeLatencyAmount / 1000.f ) - NetChan->GetLatency( FLOW_OUTGOING ) );

	int Result = OriginalFunction( ECX, EDX, datagram );

	/*auto latency_out = NetChan->GetLatency( FLOW_OUTGOING );
	if ( latency_out < 0.2f )
	{
		auto v13 = NetChan->InSequenceNr - ( ( ( 0.2f - latency_out ) / gGlobals->interval_per_tick ) + 0.5f );
		NetChan->InSequenceNr = v13;

		for ( auto& seq : G->Sequences )
		{
			if ( seq.seq != seq.state )
			{
				//std::clamp(seq.m_seq, 0, (int)v13);

				while ( seq.seq <= v13 )
				{
					seq.seq += 2;
					if ( seq.seq == v13 )
					{
						NetChan->InReliableState = InReliableStateBackup;
						NetChan->InSequenceNr = InSequenceNrBackup;

						return Result;
					}
				}

				NetChan->InReliableState = seq.state;
			}
		}
	}

	int nTotalSize = ( int )( ( NetChan->m_fClearTime - originalcleartime ) * NetChan->m_Rate );

	TF2->FlowNewPacket( NetChan, FLOW_OUTGOING, NetChan->OutSequenceNr, InSequenceNrBackup, originalchokedpackets, 0, nTotalSize );
	TF2->FlowUpdate( NetChan, FLOW_OUTGOING, nTotalSize );*/

	NetChan->InSequenceNr = InSequenceNrBackup;
	NetChan->InReliableState = InReliableStateBackup;

	return Result;
}

//this shit doesnt even run in this game XDDDXDXDXDXDXXDDXDXDD (what sets event delay)
HOOK_POINTER( ProcessTempEntities, "engine.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B F1 32 DB B9",
			  bool, __fastcall, void* ECX, void* EDX, void* msg )
{
	return OriginalFunction( ECX, EDX, msg );
}

HOOK_POINTER( CL_LoadWhitelist, "engine.dll", "55 8B EC 83 EC ? 56 8B 75 ? 8B CE FF 75", void*, __cdecl, void* table, const char* name )
{
	return G->DetachSystem->WantsDetach ? OriginalFunction( table, name ) : nullptr;
}

HOOK_POINTER( CL_CheckForPureServerWhitelist, "engine.dll", "55 8B EC 83 3D ? ? ? ? 01 7E ? 80 3D ? ? ? ? 00", void, __cdecl, void** FilesToReload )
{
	if ( G->DetachSystem->WantsDetach )
	{
		return OriginalFunction( FilesToReload );
	}

	return;
}

HOOK_POINTER( GetMaterial, "client.dll", "55 8B EC 56 8B 75 ? 57 8B F9 83 FE ? 74 ? 83 FE ? 74 ? 8B 0F 85 C9 74 ? 8B 01 FF 75 ? FF 50",
			  IMaterial*, __fastcall, void* ECX, void* EDX, void* RenderMode, int Frame )
{
	//stop crashing you fucking nigger
	return OriginalFunction( ECX, EDX, RenderMode, Frame );
}

static int AntiAutoBalance = 0;
static const char* PreviousServer = { };
HOOK_VTABLE( DispatchUserMessage, gClient, 36u, bool, __fastcall, void* ECX, void* EDX, int type, bf_read& data )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, type, data );

	if ( !NoSpread->DispatchUserMessageHandler( type, &data ) )
	{
		return true;
	}

	auto Data = reinterpret_cast< const char* >( data.m_pData );
	data.SetAssertOnOverflow( false );

	if ( Data )
	{
		switch ( type )
		{
			case TextMsg:
			{
				if ( data.GetNumBitsLeft( ) > 35 && strstr( Data, "#TF_Autobalance_TeamChangePending" ) )
				{
					if ( gClientState->m_NetChannel && gClientState->m_NetChannel->GetAddress( ) != PreviousServer ) {
						PreviousServer = gClientState->m_NetChannel->GetAddress( );
						AntiAutoBalance = 0;
					}

					if ( AntiAutoBalance < 2 ) {
						gEngineClient->ClientCmd_Unrestricted( "retry" );
					}

					AntiAutoBalance++;
				}
				break;
			}
		}
	}

	data.Seek( 0 );
	return OriginalFunction( ECX, EDX, type, data );
}

HOOK_POINTER( SendNetMessage, "engine.dll", "55 8B EC 57 8B F9 8D 8F ? ? ? ? E8 ? ? ? ? 85 C0 75 07 B0 01 5F 5D C2 0C 00", 
			  bool, __fastcall, void* ECX, void* EDX, INetMessage* msg, bool force_reliable, bool voice )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, msg, force_reliable, voice );

	if ( msg->GetType( ) == 14 )
		return true;

	if ( msg->GetType( ) == 10 )
		voice = true;

	bool retn = false, runfurther = false;
	if ( NoSpread->SendNetMessageHandler( msg ) ) {
		force_reliable = true;

		//event firing is normally done in FRAME_RENDER_START, which is too late
		//we can fire our events in SendNetMsg, since it runs in _Host_RunFrame_Client, after we have received server data
		//potential engine fix, remove if it causes issues
		//gEngineClient->FireEvents( );
		runfurther = true;
	}

	retn = OriginalFunction( ECX, EDX, msg, force_reliable, voice );

	if ( runfurther )
		NoSpread->SendNetMessagePostHandler( );

	return retn;
}

HOOK_POINTER(TFGameMovementShit, "client.dll", "55 8B EC 83 EC ? 53 56 8B D9 57 89 5D ? E8 ? ? ? ? 8B 73",
	void, __fastcall, void* ECX, void* EDX)
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX );

	if (!gTFGameMovement && ECX )
		gTFGameMovement = reinterpret_cast< CGameMovement* >( ECX );

	return OriginalFunction(ECX, EDX);
}

HOOK_VTABLE( GetDirectPingToPOP, gSteamNetwork, 9u, int, __fastcall, void* ECX, void* EDX, SteamNetworkingPOPID ID )
{
	if ( !Config->Misc->Matchmaking->RegionSelector->Enabled || !ECX || !EDX || G->DetachSystem->WantsDetach )
	{
		return OriginalFunction( ECX, EDX, ID );
	}

	char popIDName[ 5 ];
	ID.ToString( popIDName );

	for ( auto& dc : DataCenterHashMap )
	{
		if ( dc.first == popIDName && TF2->IsRegionAllowed( dc.second ) )
		{
			return 1;
		}

		if ( dc.first == popIDName && !TF2->IsRegionAllowed( dc.second ) )
		{
			return std::numeric_limits<int>::max( );
		}
	}

	return OriginalFunction( ECX, EDX, ID );
}

HOOK_POINTER( ViewmodelPos, "client.dll", "55 8B EC 83 EC 70 8B 55 0C 53 8B 5D 08 89 4D FC 8B 02 89 45 E8 8B 42 04 89 45 EC 8B 42 08 89 45 F0 56 57", void, __fastcall, void* ECX, void* EDX, CBasePlayer* Owner, Vector& Position, Vector& Angles )
{
	if ( Config->Visuals->ViewmodelPos && !G->DetachSystem->WantsDetach )
	{
		Vector AxisX{}, AxisY{}, AxisZ{};

		gMath->AngleVectors( Angles, &AxisY, &AxisX, &AxisZ );

		Vector eyepos = Position + ( AxisX * Config->Visuals->ViewmodelX ) + ( AxisY * Config->Visuals->ViewmodelY ) + ( AxisZ * Config->Visuals->ViewmodelZ );
		Angles.z += Config->Visuals->ViewmodelRoll;

		OriginalFunction( ECX, EDX, Owner, eyepos, Angles );
	}
	else
	{
		OriginalFunction( ECX, EDX, Owner, Position, Angles );
	}
}

HOOK_POINTER( SetupBones, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 33 C9 33 D2 89 4D ? 89 55 ? 8B 46 ? 85 C0 74 ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68",
			  bool, __fastcall, void* ECX, void* EDX, Matrix3x4* BoneToWorldOut, int MaxBones, int BoneMask, float CurrentTime )
{
	static float CurTime = 0.f;
	if ( !ECX || G->DetachSystem->WantsDetach )
	{
		return OriginalFunction( ECX, EDX, BoneToWorldOut, MaxBones, BoneMask, CurrentTime );
	}

	if ( CurrentTime != CurTime )
	{
		if ( !G->CachedBones.empty( ) )
			G->CachedBones.clear( );
	}

	CurTime = CurrentTime; // this crashes?

	if ( !G->CachedBones[ ECX ].Cached )
	{
		G->CachedBones[ ECX ].Cached = false;
		bool retvalue = OriginalFunction( ECX, EDX, G->CachedBones[ ECX ].Bones, 128, BONE_USED_BY_ANYTHING | BoneMask, CurrentTime );
		if ( !retvalue )
			return false;

		G->CachedBones[ ECX ].Cached = true;
	}

	if ( BoneToWorldOut )
	{
		memcpy( BoneToWorldOut, G->CachedBones[ ECX ].Bones, MaxBones * sizeof( Matrix3x4 ) );
	}

	return true;
}

HOOK_POINTER( VarBaseArray_Interpolate, "client.dll", "55 8B EC 83 EC 1C D9 45 0C 8D 45 FC 56 50 83 EC 08 C7 45 ? ? ? ? ? 8D 45 E4 8B F1 D9 5C 24 04 D9 45 08 D9 1C 24 50 E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? F6 46 2C 01 53 57 74 31 83 7D FC 00 B9 ? ? ? ? D9 45 08 B8 ? ? ? ? 0F 44 C1 8B CE 50 8B 06 83 EC 08 DD 1C 24 FF 50 28 50 68 ? ? ? ? FF 15 ? ? ? ? 83 C4 14 80 7D E4 00 74 60 0F B7 5E 0E 8B 55 F0 0F B7 4E 0C 03", 
	int, __fastcall, void* ECX, void* EDX, float Current, float Interp )
{
	return G->DetachSystem->WantsDetach ? OriginalFunction( ECX, EDX, Current, Interp ) : 1;
}

HOOK_POINTER( CreateTFRagdoll, "client.dll", "55 8B EC B8 ? ? ? ? E8 ? ? ? ? 53 56 57 8B F9 8B 8F ? ? ? ? 85 C9 0F 85",
	void, __fastcall, void* ECX, void* EDX )
{
	if ( !ECX || G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX );

	if ( !Misc->CanDoRagdollEffects( reinterpret_cast< CTFRagdoll* >( ECX ) ) )
		return OriginalFunction( ECX, EDX );

	CTFRagdoll* _this = reinterpret_cast< CTFRagdoll* >( ECX );
	_this->Gib( ) = false;
	_this->FeignDeath( ) = false;
	_this->Cloaked( ) = false;

	_this->BecomeAsh( ) = Config->Visuals->RagdollBecomeAsh;
	_this->Burning( ) = Config->Visuals->RagdollBurning;
	_this->Electrocuted( ) = Config->Visuals->RagdollElectrocuted;
	_this->Dissolving( ) = Config->Visuals->RagdollDissolving;
	_this->GoldRagdoll( ) = Config->Visuals->RagdollBaseType == 1;
	_this->IceRagdoll( ) = Config->Visuals->RagdollBaseType == 2;

	OriginalFunction( ECX, EDX );
}

HOOK_POINTER( CMatchInvite_OnTick, "client.dll", "55 8B EC 83 EC 0C 56 8B F1 E8 ? ? ? ? 8B 86 ? ? ? ? 85 C0 74 09 83 F8 02 0F 85",
	void, __fastcall, void* ECX, void* EDX )
{
	static auto CMatchInviteNotification_AcceptMatch = Memory->FindSignature<void( __thiscall* )( void* )>( 
			"client.dll", "55 8B EC 83 EC 10 56 8B F1 8B 86 ? ? ? ? 83 E8 00" );
	if ( !CMatchInviteNotification_AcceptMatch || G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX );

	if ( !gEngineClient->IsInGame( ) )
	{
		if ( Config->Misc->Matchmaking->AutoAccept )
		{
			OriginalFunction( ECX, EDX );
			return CMatchInviteNotification_AcceptMatch( ECX );
		}
	}

	if ( Config->Misc->Matchmaking->PreventAutoJoin )
		*reinterpret_cast< double* >( reinterpret_cast< DWORD >( ECX ) + 424 ) = 0.f;

	//jod bless this address holy shit
	DWORD* SurveyAddress = reinterpret_cast< DWORD* >( gTFGCClientSystem + 1488 );
	if ( SurveyAddress && *SurveyAddress & 2 )
	{
		*SurveyAddress &= ~2;
	}

	OriginalFunction( ECX, EDX );
}

/*HOOK_POINTER( DrawViewmodels, "client.dll", "55 8B EC 81 EC ? ? ? ? 8B 15 ? ? ? ? 53 8B D9 33 C9 89 4D C4 89 4D C8 8B 02 89 5D D4 89 4D CC 89 4D D0 56", void, __fastcall, void* ECX, void* EDX, const CViewSetup& View, bool Draw )
{
	if ( !Config->Visuals->HandsGlow || !gLocalPlayer || !gLocalPlayer->IsAlive( ) )
	{
		return OriginalFunction( ECX, EDX, View, Draw );
	}

	const auto& RenderContext = gMaterialSystem->GetRenderContext( );
	if ( !RenderContext )
	{
		return OriginalFunction( ECX, EDX, View, Draw );
	}


}*/

/*double __cdecl Plat_FloatTime::Function( )
{
	return OriginalFunction( ) + ( 67112.0f * 4.f );
}

void Plat_FloatTime::Init( )
{
	auto target = reinterpret_cast< Fn >( GetProcAddress( GetModuleHandleA( "tier0.dll" ), "Plat_FloatTime" ) );
	MH_CreateHook( target, &Function, reinterpret_cast< LPVOID* >( &OriginalFunction ) );
}*/

HOOK_POINTER( StartDynamicSound, "engine.dll", "55 8B EC 8B 0D ? ? ? ? 81 EC ? ? ? ? 85 C9 0F 84 ? ? ? ? 8B 01 8B 40 04 FF D0 84 C0 0F 84 ? ? ? ? 56", void, __cdecl, StartSoundParams_t& Params )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( Params );

	CBasePlayer* Player = gEntityList->GetClientEntity( Params.soundsource )->As<CBasePlayer>( );

	if ( !Player || !Player->IsDormant( ) || !Player->IsAlive( ) || !Player->IsPlayer( ) || Player == gLocalPlayer )
	{
		return OriginalFunction( Params );
	}

	if ( Params.origin.IsZero( ) )
		return OriginalFunction( Params );

	Player->SetAbsOrigin( Params.origin );
	G->FarEsp[ Player ] = gGlobals->curtime;
	if ( gPlayerResource )
		Player->Health( ) = gPlayerResource->GetHealth( Player->GetIndex( ) );

	/*if ( !Player->IsAlive( ) )
		Player->LifeState( ) = LIFE_ALIVE;*/

	return OriginalFunction( Params );
}

HOOK_POINTER( NotificationQueueAdd, "client.dll", "55 8B EC 8B 0D ? ? ? ? 56 57 8B 01 8B 40 68 FF D0 8B 75 08 84 C0 75 09 80 BE ? ? ? ? ? 75 29", int, __cdecl, CEconNotification* Notification )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( Notification );

	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == G->HasNewItemsCallAddress )
	{
		Notification->Accept( );
		Notification->Trigger( );
		Notification->UpdateTick( );
		Notification->MarkForDeletion( );
		return 0;
	}

	return OriginalFunction( Notification );
}

HOOK_POINTER( ShowItemsPickedUp, "client.dll", "55 8B EC 83 EC 18 80 7D 08 00", bool, __fastcall, void* ECX, void* EDX, bool Force, bool ReturnToGame, bool NoPanel )
{
	if ( G->DetachSystem->WantsDetach )
		return OriginalFunction( ECX, EDX, Force, ReturnToGame, NoPanel );

	OriginalFunction( ECX, EDX, true, true, true );
	return false;
}

HOOK_POINTER( SetInt, "client.dll", "55 8B EC 6A 01 FF 75 08 E8 ? ? ? ? 85 C0 74 0A 8B 4D 0C 89 48 0C C6 40 10 02", void, __fastcall, void* ECX, void* EDX, const char* keyName, int value )
{
	OriginalFunction( ECX, EDX, keyName, value );

	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == G->ScoreboardReturnAddressDesired && std::string_view( keyName ).find( "nemesis" ) != std::string_view::npos )
		*reinterpret_cast< uintptr_t* >( _AddressOfReturnAddress( ) ) = G->ScoreboardReturnAddressJump;
}

HOOK_POINTER( IsPlayerDominated, "client.dll", "55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 75 08 5F 32 C0 5E 5D C2 04 00", bool, __fastcall, void* ECX, void* EDX, int index )
{
	bool result = OriginalFunction( ECX, EDX, index );

	if ( !result )
	{
		if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) == G->IsPlayerDominatedReturnAddressDesired )
			*reinterpret_cast< uintptr_t* >( _AddressOfReturnAddress( ) ) = G->ScoreboardReturnAddressJump;
	}

	return result;
}

HOOK_POINTER( SetAbsVelocity, "client.dll", "55 8B EC 83 EC ? 56 57 8B 7D ? 8B F1 F3 0F", void, __fastcall, void* ECX, void* EDX, const Vector& Velocity )
{
	if ( reinterpret_cast< DWORD >( _ReturnAddress( ) ) != G->SetAbsVelocityCallAddress )
	{
		return OriginalFunction( ECX, EDX, Velocity );
	}

	const auto& Player = reinterpret_cast< CBasePlayer* >( ECX );
	if ( Player == gLocalPlayer )
		return;

	if ( MovementSim->VelocityRecords.find( Player->GetIndex( ) ) == MovementSim->VelocityRecords.end( ) )
	{
		return OriginalFunction( ECX, EDX, Velocity );
	}

	const auto& Record = MovementSim->VelocityRecords[ Player->GetIndex( ) ];
	if ( Player->SimulationTime( ) - Record.SimTime > 0.0f )
	{
		Vector Origin = Record.Origin;

		if ( !( Player->Flags( ) & FL_ONGROUND ) && !( Record.Flags & FL_ONGROUND ) )
		{
			bool Correct = false;

			if ( ( Player->Flags( ) & FL_DUCKING ) && !( Record.Flags & FL_DUCKING ) ) {
				Origin.z += 20.0f;
				Correct = true;
			}

			if ( !( Player->Flags( ) & FL_DUCKING ) && ( Record.Flags & FL_DUCKING ) ) {
				Origin.z -= 20.0f;
				Correct = true;
			}

			if ( Correct )
			{
				Vector NewVelocity = Velocity;
				NewVelocity.z = ( Player->Origin( ).z - Origin.z ) / ( Player->SimulationTime( ) - Record.SimTime );
				return OriginalFunction( ECX, EDX, NewVelocity );
			}
		}
	}

	return OriginalFunction( ECX, EDX, Velocity );
}

HOOK_POINTER( ProcessFixAngle, "engine.dll", "55 8B EC 8B 45 08 83 EC 08 F3 0F 10 15 ? ? ? ? F3 0F 10 0D ? ? ? ? F3 0F 10 40 ? 0F 2F C2 76 09 F3 0F 5C C1",
	bool, __fastcall, void* ECX, void* EDX, void* Msg )
{
	return true;
}

HOOK_POINTER( R_LoadSkys, "engine.dll", "55 8B EC 81 EC ? ? ? ? 8D 4D ? 53 68 ? ? ? ? B3 ? E8 ? ? ? ? 8D 4D", void, __cdecl, void )
{
	if ( !Config->Visuals->SkyboxName.empty( ) )
	{
		static auto sv_skyname = gCvar->FindVar( "sv_skyname" );

		std::string Original = sv_skyname->GetString( );

		sv_skyname->SetValue( Config->Visuals->SkyboxName.c_str( ) );

		OriginalFunction( );

		sv_skyname->SetValue( Original.c_str( ) );

		return;
	}

	OriginalFunction( );
}

HOOK_POINTER( ResetLatched, "client.dll", "53 8B D9 8B 03 8B 80 ? ? ? ? FF D0 84 C0 75 25 57 8B 7B 20", 
	void, __fastcall, void* ECX, void* EDX )
{
	// :shibasmart:
	return;
}

HOOK_POINTER( PlayerMove, "client.dll", "55 8B EC 83 EC 6C 56 8B F1 6A 7E 8B 8E ? ? ? ? 81 C1 ? ? ? ? E8 ? ? ? ? 84 C0 74 09 8B 46 08",
	void, __fastcall, void* ECX, void* EDX)
{
	static auto CGameMovement_PlayerMove = Memory->FindSignature<DWORD>( "client.dll", "E8 ? ? ? ? 8B BE ? ? ? ? 8B 4E 0C" );
	static auto CallFunction =
		*reinterpret_cast< std::uintptr_t* >( CGameMovement_PlayerMove + 1 ) + CGameMovement_PlayerMove + 5;

	if ( MovementSim->IsRunning( ) )
	{
		reinterpret_cast< void( __thiscall* )( void* ) >( CallFunction )( ECX );
		return;
	}

	OriginalFunction( ECX, EDX );
}

HOOK_POINTER( DoEnginePostProcessing, "client.dll", "55 8B EC 81 EC ? ? ? ? 8B 0D ? ? ? ? 53 33 DB 89 5D 8C 89 5D 90 8B 01",
	void, __cdecl, int x, int y, int w, int h, bool FleshlightOn, bool PostVGui )
{
	if ( Config->Visuals->RemovePostProcessing )
		return;

	OriginalFunction( x, y, w, h, FleshlightOn, PostVGui );
}

//removing local and friend's name everywhere
HOOK_POINTER( GetPlayerName, "client.dll", "55 8B EC 56 8B 75 ? 57 8B F9 8D 46 ? 83 F8",
			  const char*, __fastcall, void* ECX, void* EDX, int index )
{
	const auto& Entity = gEntityList->GetClientEntity( index )->As<CBasePlayer>( );
	if ( !Entity || !Config->Visuals->StreamerMode )
	{
		return OriginalFunction( ECX, EDX, index );
	}

	PlayerInfo_t info;
	if ( !gEngineClient->GetPlayerInfo( index, &info ) )
	{
		return OriginalFunction( ECX, EDX, index );
	}

	if ( ( Config->Visuals->HideLocal && index == gEngineClient->GetLocalPlayer( ) ) ||
		 ( Config->Visuals->HideFriends && Entity->IsSteamFriend( ) ) )
	{
		if ( !G->HiddenNames.empty( ) )
		{
			for ( const auto& [playerindex, name] : G->HiddenNames )
			{
				if ( playerindex == info.friendsID )
				{
					//stop pushing useless shit to the vector if we've already found our names
					return ( index == gEngineClient->GetLocalPlayer( ) ) ? "Local" : "Friend";
				}
			}
		}

		G->HiddenNames.push_back( { info.friendsID, { ( index == gEngineClient->GetLocalPlayer( ) ), OriginalFunction( ECX, EDX, index ) } } );
		return ( index == gEngineClient->GetLocalPlayer( ) ) ? "Local" : "Friend";
	}

	return OriginalFunction( ECX, EDX, index );
}

std::string ColorRed = { '\x7', 'F', 'F', '3', 'F', '3', 'F' };
std::string ColorBlue = { '\x7', '9', '9', 'C', 'C', 'F', 'F' };
HOOK_VTABLE( ChatPrintf, gClientMode->m_pChatElement, 19u, void, __cdecl, void* ECX, int index, int filter, const char* message, ... )
{
	va_list marker;
	char buffer[ 4096 ];
	va_start( marker, message );
	vsnprintf_s( buffer, sizeof( buffer ), message, marker );
	va_end( marker );

	if ( strlen( buffer ) > 0 && buffer[ strlen( buffer ) - 1 ] == '\n' )
	{
		buffer[ strlen( buffer ) - 1 ] = 0;
	}
	char* msg = buffer;
	while ( *msg && ( *msg == '\n' || *msg == '\r' || *msg == '\x1A' ) )
	{
		msg++;
	}
	if ( !*msg ) { return; }

	std::string Message = msg;

	const auto& Entity = gEntityList->GetClientEntity( index )->As<CBasePlayer>( );
	if ( !Entity )
	{
		return OriginalFunction( ECX, index, filter, "%s", Message.c_str( ) );
	}

	PlayerInfo_t info{};
	if ( !gEngineClient->GetPlayerInfo( index, &info ) )
	{
		/*if ( G->HiddenNames.empty( ) )
		{
			Message = "";
		}
		else
		{
			for ( const auto& [playerindex, playername] : G->HiddenNames )
			{
				//doing it like this since other players or the engine can contain our name, yet we still have to identify it
				std::string hiddenname = playername.second;
				std::string entityname = playername.first ? "Local" : "Friend";
				if ( Message.find( hiddenname.c_str( ) ) != std::string::npos )
				{
					Message = Message.replace( Message.find( hiddenname.c_str( ) ), Message.find( hiddenname.c_str( ) ) + hiddenname.length( ), entityname.c_str( ) );
				}
			}
		}*/
		return OriginalFunction( ECX, index, filter, "%s", Message.c_str( ) );
	}

	if ( index != gEngineClient->GetLocalPlayer( ) && !Entity->IsSteamFriend( ) )
	{
		return OriginalFunction( ECX, index, filter, "%s", Message.c_str( ) );
	}

	std::string name = info.name;
	if ( Message.find( name ) == std::string::npos )
	{
		return OriginalFunction( ECX, index, filter, "%s", Message.c_str( ) );
	}

	//loop through our hidden names each message to see if we have anything to remove
	//couldn't make this work with a map, since GetPlayerName would always crash every time I assigned it, so a vector will have to do
	if ( !G->HiddenNames.empty( ) )
	{
		for ( const auto& [playerindex, playername] : G->HiddenNames )
		{
			if ( playerindex != info.friendsID )
			{
				continue;
			}

			std::string hiddenname = playername.second;
			std::string entityname = playername.first ? "Local" : "Friend";

			/*if ( Message.find( hiddenname ) != std::string::npos && index != gEngineClient->GetLocalPlayer( ) && !Entity->IsSteamFriend( ) )
			{
				Message = Message.replace( Message.find( hiddenname ), Message.find( hiddenname ) + hiddenname.length( ), entityname.c_str( ) );
			}*/

			std::string newname = Entity->IsAlive( ) ? ( entityname + "\x1" ) : entityname;
			std::string finalname = ( Entity->TeamNum( ) == TEAM_RED ? ColorRed : ColorBlue ) + newname;
			std::string finalmessage = ( Entity->IsAlive( ) ? "" : "\x1*DEAD* " ) + finalname;
			Message = Message.replace( 0, Message.find( hiddenname ) + hiddenname.length( ), finalmessage.c_str( ) );
		}
	}
	return OriginalFunction( ECX, index, filter, "%s", Message.c_str( ) );
}

//i'm not sure if we need to change anything here
//nvm we do need this
HOOK_VTABLE( ConsoleColorPrintf, gCvar, 23u, void, __cdecl, void* ECX, const Color& color, const char* message, ... )
{
	va_list marker;
	char buffer[ 4096 ];
	va_start( marker, message );
	vsnprintf_s( buffer, sizeof( buffer ), message, marker );
	va_end( marker );

	char* msg = buffer;
	if ( !*msg ) { return; }

	std::string Message = msg;

	for ( const auto& [playerindex, playername] : G->HiddenNames )
	{
		std::string hiddenname = playername.second;
		std::string entityname = playername.first ? "Local" : "Friend";

		if ( Message.find( hiddenname ) != std::string::npos )
		{
			Message = Message.replace( 0, Message.find( hiddenname ) + hiddenname.length( ), entityname.c_str( ) );
		}
	}

	return OriginalFunction( ECX, color, "%s", Message.c_str( ) );
}

//while fakelagging, we can maintain our connection to the server so that instead of events being sent when you unchoke, they're sent instantly
//the advantage we gain by doing this is that we won't have to unchoke when attacking anymore and risk the enemy hitting us while breaking lagcomp
//we can achieve this by keeping a reliable client state without actually sending any usercmds
HOOK_POINTER( SetChoked, "engine.dll", "FF 41 ? FF 41", void, __fastcall, CNetChannel* ECX, void* EDX )
{
	if ( !ECX )
	{
		return OriginalFunction( ECX, EDX );
	}

	const auto ChokedBackup = ECX->ChokedPackets;
	ECX->ChokedPackets = 0;
	ECX->SendDatagram( NULL );
	--ECX->OutSequenceNr;
	ECX->ChokedPackets = ChokedBackup;

	return OriginalFunction( ECX, EDX );
}

//this is unfinished because prediction is still broken, so our movement will jitter around constantly while choking a packet
//this happens since CL_Predict uses the last acknowledged command for prediction, which is set by command_ack, which in turn is set by OutSequenceNrAck
//#1 CClientState::PacketStart	https://github.com/sr2echa/TF2-Source-Code/blob/c61d9750613a088617fa183a56708becfd38b81a/tf2_src/engine/servermsghandler.cpp#L171
//#2 CClientState::PacketEnd	https://github.com/sr2echa/TF2-Source-Code/blob/c61d9750613a088617fa183a56708becfd38b81a/tf2_src/engine/servermsghandler.cpp#L201
//#3 CL_RunPrediction			https://github.com/sr2echa/TF2-Source-Code/blob/c61d9750613a088617fa183a56708becfd38b81a/tf2_src/engine/cl_pred.cpp#L72
// 
//without the changes we do in SetChoked, while lagging OutSequenceNrAck increases with the rate at which you unchoke together with the amount of cmds you choked
//for example: if you were choking 22 ticks at cmd number 100, then your OutSequenceNrAck would be at 100 for 22 ticks then jump to
//cmd number + choked ticks when unchoking, or 122 in our case and then keep repeating in the same pattern, 122 for 22 ticks, 144 for 22 ticks and so on
//yet with our changes to SetChoked, OutSequenceNrAck increases at the same rate, but only by 1 constantly, because we make the client think that
// we're not choking any cmds, this sends you way behind the actual last command number and causes prediction errors
//TODO: figure out how to make PacketStart use the proper OutSequenceNrAck instead of the bogus one
HOOK_POINTER( PacketStart, "engine.dll", "55 8B EC 8B 45 ? 89 81 ? ? ? ? 8B 45 ? 89 81 ? ? ? ? 5D C2 ? ? CC CC CC CC CC CC CC 55 8B EC 56",
			  void, __fastcall, void* ECX, void* EDX, int incoming_sequence, int outgoing_acknowledged )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) )
		return OriginalFunction( ECX, EDX, incoming_sequence, outgoing_acknowledged );

	OriginalFunction( ECX, EDX, incoming_sequence, outgoing_acknowledged );
}

HOOK_VTABLE( DrawModelStaticProp, gStudioRender, 30u, void, __fastcall, void* ECX, void* EDX, const DrawModelInfo_t& drawInfo, const Matrix3x4& modelToWorld, int flags )
{
	if ( !Config->Visuals->WorldModulation->Enabled || Config->Visuals->WorldModulation->Mode == 1 || !Config->Visuals->WorldModulation->World )
	{
		return OriginalFunction( ECX, EDX, drawInfo, modelToWorld, flags );
	}

	switch ( Config->Visuals->WorldModulation->Mode )
	{
		case 0:
		{
			gStudioRender->SetColorModulation( Color( Config->Visuals->WorldModulation->Amount * 255.f,
											   Config->Visuals->WorldModulation->Amount * 255.f,
											   Config->Visuals->WorldModulation->Amount * 255.f ) );
			gStudioRender->SetAlphaModulation( Config->Visuals->WorldModulation->Alpha );
			break;
		}

		case 2:
		{
			gStudioRender->SetColorModulation( Config->Colors->WorldColor );
			gStudioRender->SetAlphaModulation( Config->Visuals->WorldModulation->Alpha );
			break;
		}

		default:
		{
			break;
		}
	}
	OriginalFunction( ECX, EDX, drawInfo, modelToWorld, flags );
}

HOOK_POINTER( GetItemDefinition, "client.dll", "55 8B EC 56 8B F1 8D 45 ? 57 50 8D 8E",
	void*, __fastcall, void* ECX, void* EDX, int Item )
{
	auto ItemDef = (CEconItemDefinition_t*)OriginalFunction( ECX, EDX, Item );
	if ( ItemDef && Config->Visuals->UnlockEquipRegion )
	{
		ItemDef->m_unEquipRegionMask = 0;
		ItemDef->m_unEquipRegionConflictMask = 0;
	}

	return OriginalFunction( ECX, EDX, Item );
}

HOOK_POINTER( GetOuterAbsVelocity, "client.dll", "55 8B EC 8B 49 ? 5D E9 ? ? ? ? CC CC CC CC 55 8B EC 83 EC ? 8B 01",
			  void, __fastcall, void* ECX, void* EDX, Vector& Velocity )
{
	if ( !ECX )
	{
		return OriginalFunction( ECX, EDX, Velocity );
	}

	const auto& BaseAnimState = reinterpret_cast< CMultiPlayerAnimState* >( ECX );
	const auto& Player = BaseAnimState->GetBasePlayer( );

	if ( Player )
	{
		PlayerInfo_t pi;
		if ( gEngineClient->GetPlayerInfo( Player->GetIndex( ), &pi ) )
		{
			if ( pi.fakeplayer && Player->IsOnGround( ) && strstr( gEngineClient->GetLevelName( ), "tr_walkway" ) )
			{
				Velocity = Vector( );
				return;
			}
		}
	}

	return OriginalFunction( ECX, EDX, Velocity );
}

HOOK_POINTER( UpdateRankControls, "client.dll", "55 8B EC 81 EC ? ? ? ? 53 8B D9 56 57 8B 7D",
			  void, __fastcall, void* ECX, void* EDX, unsigned int PreviousXP, unsigned int CurrentXP, LevelInfo& CurrentLevel )
{
	if ( !Config->Visuals->MedalChanger )
	{
		return OriginalFunction( ECX, EDX, PreviousXP, CurrentXP, CurrentLevel );
	}

	CurrentLevel.Medal = ( short )( ( ( Config->Visuals->MedalTier - 1 ) * 150 ) + Config->Visuals->MedalLevel );
	CurrentLevel.Level = Config->Visuals->MedalLevel;

	return OriginalFunction( ECX, EDX, PreviousXP, CurrentXP, CurrentLevel );
}

void CHookManager::Init( )
{
	MH_Initialize( );
	std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

	CreateMove::Init( );
	FrameStageNotify::Init( );
	Paint::Init( );
	CL_Move::Init( );
	CL_SendMove::Init( );
	CL_ReadPackets::Init( );
	LockCursor::Init( );
	RunCommand::Init( );
	Interpolate::Init( );
	InterpolateServerEntities::Init( );
	CheckForSequenceChange::Init( );
	AddToCritBucket::Init();
	CalcIsAttackCritical::Init();
	CanFireRandomCriticalShot::Init();
	IsAllowedToWithdrawFromBucket::Init();
	GetUserCmd::Init();
	RandomSeed::Init();
	SurfaceWndProc::Init( );
	LevelInit::Init( );
	ShutDown::Init();
	ClientCmd::Init( );
	ServerCmd::Init( );
	OverrideView::Init();
	DoPostScreenSpaceEffects::Init( );
	DrawModelExecute::Init( );
	ForcedMaterialOverride::Init( );
	AnimStateUpdate::Init( );
	UpdateClientSideAnimation::Init( );
	PaintTraverse::Init( );
	UpdatePlayerAvatar::Init( );
	InCond::Init( );
	RenderView::Init( );
	OnScreenSizeChanged::Init( );
	FireBullets::Init( );
	BaseInterpolatePart1::Init( );
	DatatableWarning::Init( );
	CL_LoadWhitelist::Init( );
	CL_CheckForPureServerWhitelist::Init( );
	GetMaterial::Init( );
	DispatchUserMessage::Init( );
	SendNetMessage::Init( );
	TFGameMovementShit::Init( );
	GetDirectPingToPOP::Init( );
	ViewmodelPos::Init( );
	VarBaseArray_Interpolate::Init( );
	StartDynamicSound::Init( );
	CreateTFRagdoll::Init( );
	NotificationQueueAdd::Init( );
	ShowItemsPickedUp::Init( );
	SetInt::Init( );
	IsPlayerDominated::Init( );
	SetAbsVelocity::Init( );
	CMatchInvite_OnTick::Init( );
	R_LoadSkys::Init( );
	ResetLatched::Init( );
	PlayerMove::Init( );
	GetTracerType::Init( );
	DoEnginePostProcessing::Init( );
	GetPlayerName::Init( );
	ChatPrintf::Init( );
	ConsoleColorPrintf::Init( );
	RenderSpriteCard::Init( );
	DrawModelStaticProp::Init( );
	FireEventClientSide::Init( );
	ProcessPacket::Init( );
	SendDatagram::Init( );
	GetItemDefinition::Init( );
	GetOuterAbsVelocity::Init( );
	UpdateRankControls::Init( );
	
	//SetupBones::Init( );
	//ProcessFixAngle::Init( );
	//SetChoked::Init( );
	//PacketStart::Init( );
	//Plat_FloatTime::Init( );
	//IsUsingStaticPropDebugModes::Init( );
	//AddVar::Init( );
	//SceneEnd::Init( );
	//RenderSpriteTrail::Init( );
	//GetScreenAspectRatio::Init( );
	//WndProc::Init( );
	//GetTime::Init( );
	
	//change addresses if you uncomment this
	//LookupSequence::Init( );
	//AddVCDSequenceToGestureSlot::Init( );

	MH_EnableHook( MH_ALL_HOOKS );
}

/*HOOK_POINTER( AddVCDSequenceToGestureSlot, "client.dll", "55 8B EC 51 56 8B F1 8B 46 ? 89 45 ? 85 C0 0F 84 ? ? ? ? 8B 46 ? 53 8B 5D ? 57 8B FB 03 FF 83 7C F8 ? ? 0F 84 ? ? ? ? 53 E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B",
			  void, __fastcall, void* ECX, void* EDX, int iGestureSlot, int iGestureSequence, float flCycle, bool bAutoKill )
{
	gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "[AddVCDSequenceToGestureSlot] %i, %i, %.2f, %i\n", iGestureSlot, iGestureSequence, flCycle, bAutoKill );
	return OriginalFunction( ECX, EDX, iGestureSlot, iGestureSequence, flCycle, bAutoKill );
}

HOOK_POINTER( LookupSequence, "client.dll", "55 8B EC 56 8B 75 ? 85 F6 75 ? 33 C0 5E 5D C3 8B CE E8 ? ? ? ? 84 C0 74 ? 53 57 8B",
			  int, __cdecl, CStudioHdr* hdr, const char* label )
{
	if ( !G->FakeTaunt->Printing )
	{
		if ( strcmp( label, "idle" ) )
			gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "[LookupSequence] %s\n", label );
	}

	return OriginalFunction( hdr, label );
}*/

void CHookManager::Restore( )
{
	MH_DisableHook( MH_ALL_HOOKS );
	std::this_thread::sleep_for( std::chrono::milliseconds( 50 ) );

	CreateMove::Uninit( );
	FrameStageNotify::Uninit( );
	Paint::Uninit( );
	CL_Move::Uninit( );
	CL_SendMove::Uninit( );
	CL_ReadPackets::Uninit( );
	LockCursor::Uninit( );
	RunCommand::Uninit( );
	Interpolate::Uninit( );
	InterpolateServerEntities::Uninit( );
	CheckForSequenceChange::Uninit( );
	AddToCritBucket::Uninit( );
	CalcIsAttackCritical::Uninit( );
	CanFireRandomCriticalShot::Uninit( );
	IsAllowedToWithdrawFromBucket::Uninit( );
	GetUserCmd::Uninit( );
	RandomSeed::Uninit( );
	SurfaceWndProc::Uninit( );
	LevelInit::Uninit( );
	ShutDown::Uninit( );
	ClientCmd::Uninit( );
	ServerCmd::Uninit( );
	OverrideView::Uninit( );
	DoPostScreenSpaceEffects::Uninit( );
	DrawModelExecute::Uninit( );
	ForcedMaterialOverride::Uninit( );
	AnimStateUpdate::Uninit( );
	UpdateClientSideAnimation::Uninit( );
	PaintTraverse::Uninit( );
	UpdatePlayerAvatar::Uninit( );
	InCond::Uninit( );
	RenderView::Uninit( );
	OnScreenSizeChanged::Uninit( );
	FireBullets::Uninit( );
	BaseInterpolatePart1::Uninit( );
	DatatableWarning::Uninit( );
	CL_LoadWhitelist::Uninit( );
	CL_CheckForPureServerWhitelist::Uninit( );
	GetMaterial::Uninit( );
	DispatchUserMessage::Uninit( );
	SendNetMessage::Uninit( );
	TFGameMovementShit::Uninit( );
	GetDirectPingToPOP::Uninit( );
	ViewmodelPos::Uninit( );
	VarBaseArray_Interpolate::Uninit( );
	StartDynamicSound::Uninit( );
	CreateTFRagdoll::Uninit( );
	NotificationQueueAdd::Uninit( );
	ShowItemsPickedUp::Uninit( );
	SetInt::Uninit( );
	IsPlayerDominated::Uninit( );
	SetAbsVelocity::Uninit( );
	CMatchInvite_OnTick::Uninit( );
	R_LoadSkys::Uninit( );
	ResetLatched::Uninit( );
	PlayerMove::Uninit( );
	GetTracerType::Uninit( );
	DoEnginePostProcessing::Uninit( );
	GetPlayerName::Uninit( );
	ChatPrintf::Uninit( );
	ConsoleColorPrintf::Uninit( );
	RenderSpriteCard::Uninit( );
	DrawModelStaticProp::Uninit( );
	FireEventClientSide::Uninit( );
	ProcessPacket::Uninit( );
	SendDatagram::Uninit( );
	GetItemDefinition::Uninit( );
	GetOuterAbsVelocity::Uninit( );
	UpdateRankControls::Uninit( );

	//SetChoked::Uninit( );
	//PacketStart::Uninit( );
	//ProcessFixAngle::Uninit( );
	//SetupBones::Uninit( );

	//GetPlayerName::Uninit( );
	//ChatPrintf::Uninit( );
	//ConsoleColorPrintf::Uninit( );
	
	MH_Uninitialize( );
}