#pragma once
#include "../../SDK/TF2/TF2.h"
#include "ProjectileSim/ProjectileSim.h"
#include "Movement/Movement.h"

class CProjectiles
{
public:
	void Run(CUserCmd* pCmd);
private:
	struct Target_t
	{
		float Fov = -1.f;
		int Priority = 20; // normal player
		float Distance = 0;
		CBasePlayer* This = nullptr;
		Target_t(float Fov, float Distance, int Priority, CBasePlayer* This, bool ForceBacktrack)
			: Fov(Fov), Distance(Distance), Priority(Priority), This(This)
		{
		}
	};

	void HandleGaza( CUserCmd* pCmd );
	bool SolveTarget( CBasePlayer* Target, Vector& AngleOut, float& TimeOut );
	bool CanSee( const Vector From, const Vector To, CBasePlayer* Target, Vector AngleTo, float TimeTo );
	bool TraceProjectile( CBasePlayer* Target, const Vector& From, const Vector& To, unsigned int MASK = MASK_SOLID );
	bool CanArcReach( const Vector From, const Vector To, CBasePlayer* Target, Vector AngleTo, float TimeTo );
	bool HandleSplashBot( CBasePlayer* Target, const Vector Start, const Vector Position, Vector& AngleOut, float& TimeOut );
	bool SolveAngle( const Vector From, const Vector To, Vector& Out, float& TimeOut );
	Vector GetShootPos( );
	bool GetProjectileInfo( );

	struct ProjectileInfo_t
	{
		float Speed = 0.0f;
		float GravityMod = 0.0f;
		bool Pipes = false;
		bool Flamethrower = false;
	};

	ProjectileInfo_t SavedProjectileInfo = {};

	Vector Angle = {};
	float Time = 0.f;
	CBasePlayer* Target = {};

	void FindSuitableTargets();
	std::vector<Vector> ComputeSphere( float radius, int numSamples );
	std::vector<Target_t> Targets = {};
};

class CTraceFilterWorldCustom : public CTraceFilter
{
public:
	virtual bool ShouldHitEntity( void* pServerEntity, int contentsMask );
	virtual TraceType_t GetTraceType( ) const;

public:
	CBaseEntity* m_pTarget = nullptr;
	CBaseEntity* m_pSkip = nullptr;
};


inline CProjectiles* Projectile = new CProjectiles;