#pragma once
#include "../../Utils/NetVars/NetVars.h"
#include "TF2.h"
#include "AnimState.h"
#include "../Math/Math.h"
#include "CHandle.h"
#include "../Steam/SteamClient.h"

#define TF_AMMOPACK_SMALL_BDAY		"models/items/ammopack_small_bday.mdl"
#define TF_AMMOPACK_MEDIUM_BDAY		"models/items/ammopack_medium_bday.mdl"
#define TF_AMMOPACK_LARGE_BDAY		"models/items/ammopack_large_bday.mdl"
#define TF_AMMOPACK_SMALL "models/items/ammopack_small.mdl"
#define TF_AMMOPACK_MEDIUM "models/items/ammopack_medium.mdl"
#define TF_AMMOPACK_LARGE "models/items/ammopack_large.mdl"

#define TF_MEDKIT_SMALL_BDAY	"models/items/medkit_small_bday.mdl"
#define TF_MEDKIT_MEDIUM_BDAY	"models/items/medkit_medium_bday.mdl"
#define TF_MEDKIT_LARGE_BDAY	"models/items/medkit_large_bday.mdl"
#define TF_MEDKIT_SMALL_HALLOWEEN	"models/props_halloween/halloween_medkit_small.mdl"
#define TF_MEDKIT_MEDIUM_HALLOWEEN  "models/props_halloween/halloween_medkit_medium.mdl"
#define TF_MEDKIT_LARGE_HALLOWEEN	"models/props_halloween/halloween_medkit_large.mdl"
#define LUNCHBOX_DROP_MODEL  "models/items/plate.mdl"
#define LUNCHBOX_STEAK_DROP_MODEL  "models/items/plate_steak.mdl"
#define LUNCHBOX_ROBOT_DROP_MODEL  "models/items/plate_robo_sandwich.mdl"
#define LUNCHBOX_FESTIVE_DROP_MODEL  "models/items/plate_sandwich_xmas.mdl"
#define TF_MEDKIT_SMALL "models/items/medkit_small.mdl"
#define TF_MEDKIT_MEDIUM "models/items/medkit_medium.mdl"
#define TF_MEDKIT_LARGE "models/items/medkit_large.mdl"

#define TF_POWERUP_AGILITY "models/pickups/pickup_powerup_agility.mdl"
#define TF_POWERUP_CRIT "models/pickups/pickup_powerup_crit.mdl"
#define TF_POWERUP_DEFENSE "models/pickups/pickup_powerup_defense.mdl"
#define TF_POWERUP_HASTE "models/pickups/pickup_powerup_haste.mdl"
#define TF_POWERUP_KING "models/pickups/pickup_powerup_king.mdl"
#define TF_POWERUP_KNOCKOUT "models/pickups/pickup_powerup_knockout.mdl"
#define TF_POWERUP_PLAGUE "models/pickups/pickup_powerup_plague.mdl"
#define TF_POWERUP_PRECISION "models/pickups/pickup_powerup_precision.mdl"
#define TF_POWERUP_REFLECT "models/pickups/pickup_powerup_reflect.mdl"
#define TF_POWERUP_REGEN "models/pickups/pickup_powerup_regen.mdl"
#define TF_POWERUP_RESISTANCE "models/pickups/pickup_powerup_resistance.mdl"
#define TF_POWERUP_STRENGTH "models/pickups/pickup_powerup_strength.mdl"
#define TF_POWERUP_SUPERNOVA "models/pickups/pickup_powerup_supernova.mdl"
//#define TF_POWERUP_THORNS "models/pickups/pickup_powerup_thorns.mdl"		UNUSED
//#define TF_POWERUP_UBER "models/pickups/pickup_powerup_uber.mdl"			UNUSED
#define TF_POWERUP_VAMPIRE "models/pickups/pickup_powerup_vampire.mdl"

#define TF_BOT_SENTRY_BUSTER "models/bots/demo/bot_sentry_buster.mdl"

struct model_t;
class CUserCmd;
class IClientEntity {
public:
	inline const Vector& GetAbsOrigin( )
	{
		return Memory->GetVFunc < const Vector & ( __thiscall* )( void* )>( this, 9u )( this );
	}

	inline const Vector& GetAbsAngles( )
	{
		return Memory->GetVFunc < const Vector & ( __thiscall* )( void* )>( this, 10u )( this );
	}

	inline Vector const& GetRenderOrigin( )
	{
		return Memory->GetVFunc< Vector const& ( __thiscall* )( IClientEntity* ) >( this + 0x4, 1u )( this + 0x4 );
	}

	inline Vector const& GetRenderAngles( )
	{
		return Memory->GetVFunc< Vector const& ( __thiscall* )( IClientEntity* ) >( this + 0x4, 2u )( this + 0x4 );
	}

	inline bool ShouldDraw( )
	{
		return Memory->GetVFunc< bool( __thiscall* )( IClientEntity* ) >( this + 0x4, 3u )( this + 0x4 );
	}

	inline model_t* GetModel( )
	{
		return Memory->GetVFunc< model_t * ( __thiscall* )( IClientEntity* ) >( this + 0x4, 9u )( this + 0x4 );
	}

	inline int DrawModel( int Flags )
	{
		return Memory->GetVFunc< int( __thiscall* )( IClientEntity*, int ) >( this + 0x4, 10u )( this + 0x4, Flags );
	}

	inline bool SetupBones( Matrix3x4* Out, int Max, int Mask, float Time )
	{
		return Memory->GetVFunc< bool( __thiscall* )( IClientEntity*, Matrix3x4*, int, int, float ) >( this + 0x4, 16u )( this + 0x4, Out, Max, Mask, Time );
	}

	inline void GetRenderBounds( Vector& Mins, Vector& Maxs )
	{
		return Memory->GetVFunc< void( __thiscall* )( IClientEntity*, Vector&, Vector& ) >( this + 0x4, 20u )( this + 0x4, Mins, Maxs );
	}

	inline Matrix3x4& RenderableToWorldTransform( )
	{
		return Memory->GetVFunc< Matrix3x4 & ( __thiscall* )( IClientEntity* ) >( this + 0x4, 34u )( this + 0x4 );
	}

	inline int LookupAttachment( const char* attachment )
	{
		return Memory->GetVFunc< int( __thiscall* )( IClientEntity*, const char* ) >( this + 0x4, 35u )( this + 0x4, attachment );
	}

	inline CClientClass* GetClientClass( )
	{
		if ( !this || !( this + 0x8 ) )
			return nullptr;

		return Memory->GetVFunc< CClientClass * ( __thiscall* )( IClientEntity* ) >( this + 0x8, 2u )( this + 0x8 );
	}

	inline bool IsDormant( )
	{
		if (!this || !(this + 0x8))
			return false;

		return Memory->GetVFunc< bool( __thiscall* )( IClientEntity* ) >( this + 0x8, 8u )( this + 0x8 );
	}

	inline int GetIndex( )
	{
		if ( !this || !( this + 0x8 ) )
			return 0;

		return Memory->GetVFunc< int( __thiscall* )( IClientEntity* ) >( this + 0x8, 9u )( this + 0x8 );
	}

	inline bool IsPlayer( )
	{
		return GetClientClass() && GetClientClass( )->ClassID == EClassID::CTFPlayer;
	}

	inline bool IsBuilding( )
	{
		auto ClassID = GetClientClass( )->ClassID;
		switch ( ClassID )
		{
			case EClassID::CObjectSentrygun:
			case EClassID::CObjectDispenser:
			case EClassID::CObjectTeleporter: return true;
			default: return false;
		}
		return false;
	}

	inline bool IsIntel( )
	{
		return GetClientClass( ) && GetClientClass( )->ClassID == EClassID::CCaptureFlag;
	}

	inline bool IsBomb( )
	{
		return GetClientClass( ) && ( GetClientClass( )->ClassID == EClassID::CTFGenericBomb || GetClientClass( )->ClassID == EClassID::CTFPumpkinBomb );
	}

	inline bool IsTank( )
	{
		return GetClientClass( ) && GetClientClass( )->ClassID == EClassID::CTFTankBoss;
	}

	inline bool IsProjectile( )
	{
		if ( !GetClientClass( ) )
			return false;

		auto ClassID = GetClientClass( )->ClassID;
		switch ( ClassID )
		{
			case EClassID::CTFProjectile_Arrow:
			case EClassID::CTFProjectile_Cleaver:
			case EClassID::CTFProjectile_Flare:
			case EClassID::CTFProjectile_HealingBolt:
			case EClassID::CTFProjectile_Jar:
			case EClassID::CTFProjectile_JarGas:
			case EClassID::CTFProjectile_JarMilk:
			case EClassID::CTFProjectile_Rocket:
			case EClassID::CTFProjectile_SentryRocket:
			case EClassID::CTFGrenadePipebombProjectile: return true;
			default: return false;
		}
		return false;
	}

	template < typename T >
	inline T* As( )
	{
		return reinterpret_cast< T* >( this );
	}
};

class CBaseEntity : public IClientEntity
{
public:
	NETVAR( SimulationTime, float, "CBaseEntity", "m_flSimulationTime" );
	NETVAR( AnimationTime, float, "CBaseEntity", "m_flAnimTime" );
	NETVAR( TeamNum, int, "CBaseEntity", "m_iTeamNum" );
	NETVAR( Mins, Vector, "CBaseEntity", "m_vecMins" );
	NETVAR( Maxs, Vector, "CBaseEntity", "m_vecMaxs" );
	NETVAR( Origin, Vector, "CBaseEntity", "m_vecOrigin" );
	NETVAR( Critical, bool, "CTFWeaponBaseGrenadeProj", "m_bCritical" );
	NETVAR( ClientSideAnimation, bool, "CBaseAnimating", "m_bClientSideAnimation" );
	NETVAR( Thrower, int, "CBaseGrenade", "m_hThrower" );
	NETVAR( Owner, int, "CBaseGrenade", "m_hOwner" );
	NETVAR( OwnerEntity, int, "CBaseEntity", "m_hOwnerEntity" );
	NETVAR( InitialVelocity, Vector, "CTFWeaponBaseGrenadeProj", "m_vInitialVelocity" );
	NETVAR( Cycle, float, "CBaseAnimating", "m_flCycle" );
	NETVAR( Sequence, int, "CBaseAnimating", "m_nSequence" );
	NETVAR( PoseParameter, float*, "CBaseAnimating", "m_flPoseParameter" );
	NETVAR( PlaybackRate, float, "CBaseAnimating", "m_flPlaybackRate" );
	NETVAR( NextAttack, float, "CBaseCombatCharacter", "m_flNextAttack" );
	NETVAR( Effects, int, "CBaseEntity", "m_fEffects" );
	NETVAR( FlagStatus, int, "CCaptureFlag", "m_nFlagStatus" );
	NETVAR( ResetTime, float, "CCaptureFlag", "m_flResetTime" );
	NETVAR( StickyType, int, "CTFGrenadePipebombProjectile", "m_iType" );
	NETVAR( DefensiveBomb, int, "CTFGrenadePipebombProjectile", "m_bDefensiveBomb" );
	NETVAR( Touched, int, "CTFGrenadePipebombProjectile", "m_bTouched" );

	OFFSET_FROM( OldSimulationTime, float, "CBaseEntity", "m_flSimulationTime", sizeof( int ) );

	OFFSET_PURE( MoveType, EMoveType, 0x1A4 );
	OFFSET_PURE( PipebombType, int, 0x8FC );
	OFFSET_FROM( CreationTime, float, "CTFGrenadePipebombProjectile", "m_iType", 0x4 );

	Vector EstimateAbsVelocity()
	{
		static auto Func = Memory->FindSignature< void( __thiscall* )( CBaseEntity*, Vector& ) >("client.dll", "55 8B EC 83 EC ? 56 8B F1 E8 ? ? ? ? 3B");
		Vector v;
		Func(this, v);
		return v;
	}

	float GetStickyLiveTime()
	{
		static auto Func = Memory->FindSignature<float(__thiscall*)(CBaseEntity*)>("client.dll", "55 8B EC 83 EC ? A1 ? ? ? ? 56 8B F1 F3 0F 10 40 ? F3 0F 11 45 ? 8B 96");
		return Func(this);
	}

	Vector WorldSpaceCenter( )
	{
		Vector Temp = Origin( );
		Temp.z += ( Mins( ).z + Maxs( ).z ) * 0.5f;
		return Temp;
	}

	void UpdateVisibility( )
	{
		static auto FN = Memory->FindSignature<void( __thiscall* )( void* )>( "client.dll", "56 8B F1 E8 ? ? ? ? 8B 46 04 8D 4E 04 8B 40 0C FF D0 84 C0 74 2E" );
		FN( this );
	}

	inline void SetAbsOrigin( const Vector& origin )
	{
		static auto FN = Memory->FindSignature< void( __thiscall* )( CBaseEntity*, const Vector& ) >( "client.dll", "55 8B EC 56 57 8B F1 E8 ? ? ? ? 8B 7D 08 F3 0F 10 07" );
		return FN( this, origin );
	}

	inline void SetAbsAngles( Vector angles )
	{
		static auto FN = Memory->FindSignature< void( __thiscall* )( CBaseEntity*, const Vector& ) >( "client.dll", "55 8B EC 83 EC ? 56 57 8B F1 E8 ? ? ? ? 8B 7D ? F3 0F 10 07 0F 2E 86" );
		return FN( this, angles );
	}

	inline void InvalidateBoneCache( )
	{
		static auto FN = Memory->FindSignature< void( __thiscall* )( void* ) >( "client.dll", "A1 ? ? ? ? 48 C7 81 60 08 00 00 FF FF 7F FF" );
		return FN( this );
	}

	CUtlVector<Matrix3x4>* GetCachedBoneData( )
	{
		return reinterpret_cast< CUtlVector<Matrix3x4> * >( reinterpret_cast< DWORD >( this ) + 0x848 ); //0x844?
	}

	bool IsHealth( const FNV1A_t ModelHash );
	bool IsAmmo( const FNV1A_t ModelHash );
	bool IsPowerup( const FNV1A_t ModelHash );
	bool IsSentryBuster( );
	const wchar_t* GetPowerupName( const FNV1A_t ModelHash );
	Color GetColor( int colorscheme );
	Color GetOccludedColor( int colorscheme );
	CBaseEntity* GetOwnerEntity( );
	CBasePlayer* GetThrower( );
	CBaseEntity* MoveChild( );
	CBaseEntity* NextMovePeer( );
	CBaseEntity* MoveParent( );
	bool IsEntityOnScreen( );
	const wchar_t* GetProjectileName( );
	float GetIntelReturnTime( );
	const wchar_t* GetIntelStatus( );
	bool IsEnemy( );

	Color GetESPColor( int ColorScheme );
	Color GetModelColor( int ColorScheme );
	Color GetModelsColor( int ColorScheme, bool Occluded );
	Color GetOutlinesColor( int ColorScheme );
};

class CTFRagdoll : public CBaseEntity
{
public:
	OFFSET_PURE( Player, unsigned int, 0xC88 );
	OFFSET_PURE( Gib, bool, 0xc91 );
	OFFSET_PURE( Burning, bool, 0xc92 );
	OFFSET_PURE( Electrocuted, bool, 0xc93 );
	OFFSET_PURE( Dissolving, bool, 0xc95 );
	OFFSET_PURE( FeignDeath, bool, 0xc96 );
	OFFSET_PURE( Cloaked, bool, 0xc98 );
	OFFSET_PURE( BecomeAsh, bool, 0xc99 );
	OFFSET_PURE( GoldRagdoll, bool, 0xca0 );
	OFFSET_PURE( IceRagdoll, bool, 0xca1 );
	OFFSET_PURE( Team, int, 0xcbc );
};

class CBaseCombatWeapon;
using MyWeapons = std::array<CHandle<CBaseCombatWeapon>, 48>;

class CBasePlayer : public CBaseEntity
{
public:
	NETVAR( LifeState, uint8_t, "CBasePlayer", "m_lifeState" );
	NETVAR( Health, int, "CBasePlayer", "m_iHealth" );
	NETVAR( ViewOffset, Vector, "CBasePlayer", "m_vecViewOffset[0]" );
	NETVAR( Ducked, bool, "CBasePlayer", "m_bDucked" );
	NETVAR( ModelScale, float, "CBaseAnimating", "m_flModelScale" );
	NETVAR( FallVelocity, float, "CBasePlayer", "m_flFallVelocity");
	NETVAR( Ducking, bool, "CBasePlayer", "m_bDucking" );
	NETVAR( Velocity, Vector, "CBasePlayer", "m_vecVelocity[0]" );
	NETVAR( BaseVelocity, Vector, "CBasePlayer", "m_vecBaseVelocity[0]" );
	NETVAR( GroundEntity, int, "CBasePlayer", "m_hGroundEntity" );
	NETVAR( Flags, int, "CBasePlayer", "m_fFlags" );
	NETVAR( InDuckJump, bool, "CBasePlayer", "m_bInDuckJump" );
	NETVAR( Ducktime, float, "CBasePlayer", "m_flDucktime" );
	NETVAR( DuckJumpTime, float, "CBasePlayer", "m_flDuckJumpTime" );
	NETVAR( hActiveWeapon, CHandle<CBaseCombatWeapon>, "CBaseCombatCharacter", "m_hActiveWeapon" );
	NETVAR( PunchAngle, Vector, "CBasePlayer", "m_vecPunchAngle" );
	NETVAR( PunchAngleVel, Vector, "CBasePlayer", "m_vecPunchAngleVel" );
	NETVAR( HitboxSet, int, "CBaseAnimating", "m_nHitboxSet" );
	NETVAR( NextAttack, float, "CBaseCombatCharacter", "m_flNextAttack" );
	NETVAR( TickBase, int, "CBasePlayer", "m_nTickBase" );
	NETVAR( DamageDone, int, "CTFPlayer", "m_iDamageDone" );
	NETVAR( Crits, int, "CTFPlayer", "m_iCrits" );
	NETVAR( CritMult, int, "CTFPlayer", "m_iCritMult" );
	NETVAR( AirDash, int, "CTFPlayer", "m_iAirDash" );
	NETVAR( Headshots, int, "CTFPlayer", "m_iHeadshots" );
	NETVAR( Class, int, "CTFPlayer", "m_iClass" );
	NETVAR( PlayerCond, int, "CTFPlayer", "m_nPlayerCond" );
	NETVAR( condition_bits, int, "CTFPlayer", "_condition_bits" );
	NETVAR( PlayerCondEx, int, "CTFPlayer", "m_nPlayerCondEx" );
	NETVAR( PlayerCondEx2, int, "CTFPlayer", "m_nPlayerCondEx2" );
	NETVAR( PlayerCondEx3, int, "CTFPlayer", "m_nPlayerCondEx3" );
	NETVAR( PlayerCondEx4, int, "CTFPlayer", "m_nPlayerCondEx4" );
	NETVAR( FOV, int, "CBasePlayer", "m_iFOV" );
	NETVAR( HideHUD, int, "CBasePlayer", "m_iHideHUD" ); // idea.jpg -- Mad?
	NETVAR( WaterLevel, BYTE, "CTFPlayer", "m_nWaterLevel" );
	NETVAR( ForceTauntCam, int, "CTFPlayer", "m_nForceTauntCam" );
	NETVAR( EyeAngles, Vector, "CTFPlayer", "m_angEyeAngles[0]" );
	NETVAR( NextThinkTick, int, "CBasePlayer", "m_nNextThinkTick" );
	NETVAR( Kills, int, "CTFPlayer", "m_iKills" );
	NETVAR( Deaths, int, "CTFPlayer", "m_iDeaths" );
	NETVAR( Healers, int, "CTFPlayer", "m_nNumHealers" );
	NETVAR( ObserverMode, int, "CBasePlayer", "m_iObserverMode" );
	NETVAR( ObserverTarget, int, "CBasePlayer", "m_hObserverTarget" );
	NETVAR( hMyWeapons, MyWeapons, "CBaseCombatCharacter", "m_hMyWeapons" );
	NETVAR( hConstraintEntity, CHandle<CBaseEntity>, "CBasePlayer", "m_hConstraintEntity" );
	NETVAR( ConstraintCenter, Vector, "CBasePlayer", "m_vecConstraintCenter" );
	NETVAR( ConstraintRadius, float, "CBasePlayer", "m_flConstraintRadius" );
	NETVAR( ConstraintWidth, float, "CBasePlayer", "m_flConstraintWidth" );
	NETVAR( ConstraintSpeedFactor, float, "CBasePlayer", "m_flConstraintSpeedFactor" );
	NETVAR( m_flMaxspeed, float, "CBasePlayer", "m_flMaxspeed" );

	int& Buttons( )
	{
		static int nOffset = NetVars->GetNetVar( "CBasePlayer", "m_hConstraintEntity" ) - 8;
		return *reinterpret_cast< int* >( reinterpret_cast< DWORD >( this ) + nOffset );
	}

	inline bool IsDucked( )
	{
		return Flags( ) & FL_DUCKING;
	}

	OFFSET_PURE( CurrentCommand, CUserCmd*, 0x1184 );
	OFFSET_FROM( AnimState, CTFPlayerAnimState*, "CTFPlayer", "m_hItem", -0x34 );
	OFFSET_PURE( ModelPtr, CStudioHdr*, 0x890 );
	/*
	TODO: replace with m_hItem - 0x34 or 52 //thanks spook and yani
	*/
	OFFSET_FROM( m_nTauntSequence, int, "CTFPlayer", "m_hHighFivePartner", 0x24 );
	OFFSET_FROM( m_flTauntStartTime, float, "CTFPlayer", "m_hHighFivePartner", 0x28 );
	OFFSET_FROM( m_flTauntDuration, float, "CTFPlayer", "m_hHighFivePartner", 0x2C );
	OFFSET_PURE( TankPressure, float, 0x1B40 );

	inline void SelectItem( const char* ptr, int subtype )
	{
		return Memory->GetVFunc<void( __thiscall* )( void*, const char*, int )>( this, 271u )( this, ptr, subtype );
	}

	C_AnimationLayer* GetAnimOverlay( int i )
	{
		static auto FN = Memory->FindSignature< C_AnimationLayer * ( __thiscall* )( void*, int )>( "client.dll", "55 8B EC 6B 45 ? ? 03 81 ? ? ? ? 5D C2" );
		return FN( this, i );
	}

	//55 8B EC 83 EC ? 53 56 8B 35 ? ? ? ? 8B CE 57 8B 06 FF 50 ? 68
	//jod bless ida decompiler
	CTFPlayerAnimState* CreateTFAnimState( )
	{
		static auto FN = Memory->FindSignature< CTFPlayerAnimState * ( __cdecl* )( void* )>( "client.dll", "55 8B EC 83 EC ? 53 56 8B 35 ? ? ? ? 8B CE 57 8B 06 FF 50 ? 68" );
		return FN( this );
	}

	int& SurfaceProps( )
	{
		static int nOffset = 4768;
		return *reinterpret_cast< int* >( reinterpret_cast< DWORD >( this ) + nOffset );
	}

	void*& pSurfaceData( )
	{
		static int nOffset = 4772;
		return *reinterpret_cast< void** >( reinterpret_cast< DWORD >( this ) + nOffset );
	}

	float& SurfaceFriction( )
	{
		static int nOffset = 4776;
		return *reinterpret_cast< float* >( reinterpret_cast< DWORD >( this ) + nOffset );
	}

	char& CHTextureType( )
	{
		static int nOffset = 4780;
		return *reinterpret_cast< char* >( reinterpret_cast< DWORD >( this ) + nOffset );
	}

	int SelectWeightedSequence( int iGestureActivity )
	{
		static auto FN = Memory->FindSignature<int( __thiscall* )( void*, int )>( "client.dll", "55 8B EC 56 8B F1 80 BE ? ? ? ? ? 74 ? 6A ? FF 75 ? 33 C0 50 E8 ? ? ? ? 83 C4 ? 5E 5D C2" );
		return FN( this, iGestureActivity );
	}

	float SequenceDuration( CStudioHdr* hdr, int iSequence )
	{
		static auto FN = Memory->FindSignature<float( __thiscall* )( void*, CStudioHdr *, int )>( "client.dll", "55 8B EC 56 8B 75 ? 57 8B F9 85 F6 74 ? 8B CE E8 ? ? ? ? 84 C0 74 ? 8B CE E8 ? ? ? ? 8B 4D ? 3B C8 7D ? 85 C9 78 ? 8D 87 ? ? ? ? 50 51 56 E8 ? ? ? ? 83 C4 ? 5F 5E 5D C2 ? ? 51 68 ? ? ? ? 6A ? FF 15 ? ? ? ? 83 C4 ? D9 05 ? ? ? ? 5F 5E 5D C2" );
		return FN( this, hdr, iSequence );
	}

	float FrameAdvance( float interval )
	{
		static auto FN = Memory->FindSignature<float( __thiscall* )( void*, float )>( "client.dll", "55 8B EC 83 EC 14 56 8B F1 57 80 BE ? ? ? ? ? 0F 85 ? ? ? ? 83 BE ? ? ? ? ? 75 05" );
		return FN( this, interval );
	}

	void UpdateWearables( )
	{
		static auto FN = Memory->FindSignature<void( __thiscall* )( void* )>( "client.dll", "56 8B F1 E8 ? ? ? ? 8B 06 8B CE 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 FF 90 ? ? ? ? 50 E8 ? ? ? ? 83 C4 14 5E 85 C0 74 0A 8B 10 8B C8 FF A2 6C 01 00 00 C3" );
		FN( this );
	}

	int LookupSequence( CStudioHdr* hdr, const char* label );
	const char* SequenceLabel( );

	bool HasHighKD( )
	{
		int deaths = Deaths( ) ? Deaths( ) : 1;
		return ( this->Kills( ) / ( float )deaths ) > 2.f && this->Kills( ) > 6;
	}

	bool IsCritBoosted()
	{
		int nCond = PlayerCond(), nCondEx = PlayerCondEx();

		return (nCond & TFCond_Kritzkrieged ||
			condition_bits( ) & TFCond_Kritzkrieged ||
			nCondEx & TFCondEx_CritCanteen ||
			nCondEx & TFCondEx_CritOnFirstBlood ||
			nCondEx & TFCondEx_CritOnWin ||
			nCondEx & TFCondEx_CritOnKill ||
			nCondEx & TFCondEx_CritDemoCharge ||
			nCondEx & TFCondEx_CritOnFlagCapture ||
			nCondEx & TFCondEx_HalloweenCritCandy ||
			nCondEx & TFCondEx_PyroCrits);
	}

	bool IsLocalPlayer( );

	inline bool IsMiniCritBoosted( ) {
		return this->PlayerCond( ) & TFCond_MiniCrits;
	}

	inline bool IsBonked( ) {
		return this->PlayerCond( ) & TFCond_Bonked;
	}

	inline bool IsCharging( ) {
		return this->PlayerCond( ) & TFCond_Charging;
	}

	inline bool IsTaunting( ) {
		return this->PlayerCond( ) & TFCond_Taunting;
	}

	inline bool IsMarkedForDeath( ) {
		return this->PlayerCond( ) & TFCond_Jarated
			|| this->PlayerCond( ) & TFCond_MarkedForDeath
			|| this->PlayerCondEx( ) & TFCondEx_MarkedForDeathSilent;
	}

	inline bool IsStunned( ) {
		return this->PlayerCond( ) & TFCond_Stunned;
	}

	inline bool IsInKart( ) {
		return this->PlayerCondEx( ) & TFCondEx_InKart;
	}

	inline bool IsGhost( ) {
		return this->PlayerCondEx2( ) & TFCondEx2_HalloweenGhostMode;
	}

	inline bool IsCloaked( ) {
		return this->PlayerCond( ) & TFCond_Cloaked
			|| this->PlayerCond( ) & TFCond_CloakFlicker
			|| this->PlayerCondEx2( ) & TFCondEx2_Stealthed
			|| this->PlayerCondEx2( ) & TFCondEx2_StealthedUserBuffFade;
	}

	inline bool IsInvulnerable( ) {
		return this->PlayerCond( ) & TFCond_Ubercharged
		//|| PlayerCond() & TFCond_UberchargeFading
		//|| this->PlayerCondEx() & TFCondEx_UberchargedHidden
		|| this->PlayerCondEx() & TFCondEx_UberchargedCanteen;
	}

	inline bool IsZoomed( ) {
		return this->PlayerCond( ) & TFCond_Zoomed;
	}

	inline bool IsOverhealed( ) {
		return this->Health( ) > this->GetMaxHealth( );
	}

	inline bool IsQuickFixUber( ) {
		return this->PlayerCond( ) & TFCond_MegaHeal;
	}

	inline bool IsInConcherorRange( ) {
		return this->PlayerCond( ) & TFCond_RegenBuffed;
	}

	inline bool IsInBattalionsRange( ) {
		return this->PlayerCond( ) & TFCond_DefenseBuffed;
	}

	inline bool IsInBuffBannerRange( ) {
		return this->PlayerCond( ) & TFCond_Buffed;
	}

	inline bool IsBleeding( ) {
		return this->PlayerCond( ) & TFCond_Bleeding;
	}

	inline bool IsOnFire( )
	{
		return this->PlayerCond( ) & TFCond_OnFire || this->PlayerCondEx3( ) & ( 1 << 28 );
	}

	inline bool IsCoveredInMilk( ) {
		return this->PlayerCond( ) & TFCond_Milked;
	}

	inline bool IsSlowed( ) {
		return this->PlayerCond( ) & TFCond_Slowed;
	}

	inline bool IsBulletResistant( ) {
		return this->PlayerCondEx( ) & TFCondEx_BulletResistance;
	}

	inline bool IsExplosiveResistant( ) {
		return this->PlayerCondEx( ) & TFCondEx_ExplosiveResistance;
	}

	inline bool IsFireResistant( ) {
		return this->PlayerCondEx( ) & TFCondEx_FireResistance;
	}

	inline bool IsBulletCharged( ) {
		return this->PlayerCondEx( ) & TFCondEx_BulletCharge;
	}

	inline bool IsExplosiveCharged( ) {
		return this->PlayerCondEx( ) & TFCondEx_ExplosiveCharge;
	}

	inline bool IsFireCharged( ) {
		return this->PlayerCondEx( ) & TFCondEx_FireCharge;
	}

	bool IsInCondition( const ETFCond& Condition )
	{
		const int Cond = static_cast< int >( Condition );
		switch ( Cond / 32 )
		{
			case 0:
			{
				const int bit = ( 1 << Cond );
				if ( ( PlayerCond( ) & bit ) == bit )
				{
					return true;
				}

				if ( ( condition_bits( ) & bit ) == bit )
				{
					return true;
				}

				break;
			}
			case 1:
			{
				const int bit = 1 << ( Cond - 32 );
				if ( ( PlayerCondEx( ) & bit ) == bit )
				{
					return true;
				}

				break;
			}
			case 2:
			{
				const int bit = 1 << ( Cond - 64 );
				if ( ( PlayerCondEx2( ) & bit ) == bit )
				{
					return true;
				}

				break;
			}
			case 3:
			{
				const int bit = 1 << ( Cond - 96 );
				if ( ( PlayerCondEx3( ) & bit ) == bit )
				{
					return true;
				}

				break;
			}
			case 4:
			{
				const int bit = 1 << ( Cond - 128 );
				if ( ( PlayerCondEx4( ) & bit ) == bit )
				{
					return true;
				}

				break;
			}
			default:
				break;
		}

		return false;
	}

	bool IsAlive() 
	{ 
		return LifeState() == LIFE_ALIVE; 
	}

	bool IsOnGround( )
	{
		return Flags( ) & FL_ONGROUND;
	}

	inline bool IsSteamFriend( )
	{
		using Fn = bool( __stdcall* )( CBasePlayer* );
		static auto pattern = Memory->FindSignature<void*>( "client.dll",
														   "55 8B EC 81 EC ? ? ? ? E8 ? ? ? ? 85 C0 74 7E 8B 4D 08 85 C9 74 77 A1 ? ? ? ? 83 78 08 00 74 6C 83 78 0C 00 74 66 A1 ? ? ? ? 83 C1 08 56 8B 30 8D 85 ? ? ? ? 50 8B 01" );
		return reinterpret_cast< Fn >( pattern )( this );
	}

	CSteamID GetSteamID( );

	Vector EyePosition()
	{
		return Origin() + ViewOffset();
	}
	int GetMaxHealth();
	bool PhysicsRunTick( int Method );
	void SetNextThink( float Time, const char* context );
	int GetNextThink( const char* context );
	void PreThink( );
	void PostThink( );
	void Think( );
	Vector GetHitboxPos(int Hitbox);
	Vector GetHitboxPos(int Hitbox, Matrix3x4* Matrix, model_t* model);
	CBaseCombatWeapon* ActiveWeapon();
	float MaxSpeed( bool bIgnoreSpecialAbility = false );
	CBaseCombatWeapon* GetWeaponFromSlot( int slot );
	const char* GetWeaponName( );
	int GetAmmoCount(int iAmmoIndex);
	void UpdateClientSideAnimation( );

	Color PlayerESPColor( int ColorScheme );
	Color PlayerModelColor( int ColorScheme );
	Color PlayerModelsColor( int ColorScheme, bool Occluded );
	Color PlayerOutlinesColor( int ColorScheme );
	Color GetBeamsColor( int ColorScheme );
};

typedef enum
{
	EMPTY,
	SINGLE,
	SINGLE_NPC,
	WPN_DOUBLE,
	DOUBLE_NPC,
	BURST,
	RELOAD,
	RELOAD_NPC,
	MELEE_MISS,
	MELEE_HIT,
	MELEE_HIT_WORLD,
	SPECIAL1,
	SPECIAL2,
	SPECIAL3,
	TAUNT,
	DEPLOY,
	NUM_SHOOT_SOUND_TYPES,
} WeaponSound_t;

#define MAX_SHOOT_SOUNDS			16
#define MAX_WEAPON_STRING			80
#define MAX_WEAPON_PREFIX			16
#define MAX_WEAPON_AMMO_NAME		32
#define WEAPON_PRINTNAME_MISSING	"!!! Missing printname on weapon"

struct CHudTexture;
class FileWeaponInfo_t
{
public:
	FileWeaponInfo_t( );
	virtual void Parse( void* pKeyValuesData, const char* szWeaponName );
public:
	bool ParsedScript;
	bool LoadedHudElements;
	char ClassName[ MAX_WEAPON_STRING ];
	char PrintName[ MAX_WEAPON_STRING ];
	char ViewModel[ MAX_WEAPON_STRING ];
	char WorldModel[ MAX_WEAPON_STRING ];
	char AnimationPrefix[ MAX_WEAPON_PREFIX ];
	int Slot;
	int Position;
	int MaxClip1;
	int MaxClip2;
	int DefaultClip1;
	int DefaultClip2;
	int Weight;
	int RumbleEffect;
	bool AutoSwitchTo;
	bool AutoSwitchFrom;
	int Flags;
	char Ammo1[ MAX_WEAPON_AMMO_NAME ];
	char Ammo2[ MAX_WEAPON_AMMO_NAME ];

	char aShootSounds[ NUM_SHOOT_SOUND_TYPES ][ MAX_WEAPON_STRING ];

	int AmmoType;
	int Ammo2Type;
	bool MeleeWeapon;
	bool BuiltRightHanded;
	bool AllowFlipping;

	int SpriteCount;
	CHudTexture* iconActive;
	CHudTexture* iconInactive;
	CHudTexture* iconAmmo;
	CHudTexture* iconAmmo2;
	CHudTexture* iconCrosshair;
	CHudTexture* iconAutoaim;
	CHudTexture* iconZoomedCrosshair;
	CHudTexture* iconZoomedAutoaim;
	CHudTexture* iconSmall;

	bool ShowUsageHint;	
};

struct WeaponData_t
{
	int	Damage;
	int	BulletsPerShot;
	float Range;
	float Spread;
	float PunchAngle;
	float TimeFireDelay;
	float TimeIdle;
	float TimeIdleEmpty;
	float TimeReloadStart;
	float TimeReload;
	bool DrawCrosshair;
	int	Projectile;
	int	AmmoPerShot;
	float ProjectileSpeed;
	float SmackDelay;
	bool UseRapidFireCrits;

	void Init( void )
	{
		Damage = 0;
		BulletsPerShot = 0;
		Range = 0.0f;
		Spread = 0.0f;
		PunchAngle = 0.0f;
		TimeFireDelay = 0.0f;
		TimeIdle = 0.0f;
		TimeIdleEmpty = 0.0f;
		TimeReloadStart = 0.0f;
		TimeReload = 0.0f;
		Projectile = 0;
		AmmoPerShot = 0;
		ProjectileSpeed = 0.0f;
		SmackDelay = 0.0f;
		UseRapidFireCrits = false;
	};
};

class CTFWeaponInfo : public FileWeaponInfo_t
{
public:
	CTFWeaponInfo( );
	~CTFWeaponInfo( );

	virtual void Parse( void* pKeyValuesData, const char* szWeaponName );
	WeaponData_t const& GetWeaponData( int iWeapon ) const { return WeaponData[ iWeapon ]; }
public:
	WeaponData_t	WeaponData[ 2 ];
	int		WeaponType;
	bool	Grenade;
	float	DamageRadius;
	float	PrimerTime;
	bool	LowerWeapon;
	bool	SuppressGrenTimer;
	bool	HasTeamSkins_Viewmodel;
	bool	HasTeamSkins_Worldmodel;
	char	MuzzleFlashModel[ 128 ];
	float	MuzzleFlashModelDuration;
	char	MuzzleFlashParticleEffect[ 128 ];
	char	TracerEffect[ 128 ];
	bool	DoInstantEjectBrass;
	char	BrassModel[ 128 ];
	char	ExplosionSound[ 128 ];
	char	ExplosionEffect[ 128 ];
	char	ExplosionPlayerEffect[ 128 ];
	char	ExplosionWaterEffect[ 128 ];
	bool	DontDrop;
};

class CBaseCombatWeapon : public CBaseEntity
{
public:
	NETVAR( NextPrimaryAttack, float, "CBaseCombatWeapon", "m_flNextPrimaryAttack" );
	NETVAR( TimeWeaponIdle, float, "CBaseCombatWeapon", "m_flTimeWeaponIdle " );
	NETVAR( NextSecondaryAttack, float, "CBaseCombatWeapon", "m_flNextSecondaryAttack" );
	NETVAR( Clip1, int, "CBaseCombatWeapon", "m_iClip1" );
	NETVAR( ItemDefinitionIndex, int, "CEconEntity", "m_iItemDefinitionIndex" );
	NETVAR( ChargedDamage, float, "CTFSniperRifle", "m_flChargedDamage" );
	NETVAR( LastFireTime, float, "CTFWeaponBase", "m_flLastFireTime" );
	NETVAR( ChargeBeginTime, float, "CTFPipebombLauncher", "m_flChargeBeginTime" );
	NETVAR( DetonateTime, float, "CWeaponGrenadeLauncher", "m_flDetonateTime" );
	NETVAR( hHealingTarget, unsigned int, "CWeaponMedigun", "m_hHealingTarget" );
	NETVAR( ChargeResistType, int, "CWeaponMedigun", "m_nChargeResistType" );
	NETVAR( ChargeLevel, float, "CWeaponMedigun", "m_flChargeLevel" );
	NETVAR( PrimaryAmmoType, int, "CBaseCombatWeapon", "m_iPrimaryAmmoType" );
	NETVAR( ObservedCritChance, float, "CTFWeaponBase", "m_flObservedCritChance" );
	NETVAR( ReloadMode, int, "CTFWeaponBase", "m_iReloadMode" );

	OFFSET_PURE( MinigunState, int, 0xC48 );
	OFFSET_PURE( CritTokenBucket, float, 0xA54 );
	OFFSET_PURE( CritChecks, int, 0xA58 );
	OFFSET_PURE( CritSeedRequests, int, 0xA5C );
	OFFSET_PURE( WeaponMode, int, 0xB24 );
	OFFSET_PURE( CritTime, float, 0xB50 );
	OFFSET_PURE( LastCritCheckTime, float, 0xB54 );
	OFFSET_PURE( LastCritCheckFrame, int, 0xB58 );
	OFFSET_PURE( LastRapidfireCritCheckTime, float, 0xB60 );
	OFFSET_PURE( Clip2, int, 0xAAC );
	OFFSET_PURE( CritShot, bool, 0xB36 );
	OFFSET_PURE( RandomSeed, int, 0xB5C);

	inline int WeaponID( )
	{
		return Memory->GetVFunc<int( __thiscall* )( void* )>( this, 381u )( this );
	}

	inline const char* GetName( )
	{
		return Memory->GetVFunc<const char* ( __thiscall* )( void* )>( this, 331u )( this );
	}

	inline int DamageType( )
	{
		return Memory->GetVFunc<int( __thiscall* )( void* )>( this, 340 )( this );
	}

	inline int Slot( )
	{
		if ( !this ) return -1;

		return Memory->GetVFunc<int( __thiscall* )( void* )>( this, 330u )( this );
	}

	inline bool GetAttachment( int number, Vector& origin )
	{
		return Memory->GetVFunc<bool( __thiscall* )( void*, int, Vector& )>( this, 71u )( this, number, origin );
	}

	inline void AbortReload( )
	{
		return Memory->GetVFunc<void( __thiscall* )( void* )>( this, 279u )( this );
	}

	inline void UpdateAllViewmodelAddons( )
	{
		return Memory->FindSignature< void( __thiscall* )( void* ) >( "client.dll", "55 8B EC 83 EC 08 56 57 8B F1 E8 ? ? ? ? 8B F8 85 FF 0F 84 ? ? ? ? 8B 17" )( this );
	}

	bool IsRapidFireCrits()
	{
		switch ( ItemDefinitionIndex( ) )
		{
			case Scout_s_PrettyBoysPocketPistol:
				return true;
		}

		switch (WeaponID())
		{
		case TF_WEAPON_MINIGUN:
		case TF_WEAPON_SYRINGEGUN_MEDIC:
		case TF_WEAPON_FLAMETHROWER:
		case TF_WEAPON_PISTOL:
		case TF_WEAPON_PISTOL_SCOUT:
		case TF_WEAPON_SMG:
		case TF_WEAPON_FLAREGUN:
			return true;
		}

		switch (ItemDefinitionIndex())
		{
		case Scout_m_TheShortstop:
		case Scout_s_ScoutsPistol:
		case Scout_s_PistolR:
		case Scout_s_PrettyBoysPocketPistol:
			return true;
		}

		return false;
	}

	bool IsSingleShot( )
	{
		switch ( WeaponID( ) )
		{
			case TF_WEAPON_PISTOL:
			case TF_WEAPON_PISTOL_SCOUT:
			case TF_WEAPON_SMG:
			case TF_WEAPON_SNIPERRIFLE:
			case TF_WEAPON_SNIPERRIFLE_DECAP:
			case TF_WEAPON_SNIPERRIFLE_CLASSIC:
			{
				return true;
			}
			default: return false;
		}
		return false;
	}

	CBasePlayer* HealingTarget( );
	bool CanFireCritical(bool HeadShot);
	bool InReload();
	bool CanFire( );
	bool CanFireSecondary( );
	float SmackTime();
	void InvokeAttackHandler();
	bool IsCriticalHelper();
	const char* GetWeaponName( );
	WeaponData_t GetWeaponData( int m_iWeaponMode = 0 );
	CTFWeaponInfo* GetWeaponInfo( );
	bool IsTauntable( );
	bool IsAttacking( CUserCmd* Cmd );
	bool CanHeadshot( );
	void GetSpreadAngles( Vector& out );
	float GetWeaponSpread( );
	bool IsSpreadWeapon( );
	bool IsForceANature( )
	{
		return ItemDefinitionIndex( ) == Scout_m_ForceANature || ItemDefinitionIndex( ) == Scout_m_FestiveForceANature;
	}

	Vector GetSpreadAngles( )
	{
		Vector Out;
		GetSpreadAngles( Out );
		return Out;
	}
};

class CBaseObject : public CBaseEntity
{
public:
	NETVAR( Health, int, "CBaseObject", "m_iHealth" );
	NETVAR( MaxHealth, int, "CBaseObject", "m_iMaxHealth" );
	NETVAR( UpgradeLevel, int, "CBaseObject", "m_iUpgradeLevel" );
	NETVAR( ObjectMode, int, "CBaseObject", "m_iObjectMode" );
	NETVAR( RechargeTime, float, "CObjectTeleporter", "m_flRechargeTime" );
	NETVAR( State, int, "CObjectTeleporter", "m_iState" );
	NETVAR( MiniBuilding, bool, "CBaseObject", "m_bMiniBuilding" );
	NETVAR( Builder, int, "CBaseObject", "m_hBuilder" );
	NETVAR( Placing, bool, "CBaseObject", "m_bPlacing" );

	CBasePlayer* GetBuilder( );

	inline const wchar_t* GetBuildingName( )
	{
		switch ( GetClientClass( )->ClassID )
		{
			case EClassID::CObjectSentrygun:
			{
				if ( this->MiniBuilding( ) )
				{
					return L"Mini Sentry";
				}
				else
				{
					return L"Sentry";
				}
			}
			case EClassID::CObjectDispenser:
			{
				return L"Dispenser";
			}
			case EClassID::CObjectTeleporter:
			{
				return L"Teleporter";
			}
			default: return L"";
		}

		return L"";
	}

	inline bool IsTeleporterAndRecharging( )
	{
		return GetClientClass( )->ClassID == EClassID::CObjectTeleporter && State( ) == TELEPORTER_STATE_RECHARGING;
	}

	Color BuildingESPColor( int ColorScheme );
	Color BuildingModelColor( int ColorScheme );
	Color BuildingModelsColor( int ColorScheme, bool Occluded );
	Color BuildingOutlinesColor( int ColorScheme );
};

class CPlayerResource
{
public:
	NETVAR( m_iPing, void*, "CPlayerResource", "m_iPing" );
	NETVAR( m_iScore, void*, "CPlayerResource", "m_iScore" );
	NETVAR( m_iDeaths, void*, "CPlayerResource", "m_iDeaths" );
	NETVAR( m_bConnected, void*, "CPlayerResource", "m_bConnected" );
	NETVAR( m_iTeam, void*, "CPlayerResource", "m_iTeam" );
	NETVAR( m_bAlive, BYTE*, "CPlayerResource", "m_bAlive" );
	NETVAR( m_iHealth, void*, "CPlayerResource", "m_iHealth" );
	NETVAR( m_iAccountID, void*, "CPlayerResource", "m_iAccountID" );
	NETVAR( m_bValid, void*, "CPlayerResource", "m_bValid" );
	NETVAR( m_iUserID, void*, "CPlayerResource", "m_iUserID" );
};

class CTFPlayerResource : public CPlayerResource
{
public:
	NETVAR( m_iTotalScore, void*, "CTFPlayerResource", "m_iTotalScore" );
	NETVAR( m_iMaxHealth, void*, "CTFPlayerResource", "m_iMaxHealth" );
	NETVAR( m_iMaxBuffedHealth, void*, "CTFPlayerResource", "m_iMaxBuffedHealth" );
	NETVAR( m_iPlayerClass, void*, "CTFPlayerResource", "m_iPlayerClass" );
	NETVAR( m_bArenaSpectator, void*, "CTFPlayerResource", "m_bArenaSpectator" );
	NETVAR( m_iActiveDominations, void*, "CTFPlayerResource", "m_iActiveDominations" );
	NETVAR( m_flNextRespawnTime, void*, "CTFPlayerResource", "m_flNextRespawnTime" );
	NETVAR( m_iChargeLevel, void*, "CTFPlayerResource", "m_iChargeLevel" );
	NETVAR( m_iDamage, void*, "CTFPlayerResource", "m_iDamage" );
	NETVAR( m_iDamageAssist, void*, "CTFPlayerResource", "m_iDamageAssist" );
	NETVAR( m_iDamageBoss, void*, "CTFPlayerResource", "m_iDamageBoss" );
	NETVAR( m_iHealing, void*, "CTFPlayerResource", "m_iHealing" );
	NETVAR( m_iHealingAssist, void*, "CTFPlayerResource", "m_iHealingAssist" );
	NETVAR( m_iDamageBlocked, void*, "CTFPlayerResource", "m_iDamageBlocked" );
	NETVAR( m_iCurrencyCollected, void*, "CTFPlayerResource", "m_iCurrencyCollected" );
	NETVAR( m_iBonusPoints, void*, "CTFPlayerResource", "m_iBonusPoints" );
	NETVAR( m_iPlayerLevel, void*, "CTFPlayerResource", "m_iPlayerLevel" );
	NETVAR( m_iStreaks, void*, "CTFPlayerResource", "m_iStreaks" );
	NETVAR( m_iUpgradeRefundCredits, void*, "CTFPlayerResource", "m_iUpgradeRefundCredits" );
	NETVAR( m_iBuybackCredits, void*, "CTFPlayerResource", "m_iBuybackCredits" );
	NETVAR( m_iPartyLeaderRedTeamIndex, int, "CTFPlayerResource", "m_iPartyLeaderRedTeamIndex" );
	NETVAR( m_iPartyLeaderBlueTeamIndex, int, "CTFPlayerResource", "m_iPartyLeaderBlueTeamIndex" );
	NETVAR( m_iEventTeamStatus, int, "CTFPlayerResource", "m_iEventTeamStatus" );
	NETVAR( m_iPlayerClassWhenKilled, void*, "CTFPlayerResource", "m_iPlayerClassWhenKilled" );
	NETVAR( m_iConnectionState, void*, "CTFPlayerResource", "m_iConnectionState" );
	NETVAR( m_flConnectTime, void*, "CTFPlayerResource", "m_flConnectTime" );

	/* CPlayerResource */

	int GetPing( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_iPing" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	int GetKills( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_iScore" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	int GetDeaths( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_iDeaths" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	bool GetConnected( int idx )
	{
		if ( !this ) { return false; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_bConnected" );
		return *reinterpret_cast< bool* >( this + offset + idx );
	}

	int GetTeam( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_iTeam" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	bool IsAlive( int idx )
	{
		if ( !this ) { return false; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_bAlive" );
		return *reinterpret_cast< bool* >( this + offset + idx );
	}

	int GetHealth( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_iHealth" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	unsigned GetAccountID( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_iAccountID" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	bool GetValid( int idx )
	{
		if ( !this ) { return false; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_bValid" );
		return *reinterpret_cast< bool* >( this + offset + idx );
	}

	int GetUserID( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_iUserID" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	const char* GetPlayerName( int idx )
	{
		if ( !this ) { return ""; }
		return *reinterpret_cast< const char** >( this + 0x0554 + 4 * idx );
	}

	/* CTFPlayerResource */

	int GetScore( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CPlayerResource", "m_iTotalScore" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	int GetLevel( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CTFPlayerResource", "m_iPlayerLevel" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	int GetDamage( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CTFPlayerResource", "m_iDamage" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	int GetClass( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CTFPlayerResource", "m_iPlayerClass" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	int GetMaxHealth( int idx )
	{
		if ( !this ) { return 0; }
		static auto offset = NetVars->GetNetVar( "CTFPlayerResource", "m_iMaxHealth" );
		return *reinterpret_cast< int* >( this + offset + 4 * idx );
	}

	int* GetStreaks( int idx )
	{
		if ( !this ) { return nullptr; }
		static auto offset = NetVars->GetNetVar( "CTFPlayerResource", "m_iStreaks" );
		return reinterpret_cast< int* >( this + offset + 4 * idx );
	}
};

class IInterpolatedVar
{
public:
	virtual		 ~IInterpolatedVar( ) {}

	virtual void Setup( void* pValue, int type ) = 0;
	virtual void SetInterpolationAmount( float seconds ) = 0;

	// Returns true if the new value is different from the prior most recent value.
	virtual void NoteLastNetworkedValue( ) = 0;
	virtual bool NoteChanged( float changetime, bool bUpdateLastNetworkedValue ) = 0;
	virtual void Reset( ) = 0;

	// Returns 1 if the value will always be the same if currentTime is always increasing.
	virtual int Interpolate( float currentTime ) = 0;

	virtual int	 GetType( ) const = 0;
	virtual void RestoreToLastNetworked( ) = 0;
	virtual void Copy( IInterpolatedVar* pSrc ) = 0;

	virtual const char* GetDebugName( ) = 0;
	virtual void SetDebugName( const char* pName ) = 0;

	virtual void SetDebug( bool bDebug ) = 0;
};

inline CBasePlayer* gLocalPlayer = nullptr;
inline CBaseCombatWeapon* gLocalWeapon = nullptr;
inline CTFPlayerResource* gPlayerResource = nullptr;
inline std::vector<CBasePlayer*> gPlayers = { };
inline std::vector<CBaseObject*> gBuildings = { };
inline std::vector<CBaseEntity*> gProjectiles = { };
inline std::vector<CBaseEntity*> gHealthpacks = { };
inline std::vector<CBaseEntity*> gAmmopacks = { };
inline std::vector<CBaseEntity*> gIntels = { };
inline std::vector<CBaseEntity*> gBombs = { };
inline std::unordered_map<int, CBaseEntity*> gSniperDots = { };
inline std::vector<CBaseEntity*> gEntities = { };
inline std::vector<CBaseEntity*> gMvMCash = { };
inline std::vector<CBaseEntity*> gMvMEntities = { };
inline std::vector<CBaseEntity*> gPowerups = { };

class CEntityCache
{
public:
	void Fill( );
	void Clear( );
};

inline CEntityCache* EntityCache = new CEntityCache;