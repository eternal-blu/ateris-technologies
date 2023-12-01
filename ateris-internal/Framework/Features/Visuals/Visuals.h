#pragma once
#include "../../SDK/TF2/TF2.h"
#include "../../../Globals.h"

class CVisuals
{
public:
	void Doubletap();
	void DrawLockedTarget( );
	void Crithack();
	void FakeLatency( );
	void ThirdPerson( CViewSetup* View );
	void HitMarkerEvent( IGameEvent* Event );
	void HitMarker( );
	void Spectators( );
	void DrawBacktrack( void* ECX, void* EDX, const DrawModelState_t& State, const ModelRenderInfo_t& Info, std::function< void( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4* ) > f );
	void Watermark( );
	void ScopeVisuals( );
	void DrawNoSpread( );
	void DrawInfo( );
	void Viewmodel( );
	void PredictionLine( );
	void ProjectileLine( );
	void ProjectileLine2( );
	void ProjectileCam( );
	void DrawViewmodel( void* ECX, void* EDX, const DrawModelState_t& State, const ModelRenderInfo_t& Info, Matrix3x4* Matrix, std::function< void( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4* ) > f );

	void ModulateTexture( const Color& clr, const char* texture );
	void UpdateWorldModulation( );

	bool IsWorldModulated = false;
	bool InThirdPerson = false;

	unsigned char Alpha = G->Doubletap.Charged ? 255 : 0;
	int MouseX = 0;
	int MouseY = 0;

	std::vector<Vector> OldPrediction = {};
	std::vector<Vector> NewPrediction = {};

	int DTIndicatorX = 0;
	int DTIndicatorY = 0;
	int CritIndicatorX = 0;
	int CritIndicatorY = 0;
	int LatencyIndicatorX = 0;
	int LatencyIndicatorY = 0;
	int ProjectileCamX = 0;
	int ProjectileCamY = 0;
};

inline CVisuals* Visuals = new CVisuals;