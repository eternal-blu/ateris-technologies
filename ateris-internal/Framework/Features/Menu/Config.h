#pragma once
#include "../../SDK/TF2/TF2.h"

class CConfig
{
public:
	struct Colors_t
	{
		Color MenuOutline = Color( 15, 15, 15 );
		Color MenuBackground = Color( 13, 2, 33 ); //Color( 33, 33, 33 );
		Color MenuForeground = Color( 45, 45, 45 );
		Color MenuBorders = Color( 69, 49, 130 ); //{ 255, 128, 0, 255 }		Color( 255, 128, 0 )
		Color MenuText = Color( 255, 255, 255 );
		Color MenuFade1 = Color( 45, 226, 230 );
		Color MenuFade2 = Color( 196, 108, 241 );
		Color MenuAccent = Color( 255, 56, 100 );
		Color MenuShine = Color( 38, 20, 71 );
		Color DTFade1 = Color( 45, 226, 230 );
		Color DTFade2 = Color( 196, 108, 241 );

		Color ESPPlayerLocal = Color( 255, 255, 50 );
		Color ESPPlayerFriend = Color( 255, 255, 50 );
		Color ESPPlayerRed = Color( 255, 50, 50 );
		Color ESPPlayerBlu = Color( 0, 200, 255 );
		Color ESPPlayerEnemy = Color( 255, 100, 50 );
		Color ESPPlayerTeam = Color( 0, 255, 100 );

		Color ESPBuildingLocal = Color( 255, 255, 50 );
		Color ESPBuildingFriend = Color( 255, 255, 50 );
		Color ESPBuildingRed = Color( 255, 50, 50 );
		Color ESPBuildingBlu = Color( 0, 200, 255 );
		Color ESPBuildingEnemy = Color( 255, 100, 50 );
		Color ESPBuildingTeam = Color( 0, 255, 100 );

		Color ESPWorldHealth = Color( 60, 255, 60 );
		Color ESPWorldAmmo = Color( 220, 220, 220 );
		Color ESPWorldBomb = Color( 255, 50, 50 );
		Color ESPWorldMoney = Color( 50, 200, 80 );
		Color ESPWorldPowerup = Color( 255, 255, 255 );
		Color ESPWorldLocal = Color( 255, 255, 50 );
		Color ESPWorldFriend = Color( 255, 255, 50 );
		Color ESPWorldRed = Color( 255, 50, 50 );
		Color ESPWorldBlu = Color( 0, 200, 255 );
		Color ESPWorldEnemy = Color( 255, 100, 50 );
		Color ESPWorldTeam = Color( 0, 255, 100 );

		Color ModelsPlayerLocal = Color( 255, 255, 50 );
		Color ModelsPlayerFriend = Color( 255, 255, 50 );
		Color ModelsPlayerUber = Color( 255, 110, 255 );
		Color ModelsPlayerRed = Color( 255, 50, 50 );
		Color ModelsPlayerBlu = Color( 0, 200, 255 );
		Color ModelsPlayerEnemy = Color( 255, 100, 50 );
		Color ModelsPlayerTeam = Color( 0, 255, 100 );
		Color ModelsPlayerLocalUnoccluded = Color( 255, 255, 50 );
		Color ModelsPlayerFriendUnoccluded = Color( 255, 255, 50 );
		Color ModelsPlayerUberUnoccluded = Color( 255, 110, 255 );
		Color ModelsPlayerRedUnoccluded = Color( 255, 50, 50 );
		Color ModelsPlayerBluUnoccluded = Color( 0, 200, 255 );
		Color ModelsPlayerEnemyUnoccluded = Color( 255, 100, 50 );
		Color ModelsPlayerTeamUnoccluded = Color( 0, 255, 100 );
		Color ModelsPlayerLocalOccluded = Color( 128, 128, 25 );
		Color ModelsPlayerFriendOccluded = Color( 128, 128, 25 );
		Color ModelsPlayerUberOccluded = Color( 128, 55, 128 );
		Color ModelsPlayerRedOccluded = Color( 128, 25, 25 );
		Color ModelsPlayerBluOccluded = Color( 0, 100, 128 );
		Color ModelsPlayerEnemyOccluded = Color( 128, 50, 25 );
		Color ModelsPlayerTeamOccluded = Color( 0, 128, 50 );

		Color ModelsBuildingLocal = Color( 255, 255, 50 );
		Color ModelsBuildingFriend = Color( 255, 255, 50 );
		Color ModelsBuildingRed = Color( 255, 50, 50 );
		Color ModelsBuildingBlu = Color( 0, 200, 255 );
		Color ModelsBuildingEnemy = Color( 255, 100, 50 );
		Color ModelsBuildingTeam = Color( 0, 255, 100 );
		Color ModelsBuildingLocalUnoccluded = Color( 255, 255, 50 );
		Color ModelsBuildingFriendUnoccluded = Color( 255, 255, 50 );
		Color ModelsBuildingRedUnoccluded = Color( 255, 50, 50 );
		Color ModelsBuildingBluUnoccluded = Color( 0, 200, 255 );
		Color ModelsBuildingEnemyUnoccluded = Color( 255, 100, 50 );
		Color ModelsBuildingTeamUnoccluded = Color( 0, 255, 100 );
		Color ModelsBuildingLocalOccluded = Color( 128, 128, 25 );
		Color ModelsBuildingFriendOccluded = Color( 128, 128, 25 );
		Color ModelsBuildingRedOccluded = Color( 128, 25, 25 );
		Color ModelsBuildingBluOccluded = Color( 0, 100, 128 );
		Color ModelsBuildingEnemyOccluded = Color( 128, 50, 25 );
		Color ModelsBuildingTeamOccluded = Color( 0, 128, 50 );

		Color ModelsWorldHealth = Color( 60, 255, 60 );
		Color ModelsWorldAmmo = Color( 220, 220, 220 );
		Color ModelsWorldBomb = Color( 255, 50, 50 );
		Color ModelsWorldMoney = Color( 50, 200, 80 );
		Color ModelsWorldPowerup = Color( 255, 255, 0 );
		Color ModelsWorldLocal = Color( 255, 255, 50 );
		Color ModelsWorldFriend = Color( 255, 255, 50 );
		Color ModelsWorldRed = Color( 255, 50, 50 );
		Color ModelsWorldBlu = Color( 0, 200, 255 );
		Color ModelsWorldEnemy = Color( 255, 100, 50 );
		Color ModelsWorldTeam = Color( 0, 255, 100 );
		Color ModelsWorldHealthUnoccluded = Color( 60, 255, 60 );
		Color ModelsWorldAmmoUnoccluded = Color( 220, 220, 220 );
		Color ModelsWorldBombUnoccluded = Color( 255, 50, 50 );
		Color ModelsWorldMoneyUnoccluded = Color( 50, 200, 80 );
		Color ModelsWorldPowerupUnoccluded = Color( 255, 255, 0 );
		Color ModelsWorldLocalUnoccluded = Color( 255, 255, 50 );
		Color ModelsWorldFriendUnoccluded = Color( 255, 255, 50 );
		Color ModelsWorldRedUnoccluded = Color( 255, 50, 50 );
		Color ModelsWorldBluUnoccluded = Color( 0, 200, 255 );
		Color ModelsWorldEnemyUnoccluded = Color( 255, 100, 50 );
		Color ModelsWorldTeamUnoccluded = Color( 0, 255, 100 );
		Color ModelsWorldHealthOccluded = Color( 30, 128, 30 );
		Color ModelsWorldAmmoOccluded = Color( 110, 110, 110 );
		Color ModelsWorldBombOccluded = Color( 128, 25, 25 );
		Color ModelsWorldMoneyOccluded = Color( 25, 100, 40 );
		Color ModelsWorldPowerupOccluded = Color( 128, 128, 0 );
		Color ModelsWorldLocalOccluded = Color( 128, 128, 25 );
		Color ModelsWorldFriendOccluded = Color( 128, 128, 25 );
		Color ModelsWorldRedOccluded = Color( 128, 25, 25 );
		Color ModelsWorldBluOccluded = Color( 0, 100, 128 );
		Color ModelsWorldEnemyOccluded = Color( 128, 50, 25 );
		Color ModelsWorldTeamOccluded = Color( 0, 128, 50 );
		
		Color OutlinesPlayerLocal = Color( 255, 255, 50 );
		Color OutlinesPlayerFriend = Color( 255, 255, 50 );
		Color OutlinesPlayerUber = Color( 255, 110, 255 );
		Color OutlinesPlayerRed = Color( 255, 50, 50 );
		Color OutlinesPlayerBlu = Color( 0, 200, 255 );
		Color OutlinesPlayerEnemy = Color( 255, 100, 50 );
		Color OutlinesPlayerTeam = Color( 0, 255, 100 );

		Color OutlinesBuildingLocal = Color( 255, 255, 50 );
		Color OutlinesBuildingFriend = Color( 255, 255, 50 );
		Color OutlinesBuildingRed = Color( 255, 50, 50 );
		Color OutlinesBuildingBlu = Color( 0, 200, 255 );
		Color OutlinesBuildingEnemy = Color( 255, 100, 50 );
		Color OutlinesBuildingTeam = Color( 0, 255, 100 );

		Color OutlinesWorldLocal = Color( 255, 255, 50 );
		Color OutlinesWorldFriend = Color( 255, 255, 50 );
		Color OutlinesWorldHealth = Color( 60, 255, 60 );
		Color OutlinesWorldAmmo = Color( 220, 220, 220 );
		Color OutlinesWorldBomb = Color( 255, 50, 50 );
		Color OutlinesWorldMoney = Color( 50, 200, 80 );
		Color OutlinesWorldPowerup = Color( 255, 255, 0 );
		Color OutlinesWorldRed = Color( 255, 50, 50 );
		Color OutlinesWorldBlu = Color( 0, 200, 255 );
		Color OutlinesWorldEnemy = Color( 255, 100, 50 );
		Color OutlinesWorldTeam = Color( 0, 255, 100 );

		Color WorldColor = Color( 255, 255, 255 );
		Color HandsColor = Color( 255, 255, 50 );
		Color HandsOverlayColor = Color( 255, 255, 50 );
		Color HitboxColor = Color( 255, 255, 255 );
		Color BoundingBoxColor = Color( 255, 255, 255 );
		Color ProjectileLineColor = Color( 35, 115, 195, 200 );
		
		Color BeamsLocal = Color( 255, 255, 50 );
		Color BeamsFriend = Color( 255, 255, 50 );
		Color BeamsEnemy = Color( 255, 100, 50 );
		Color BeamsTeam = Color( 0, 255, 100 );
		Color BeamsRed = Color( 255, 50, 50 );
		Color BeamsBlu = Color( 0, 200, 255 );

		Color FakeModel = Color( 255, 255, 50 );
		Color FakeGlow = Color( 255, 255, 50 );
		Color LagModel = Color( 255, 255, 50 );
		Color LagGlow = Color( 255, 255, 50 );
	};
	Colors_t* Colors = new Colors_t;

	struct CAimbot
	{
		struct CGlobal
		{
			bool Enabled = true;
			int AimKey = 0x0;
			int FOV = 69;
			bool AutoSecondarySwitch = true;
		};

		CGlobal* Global = new CGlobal;

		struct CTargetting
		{
			bool TargetPlayers = true;
			bool TargetBonk = false;
			bool TargetUber = false;
			bool TargetVacc = false;
			bool TargetBuildings = true;
			bool TargetStickies = true;
			bool TargetFriends = false;
			bool TargetLethalBombs = true;
		};

		CTargetting* Targetting = new CTargetting;

		struct CPriorities
		{
			int Scout = 1;
			int Soldier = 1;
			int Pyro = 1;
			int Demoman = 1;
			int Heavy = 1;
			int Engineer = 1;
			int Medic = 1;
			int Sniper = 1;
			int Spy = 1;
			int Sentry = 1;
			int OtherBuildings = 1;
			int Stickies = 1;

			int Cheaters = 5;
			int Friends = 4;
			int Scoped = 2;
			int Revved = 2;
			int Cloaked = 1;
			int Other = 1;
		};

		CPriorities* Priorities = new CPriorities;

		struct CHitscan
		{
			bool TapFire = false;
			bool WaitForCharge = false;
			bool WaitForHeadshot = true;
			bool PreventBait = true;
			int Preference = 0;
			bool LagCompMethod = true;
			bool FakeLatency = false;
			bool DynamicLatency = false;
			int FakeLatencyAmount = 180;
			int FakeLatencyKey = 0x0;
			bool ReduceScoreboardPing = false;
			int ScoreboardPingGoal = 10;
			int HeavyPointScale = 72;
			int PointScale = 42;
			bool TargetHead = true;
			bool TargetChest = true;
			bool TargetStomach = true;
			bool TargetArms = true;
			bool TargetPelvis = true;
			struct CLegitBob
			{
				bool Enabled = false;
				float FOV = 4.2f;
				float DeadZone = 0.5f;
				float Strenght = 100.f;
			};
			CLegitBob* LegitBob = new CLegitBob;
		};

		CHitscan* Hitscan = new CHitscan;

		struct CMelee
		{
			bool Enabled = true;
			bool RequireAimKey = false;
			bool WarpPrediction = true;
		};

		CMelee* Melee = new CMelee;
		
		struct CProjectile
		{
			float MaxTime = 8.f;
			float MaxProjectileTime = 5.f;
			bool PredictStrafing = true;
			bool PredictJumping = false;
			bool DelayShot = true;
			bool WaitForHeadshot = true;
			float MaxGroundDebug = 2.49f;
			int HitChance = 25;
			int MaxStickyCharge = 0;
			int ChangePositionKey = 0x0;
			int ResetPositionKey = 0x0;
			int GazaBombingKey = 0x0;
			bool SplashBot = false;
			int SplashRadius = 120;
			int SplashPoints = 20;
		};

		CProjectile* Projectile = new CProjectile;
	};

	CAimbot* Aimbot = new CAimbot;

	struct CAutoVaccinator
	{
		bool Enabled = true;
		bool LogReasons = true;
		bool AutoPopHigherHealthHeavies = true;
		bool AutoPopHeavies = false;
		bool AutoPopExplosive = true;
		bool AutoPopHeadshot = true;
		bool AutoPopShotgun = false;
		bool AutoPopCrossbow = false;
		bool IgnoreFriendHeavies = false;

		float AutoPopMinHealth = 0.8f;
		float HeavyDistance = 340.f;
	};

	CAutoVaccinator* AutoVacc = new CAutoVaccinator;

	struct CAutoMedic
	{
		bool Enabled = true;
		bool TargetLock = true;
		bool Render = true;
		int LockTarget = 0x0;
	};

	CAutoMedic* AutoMedic = new CAutoMedic;

	struct CVisuals
	{
		int FOV = 120;
		int ScopedFOV = 120;
		bool RemoveViewPunch = true;
		bool RemoveFade = true;
		bool RemovePostProcessing = true;
		bool RemoveShake = true;
		bool RemoveScope = true;
		int ThirdPersonKey = 0x0;
		bool ThirdPersonOffset = false;
		int ThirdPersonSidewaysCycleKey = 0x0;
		int ThirdPersonForward = 0;
		int ThirdPersonSideways = 0;
		int ThirdPersonUpward = 0;
		float AspectRatio = 0.0f;
		float BloomScale = 1.0f;
		bool HitMarker2D = false;
		bool HitscanHitboxes = false;
		bool HitboxesCustomColor = false;
		float HitboxesAlpha = 0.5f;
		float HitboxesTime = 3.f;
		bool ProjectileBoundingBox = false;
		bool BoundingBoxCustomColor = false;
		float BoundingBoxAlpha = 0.5f;
		float BoundingBoxTime = 3.f;
		bool DrawBackTrack = false;
		bool BackTrackModel = false;
		float BackTrackAlpha = 0.25f;
		bool BackTrackOutline = false;
		bool LastTickOnly = true;
		bool ViewmodelSway = false;
		float ViewmodelSwayAmount = 0.05f;
		bool ViewmodelPos = false;
		bool ViewmodelFlip = false;
		int ViewmodelFOV = 90;
		int ViewmodelX = 0;
		int ViewmodelY = 0;
		int ViewmodelZ = 0;
		int ViewmodelRoll = 0;
		bool HandsModels = false;
		int HandsModel = 0; //0 - None, 1 - Flat, 2 - Shaded, 3 - Fresnel
		float HandsAlpha = 0.5f;
		bool DisableHands = false;
		bool DisableWeapon = false;
		bool HandsOverlay = false;
		int HandsOverlayModel = 0; //0 - Contour, 1 - Sheen, 2 - Eso Glow
		float HandsOverlayAlpha = 0.5f;
		bool HandsOverlayWireframe = false;
		bool PlayerArrows = true;
		float ArrowsScreenDistance = 0.2f;
		int ArrowsScreenShape = 0;
		bool ProjectileLine = false;
		float ProjectileLineAlpha = 0.8f;
		bool ProjectileCamera = false;
		int ProjectileCameraFOV = 80;
		int ProjectileCameraWidth = 300;
		int ProjectileCameraHeight = 300;
		int BulletTracers = 0;
		std::string SkyboxName = "";
		bool StreamerMode = false;
		bool HideLocal = false;
		bool HideFriends = false;
		bool UnlockEquipRegion = false;
		bool OptimizedFonts = false;
		bool MedalChanger = false;
		int MedalLevel = 150;
		int MedalTier = 8;

		bool RagdollEffects = false;
		bool RagdollBurning = false;
		bool RagdollElectrocuted = false;
		bool RagdollDissolving = false;
		bool RagdollBecomeAsh = false;
		int RagdollBaseType = 1;

		bool RagdollEnemy = false;
		bool RagdollTeam = false;
		bool RagdollLocal = false;
		
		struct CFakeTaunt
		{
			bool Enabled = true;
		};
		CFakeTaunt* FakeTaunt = new CFakeTaunt;

		struct CWorldModulation
		{
			bool Enabled = false;
			int Mode = 0; //0 - Night Mode, 1 - Fullbright, 2 - Custom Color
			bool World = true;
			bool Skybox = true;
			float Amount = 0.25f;
			float Alpha = 1.0f;
		};
		CWorldModulation* WorldModulation = new CWorldModulation;

		struct CBeams
		{
			bool Enabled = true;
			int ColorScheme = 0; //0 - Enemy/Team, 1 - Red/Blu
			bool Enemy = true;
			bool Team = true;
			bool Friends = true;
			bool Local = true;
		};

		CBeams* Beams = new CBeams;
	};

	CVisuals* Visuals = new CVisuals;

	struct CMisc
	{
		bool BHop = true;
		bool AutoStrafer = true;
		bool EdgeJump = true;
		int StraferScale = 0;
		bool AvoidTeamPush = true;
		bool FastStop = true;
		bool HighJump = true;
		int HighJumpKey = 0x0;
		bool AutoRetreat = false;
		int AutoRetreatKey = 0x0;
		bool FastBackup = true;
		bool AntiBackstab = true;
		bool VoiceSpam = true;
		int VoiceSpamMode = 0; //0 - KillSay, 1 - Full music
		bool AntiAutoBalance = true;

		struct CFakeLag
		{
			bool Enabled = false;
			bool DrawLag = false;
			bool LagModel = false;
			bool LagOutline = false;
			float ModelAlpha = 0.25f;
			int Value = 21;
			int FakeLagKey = 0x0;
			bool BreakLagComp = true;
			bool BreakMoving = true;
			bool BreakInAir = true;
			bool UnchokeOnAttack = true;
		};

		CFakeLag* FakeLag = new CFakeLag;

		struct CFakeDuck
		{
			bool Enabled = true;
			int FakeDuckKey = 0x0;
		};

		CFakeDuck* FakeDuck = new CFakeDuck;

		struct CAntiAimbot
		{
			bool Enabled = false;
			bool DrawFake = false;
			bool FakeModel = false;
			bool FakeOutline = false;
			float FakeAlpha = 0.25f;
			bool RageActive = false;
			int ToggleKey = 0;
			int CycleKey = 0;
			//legit
			int LegitYaw = 0;
			//rage
			int FakeYaw = 0;			// -180 / 180
			int FakeMode = 0;		// 0 - Static,	1 - Jitter,	2 - Spin, 3 - Rotate
			int FakeRotate = 3;
			int FakeDelta = 10;
			int FakeSpin = 3;
			int FakeAngle1 = 74;
			int FakeAngle2 = 104;

			int RealYaw = 0;			// -180 / 180
			int RealMode = 0;		// 0 - Static,	1 - Jitter,	2 - Spin, 3 - Rotate
			int RealRotate = 3;
			int RealDelta = 10;
			int RealSpin = 3;
			int RealAngle1 = 74;
			int RealAngle2 = 104;

			int Pitch = 0;			// 0 - None,	1 - Automatic,	2 - Manual
			bool FakePitch = false;
			int RealPitch = 0;		// 0 - Up,		1 - Down,	2 - Jitter
		};

		CAntiAimbot* AntiAim = new CAntiAimbot;

		struct CDT
		{
			bool Enabled = true;
			int RechargeKey = 0x0;
			int WarpKey = 0x0;
			int DTKey = 0x0;
			int WarpMode = 0;
			int WarpTicks = 2;
			int DelayMode = 0; //0 - Fast, 1 - Safe
			int DelayTicks = 2;
			bool AntiWarp = true;
			bool ShootWood = false;

			bool ActiveInAir = true;
			bool ActiveOnlyAimbot = false;
			bool ActivateOnlyLethal = false;

			struct CIndicator
			{
				bool Enabled = true;
				bool FatCharged = true;
				bool CustomFade = false;
			};

			CIndicator* Indicator = new CIndicator;
		};

		CDT* DT = new CDT;

		struct CCritHack
		{
			bool Enabled = true;
			int CritKey = 0x0;
			bool SkipRandom = false;
			int Optimization = 0;

			bool ForceOnMelee = true;
			bool ForceOnCheater = true;
			bool ForceOnFriends = false;
			bool ForceOnStreamers = true;
			bool ForceOnSuspicious = false;

			struct CIndicator
			{
				bool Enabled = true;
				bool FatCrits = true;
			};
			CIndicator* Indicator = new CIndicator;
		};

		CCritHack* CritHack = new CCritHack;

		struct CNoSpread
		{
			bool Enabled = true;
			bool Indicator = true;
		};

		CNoSpread* NoSpread = new CNoSpread;

		struct CMatchmaking
		{
			bool AutoAccept = false;
			bool PreventAutoJoin = false;

			struct CRegionSelector
			{
				bool Enabled = false;

				bool EUEast = false;
				bool EUWest = false;				//EU West		- Amsterdam, London, Luxembourg, Madrid, Paris
				bool Asia = false;					//Asia			- Manila, Singapore
				bool MiddleEast = false;			//Middle East	- Dubai
				bool HongKong = false;				//Hong Kong		- Hong Kong
				bool Japan = false;					//Japan			- Tokyo (GNRT)
				bool India = false;					//India			- Mumbai, Chennai
				bool Australia = false;				//Australia		- Sydney
				bool Africa = false;				//Africa		- Johannesburg
				bool Chile = false;					//Chile			- Santiago
				bool Peru = false;					//Peru			- Lima
				bool Brazil = false;				//Brazil		- Sao Paulo
				bool USEast = false;				//US East		- Atlanta, Virginia, Chicago
				bool USNorthWest = false;			//US North West - Moses Lake, Washington, Seattle
				bool USSouthWest = false;			//US South West - Los Angeles, Oklahoma City
			};

			CRegionSelector* RegionSelector = new CRegionSelector;
		};

		CMatchmaking* Matchmaking = new CMatchmaking;
		int EdgeJumpKey = 0x0;
	};

	CMisc* Misc = new CMisc;

	struct CESP
	{
		struct CGlobal
		{
			bool Enabled = true;
			int ColorScheme = 0; //0 - Enemy/Team, 1 - Red/Blu
		};

		CGlobal* Global = new CGlobal;

		struct CPlayers
		{
			bool Enabled = true;
			bool Enemy = true;
			bool Team = true;
			bool Friends = true;
			bool Local = true;
			bool EnabledBox = false;
			int Box = 0; //0 - 2D, 1 - 3D
			bool Name = true;
			bool Flags = true;
			bool Weapon = true;
			bool HealthBar = true;
			bool UberBar = true;
			bool Avatar = true;

			struct CHealthBar
			{
				bool Lines = true;
				int LinesCount = 3;
				bool Animated = true;
				bool CustomColor = true;
			};

			CHealthBar* PHealthBar = new CHealthBar;
		};

		CPlayers* Players = new CPlayers;

		struct CBuildings
		{
			bool Enabled = true;
			bool Enemy = true;
			bool Team = false;
			bool Friends = true;
			bool Local = true;
			bool EnabledBox = false;
			int Box = 0; //0 - 2D, 1 - 3D
			bool Name = true;
			bool Flags = true;
			bool HealthBar = true;

			struct CHealthBar
			{
				bool Lines = true;
				int LinesCount = 3;
				bool Animated = true;
				bool CustomColor = true;
			};

			CHealthBar* PHealthBar = new CHealthBar;
		};

		CBuildings* Buildings = new CBuildings;

		struct CWorld
		{
			bool Enabled = true;
			bool Enemy = true;
			bool Team = false;
			bool Friends = true;
			bool Local = true;
			bool Healthpacks = true;
			bool Ammopacks = true;
			bool Intels = true;
			bool Bombs = true;
			bool Projectiles = true;
			bool MvMCash = true;
			bool MvMTanks = true;
			bool MvMSentryBusters = true;
			bool MPPowerups = true;
			bool EnabledBox = false;
			int Box = 0; //0 - 2D, 1 - 3D
			bool Name = true;
			bool Distance = true;
			bool HealthBar = true;

			struct CHealthBar
			{
				bool Lines = true;
				int LinesCount = 3;
				bool Animated = true;
				bool CustomColor = true;
			};

			CHealthBar* PHealthBar = new CHealthBar;
		};

		CWorld* World = new CWorld;
	};

	CESP* ESP = new CESP;

	struct CModels
	{
		struct CGlobal
		{
			bool Enabled = true;
			int RenderingOrder = 0;
			int ColorScheme = 0; //0 - Enemy/Team, 1 - Red/Blu
		};

		CGlobal* Global = new CGlobal;

		struct CPlayers
		{
			bool Enabled = true;
			bool Enemy = true;
			bool Team = true;
			bool Friends = true;
			bool Local = true;
			bool TwoModels = false;
			bool IgnoreZ = true;
			bool Overlay = true;
			float Alpha = 0.5f;
			int Model = 0;
			int OverlayType = 0; //0 - Custom Overlay, 1 - High KD Sheen
			int OverlayModel = 1;//0 - Contour, 1 - Sheen
			float OverlayAlpha = 1.0f;
			bool OverlayWireframe = true;

			struct COccluded
			{
				bool Overlay = true;
				float Alpha = 1.0f;
				int Model = 1;

				int OverlayType = 0; //0 - Custom Overlay, 1 - High KD Sheen
				int OverlayModel = 0;//0 - Contour, 1 - Sheen
				float OverlayAlpha = 1.0f;
				bool OverlayWireframe = true;
			};
			COccluded* Occluded = new COccluded;

			struct CUnoccluded
			{
				bool Overlay = true;
				float Alpha = 1.0f;
				int Model = 1;

				int OverlayType = 0; //0 - Custom Overlay, 1 - High KD Sheen
				int OverlayModel = 0;//0 - Contour, 1 - Sheen, 2 - Eso Glow
				float OverlayAlpha = 1.0f;
				bool OverlayWireframe = true;
			};
			CUnoccluded* Unoccluded = new CUnoccluded;
		};

		CPlayers* Players = new CPlayers;

		struct CBuildings
		{
			bool Enabled = true;
			bool Enemy = true;
			bool Team = false;
			bool Friends = true;
			bool Local = true;
			bool TwoModels = false;
			bool IgnoreZ = true;
			bool Overlay = true;
			float Alpha = 0.0f;
			int Model = 1;
			int OverlayModel = 0;//0 - Contour, 1 - Sheen
			float OverlayAlpha = 1.0f;
			bool OverlayWireframe = true;

			struct COccluded
			{
				bool Overlay = true;
				float Alpha = 1.0f;
				int Model = 1;

				int OverlayModel = 0;//0 - Contour, 1 - Sheen
				float OverlayAlpha = 1.0f;
				bool OverlayWireframe = true;
			};
			COccluded* Occluded = new COccluded;

			struct CUnoccluded
			{
				bool Overlay = true;
				float Alpha = 1.0f;
				int Model = 1;

				int OverlayModel = 0;//0 - Contour, 1 - Sheen
				float OverlayAlpha = 1.0f;
				bool OverlayWireframe = true;
			};
			CUnoccluded* Unoccluded = new CUnoccluded;
		};

		CBuildings* Buildings = new CBuildings;

		struct CWorld
		{
			bool Enabled = true;
			bool Enemy = true;
			bool Team = false;
			bool Friends = true;
			bool Local = true;
			bool Healthpacks = true;
			bool Ammopacks = true;
			bool Intels = true;
			bool Bombs = true;
			bool Projectiles = true;
			bool MvMCash = true;
			bool MvMTanks = true;
			bool MvMSentryBusters = true;
			bool MPPowerups = true;
			bool TwoModels = false;
			bool IgnoreZ = true;
			bool Overlay = true;
			float Alpha = 0.5f;
			int Model = 1;
			int OverlayModel = 0;//0 - Contour, 1 - Sheen
			float OverlayAlpha = 1.0f;
			bool OverlayWireframe = true;

			struct COccluded
			{
				bool Overlay = true;
				float Alpha = 1.0f;
				int Model = 1;

				int OverlayModel = 0;//0 - Contour, 1 - Sheen
				float OverlayAlpha = 1.0f;
				bool OverlayWireframe = true;
			};
			COccluded* Occluded = new COccluded;

			struct CUnoccluded
			{
				bool Overlay = true;
				float Alpha = 1.0f;
				int Model = 1;

				int OverlayModel = 0;//0 - Contour, 1 - Sheen
				float OverlayAlpha = 1.0f;
				bool OverlayWireframe = true;
			};
			CUnoccluded* Unoccluded = new CUnoccluded;
		};

		CWorld* World = new CWorld;
	};

	CModels* Models = new CModels;

	class COutlines
	{
	public:
		class CGlobal
		{
		public:
			bool Enabled = true;
			int ColorScheme = 0; //0 - Enemy/Team, 1 - Red/Blu
			bool BlurOutline = true;
			bool StencilOutline = true;
			bool CheapStencil = false;
			int BlurScale = 5;
			int StencilScale = 1;
		};

		CGlobal* Global = new CGlobal;

		class CPlayers
		{
		public:
			bool Enabled = true;
			bool Enemy = true;
			bool Team = false;
			bool Friends = true;
			bool Local = true;
			float Alpha = 1.0f;
		};

		CPlayers* Players = new CPlayers;

		class CBuildings
		{
		public:
			bool Enabled = true;
			bool Enemy = true;
			bool Team = false;
			bool Friends = true;
			bool Local = true;
			float Alpha = 1.0f;
		};

		CBuildings* Buildings = new CBuildings;

		class CWorld
		{
		public:
			bool Enabled = true;
			bool Enemy = true;
			bool Team = false;
			bool Friends = true;
			bool Local = true;
			bool Healthpacks = true;
			bool Ammopacks = true;
			bool Intels = true;
			bool Bombs = true;
			bool Projectiles = true;
			bool MvMCash = true;
			bool MvMTanks = true;
			bool MvMSentryBusters = true;
			bool MPPowerups = true;
			float Alpha = 1.0f;
		};

		CWorld* World = new CWorld;
	};

	COutlines* Outlines = new COutlines;

	struct COtherAuto
	{
		bool Enabled = true;
		bool AutoDetonation = true;
		int AutoDetonationMode = 0;
		bool AutoAirblast = true;
		bool AutoAirblastPath = true;
		bool BlastArrows = true;
		bool BlastExplosive = true;
		bool BlastStickies = true;

		bool TargetPlayers = true;
		bool TargetBonk = false;
		bool TargetUber = false;
		bool TargetVacc = false;
		bool TargetBuildings = true;
		bool TargetStickies = true;
		bool TargetFriends = false;
	};

	COtherAuto* OtherAuto = new COtherAuto;

	struct CSkinChanger
	{
		bool Enabled = false;

		struct CSkinInfo
		{
			bool Aussie = false;
			bool SwapModel = false;
			int Model = 0;
		};

		std::map<int, CSkinInfo> Skins;
	};

	CSkinChanger* SkinChanger = new CSkinChanger;

	struct CDebug
	{
		bool EnableInfo = false;
		float DecayRate = 0.56f;
		float ClampCheck = 4.2f;
		int Samples = 19;
	};

	CDebug* Debug = new CDebug;
};

struct PriorityPlayer_t
{
	int Priority = 0;
	bool Resolver = false;
	bool VoiceSay = false;

	const char* CustomFlag = "";
};

inline std::map<int, PriorityPlayer_t> gPriorities = {};
inline std::map<int, bool> gPrioritiesOpen = {};
inline std::map<int, bool> gSkinsOpen = {};
inline CConfig* Config = new CConfig;