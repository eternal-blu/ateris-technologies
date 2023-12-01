#pragma once
#include "../../SDK/TF2/TF2.h"
#include "../LagCompensation/LagComp.h"

class CMeleeBob
{
public:

	void Run(CUserCmd* pCmd);

private:
	struct Target_t
	{
		float Distance = 0.f;
		CBasePlayer* Player = nullptr;
	};
	void FindSuitableTargets();
	bool CanBackstab(const Vector& Source, const Vector& Destination, Vector Delta);

	std::vector<Target_t> Targets = {};
};

inline CMeleeBob* MeleeBob = new CMeleeBob;