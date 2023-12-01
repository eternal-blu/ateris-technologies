#pragma once
#include "../../SDK/TF2/TF2.h"
#include "../LagCompensation/LagComp.h"

class CAimbot
{
public:
	void Run(CUserCmd* pCmd);
private:
	struct Target_t
	{
		float Fov = -1.f;
		float Distance = 0.f;
		int Priority = 20; // normal player
		CBasePlayer* This = nullptr;
		bool ForceBacktrack = false;

		Target_t(float Fov, float Distance, int Priority, CBasePlayer* This, bool ForceBacktrack)
			: Fov(Fov), Distance(Distance), Priority(Priority), This(This), ForceBacktrack(ForceBacktrack)
		{
		}
	};

	bool HandleDelayShot( CBasePlayer* BestTarget, bool HasShotAtTarget, const Vector Point );
	bool CanTargetHitbox( int Hitbox, CBasePlayer* Player );
	Vector FindPoint(CBasePlayer* Player, float& Simtime, Vector& point, CLagComp::CLagCompRecord& Record);
	bool HandleHeadshotStuff(CUserCmd* pCmd, CBasePlayer* BestTarget);
	std::vector<Vector> GeneratedPointsForHitbox(CBasePlayer* Player, int Hitbox, Matrix3x4* Matrix, const model_t* Model);
	bool HandleHeadshot();
	bool CanTargetArms( CBasePlayer* Player );
	void FindSuitableTargets();

	std::vector<Target_t> Targets = {};
};

inline CAimbot* Aimbot = new CAimbot;