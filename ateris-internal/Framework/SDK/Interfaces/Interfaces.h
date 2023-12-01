#pragma once
#include "../TF2/Cvar.h"
#include "../TF2/ClientMode.h"
#include "../TF2/GlobalVars.h"
#include "../TF2/EngineVGui.h"
#include "../TF2/EngineClient.h"
#include "../TF2/EntityList.h"
#include "../TF2/Prediction.h"
#include "../TF2/GameMovement.h"
#include "../TF2/Input.h"
#include "../TF2/Surface.h"
#include "../TF2/RenderView.h"
#include "../TF2/ModelInfo.h"
#include "../TF2/Physics.h"
#include "../TF2/GameEvents.h"
#include "../TF2/MaterialSystem.h"
#include "../TF2/DebugOverlay.h"
#include "../TF2/Beams.h"
#include "../TF2/UniformRandomStream.h"
#include "../TF2/MemAlloc.h"
#include "../TF2/StudioRender.h"
#include "../TF2/CTFGCClient.h"
#include "../TF2/GameRules.h"
#include "../Steam/SteamClient.h"

inline IClientModeShared* gClientMode = nullptr;
inline IBaseClientDLL* gClient = nullptr;
inline ICvar* gCvar = nullptr;
inline CBaseClientState* gClientState = nullptr;
inline CGlobalVarsBase* gGlobals = nullptr;
inline IEngineVGui* gEngineVGui = nullptr;
inline CEngineClient* gEngineClient = nullptr;
inline IEngineTrace* gEngineTrace = nullptr;
inline IClientEntityList* gEntityList = nullptr;
inline IMoveHelper* gMoveHelper = nullptr;
inline CPrediction* gPrediction = nullptr;
inline CGameMovement* gGameMovement = nullptr;
inline IInput* gInput = nullptr;
inline IInputSystem* gInputSystem = nullptr;
inline IMatSystemSurface* gSurface = nullptr;
inline IVRenderView* gRenderView = nullptr;
inline IVModelInfo* gModelInfo = nullptr;
inline IPhysics* gPhysics = nullptr;
inline IPhysicsCollision* gPhysicsCollision = nullptr;
inline int* gRandomSeed = nullptr;
inline IGameEventManager* gEventManager = nullptr;
inline IMaterialSystem* gMaterialSystem = nullptr;
inline IVModelRender* gModelRender = nullptr;
inline IVDebugOverlay* gDebugOverlay = nullptr;
inline CPanel* gPanel = nullptr;
inline IViewRender* gViewRender = nullptr;
inline IViewRenderBeams* gBeams = nullptr;
inline IUniformRandomStream* gRandomStream = nullptr;
inline ISteamClient* gSteamClient = nullptr;
inline CGameMovement* gTFGameMovement = nullptr;
inline ISteamNetworkingUtils* gSteamNetwork = nullptr;
inline CTFGCClientSystem* gTFGCClientSystem = nullptr;
inline CTFPartyClient* gPartyClient = nullptr;
inline IMemAlloc* gMemAlloc = nullptr;
inline ISteamFriends015* gSteamFriends = nullptr;
inline ISteamUtils007* gSteamUtils = nullptr;
inline IStudioRender* gStudioRender = nullptr;
inline CTFGameRules* gGameRules = nullptr;

class CInterfaces
{
public:
	inline void Init( )
	{
		//interfaces
		gClient = Memory->FindInterface<IBaseClientDLL*>("client.dll", "VClient017");
		gEntityList = Memory->FindInterface<IClientEntityList*>("client.dll", "VClientEntityList003");
		gCvar = Memory->FindInterface<ICvar*>("vstdlib.dll", "VEngineCvar004");
		gEngineVGui = Memory->FindInterface<IEngineVGui*>("engine.dll", "VEngineVGui002");
		gEngineClient = Memory->FindInterface<CEngineClient*>("engine.dll", "VEngineClient013");
		gEngineTrace = Memory->FindInterface<IEngineTrace*>("engine.dll", "EngineTraceClient003");
		gPrediction = Memory->FindInterface<CPrediction*>("client.dll", "VClientPrediction001");
		gGameMovement = Memory->FindInterface<CGameMovement*>("client.dll", "GameMovement001");
		gSurface = Memory->FindInterface<IMatSystemSurface*>("vguimatsurface.dll", "VGUI_Surface030");
		gRenderView = Memory->FindInterface<IVRenderView*>("engine.dll", "VEngineRenderView014");
		gInputSystem = Memory->FindInterface<IInputSystem*>( "inputsystem.dll", "InputSystemVersion001" );
		gModelInfo = Memory->FindInterface<IVModelInfo*>( "engine.dll", "VModelInfoClient006" );
		gPhysics = Memory->FindInterface<IPhysics*>( "vphysics.dll", "VPhysics031" );
		gPhysicsCollision = Memory->FindInterface<IPhysicsCollision*>( "vphysics.dll", "VPhysicsCollision007" );
		gEventManager = Memory->FindInterface<IGameEventManager*>( "engine.dll", "GAMEEVENTSMANAGER002" );
		gMaterialSystem = Memory->FindInterface< IMaterialSystem*>( "materialsystem.dll", "VMaterialSystem081" );
		gModelRender = Memory->FindInterface< IVModelRender*>( "engine.dll", "VEngineModel016" );
		gDebugOverlay = Memory->FindInterface< IVDebugOverlay*>( "engine.dll", "VDebugOverlay003" );
		gPanel = Memory->FindInterface< CPanel*>( "vgui2.dll", "VGUI_Panel009" );
		gStudioRender = Memory->FindInterface< IStudioRender*>( "studiorender.dll", "VStudioRender025" );
		gSteamClient = Memory->FindInterface< ISteamClient* >( "steamclient.dll", "SteamClient017" );

		//offsets
		gClientMode = **Memory->FindSignature< IClientModeShared***, 2u>("client.dll", "8B 0D ? ? ? ? 83 EC ? 8B 01 FF 75 ? FF 50 ? 68");
		gClientState = *Memory->FindSignature<CBaseClientState**, 1u>("engine.dll", "68 ? ? ? ? E8 ? ? ? ? 83 C4 08 5F 5E 5B 5D C3");
		gGlobals = *Memory->FindSignature< CGlobalVarsBase**, 8u>("engine.dll", "A1 ? ? ? ? 8B 11 68");
		gMoveHelper = **Memory->FindSignature< IMoveHelper***, 2u>("client.dll", "8B 0D ? ? ? ? 8B 01 FF 50 28 56 8B C8");
		gInput = **Memory->FindSignature< IInput***, 2u>("client.dll", "8B 0D ? ? ? ? 56 8B 01 FF 50 24 8B 45 FC");
		gRandomSeed = *Memory->FindSignature<int**, 2u>("client.dll", "C7 05 ? ? ? ? ? ? ? ? 5D C3 8B 40 34");
		gViewRender = **Memory->FindSignature< IViewRender***, 2u>( "client.dll", "8B 0D ? ? ? ? 8B 01 FF 50 ? 8B 06 8D 4D ? 51 8B CE" );
		gRandomStream = *Memory->FindSignature< IUniformRandomStream**, 1u>( "vstdlib.dll", "B9 ? ? ? ? 85 C0 0F 45 C8 89 0D ? ? ? ? 5D C3" );
		gBeams = **Memory->FindSignature< IViewRenderBeams***, 2u>( "client.dll", "8B 0D ? ? ? ? 56 8B 01 FF 50 18 0F B7 96 ? ? ? ?" );
		gTFGCClientSystem = *Memory->FindSignature < CTFGCClientSystem**, 1u > ( "client.dll", "B9 ? ? ? ? 50 E8 ? ? ? ? 8B 5D" );
		gMemAlloc = **Memory->FindSignature< IMemAlloc***, 2u>( "engine.dll", "8B 0D ? ? ? ? 83 C4 ? 8B 09" );
		gPartyClient = Memory->FindSignature< CTFPartyClient * ( __cdecl* )( )>(
			"client.dll", "A1 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC 55 8B EC 83 EC ? 53" )( );
		// jod bless fedora pasters
		
		//should be a pointer to g_pGameRules
		gGameRules = **Memory->FindSignature<CTFGameRules***, 2u>( "client.dll", "8B 0D ? ? ? ? 56 57 8B 01 8B 40 ? FF D0 84 C0" );

		gTFGameMovement = nullptr;
		gSteamNetwork = reinterpret_cast< ISteamNetworkingUtils * ( __cdecl* )( ) >( GetProcAddress( GetModuleHandleA( "steamnetworkingsockets.dll" ), "SteamNetworkingUtils_LibV4" ) )( );
	}
};

inline CInterfaces* Interfaces = new CInterfaces;