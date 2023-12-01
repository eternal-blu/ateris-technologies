#pragma once
#include "../../../SDK/TF2/TF2.h"

class CAutoAirblast
{
public:
	std::vector<CBaseEntity*> ValidEntities = {};
	void Run( CUserCmd* );
};

inline CAutoAirblast* AutoAirblast = new CAutoAirblast;