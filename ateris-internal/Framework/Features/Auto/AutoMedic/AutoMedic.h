#pragma once
#include "../../../SDK/TF2/TF2.h"

class CAutoMedic
{
public:
	int LockedTarget = -1;
	CBasePlayer* ScanTargets();
	void Run(CUserCmd* pCmd);

};

inline CAutoMedic* AutoMedic = new CAutoMedic;