#include "Beams.h"
#include "../Interfaces/Interfaces.h"

void IViewRenderBeams::DrawBeam( const Vector& source, const Vector& end, const Color& col, float time, float size )
{
	static BeamInfo_t beamInfo;
	beamInfo.m_nType = 0;
	beamInfo.m_pszModelName = "sprites/physbeam.vmt";
	beamInfo.m_nModelIndex = -1;
	beamInfo.m_flHaloScale = 0.0f;
	beamInfo.m_flLife = time;
	beamInfo.m_flWidth = size;
	beamInfo.m_flEndWidth = size;
	beamInfo.m_flFadeLength = 1.f;
	beamInfo.m_flAmplitude = 0.f;
	beamInfo.m_flBrightness = 255.f;
	beamInfo.m_flSpeed = 0.f;
	beamInfo.m_nStartFrame = 0;
	beamInfo.m_flFrameRate = 0.f;
	beamInfo.m_flRed = col.r;
	beamInfo.m_flGreen = col.g;
	beamInfo.m_flBlue = col.b;
	beamInfo.m_nSegments = 2;
	beamInfo.m_bRenderable = true;
	beamInfo.m_nFlags = FBEAM_NOTILE;
	beamInfo.m_vecStart = source;
	beamInfo.m_vecEnd = end;

	Beam_t* coolBeam = gBeams->CreateBeamPoints( beamInfo );
	if ( coolBeam )
	{
		gBeams->DrawBeam( coolBeam );
	}
}