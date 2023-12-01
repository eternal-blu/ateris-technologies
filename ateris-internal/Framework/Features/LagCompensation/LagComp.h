#pragma once
#include "../../SDK/TF2/TF2.h"
#include <deque>

struct ResolvarInfo
{
	float Yaw = 0.f;
	float Pitch = 0.f;
	bool AhHesLegitWhoCouldHaveThought = true;
	int MissedShots = -1;
	int HitShots = 0;
	bool ChangeAngles = false;
	bool CycleYaw = false;
	bool Resolved = false;
	bool LegitAA = false;

	void Reset( )
	{
		// dont reset AhHes u get it
		// keep it the same
		HitShots = MissedShots = -1;
		Yaw = 0.f;
		Pitch = 0.f;
		ChangeAngles = false;
	}

	// https://youtu.be/7a49IPmpM0c
	//bool SoThatWasActuallyTheFake = false;
	//bool TryTheOtherSideTryTheOtherSide = false;
	//bool MaybeHesNotEvenAntiAimingTryTheMiddleTryTheMiddle = false;
	//bool ThenYouShootBoopAhHesLegitHaWhoCouldHaveThought = false;
};

class CLagComp
{
public:
	struct CLagCompRecord
	{
		Vector Origin{}, Mins{}, Maxs{}, Velocity{}, Angles{};
		Vector WorldSpaceCenter{};
		Matrix3x4 Boner[128]{};
		model_t* Model = nullptr;
		bool SetupBoner = false;
		CBasePlayer* Player = nullptr;
		bool Valid = false;
		int Flags = 0;

		std::array<GestureSlot_t, GESTURE_SLOT_COUNT> GestureSlots = {};
		//every gesture slot has 1 animlayer
		std::array<C_AnimationLayer, GESTURE_SLOT_COUNT> AnimationLayers = {};

		int Choked = 0;
		float SimTime = 0.f;
		float AnimTime = 0.f;
		float OldSimTime = 0.f;
		float DistanceToLocal = 0.f;
		void UpdateRecord( );
	};

	std::unordered_map<int, ResolvarInfo> Resolverer = {};

	CLagComp::CLagCompRecord GetLastTick(int Player);
	CLagComp::CLagCompRecord GetClosestTick(int Player);
	std::deque< CLagCompRecord> GetValidTicks( int Player );
	void Restore(CUserCmd*, CLagCompRecord&, CBasePlayer*);
	void Record( CBasePlayer* Player );
	void HandleResolving(CBasePlayer* Entity);
	void ResolverHurtEvent(IGameEvent* Event);
	bool IsTickValid( const CLagCompRecord Rec );
	std::deque<CLagCompRecord> Records[65];
};

inline CLagComp* LagComp = new CLagComp;