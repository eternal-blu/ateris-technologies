#pragma once
#include "../../SDK/TF2/TF2.h"
#include "../../Utils/DrawTools/DrawTools.h"

class CESP
{
	void DrawPlayers( );
	void DrawBuildings( );
	void DrawWorld( );
	void Draw3DBox( const Vector* points, Color& Clr );
	void PlayerArrow( CBasePlayer* Player, Color& Clr );

	std::array<float, 4096> PlayerHealth = { };
	std::array<float, 4096> building_health = { };
	std::unordered_map<int, int> PlayerAvatars = {};
public:
	void DrawEntities( );
};

inline CESP* ESP = new CESP;