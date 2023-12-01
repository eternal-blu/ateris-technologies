#pragma once
#include "../../../SDK/TF2/TF2.h"

struct ProjectileInfo
{
	EProjectileType Type {};
	Vector Position {};
	Vector Angle {};
	float Speed {};
	float Gravity {};
	bool NoSpin {};
};

class CProjectileSimulation
{
public:
	void Simulate( );
	Vector GetOrigin( );
	bool GetInfo( const Vector& Angles, ProjectileInfo& Out );
	bool Init( const ProjectileInfo& Info, bool NoVecUp = false );
};

inline CProjectileSimulation* ProjectileSim = new CProjectileSimulation;