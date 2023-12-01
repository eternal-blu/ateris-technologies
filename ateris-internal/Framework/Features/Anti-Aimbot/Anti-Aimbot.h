#pragma once
#include "../../SDK/TF2/TF2.h"

class CAntiAim
{
	bool bPacketFlip = false;
	bool IsFakeJittering = false;
	bool IsRealJittering = false;
	bool IsPitchJittering = false;
	float LastRealAngleUpdate = -90.f;
	float LastFakeAngleUpdate = 90.f;

	//legit
	float GetLegitYaw( ); //fake yaw static forward + real

	//rage
	float GetRealRageYaw( ); 	// real - static + jitter + spin
	float GetFakeRageYaw( ); 	// fake - static + jitter + spin
	std::pair<float, float> GetRagePitch( ); //real + fake
public:
	//bool IsRealFlipped = false;
	//bool IsFakeFlipped = false;
	bool IsFlipped = false;
	bool IsToggled = true;
	void RealAngles( CUserCmd* pCmd );
	void FakeAngles( CUserCmd* pCmd );
	void HandleChoke( CUserCmd* Cmd, bool* SendPacket );
	void CreateMove( CUserCmd* pCmd, bool* SendPacket );
	void DrawFakeModel( void* ECX, void* EDX, const DrawModelState_t& State, const ModelRenderInfo_t& Info, std::function< void( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4* ) > f );
	void DrawLagModel( void* ECX, void* EDX, const DrawModelState_t& State, const ModelRenderInfo_t& Info, std::function< void( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4* ) > f );
};

inline CAntiAim* AntiAim = new CAntiAim;