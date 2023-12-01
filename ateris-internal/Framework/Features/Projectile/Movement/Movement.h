#pragma once
#include "../../../SDK/TF2/TF2.h"
#include <deque>

class CMovementSim
{
public:
	bool IsRunning( )
	{
		return bIsRunning;
	}

	struct CDirection
	{
		std::deque< float > Vec {};
		bool GroundState = false;
		bool WasFalling = false;
	};

	std::unordered_map< int, CDirection > Directions = { };
	bool Init( CBasePlayer* player );
	void Restore( );
	void JumpPrediction( CBasePlayer* Player, CMoveData* mData );
	void FillDirections( int PlrIndex );
	Vector RunTick( );

	std::vector<Vector> OldPositions = {};
	std::vector<Vector> PredictedPositions = {};
	float ShotTime = -1.f;
	bool ClearCache = false;

	struct VelocityData_t
	{
		float SimTime = 0.f;
		int Flags = 0;
		Vector Origin = { };
	};

	Vector GetOrigin( )
	{
		return MoveData.AbsOrigin;
	}

	float GetSideMove( )
	{
		return MoveData.SideMove;
	}

	std::unordered_map<int, VelocityData_t> VelocityRecords;

private:
	CBasePlayer* Player = nullptr;
	CMoveData MoveData = { };
	bool OldInPrediction = false, OldFirstTimePrediction = false, bIsRunning = false;
	float OldFrameTime = 0.0f, grandpa_side = 0.0f, grandpa_forward = 0.0f;
	float Iacob = 0.f;
	void SetupMoveData( CBasePlayer* player, CMoveData* move_data );
	void ComputeAverageWalletTheft( CBasePlayer* player, CMoveData* move_data );
};

inline CMovementSim* MovementSim = new CMovementSim;