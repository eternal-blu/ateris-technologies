#pragma once
#include "Hooks.h"
#include "../SDK/Interfaces/Interfaces.h"
#include "../SDK/TF2/CEconNotification.h"

HOOK_HEADER( CreateMove, bool( __fastcall* )( void*, void*, float, CUserCmd* ), bool, __fastcall,
			 void* ECX, void* EDX, float InputSampleTime, CUserCmd* Cmd );
HOOK_HEADER( Paint, void( __fastcall* )( void*, void*, PaintMode_t ), void, __fastcall,
			 void* ECX, void* EDX, PaintMode_t Mode );
HOOK_HEADER( FrameStageNotify, void( __fastcall* )( void*, void*, ClientFrameStage_t ), void, __fastcall,
			 void* ECX, void* EDX, ClientFrameStage_t Stage );
HOOK_HEADER( CL_Move, void( __cdecl* )( float, bool ), void, __cdecl, float AccumulatedExtraSamples, bool FinalTick );
HOOK_HEADER( CL_SendMove, void( __fastcall* )( void*, void* ), void, __fastcall,
			 void* ECX, void* EDX );
HOOK_HEADER( CL_ReadPackets, void( __cdecl* )( bool ), void, __cdecl, bool FinalTick );
HOOK_HEADER( RunCommand, void( __fastcall* )( void*, void*, CBasePlayer*, CUserCmd*, IMoveHelper* ), void, __fastcall,
			 void* ECX, void* EDX, CBasePlayer* Player, CUserCmd* Cmd, IMoveHelper* MoveHelper );
HOOK_HEADER( Interpolate, bool( __fastcall* )( CBaseEntity*, void*, float ), bool, __fastcall, CBaseEntity* ECX, void* EDX, float CurTime );
HOOK_HEADER( InterpolateServerEntities, void( __cdecl* )( void ), void, __cdecl );
HOOK_HEADER( CheckForSequenceChange, void( __fastcall* )( void*, void*, CStudioHdr*, int, bool, bool ), void, __fastcall,
			 void* ECX, void* EDX, CStudioHdr* hdr, int nCurSequence, bool bForceNewSequence, bool bInterpolate );
HOOK_HEADER( AddVar, void( __fastcall* )( void*, void*, void*, IInterpolatedVar*, int, bool ), void, __fastcall,
			 void* ECX, void* EDX, void* data, IInterpolatedVar* watcher, int type, bool bSetup );
HOOK_HEADER( BaseInterpolatePart1, int( __fastcall* )( void*, void*, float&, Vector&, Vector&, Vector&, int& ), int, __fastcall,
			 void* ECX, void* EDX, float& currentTime, Vector& oldOrigin, Vector& oldAngles, Vector& oldVel, int& bNoMoreChanges );
//HOOK_HEADER( WndProc, LRESULT( __stdcall* )( HWND, UINT, WPARAM, LPARAM ); inline HWND GameWindow; inline WNDPROC OriginalWndProc , LRESULT, CALLBACK, HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
HOOK_HEADER( LockCursor, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( AddToCritBucket, void( __fastcall* )( void*, void*, float ), void, __fastcall, void* ECX, void* EDX, float Damage );
HOOK_HEADER( CalcIsAttackCritical, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( CanFireRandomCriticalShot, bool( __fastcall* )( void*, void*, float ), bool, __fastcall, void* ECX, void* EDX, float Chance );
HOOK_HEADER( IsAllowedToWithdrawFromBucket, bool( __fastcall* )( void*, void*, float ), bool, __fastcall, void* ECX, void* EDX, float Damage );
//HOOK_HEADER( RandomSeed, void( __cdecl* )( int ), void, __cdecl, int );
namespace RandomSeed
{
	using FN = void( __cdecl* )( int );
	inline PVOID Target;
	__declspec( dllexport ) inline FN OriginalFunction;
	__declspec( dllexport ) void __cdecl Function( int );
	void Init( );
	void Uninit( );
}
HOOK_HEADER( GetUserCmd, CUserCmd* (__thiscall*)(void*, int), CUserCmd*, __stdcall, int );
HOOK_HEADER( GetTime, float( __fastcall* )( void*, void* ), float, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( LevelInit, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( ShutDown, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( ClientCmd, void( __fastcall* )( void*, void*, const char* ), void, __fastcall, void* ECX, void* EDX, const char* cmd );
HOOK_HEADER( ServerCmd, void( __fastcall* )( void*, void*, const char*, bool ), void, __fastcall, void* ECX, void* EDX, const char* cmd, bool reliable );
HOOK_HEADER( AnimStateUpdate, void( __fastcall* )( CTFPlayerAnimState*, void*, float, float ), void, __fastcall, CTFPlayerAnimState* ECX, void* EDX, float yaw, float pitch );
HOOK_HEADER( UpdateClientSideAnimation, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( OverrideView, void( __fastcall* )( void*, void*, CViewSetup* ), void, __fastcall, void* ECX, void* EDX, CViewSetup* ViewSetup );
HOOK_HEADER( ViewEffectsFade, void( __fastcall* )( void*, void*, void* ), void, __fastcall, void* ecx, void* edx, void* idfk )
HOOK_HEADER( ViewEffectsShake, void( __fastcall* )( void*, void*, void* ), void, __fastcall, void* ecx, void* edx, void* idfk )
HOOK_HEADER( FireEventClientSide, bool( __fastcall* )( void*, void*, IGameEvent* ), bool, __fastcall, void* ECX, void* EDX, IGameEvent* Event )
HOOK_HEADER( DoPostScreenSpaceEffects, bool( __fastcall* )( void*, void*, CViewSetup* ), bool, __fastcall, void* ECX, void* EDX, CViewSetup* View );
HOOK_HEADER( DrawModelExecute, void( __fastcall* )( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4* ),
			 void, __fastcall, void* ECX, void* EDX, const DrawModelState_t& State, const ModelRenderInfo_t& Info, Matrix3x4* Matrix );
HOOK_HEADER( ForcedMaterialOverride, void( __fastcall* )( void*, void*, IMaterial*, int ), void, __fastcall,
			 void* ECX, void* EDX, IMaterial* Material, int OverrideType );
HOOK_HEADER( PaintTraverse, void( __fastcall* )( void*, void*, unsigned int, bool, bool ), void, __fastcall,
			 void* ECX, void* EDX, unsigned int VGuiPanel, bool ForceRepaint, bool AllowForce );
HOOK_HEADER( UpdatePlayerAvatar, void( __fastcall* )( void*, void*, int, KeyValues* ), void, __fastcall, void* ECX, void* EDX, int index, KeyValues* kv );
HOOK_HEADER( InCond, bool( __fastcall* )( void*, void*, ETFCond ), bool, __fastcall, void* ECX, void* EDX, ETFCond Cond );
HOOK_HEADER( SurfaceWndProc, LRESULT( CALLBACK* )( HWND, UINT, WPARAM, LPARAM ), LRESULT, CALLBACK, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam );
HOOK_HEADER( RenderView, void( __fastcall* )( void*, void*, const CViewSetup&, int, int ), void, __fastcall, void* ECX, void* EDX,
			 const CViewSetup& View, int ClearFlags, int WhatToDraw );
HOOK_HEADER( SceneEnd, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );

HOOK_HEADER( RenderSpriteCard, void( __fastcall* )( void*, void*, void*, void*, SpriteRenderInfo_t&, int, void*, Vector* ), void, __fastcall, void* ECX, void* EDX,
			 void* meshbuilder, void* ctx, SpriteRenderInfo_t& info, int particle, void* sortlist, Vector* camera );
HOOK_HEADER( RenderSpriteTrail, void( __fastcall* )( void*, void*, void*, SpriteRenderInfo_t&, int, const Vector&, float, void* ), void, __fastcall, void* ECX, void* EDX,
			 void* meshbuilder, SpriteRenderInfo_t& info, int particle, const Vector& CameraPos, float flOODt, ParticleRenderData_t* const sortlist );
HOOK_HEADER( OnScreenSizeChanged, void( __fastcall* )( void*, void*, int, int ), void, __fastcall, void* ECX, void* EDX, int oldwidth, int oldheight );
HOOK_HEADER( GetScreenAspectRatio, float( __fastcall* )( void*, void* ), float, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( FireBullets, void( __fastcall* )( void*, void*, CBaseCombatWeapon*, const FireBulletsInfo_t&, bool, int, int ), void, __fastcall,
			 void* ECX, void* EDX, CBaseCombatWeapon* weapon, const FireBulletsInfo_t& info, bool doeffects, int damagetype, int customdamagetype );
HOOK_HEADER( IsUsingStaticPropDebugModes, bool( __cdecl* )( ), bool, __cdecl );
HOOK_HEADER( DatatableWarning, void( __cdecl* )( const char*, ... ), void, __cdecl, const char* msg, ... );
HOOK_HEADER( VarBaseArray_Interpolate, int( __fastcall* )(void*, void*, float, float ), int, __fastcall, void* ECX, void* EDX, float curret, float interp );
/*HOOK_HEADER( AddVCDSequenceToGestureSlot, void( __fastcall* )( void*, void*, int, int, float, bool ), void, __fastcall,
			 void* ECX, void* EDX, int iGestureSlot, int iGestureSequence, float flCycle, bool bAutoKill );
HOOK_HEADER( LookupSequence, int( __cdecl* )( CStudioHdr*, const char* ), int, __cdecl, CStudioHdr* hdr, const char* label );*/
HOOK_HEADER( ProcessPacket, void( __fastcall* )( void*, void*, void*, bool ), void, __fastcall, void* ECX, void* EDX, void* packet, bool header );
HOOK_HEADER( SendDatagram, int( __fastcall* )( void*, void*, bf_write* ), int, __fastcall, void* ECX, void* EDX, bf_write* datagram );
HOOK_HEADER( ProcessTempEntities, bool( __fastcall* )( void*, void*, void* ), bool, __fastcall, void* ECX, void* EDX, void* msg );
HOOK_HEADER( CL_LoadWhitelist, void*( __cdecl* )( void*, const char* ), void*, __cdecl, void* table, const char* name );
HOOK_HEADER( CL_CheckForPureServerWhitelist, void( __cdecl* )( void** ), void, __cdecl, void** FilesToReload );
//55 8B EC 56 8B 75 ? 57 8B F9 83 FE ? 74 ? 83 FE ? 74 ? 8B 0F 85 C9 74 ? 8B 01 FF 75 ? FF 50
HOOK_HEADER( GetMaterial, IMaterial* ( __fastcall* )( void*, void*, void*, int ), IMaterial*, __fastcall, void* ECX, void* EDX, void* RenderMode, int Frame );
HOOK_HEADER( DispatchUserMessage, bool( __fastcall* )( void*, void*, int, bf_read& ), bool, __fastcall, void* ECX, void* EDX, int type, bf_read& data );
HOOK_HEADER( SendNetMessage, bool( __fastcall* )( void*, void*, INetMessage*, bool, bool ), bool, __fastcall, void* ECX, void* EDX, INetMessage* msg, bool force_reliable, bool voice );
HOOK_HEADER(TFGameMovementShit, void(__fastcall*)(void*, void*), void, __fastcall, void* ecx, void* edx);
HOOK_HEADER( GetDirectPingToPOP, int( __fastcall* )( void*, void*, SteamNetworkingPOPID ), int, __fastcall, void* ECX, void* EDX, SteamNetworkingPOPID ID );
HOOK_HEADER( ViewmodelPos, void( __fastcall* )( void*, void*, CBasePlayer*, Vector&, Vector& ), void, __fastcall, void* ECX, void* EDX, CBasePlayer* Owner, Vector& Position, Vector& Angles );
//HOOK_HEADER( DrawViewmodels, void( __fastcall* )( void*, void*, const CViewSetup&, bool ), void, __fastcall, void* ECX, void* EDX, const CViewSetup& View, bool Draw );
//55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 33 C9 33 D2 89 4D ? 89 55 ? 8B 46 ? 85 C0 74 ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68 ? ? ? ? 68
HOOK_HEADER( SetupBones, bool( __fastcall* )( void*, void*, Matrix3x4*, int, int, float ), bool, __fastcall, void* ECX, void* EDX, Matrix3x4* BoneToWorldOut, int MaxBones, int BoneMask, float CurrentTime );
HOOK_HEADER( CreateTFRagdoll, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( StartDynamicSound, void( __cdecl* )( StartSoundParams_t& ), void, __cdecl, StartSoundParams_t& Params );
HOOK_HEADER( CMatchInvite_OnTick, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( NotificationQueueAdd, int( __cdecl* )( CEconNotification* ), int, __cdecl, CEconNotification* Notification );
HOOK_HEADER( ShowItemsPickedUp, bool( __fastcall* )( void*, void*, bool, bool, bool ), bool, __fastcall, void* ECX, void* EDX, bool Force, bool ReturnToGame, bool NoPanel );
HOOK_HEADER( SetInt, void( __fastcall* )( void*, void*, const char*, int ), void, __fastcall, void* ECX, void* EDX, const char* keyName, int value );
HOOK_HEADER( IsPlayerDominated, bool( __fastcall* )( void*, void*, int ), bool, __fastcall, void* ECX, void* EDX, int index );
HOOK_HEADER( SetAbsVelocity, void( __fastcall* )( void*, void*, const Vector& ), void, __fastcall, void* ECX, void* EDX, const Vector& Velocity );
HOOK_HEADER( ProcessFixAngle, bool( __fastcall* )( void*, void*, void* ), bool, __fastcall, void* ECX, void* EDX, void* Msg );
HOOK_HEADER( R_LoadSkys, void( __cdecl* )( void ), void, __cdecl );
HOOK_HEADER( ResetLatched, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( PlayerMove, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( GetTracerType, const char*( __fastcall* )( void*, void* ), const char*, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( DoEnginePostProcessing, void( __cdecl* )( int, int, int, int, bool, bool ), void, __cdecl, int, int, int, int, bool, bool );
HOOK_HEADER( GetPlayerName, const char* ( __fastcall* )( void*, void*, int ), const char*, __fastcall, void* ECX, void* EDX, int index );
HOOK_HEADER( ChatPrintf, void( __cdecl* )( void*, int, int, const char*, ... ), void, __cdecl, void* ECX, int index, int filter, const char* message, ... );
HOOK_HEADER( ConsoleColorPrintf, void( __cdecl* )( void*, const Color&, const char*, ... ), void, __cdecl, void* ECX, const Color& color, const char* message, ... );
HOOK_HEADER( SetChoked, void( __fastcall* )( CNetChannel*, void* ), void, __fastcall, CNetChannel* ECX, void* EDX );
HOOK_HEADER( PacketStart, void( __fastcall* )( void*, void*, int, int ), void, __fastcall, void* ECX, void* EDX, int incoming_sequence, int outgoing_acknowledged );
HOOK_HEADER( DrawModelStaticProp, void( __fastcall* )( void*, void*, const DrawModelInfo_t&, const Matrix3x4&, int ),
			 void, __fastcall, void* ECX, void* EDX, const DrawModelInfo_t& drawInfo, const Matrix3x4& modelToWorld, int flags );
HOOK_HEADER( GetItemDefinition, void*( __fastcall* )( void*, void*, int ), void*, __fastcall, void* ECX, void* EDX, int Item );
HOOK_HEADER( GetOuterAbsVelocity, void( __fastcall* )( void*, void*, Vector& ), void, __fastcall, void* ECX, void* EDX, Vector& Velocity );
HOOK_HEADER( UpdateRankControls, void( __fastcall* )( void*, void*, unsigned int, unsigned int, LevelInfo& ),
			 void, __fastcall, void* ECX, void* EDX, unsigned int PreviousXP, unsigned int CurrentXP, LevelInfo& CurrentLevel );

/*HOOK_HEADER( VPhysicsCrashFix, void( __fastcall* )( void*, void* ), void, __fastcall, void* ECX, void* EDX );
HOOK_HEADER( VPhysicsCrashFixPart2, void( __fastcall* )( void*, void*, int, float* ), void, __fastcall, void* ECX, void* EDX, int a2, float* a3 );
HOOK_HEADER( VPhysicsCrashFixPart3, void( __fastcall* )( void*, void*, float*, float* ), void, __fastcall, void* ECX, void* EDX, float* a2, float* a3 );
HOOK_HEADER( VPhysicsCrashFixPart4, void( __fastcall* )( void*, void*, int, int ), void, __fastcall, void* ECX, void* EDX, int a2, int a3 );
HOOK_HEADER( VPhysicsCrashFixPart5, void( __fastcall* )( void*, void*, int, int ), void, __fastcall, void* ECX, void* EDX, int a2, int a3 );
*/
/*namespace Plat_FloatTime
{
	using Fn = double( __cdecl* )( );
	inline Fn OriginalFunction;
	double __cdecl Function( );
	void Init( );
}*/

class CHookManager
{
public:
	void Init( );
	void Restore( );
};

inline CHookManager* HookManager = new CHookManager;