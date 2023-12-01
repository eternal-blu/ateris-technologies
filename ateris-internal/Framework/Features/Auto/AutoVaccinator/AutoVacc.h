#pragma once
#include "../../../SDK/TF2/TF2.h"

enum ResistanceTypes : int
{
	RESISTANCE_BULLET = 0,
	RESISTANCE_EXPLOSIVE = 1,
	RESISTANCE_FIRE = 2
};

struct CPriority
{
	int Threat {};
	float Distance {};
	float LocalDistance {};
	bool IsPlayer = false;
	CBaseEntity* Entity {};

	void UpdateDistance( );

	CPriority( )
	{
		Threat = -1;
		float Distance = -1.f;
		Entity = nullptr;
	}

	CPriority( int Threat, float Distance, CBaseEntity* Entity, bool IsPlayer, float LocalDistance )
		: Threat( Threat ), Distance( Distance ), Entity( Entity ),
		IsPlayer( IsPlayer ), LocalDistance( LocalDistance )
	{
	}
};

class CAutoVacc
{
private:
	ResistanceTypes GetResistanceForClass( CBasePlayer* Player );
	ResistanceTypes GetResistanceForProjectile( CBaseEntity* Projectile );
	int FindSwapsNeededFor( int Resistance );
	void FindThreats( );
	void SwapResistance( CUserCmd* pCmd, int Wanted );

	std::vector<CPriority*> gThreats;
public:
	void Run( CUserCmd* pCmd );
	void HandlePop( CUserCmd* pCmd, CBasePlayer* BestThreat, int Resistance, bool IsProjectile, const std::string& Reason );
};

inline CAutoVacc* AutoVaccinator = new CAutoVacc;