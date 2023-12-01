#pragma once
#include "../../SDK/TF2/TF2.h"

class CAnimFix
{
public:
	void Update( );
	int TicksElapsed = 0;
};

inline CAnimFix* AnimFix = new CAnimFix;