#pragma once
#include "Color.h"
#include "Vector.h"
#include "EngineTrace.h"

struct FireBulletsInfo_t
{
	FireBulletsInfo_t( )
	{
		m_iShots = 1;
		m_vecSpread = Vector( );
		m_flDistance = 8192;
		m_iTracerFreq = 4;
		m_flDamage = 0;
		m_iPlayerDamage = 0;
		m_pAttacker = NULL;
		m_nFlags = 0;
		m_pAdditionalIgnoreEnt = NULL;
		m_flDamageForceScale = 1.0f;
		m_bPrimaryAttack = true;
		m_bUseServerRandomSeed = false;
	}

	FireBulletsInfo_t( int nShots, const Vector& vecSrc, const Vector& vecDir, const Vector& vecSpread, float flDistance, int nAmmoType, bool bPrimaryAttack = true )
	{
		m_iShots = nShots;
		m_vecSrc = vecSrc;
		m_vecDirShooting = vecDir;
		m_vecSpread = vecSpread;
		m_flDistance = flDistance;
		m_iAmmoType = nAmmoType;
		m_iTracerFreq = 4;
		m_flDamage = 0;
		m_iPlayerDamage = 0;
		m_pAttacker = NULL;
		m_nFlags = 0;
		m_pAdditionalIgnoreEnt = NULL;
		m_flDamageForceScale = 1.0f;
		m_bPrimaryAttack = bPrimaryAttack;
		m_bUseServerRandomSeed = false;
	}

	int m_iShots = 0;
	Vector m_vecSrc = { };
	Vector m_vecDirShooting = { };
	Vector m_vecSpread = { };
	float m_flDistance = 0.f;
	int m_iAmmoType = 0;
	int m_iTracerFreq = 0;
	float m_flDamage = 0.f;
	int m_iPlayerDamage = 0;
	int m_nFlags = 0;
	float m_flDamageForceScale = 0.f;
	CBaseEntity* m_pAttacker = nullptr;
	CBaseEntity* m_pAdditionalIgnoreEnt = nullptr;
	bool m_bPrimaryAttack = false;
	bool m_bUseServerRandomSeed = false;
};

struct BeamTrail_t
{
	BeamTrail_t* next;
	float die;
	Vector org;
	Vector vel;
};

struct Beam_t
{
	unsigned char CDefaultClientRenderable[ 8 ];
	unsigned __int16 m_hRenderHandle;
	Vector m_Mins;
	Vector m_Maxs;
	int* m_queryHandleHalo;
	float m_haloProxySize;
	Beam_t* next;
	int type;
	int flags;
	int numAttachments;
	Vector attachment[ 10 ];
	Vector delta;
	float t;
	float freq;
	float die;
	float width;
	float endWidth;
	float fadeLength;
	float amplitude;
	float life;
	float r;
	float g;
	float b;
	float brightness;
	float speed;
	float frameRate;
	float frame;
	int segments;
	unsigned int entity[ 10 ];
	int attachmentIndex[ 10 ];
	int modelIndex;
	int haloIndex;
	float haloScale;
	int frameCount;
	float rgbNoise[ 129 ];
	BeamTrail_t* trail;
	float start_radius;
	float end_radius;
	bool m_bCalculatedNoise;
	float m_flHDRColorScale;
};

enum
{
	FBEAM_STARTENTITY = 0x00000001,
	FBEAM_ENDENTITY = 0x00000002,
	FBEAM_FADEIN = 0x00000004,
	FBEAM_FADEOUT = 0x00000008,
	FBEAM_SINENOISE = 0x00000010,
	FBEAM_SOLID = 0x00000020,
	FBEAM_SHADEIN = 0x00000040,
	FBEAM_SHADEOUT = 0x00000080,
	FBEAM_ONLYNOISEONCE = 0x00000100,		// Only calculate our noise once
	FBEAM_NOTILE = 0x00000200,
	FBEAM_USE_HITBOXES = 0x00000400,		// Attachment indices represent hitbox indices instead when this is set.
	FBEAM_STARTVISIBLE = 0x00000800,		// Has this client actually seen this beam's start entity yet?
	FBEAM_ENDVISIBLE = 0x00001000,		// Has this client actually seen this beam's end entity yet?
	FBEAM_ISACTIVE = 0x00002000,
	FBEAM_FOREVER = 0x00004000,
	FBEAM_HALOBEAM = 0x00008000,		// When drawing a beam with a halo, don't ignore the segments and endwidth
	FBEAM_REVERSED = 0x00010000,
	NUM_BEAM_FLAGS = 17	// KEEP THIS UPDATED!
};

struct BeamInfo_t
{
	int m_nType;
	CBaseEntity* m_pStartEnt;
	int m_nStartAttachment;
	CBaseEntity* m_pEndEnt;
	int m_nEndAttachment;
	Vector m_vecStart;
	Vector m_vecEnd;
	int m_nModelIndex;
	const char* m_pszModelName;
	int m_nHaloIndex;
	const char* m_pszHaloName;
	float m_flHaloScale;
	float m_flLife;
	float m_flWidth;
	float m_flEndWidth;
	float m_flFadeLength;
	float m_flAmplitude;
	float m_flBrightness;
	float m_flSpeed;
	int m_nStartFrame;
	float m_flFrameRate;
	float m_flRed;
	float m_flGreen;
	float m_flBlue;
	bool m_bRenderable;
	int m_nSegments;
	int m_nFlags;
	Vector m_vecCenter;
	float m_flStartRadius;
	float m_flEndRadius;
};

class IViewRenderBeams
{
public:
	virtual void	InitBeams( void ) = 0;
	virtual void	ShutdownBeams( void ) = 0;
	virtual void	ClearBeams( void ) = 0;

	// Updates the state of the temp ent beams
	virtual void	UpdateTempEntBeams( ) = 0;

	virtual void	DrawBeam( CBaseEntity* pbeam, CTraceFilter* pEntityBeamTraceFilter = NULL ) = 0;
	virtual void	DrawBeam( Beam_t* pbeam ) = 0;

	virtual void	KillDeadBeams( CBaseEntity* pEnt ) = 0;

	// New interfaces!
	virtual Beam_t* CreateBeamEnts( BeamInfo_t& beamInfo ) = 0;
	virtual Beam_t* CreateBeamEntPoint( BeamInfo_t& beamInfo ) = 0;
	virtual	Beam_t* CreateBeamPoints( BeamInfo_t& beamInfo ) = 0;
	virtual Beam_t* CreateBeamRing( BeamInfo_t& beamInfo ) = 0;
	virtual Beam_t* CreateBeamRingPoint( BeamInfo_t& beamInfo ) = 0;
	virtual Beam_t* CreateBeamCirclePoints( BeamInfo_t& beamInfo ) = 0;
	virtual Beam_t* CreateBeamFollow( BeamInfo_t& beamInfo ) = 0;

	virtual void	FreeBeam( Beam_t* pBeam ) = 0;
	virtual void	UpdateBeamInfo( Beam_t* pBeam, BeamInfo_t& beamInfo ) = 0;

	// These will go away!
	virtual void	CreateBeamEnts( int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
									float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
									float brightness, float speed, int startFrame,
									float framerate, float r, float g, float b, int type = -1 ) = 0;
	virtual void	CreateBeamEntPoint( int	nStartEntity, const Vector* pStart, int nEndEntity, const Vector* pEnd,
										int modelIndex, int haloIndex, float haloScale,
										float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
										float brightness, float speed, int startFrame,
										float framerate, float r, float g, float b ) = 0;
	virtual void	CreateBeamPoints( Vector& start, Vector& end, int modelIndex, int haloIndex, float haloScale,
									  float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
									  float brightness, float speed, int startFrame,
									  float framerate, float r, float g, float b ) = 0;
	virtual void	CreateBeamRing( int startEnt, int endEnt, int modelIndex, int haloIndex, float haloScale,
									float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
									float brightness, float speed, int startFrame,
									float framerate, float r, float g, float b, int flags = 0 ) = 0;
	virtual void	CreateBeamRingPoint( const Vector& center, float start_radius, float end_radius, int modelIndex, int haloIndex, float haloScale,
										 float life, float width, float m_nEndWidth, float m_nFadeLength, float amplitude,
										 float brightness, float speed, int startFrame,
										 float framerate, float r, float g, float b, int flags = 0 ) = 0;
	virtual void	CreateBeamCirclePoints( int type, Vector& start, Vector& end,
											int modelIndex, int haloIndex, float haloScale, float life, float width,
											float m_nEndWidth, float m_nFadeLength, float amplitude, float brightness, float speed,
											int startFrame, float framerate, float r, float g, float b ) = 0;
	virtual void	CreateBeamFollow( int startEnt, int modelIndex, int haloIndex, float haloScale,
									  float life, float width, float m_nEndWidth, float m_nFadeLength, float r, float g, float b,
									  float brightness ) = 0;

	void DrawBeam( const Vector& source, const Vector& end, const Color& col, float time, float size );
};