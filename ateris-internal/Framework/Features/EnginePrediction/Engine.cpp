#include "Engine.h"
#include "../../SDK/Math/MD5.h"
#include "../../../Globals.h"

// if u want to argue
// https://github.com/sr2echa/TF2-Source-Code/blob/master/tf2_src/game/server/player_command.cpp#L316
void CEnginePrediction::Predict( CUserCmd* pCmd )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) )
		return;

	static auto UpdateButtons = reinterpret_cast< void( __thiscall* )( CBasePlayer*, int ) >( G->UpdateButtonsAddress );
	static auto Player = reinterpret_cast< CBasePlayer* >( G->PredictionPlayerAddress );
	static auto ResetInstanceCounters = reinterpret_cast< void( __cdecl* )( ) >( G->ResetInstanceCountersAddress );

	gLocalPlayer->CurrentCommand( ) = pCmd;
	Player = gLocalPlayer;
	*gRandomSeed = MD5_PseudoRandom( pCmd->command_number ) & std::numeric_limits<int>::max( );
	ResetInstanceCounters( );

	OldCurtime = gGlobals->curtime;
	OldFrameTime = gGlobals->frametime;
	OldTickcount = gGlobals->tickcount;

	//int OldTickbase = gLocalPlayer->TickBase( );
	OldInPrediction = gPrediction->bInPrediction;
	OldFirstPrediction = gPrediction->FirstTimePredicted;

	gGlobals->curtime = TICKS_TO_TIME( GetTickBase( pCmd ) );
	gGlobals->frametime = gPrediction->EnginePaused ? 0.f : gGlobals->interval_per_tick;
	gGlobals->tickcount = GetTickBase( pCmd );

	gPrediction->bInPrediction = true;
	gPrediction->FirstTimePredicted = false;

	if ( !G->Doubletap.ToSend )
	{
		gGameMovement->StartTrackPredictionErrors( gLocalPlayer );

		if ( pCmd->weaponselect != 0 )
			if ( const auto& Weapon = gEntityList->GetClientEntity( pCmd->weaponselect )->As<CBaseCombatWeapon>( ) )
				gLocalPlayer->SelectItem( Weapon->GetName( ), pCmd->weaponsubtype );

		UpdateButtons( gLocalPlayer, pCmd->buttons );
		gPrediction->SetLocalViewAngles( pCmd->viewangles );
		//gMoveHelper->SetHost( gLocalPlayer );

		int ThinkTick = gLocalPlayer->NextThinkTick( );
		if ( gLocalPlayer->PhysicsRunTick( 0 ) )
			gLocalPlayer->PreThink( );

		if ( ThinkTick > 0 && ThinkTick <= gGlobals->tickcount )
		{
			gLocalPlayer->SetNextThink( -1, 0 );
			gLocalPlayer->Think( );
		}

		gPrediction->SetupMove( gLocalPlayer, pCmd, gMoveHelper, &MoveData );
		gGameMovement->ProcessMovement( gLocalPlayer, &MoveData );
		gPrediction->FinishMove( gLocalPlayer, pCmd, &MoveData );

		gLocalPlayer->PostThink( );
	}

	//gMoveHelper->ProcessImpacts( );

	//gLocalPlayer->TickBase( ) = OldTickbase;

	gPrediction->bInPrediction = OldInPrediction;
	gPrediction->FirstTimePredicted = OldFirstPrediction;
}

void CEnginePrediction::End(CUserCmd* pCmd)
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) )
		return;

	gGameMovement->FinishTrackPredictionErrors( gLocalPlayer );

	gGlobals->curtime = OldCurtime;
	gGlobals->frametime = OldFrameTime;
	gGlobals->tickcount = OldTickcount;

	gPrediction->bInPrediction = OldInPrediction;
	gPrediction->FirstTimePredicted = OldFirstPrediction;

	static auto Player = reinterpret_cast<CBasePlayer*>( G->PredictionPlayerAddress );
	gLocalPlayer->CurrentCommand() = nullptr;
	Player = nullptr;
	*gRandomSeed = -1;
	//gMoveHelper->SetHost( nullptr );
}

int CEnginePrediction::GetTickBase(CUserCmd* cmd) {
	static int tick = 0;
	static CUserCmd* LastCmd = nullptr;

	if (cmd) {
		if (!LastCmd || LastCmd->hasbeenpredicted)
			tick = gLocalPlayer->TickBase();

		else tick++;
		LastCmd = cmd;
	}

	return tick;
}