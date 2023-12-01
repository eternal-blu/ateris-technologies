#pragma once
#include "../Interfaces/Interfaces.h"
#include "Color.h"
#include "Vector.h"
#include "../Math/Math.h"
#include "RenderView.h"
#include "../../Features/Menu/Config.h"
#include "../../Features/Menu/Input/Input.h"
#include "CEconNotification.h"

static const std::string Logo( { '\x7', '0', '0', '9', '5', 'F', 'F' } );
static const std::string Feature( { '\x7', '8', '0', 'F', 'F', '8', '0' } );
static const std::string Player( { '\x7', 'F', 'F', '8', '0', '0', '0' } );
static const std::string Reason( { '\x7', 'F', 'F', '3', '2', '3', '2' } );
static const std::string White( { '\x7', 'F', 'F', 'F', 'F', 'F', 'F' } );
static const std::string Threat( { '\x7', 'F', 'F', '3', '2', '3', '2' } );

#define TICKS_TO_TIME(t) (gGlobals->interval_per_tick * t)
#define TIME_TO_TICKS(t) ((int)(0.5f + (float)(t) / gGlobals->interval_per_tick))
#undef GetUserName

class CBasePlayer;
class CBaseCombatWeapon;
class CUserCmd;
class CTF2
{
public:
	void UpdateWorldToScreen( );
	bool WorldToScreen( const Vector& origin, Vector& screen );
	bool ComputeBoundingBox( CBaseEntity* entity, Vector* Transformed, Rect_t& bounding_box );
	std::wstring Utf8ToWide( const std::string_view& str );
	bool IsVisible(CBasePlayer* Skip, CBasePlayer* Entity, const Vector& from, const Vector& to, unsigned int Mask = MASK_SHOT | CONTENTS_GRATE );
	bool IsVisible(CBasePlayer* skip, CBasePlayer* target, const Vector& from, const Vector& to, unsigned int mask, const Vector& mins, const Vector& maxs);
	bool IsVisible( const Vector& from, const Vector& to, const Vector& mins, const Vector& maxs );
	void TraceHull( const Vector& from, const Vector& to, const Vector& mins, const Vector& maxs, CTraceFilter* Filter, CGameTrace* Trace, unsigned int MASK );
	void Trace( const Vector& from, const Vector& to, CTraceFilter* Filter, CGameTrace* Trace, unsigned int MASK );
	float LerpTime();
	int ServerTick( );
	bool IsNonAimable(CBaseCombatWeapon* Weapon);
	bool IsProjectileWeapon(CBaseCombatWeapon* Weapon);
	bool IsHitscanWeapon(CBaseCombatWeapon* Weapon);
	void GetProjectileFireSetup(CBasePlayer* Player, const Vector& Angles, Vector Offset, Vector& PosOut, Vector& AngOut, bool Pipers);
	void GetProjectileFireSetup( CBasePlayer* Player, const Vector& Angles, Vector Offset, Vector* Out );
	float AttribHookFloat(float Base, const char* Str, CBaseEntity* Entity, void* Buffer, bool IsGlobalString = false );
	void FixMovement(CUserCmd* pCmd, Vector Old);
	void RandomSeed(int iSeed);
	int RandomInt(int min, int max);
	float RandomFloat( float min, float max );
	Color GetHealthColor( int Health, int MaxHealth );
	bool IsAttacking(CUserCmd* pCmd);
	Color HSVToColor( float H, float S, float V );
	void ColorToHSV( const Color& Col, float* HSV );
	std::string GetUserName( unsigned long long SteamID );
	std::string GetBuildName( unsigned long long SteamID );
	int SeedFileLineHash( int seedvalue, const char* sharedname, int additionalSeed );
	int SharedRandomInt( unsigned int iSeed, const char* sharedname, int iMinVal, int iMaxVal, int additionalSeed );
	double PlatFloatTime( );
	bool IsRegionAllowed( int ID );
	void DrawHitboxes( CBasePlayer* Player, Color Face, Color Edge, float Duration );
	bool DownloadFile( const std::string& url, const std::string& output_file_path );
	void DownloadPlayerList( );
	bool IsInLethalDTRange( CBasePlayer* Player );
	Vector GetVelocityVectorForCleaver( const Vector& vecForward, const Vector& vecUp );

	void UpdateIncomingSequences( );
	void AddLatencyToNetChannel( CNetChannel* NetChan, float Latency );
	void FlowNewPacket( CNetChannel* NetChan, int flow, int seqnr, int acknr, int nChoked, int nDropped, int nSize );
	void FlowUpdate( CNetChannel* NetChan, int flow, int addbytes );

	ViewMatrix WorldToProjection = {};
	int ScreenWidth = 0, ScreenHeight = 0;
	std::vector<int> GlobalPlayerList = {};
};


struct CEconItemDefinition_t
{
	DWORD dword0;
	KeyValues* m_pKVItem;
	WORD m_nDefIndex;
	DWORD dwordC;
	BYTE byte10;
	BYTE m_unMinItemLevel;
	BYTE m_unMaxItemLevel;
	BYTE gap13[ 3 ];
	WORD m_nDefaultDropQuantity;
	BYTE gap18[ 4 ];
	int int1C;
	DWORD dword20;
	BYTE gap24[ 4 ];
	DWORD dword28;
	DWORD dword2C;
	BYTE m_nPopularitySeed;
	const char* m_pszItemBaseName;
	bool m_bProperName;
	const char* m_pszItemTypeName;
	const char* m_pszItemDesc;
	DWORD m_rtExpiration;
	const char* m_pszInventoryModel;
	const char* m_pszInventoryImage;
	BYTE m_pszInventoryOverlayImages[ 20 ];
	int m_iInventoryImagePosition_0;
	int m_iInventoryImagePosition_1;
	int m_iInventoryImageSize_0;
	int m_iInventoryImageSize_1;
	int m_iInspectPanelDistance;
	const char* m_pszBaseDisplayModel;
	int m_iDefaultSkin;
	bool m_bLoadOnDemand;
	__declspec( align( 2 ) ) bool m_bHideBodyGroupsDeployedOnly;
	const char* m_pszWorldDisplayModel;
	const char* m_pszWorldExtraWearableModel;
	const char* m_pszWorldExtraWearableViewModel;
	const char* m_pszVisionFilteredDisplayModel;
	const char* m_pszCollectionReference;
	bool m_bAttachToHands;
	bool m_bAttachToHandsVMOnly;
	bool m_bFlipViewModel;
	bool m_bActAsWearable;
	bool m_bActAsWeapon;
	bool m_bIsTool;
	BYTE gap9E[ 30 ];
	const char* m_pszBrassModelOverride;
	void* m_pTool;
	DWORD dwordC4;
	int m_iCapabilities;
	BYTE gapCC[ 8 ];
	const char* m_pszItemClassname;
	const char* m_pszItemLogClassname;
	const char* m_pszItemIconClassname;
	const char* m_pszDefinitionName;
	const char* m_pszDatabaseAuditTable;
	bool m_bHidden;
	bool m_bShouldShowInArmory;
	bool m_bBaseItem;
	bool m_bImported;
	bool m_bIsPackBundle;
	BYTE gapED[ 11 ];
	const char* m_pszArmoryDesc;
	const char* m_pszXifierRemapClass;
	const char* m_pszBaseFunctionalItemName;
	const char* m_pszParticleSuffix;
	BYTE gap108[ 8 ];
	const char* m_pszArmoryRemap;
	const char* m_pszStoreRemap;
	const char* m_pszClassToken;
	const char* m_pszSlotToken;
	int m_iDropType;
	const char* m_pszHolidayRestriction;
	DWORD m_nVisionFilterFlags;
	DWORD m_iSubType;
	BYTE gap130[ 4 ];
	DWORD m_unEquipRegionMask;
	DWORD m_unEquipRegionConflictMask;
	WORD word13C;
	BYTE gap13E[ 22 ];
	bool m_bValidForShuffle;
	bool m_bValidForSelfMade;
	int int158;
	DWORD dword15C;
	BYTE gap160[ 4 ];
	char* pchar164;
	DWORD dword168;
};


inline CTF2* TF2 = new CTF2;