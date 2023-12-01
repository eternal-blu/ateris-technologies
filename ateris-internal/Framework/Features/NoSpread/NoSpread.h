#pragma once
#include "../../SDK/TF2/TF2.h"

class CNoSpread
{
public:
	//projectiles
	void RunProjectile( CUserCmd* Cmd );

	//hitscan
	enum SyncState_t {
		NOT_SYNCED = 0,
		CORRECTING,
		SYNCED,
		DEAD_SYNC,
	};

	SyncState_t SyncState = NOT_SYNCED; // this will be set to 0 each level init / level shutdown
	bool IsWaitingForPerfData = false;
	bool ShouldUpdate = false;
	bool IsWaitingForPostSNM = false;
	bool IsResyncNeeded = false;
	bool IsSyncing = false;
	bool WasLastPerf = false;
	bool ShouldUpdateCmdCorrection = false;
	bool IsBadMantissa = false;      // Also reset every levelinit/shutdown
	bool WasInAttack = false;
	double FloatTimeDelta = 0.0;
	double SentClientFloatTime = 0.0;
	double LastTimeCorrection = 0.0;
	double WriteUserCmdCorrection = 0.0;
	double LastSyncDeltaTime = 0.0;
	float PredictionSeed = 0.0f;
	float StartTime = 0.0f;
	float MantissaStep = 0.0f;
	float ServerTime = 0.0f;
	float CurrentWeaponSpread = 0.0f;
	int NewPackets = 0;
	int ServerSeed = 0;

	std::vector<std::pair<float, float>> RandomFloatVals = {};

	float CalcMantissaStep( float Value );
	bool IsPerfectShot( );
	bool SendNetMessageHandler( INetMessage* Data );
	void SendNetMessagePostHandler( );
	bool DispatchUserMessageHandler( int type, bf_read* buf );
	void CL_SendMoveHandler( );
	void CL_SendMovePostHandler( );
	void CreateMoveHandler( CUserCmd* Cmd );
	void ApplySpreadCorrection( Vector& Angles, int seed, float spread );

	void Reset( );
};

inline CNoSpread* NoSpread = new CNoSpread;