#include "Framework.h"
#include "SDK/Interfaces/Interfaces.h"
#include "Hooks/HookList.h"
#include "Utils/DrawTools/DrawTools.h"
#include "Features/Menu/Menu.h"
#include "Features/Events/Events.h"
#include "../Globals.h"
#include "Features/Models/Models.h"
#include "Features/Outlines/Outlines.h"
#include "Features/Visuals/Visuals.h"

//dont look at the fact that it looks misaligned here, string formatting fucks it over in the code but it looks proper in the console
void CFramework::PrintOnInject( )
{
	gCvar->ConsoleColorPrintf( { 0, 255, 255, 255 }, 
"-----------------------------------------------------------------------------------------------------------\n\n\
           _               _       __      __ ____  \n\
     /\\   | |             (_)      \\ \\    / /|___ \\ \n\
    /  \\  | |_  ___  _ __  _  ___   \\ \\  / /   __) |\n\
   / /\\ \\ | __|/ _ \\| '__|| |/ __|   \\ \\/ /   |__ < \n\
  / ____ \\| |_|  __/| |   | |\\__ \\    \\  /    ___) |\n\
 /_/    \\_\\\\__|\\___||_|   |_||___/     \\/    |____/ \n\n\
-----------------------------------------------------------------------------------------------------------\n\n\
                     @@@@@@@@@ \n\
                    @@@@@@@@@@@\n\
                   @@@@@@@@@@@@@\n\
                  @@@@@@   @@@@@@\n\
                 @@@@@@     @@@@@@\n\
                @@@@@@       @@@@@@\n\
               @@@@@@         @@@@@@\n\
              @@@@@@           @@@@@@\n\
             @@@@@@             @@@@@@\n\
            @@@@@@     @@@@@@@@@@@@@@@@\n\
           @@@@@@        @@@@@@@@@@@@@@@\n\
          @@@@@@           @@@@@@@@@@@@@@\n\n\
-----------------------------------------------------------------------------------------------------------\n\n" );
}

void CFramework::Attach( )
{
	while ( !GetModuleHandleA( "mss32.dll" ))
		std::this_thread::sleep_for( std::chrono::milliseconds( 175 ) );
	G->DetachSystem->WantsDetach = false;

	//LoadSavedPList( );

	Interfaces->Init( );

	auto SteamPipe = gSteamClient->CreateSteamPipe( );
	auto SteamUser = gSteamClient->ConnectToGlobalUser( SteamPipe );
	gSteamUtils = reinterpret_cast< ISteamUtils007* >( gSteamClient->GetISteamUtils( SteamPipe, "SteamUtils007" ) );
	gSteamFriends = reinterpret_cast< ISteamFriends015* >( gSteamClient->GetISteamFriends( SteamUser, SteamPipe, "SteamFriends015" ) );
	G->Authentication->SteamUser = gSteamClient->GetISteamUser( SteamUser, SteamPipe, "SteamUser017" );

	Menu->Init( );
	Draw->InitFonts( );
	Models->InitMaterials( );
	Outlines->InitMaterials( );
	Visuals->UpdateWorldModulation( );

	std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	G->ConVars->AspectRatio = gCvar->FindVar( "r_aspectratio" );
	G->ConVars->DefaultAspectRatio = G->ConVars->AspectRatio->GetFloat( );

	G->ConVars->HdrLevel = gCvar->FindVar( "mat_hdr_level" );
	G->ConVars->DefaultHdrLevel = G->ConVars->HdrLevel->GetInt( );

	G->ConVars->BloomScale = gCvar->FindVar( "mat_bloom_scalefactor_scalar" );
	G->ConVars->DefaultBloomScale = G->ConVars->BloomScale->GetFloat( );

	G->ConVars->Fullbright = gCvar->FindVar( "mat_fullbright" );

	G->ConVars->WeaponSway = gCvar->FindVar( "cl_wpn_sway_interp" );
	G->ConVars->DefaultWeaponSway = G->ConVars->WeaponSway->GetFloat( );

	G->ConVars->FlipViewmodels = gCvar->FindVar( "cl_flipviewmodels" );
	G->ConVars->DefaultFlipViewmodels = G->ConVars->FlipViewmodels->GetBool( );

	G->ConVars->ViewmodelFOV = gCvar->FindVar( "viewmodel_fov" );
	G->ConVars->DefaultViewmodelFOV = G->ConVars->ViewmodelFOV->GetInt( );

	//G->FakeTaunt->AddVCDSequenceToGestureSlot = Memory->FindSignature<DWORD>( "client.dll", "55 8B EC 51 56 8B F1 8B 46 ? 89 45 ? 85 C0 0F 84 ? ? ? ? 8B 46 ? 53 8B 5D ? 57 8B FB 03 FF 83 7C F8 ? ? 0F 84 ? ? ? ? 53 E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B" );
	//G->FakeTaunt->LookupSequence = Memory->FindSignature<DWORD>( "client.dll", "55 8B EC 56 8B 75 ? 85 F6 75 ? 33 C0 5E 5D C3 8B CE E8 ? ? ? ? 84 C0 74 ? 53 57 8B" );
	G->AnimationFix->UpdateClientSideAnimation = Memory->FindSignature<DWORD>( "client.dll", "55 8B EC 83 EC 0C 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 74 10 8B 17 8B CF 8B 92 ? ? ? ? FF D2" );
	G->PlayerDrawAddress = Memory->FindSignature<DWORD, 5u>( "client.dll", "E8 ? ? ? ? 84 C0 75 C5" );
	G->WearableDrawAddress = Memory->FindSignature<DWORD, 5u>( "client.dll", "E8 ? ? ? ? 84 C0 75 E1 6A 03" );
	G->HudScopeDrawAddress = Memory->FindSignature<DWORD>( "client.dll", "84 C0 74 ? 8B CE E8 ? ? ? ? 85 C0 74 ? 8B CE E8 ? ? ? ? 8B C8 8B 10 FF 92 ? ? ? ? 83 F8 ? 0F 94" );
	G->RandomSeedRet1Address = Memory->FindSignature<DWORD>( "client.dll", "83 C4 ? 0F 57 ? 80 7D FF" );
	G->RandomSeedRet2Address = Memory->FindSignature<DWORD>( "client.dll", "83 C4 ? 68 ? ? ? ? 6A ? FF 15 ? ? ? ? F3 0F ? ? ? 83 C4 ? 0F 28" );
	G->RandomSeedRet3Address = Memory->FindSignature<DWORD>( "client.dll", "83 C4 ? 68 ? ? ? ? 6A ? FF 15 ? ? ? ? F3 0F ? ? ? 83 C4 ? F3 0F" );
	G->RenderLineAddress = Memory->FindSignature<DWORD>( "engine.dll", "55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 85 F6" );
	G->RenderBoxAddress = Memory->FindSignature<DWORD>( "engine.dll", "55 8B EC 51 8B 45 ? 8B C8 FF 75" );
	G->RenderWireframeBoxAddress = Memory->FindSignature<DWORD>( "engine.dll", "55 8B EC 81 EC ? ? ? ? 56 E8 ? ? ? ? 8B 0D ? ? ? ? 8B 01 FF 90 ? ? ? ? 8B F0 89 75 ? 85 F6 74 ? 8B 06 8B CE FF 50 ? A1" );
	G->RenderSphereAddress = Memory->FindSignature<DWORD>( "engine.dll", "53 8B DC 83 EC ? 83 E4 ? 83 C4 ? 55 8B 6B ? 89 6C 24 ? 8B EC 81 EC ? ? ? ? 56 57 E8 ? ? ? ? 8B 0D" );
	G->RenderTriangleAddress = Memory->FindSignature<DWORD>( "engine.dll", "55 8B EC 51 8B 45 ? 8B C8 C1 E9" );
	G->UpdateButtonsAddress = Memory->FindSignature<DWORD>( "client.dll", "55 8B EC 8B 81 ? ? ? ? 8B D0" );
	G->PredictionPlayerAddress = Memory->FindSignature<DWORD, 0x2>( "client.dll", "55 8B EC 8B 81 ? ? ? ? 8B D0" );
	G->ResetInstanceCountersAddress = Memory->FindSignature<DWORD>( "client.dll", "68 ? ? ? ? 6A ? 68 ? ? ? ? C7 05 ? ? ? ? ? ? ? ? E8 ? ? ? ? 83 C4 ? C3" );
	G->HasNewItemsCallAddress = Memory->FindSignature<DWORD>( "client.dll", "83 C4 ? 5E C3 CC CC CC CC CC CC CC 56 8B F1 E8" );
	G->ScoreboardReturnAddressDesired = Memory->FindSignature<DWORD>( "client.dll", "8B 03 8B CB 56 57 FF 90 ? ? ? ? 6A" );
	G->IsPlayerDominatedReturnAddressDesired = Memory->FindSignature<DWORD>( "client.dll", "84 C0 74 ? 80 7D ? ? 74 ? 8B 83" );
	G->ScoreboardReturnAddressJump = Memory->FindSignature<DWORD>( "client.dll", "89 45 BC E8 ? ? ? ? 3B C7 75 1D 80 7D F8 00 75 17 8B 4D C0" );
	G->SetAbsVelocityCallAddress = Memory->FindSignature<DWORD>( "client.dll", "E8 ? ? ? ? 8D 47 ? 39 05" ) - 0x3;
	G->ParticleTracerAddress = Memory->FindSignature<DWORD>( "client.dll", "55 8B EC FF 75 08 E8 ? ? ? ? D9 EE 83" );
	//G->AnimationFix->AnimStateUpdate = Memory->FindSignature<DWORD>( "client.dll", "55 8B EC 81 EC ? ? ? ? 53 57 8B F9 8B 9F ? ? ? ? 89 5D E0 85 DB 0F 84" );
	G->Plat_FloatTimeAddress = reinterpret_cast< DWORD >( GetProcAddress( GetModuleHandleA( "tier0.dll" ), "Plat_FloatTime" ) );

	//Sleep( 2000 );

	gEngineClient->ClientCmd_Unrestricted( "clear" );

	//ragdoll fix
	gEngineClient->ClientCmd_Unrestricted( "cl_ragdoll_fade_time 15; cl_ragdoll_forcefade 0; cl_ragdoll_physics_enable 1; g_ragdoll_fadespeed 600; g_ragdoll_lvfadespeed 100; ragdoll_sleepaftertime 5.0f" );
	HookManager->Init( );
	//std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	//NetVarHooks->Init( );
	Events->Setup( { "player_death", "player_hurt", "teamplay_round_start", "game_newmap", "teamplay_round_active", "teamplay_restart_round" } );
	PrintOnInject( );
	gSurface->PlaySound( "hl1/fvox/activated.wav" );
}

void CFramework::Detach( )
{
	G->DetachSystem->WantsDetach = true;

	if ( G->ConVars->AspectRatio )
		G->ConVars->AspectRatio->SetValue( G->ConVars->DefaultAspectRatio );

	if ( G->ConVars->HdrLevel )
		G->ConVars->HdrLevel->SetValue( G->ConVars->DefaultHdrLevel );

	if ( G->ConVars->BloomScale )
		G->ConVars->BloomScale->SetValue( G->ConVars->DefaultBloomScale );

	if ( G->ConVars->Fullbright )
		G->ConVars->Fullbright->SetValue( 0 );

	if ( G->ConVars->WeaponSway )
		G->ConVars->WeaponSway->SetValue( G->ConVars->DefaultWeaponSway );

	if ( G->ConVars->FlipViewmodels )
		G->ConVars->FlipViewmodels->SetValue( G->ConVars->DefaultFlipViewmodels );

	if ( G->ConVars->ViewmodelFOV )
		G->ConVars->ViewmodelFOV->SetValue( G->ConVars->DefaultViewmodelFOV );

	//std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

	Visuals->ModulateTexture( { 255, 255, 255, 255 }, "World textures" );
	Visuals->ModulateTexture( { 255, 255, 255, 255 }, "SkyBox textures" );
	Visuals->IsWorldModulated = false;

	//std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );
	HookManager->Restore( );
	//std::this_thread::sleep_for( std::chrono::milliseconds( 200 ) );

	Menu->Reset( );
	//std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

	Models->ResetMaterials( );
	Outlines->ResetMaterials( );
	//std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );

	//NetVarHooks->Restore( );
	Events->Destroy( );
	//std::this_thread::sleep_for( std::chrono::milliseconds( 100 ) );
	gSurface->PlaySound( "hl1/fvox/deactivated.wav" );
}

std::vector<StackFrame> CFramework::GetStackTrace( PCONTEXT context ) {
	HANDLE process = GetCurrentProcess( );
	HANDLE thread = GetCurrentThread( );

	if ( SymInitialize( process, nullptr, TRUE ) == FALSE )
		return {};

	SymSetOptions( SYMOPT_LOAD_LINES );

	STACKFRAME frame = {};
	frame.AddrPC.Offset = context->Eip;
	frame.AddrPC.Mode = AddrModeFlat;
	frame.AddrFrame.Offset = context->Ebp;
	frame.AddrFrame.Mode = AddrModeFlat;
	frame.AddrStack.Offset = context->Esp;
	frame.AddrStack.Mode = AddrModeFlat;

	bool first = true;

	std::vector<StackFrame> frames;
	while ( StackWalk( IMAGE_FILE_MACHINE_I386, process, thread, &frame, context, nullptr, SymFunctionTableAccess, SymGetModuleBase, nullptr ) ) {
		StackFrame f = {};
		f.address = frame.AddrPC.Offset;

		auto moduleBase = SymGetModuleBase( process, frame.AddrPC.Offset );

		char moduleBuf[ MAX_PATH ];
		if ( moduleBase && GetModuleFileNameA( ( HINSTANCE )moduleBase, moduleBuf, MAX_PATH ) )
			f.module = moduleBuf;
		else
			f.module = "Unknown Module";

		DWORD offset = 0;
		char symbolBuffer[ sizeof( IMAGEHLP_SYMBOL ) + 255 ];
		auto symbol = ( PIMAGEHLP_SYMBOL )symbolBuffer;
		symbol->SizeOfStruct = sizeof( IMAGEHLP_SYMBOL ) + 255;
		symbol->MaxNameLength = 254;

		if ( SymGetSymFromAddr( process, frame.AddrPC.Offset, &offset, symbol ) ) {
			f.name = symbol->Name;
		}
		else {
			f.name = "Unknown Function";
		}

		IMAGEHLP_LINE line{};
		line.SizeOfStruct = sizeof( IMAGEHLP_LINE );

		DWORD offset_ln = 0;
		if ( SymGetLineFromAddr( process, frame.AddrPC.Offset, &offset_ln, &line ) ) {
			f.file = line.FileName;
			f.line = line.LineNumber;
		}
		else {
			f.line = 0;
		}

		if ( !first ) {
			frames.push_back( f );
		}
		first = false;
	}

	SymCleanup( process );

	return frames;
}