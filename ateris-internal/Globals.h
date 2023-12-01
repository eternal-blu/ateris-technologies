#pragma once
#include "Framework/SDK/TF2/TF2.h"

class CSequence
{
public:
	float time {};
	int state {};
	int seq {};
public:
	CSequence( float realtime, int instate, int inseq ) : time{ realtime }, state{ instate }, seq{ inseq } {};
};

class CGlobals
{
public:
	struct {
		int Charged = 0, Sent = 22, ToSend = 0, Wait = 24, ToSendInternal = 0;
		bool Recharge = false, QueueRecharge = false, Warp = false;
		bool* SendPacket;
	} Doubletap;

	struct {
		bool Sticky = false;
		float TurnSpeed = 0.f;
		Vector PredictedPos = { };
		float ReflectVelocity = -1.f;

		Vector PunchAngle = {};
		bool Active = false;
		CBasePlayer* ClosestToCrosshair = nullptr;
		CBasePlayer* Target = nullptr;
		bool TargetHead = false;
		float SentShot = 0.f;
		bool ConfirmedHit = false;
		bool ConfirmedHeadshot = false;
		Vector AimPoint = { };
		
		bool LethalDTRange = false;

		Vector LockedAngles = {};

		int SavedWeaponID = -1;
	} Aimbot;

	struct CFakeTaunt
	{
		//DWORD AddVCDSequenceToGestureSlot = 0x0;
		//DWORD LookupSequence = 0x0;
		bool Printing = false;
		bool ShouldFakeTaunt = false;
		bool IsFakeTaunting = false;
		bool DefaultFakeTaunt = false;
		int FakeTauntSlot = 0;
		float FakeTauntTime = 0.f;
		Vector FakeTauntViewAngles = { };
	};
	
	CFakeTaunt* FakeTaunt = new CFakeTaunt;

	struct CAnimationFix
	{
		bool UpdateAnims = false;
		std::unordered_map<int, float> WarpTimeStamps = { };
		bool DrawingFakeAngles = false;
		DWORD UpdateClientSideAnimation = 0x0;
		Vector CurrentAngles = { };
		bool FakeLagMatrixBuilt = false;
		Matrix3x4 NonInterpolatedBones[ 128 ];
		bool ResetAnimState = false;
		CUserCmd* CurrentCmd {};
		Vector LastReal = { };
		bool FakeMatrixBuilt = false;
		Matrix3x4 FakeMatrix[ 128 ];
		bool RealMatrixBuilt = false;
		Matrix3x4 RealMatrix[ 128 ];
		MultiPlayerMovementData_t* RealMoveData {};
		CTFPlayerAnimState* FakeAnimState {};
		bool UsingFakeAngles = false;
		Vector UninterpolatedOrigin = { };
		Vector InterpolatedOrigin = { };
	};

	CAnimationFix* AnimationFix = new CAnimationFix;

	struct CHitMarker
	{
		float DrawTime = 0.f;
		float Alpha = 0.f;
		Color Col = Color( );
		int Damage = 0;
		Vector Position = {};
		bool Killed = {};
	};
	CHitMarker* HitMarker = new CHitMarker;

	struct CFakeLag
	{
		bool Active = false;
		bool Choking = false;
		bool BreakingLagComp = false;
		bool FakeDucking = false;
		int Ticks = 0;
		bool PreservingBlastJump = false;
	};

	CFakeLag* FakeLag = new CFakeLag;

	struct CAntiAimbot
	{
		std::pair<bool, bool> AASet = {};	//	real, fake
		Vector RealAngles = {};
		Vector FakeAngles = {};
		bool BuiltFakeBones = false;
		Matrix3x4 FakeBones[ 128 ];
		float RealYaw = 0.f;
		std::unordered_map<int, float> FiredShotsTimes = {};
		std::unordered_map<int, bool> FiredShotHit = {};
	};

	CAntiAimbot* AntiAim = new CAntiAimbot;

	struct CCrithack
	{
		bool Force = false;
	};

	CCrithack* CritHack = new CCrithack;

	struct CConVars
	{
		ConVar* AspectRatio;
		float DefaultAspectRatio;
		ConVar* HdrLevel;
		float DefaultHdrLevel;
		ConVar* BloomScale;
		float DefaultBloomScale;
		ConVar* Fullbright;
		ConVar* WeaponSway;
		float DefaultWeaponSway;
		ConVar* FlipViewmodels;
		bool DefaultFlipViewmodels;
		ConVar* ViewmodelFOV;
		float DefaultViewmodelFOV;
	};

	CConVars* ConVars = new CConVars;

	struct CKillSay
	{
		bool Stopped = false;
		float Time = 0.f;
		float Duration = 0.f;
	};

	CKillSay* KillSay = new CKillSay;

	struct CAuthentication
	{
		ISteamUser* SteamUser = nullptr;
		std::string UserName = "";
		std::string BuildName = "";
		bool IsDebugAllowed = false;
	};

	CAuthentication* Authentication = new CAuthentication;

	struct CProjectileCamera
	{
		bool IsDrawingProjectileCamera = false;
		Vector ProjectileCameraPos = { };
		Vector SavedAimPoint = { };
	};

	CProjectileCamera* ProjectileCamera = new CProjectileCamera;

	struct BonesCache
	{
		bool Cached = false;
		Matrix3x4 Bones[ 128 ] = {};
	};
	std::map<void*, BonesCache> CachedBones = { };

	struct CDetachSystem
	{
		bool WantsDetach = false;
	};

	CDetachSystem* DetachSystem = new CDetachSystem;

	std::deque<CSequence> Sequences;
	std::map< CBasePlayer*, float> FarEsp;

	//PlayerIndex, IsLocal, OriginalName
	std::vector<std::pair<unsigned int, std::pair<bool, const char*>>> HiddenNames;
	DWORD Plat_FloatTimeAddress = 0x0;
	DWORD PlayerDrawAddress = 0x0;
	DWORD WearableDrawAddress = 0x0;
	DWORD HudScopeDrawAddress = 0x0;
	DWORD RandomSeedRet1Address = 0x0;
	DWORD RandomSeedRet2Address = 0x0;
	DWORD RandomSeedRet3Address = 0x0;
	DWORD RenderLineAddress = 0x0;
	DWORD RenderBoxAddress = 0x0;
	DWORD RenderWireframeBoxAddress = 0x0;
	DWORD RenderSphereAddress = 0x0;
	DWORD RenderTriangleAddress = 0x0;
	DWORD UpdateButtonsAddress = 0x0;
	DWORD PredictionPlayerAddress = 0x0;
	DWORD ResetInstanceCountersAddress = 0x0;
	DWORD HasNewItemsCallAddress = 0x0;
	DWORD ScoreboardReturnAddressDesired = 0x0;
	DWORD IsPlayerDominatedReturnAddressDesired = 0x0;
	DWORD ScoreboardReturnAddressJump = 0x0;
	DWORD SetAbsVelocityCallAddress = 0x0;
	DWORD ParticleTracerAddress = 0x0;
};

inline CGlobals* G = new CGlobals;