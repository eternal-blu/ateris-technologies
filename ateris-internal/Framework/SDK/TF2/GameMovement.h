#pragma once
#include "Prediction.h"

class CGameMovement
{
public:
	virtual			~CGameMovement( void ) {}
	virtual void	ProcessMovement( CBaseEntity* pPlayer, CMoveData* pMove ) = 0;
	virtual void	StartTrackPredictionErrors( CBaseEntity* pPlayer ) = 0;
	virtual void	FinishTrackPredictionErrors( CBaseEntity* pPlayer ) = 0;
	virtual void	DiffPrint( char const* fmt, ... ) = 0;
	virtual Vector GetPlayerMins( bool ducked ) const = 0;
	virtual Vector GetPlayerMaxs( bool ducked ) const = 0;
	virtual Vector GetPlayerViewOffset( bool ducked ) const = 0;
};

