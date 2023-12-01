#pragma once
#include "../../SDK/TF2/TF2.h"

class CLegitBob
{
private:
	struct CTarget
	{
		float Fov = 0.f;
		CBasePlayer* Target = nullptr;
	};

	std::vector<CTarget> Targets = {};
public:
	void GatherTargets();
	void Run(CUserCmd* pCmd);
};

inline CLegitBob* LegitBob = new CLegitBob;