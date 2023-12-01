#include "AnimFix.h"
#include "../../../Globals.h"
#include "../Anti-Aimbot/Anti-Aimbot.h"

//the things the game really needs to setup a bonematrix are: player origin, model rotation, pose params, cycle, sequence and animlayers
//the problem with allocating a separate animation state and updating that without unlinking the client side animation list is that
//they will both write to the same pose parameters. the client will update every frame as well.
//this means they're constantly fighting over what poses get used for bone set ups
//should we back those values and restore them ? (pose params, cycle, sequence and animlayers)
//running it in fsn fixes the old velocity issue on high ping (at least on net_fakelag 200 it does), either that or changing frametime fucks it

//split this for convenience

void CAnimFix::Update( )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) )
	{
		return;
	}

	auto AnimState = gLocalPlayer->AnimState( );
	if ( !AnimState || gLocalPlayer->IsTaunting( ) || G->FakeTaunt->FakeTauntTime > gEngineClient->Time( ) || G->Doubletap.Recharge )
	{
		return;
	}

	/*if ( !Config->Misc->AntiAim->Enabled || !AntiAim->IsToggled || G->Doubletap.Recharge || G->Doubletap.QueueRecharge )
	{
		return;
	}*/

	if ( gClientState->chokedcommands )
	{
		TicksElapsed = gClientState->chokedcommands;
		return;
	}

	static int OldTickCount = gGlobals->tickcount;
	if ( OldTickCount == gGlobals->tickcount )
	{
		return;
	}

	if ( OldTickCount != gGlobals->tickcount )
	{
		OldTickCount = gGlobals->tickcount;
	}

	if ( gLocalPlayer->IsTaunting( ) || G->FakeTaunt->FakeTauntTime > gEngineClient->Time( ) )
	{
		return;
	}

	if ( gLocalPlayer->SimulationTime( ) < gLocalPlayer->OldSimulationTime( ) )
	{
		G->AnimationFix->WarpTimeStamps[ gLocalPlayer->GetIndex( ) ] = gEngineClient->Time( ) + TICKS_TO_TIME( 4 );
	}

	if ( G->AnimationFix->WarpTimeStamps[ gLocalPlayer->GetIndex( ) ] > gEngineClient->Time( ) )
	{
		return;
	}

	float OldFrameTime = gGlobals->frametime;
	int OldSequence = gLocalPlayer->Sequence( );
	float OldCycle = gLocalPlayer->Cycle( );
	auto OldPoseParams = gLocalPlayer->PoseParameter( );
	char pOldAnimState[ sizeof( CTFPlayerAnimState ) ] = {};

	memcpy( pOldAnimState, AnimState, sizeof( CTFPlayerAnimState ) );

	auto Restore = [ & ] ( )
	{
		gGlobals->frametime = OldFrameTime;
		gLocalPlayer->Sequence( ) = OldSequence;
		gLocalPlayer->Cycle( ) = OldCycle;
		gLocalPlayer->PoseParameter( ) = OldPoseParams;
		memcpy( AnimState, pOldAnimState, sizeof( CTFPlayerAnimState ) );
	};

	if ( Config->Misc->AntiAim->Enabled && AntiAim->IsToggled && !G->Doubletap.Recharge && !G->Doubletap.QueueRecharge && !G->FakeLag->FakeDucking )
	{
		//gGlobals->frametime = gGlobals->interval_per_tick * TicksElapsed;
		AnimState->m_flCurrentFeetYaw = G->AnimationFix->CurrentAngles.y;
		//AnimState->m_flGoalFeetYaw = G->AnimationFix->CurrentAngles.y;
		AnimState->m_flEyeYaw = G->AnimationFix->CurrentAngles.y;

		G->AnimationFix->UpdateAnims = true;
		AnimState->Update( G->AnimationFix->CurrentAngles.y, Config->Misc->AntiAim->FakePitch && Config->Misc->AntiAim->Pitch == 2 ? -G->AnimationFix->CurrentAngles.x : G->AnimationFix->CurrentAngles.x );

		if ( !gLocalPlayer->SetupBones( G->AnimationFix->FakeMatrix, 128, BONE_USED_BY_ANYTHING, gGlobals->curtime ) )
		{
			G->AnimationFix->FakeMatrixBuilt = false;
			Restore( );
			return;
		}

		G->AnimationFix->FakeMatrixBuilt = true;
		Restore( );

		G->AnimationFix->CurrentAngles = G->AntiAim->RealAngles;
	}

	AnimState->m_flCurrentFeetYaw = G->AnimationFix->CurrentAngles.y;
	//AnimState->m_flGoalFeetYaw = G->AnimationFix->CurrentAngles.y;
	AnimState->m_flEyeYaw = G->AnimationFix->CurrentAngles.y;

	G->AnimationFix->UpdateAnims = true;
	gLocalPlayer->UpdateClientSideAnimation( );

	gGlobals->frametime = gPrediction->EnginePaused ? 0.0f : ( gGlobals->interval_per_tick * TicksElapsed );

	G->AnimationFix->UpdateAnims = true;
	AnimState->Update( G->AnimationFix->CurrentAngles.y, G->AnimationFix->CurrentAngles.x );

	gGlobals->frametime = OldFrameTime;

	TicksElapsed = 1;
}

/*void CAnimFix::UpdateReal( )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) )
	{
		return;
	}

	auto AnimState = gLocalPlayer->AnimState( );
	if ( !AnimState )
	{
		return;
	}

	static int OldTickCount = gGlobals->tickcount;
	if ( OldTickCount == gGlobals->tickcount )
	{
		return;
	}

	if ( OldTickCount != gGlobals->tickcount )
	{
		OldTickCount = gGlobals->tickcount;
	}

	if ( gClientState->chokedcommands )
	{
		TicksElapsed = gClientState->chokedcommands;
		return;
	}

	float OldFrameTime = gGlobals->frametime;
	if ( !gLocalPlayer->IsTaunting( ) && G->FakeTaunt->FakeTauntTime < gEngineClient->Time( ) && TicksElapsed )
	{
		gGlobals->frametime = gGlobals->interval_per_tick * TicksElapsed;
	}
	else
	{
		gGlobals->frametime = 0.0f;
	}
	
	Vector OldAngles = G->AnimationFix->CurrentAngles;

	if ( Config->Misc->AntiAim->Enabled )
	{
		G->AnimationFix->CurrentAngles = G->AntiAim->RealAngles;
	}

	if ( G->FakeTaunt->FakeTauntTime > gEngineClient->Time( ) )
	{
		G->AnimationFix->CurrentAngles = G->FakeTaunt->FakeTauntViewAngles;
	}

	AnimState->m_flCurrentFeetYaw = G->AnimationFix->CurrentAngles.y;
	//AnimState->m_flGoalFeetYaw = G->AnimationFix->CurrentAngles.y;
	AnimState->m_flEyeYaw = G->AnimationFix->CurrentAngles.y;

	G->AnimationFix->UpdateAnims = true;
	gLocalPlayer->UpdateClientSideAnimations( );

	G->AnimationFix->UpdateAnims = true;
	AnimState->Update( G->AnimationFix->CurrentAngles.y, G->AnimationFix->CurrentAngles.x );

	//if ( !gLocalPlayer->IsTaunting( ) && G->FakeTaunt->FakeTauntTime < gEngineClient->Time( ) )
	//{
		//gGlobals->frametime = OldFrameTime;
	//}
	gGlobals->frametime = OldFrameTime;
	G->AnimationFix->CurrentAngles = OldAngles;
}*/