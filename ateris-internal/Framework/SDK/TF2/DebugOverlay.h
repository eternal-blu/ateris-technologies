#pragma once
#include "BaseClientDLL.h"

class OverlayText_t
{
public:
	OverlayText_t( )
	{
		nextOverlayText = 0;
		origin.Init( );
		bUseOrigin = false;
		lineOffset = 0;
		flXPos = 0;
		flYPos = 0;
		text[ 0 ] = 0;
		m_flEndTime = 0.0f;
		m_nServerCount = -1;
		m_nCreationTick = -1;
		r = g = b = a = 255;
	}

	Vector			origin;
	bool			bUseOrigin;
	int				lineOffset;
	float			flXPos;
	float			flYPos;
	char			text[ 512 ];
	float			m_flEndTime;			// When does this text go away
	int				m_nCreationTick;		// If > 0, show only one server frame
	int				m_nServerCount;			// compare server spawn count to remove stale overlays
	int				r;
	int				g;
	int				b;
	int				a;
	OverlayText_t* nextOverlayText;
};

class IVDebugOverlay
{
public:
	virtual void AddEntityTextOverlay( int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char* format, ... ) = 0;
	virtual void AddBoxOverlay( const Vector& origin, const Vector& mins, const Vector& max, Vector const& orientation, int r, int g, int b, int a, float duration ) = 0;
	virtual void AddTriangleOverlay( const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration ) = 0;
	virtual void AddLineOverlay( const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration ) = 0;
	virtual void AddTextOverlay( const Vector& origin, float duration, const char* format, ... ) = 0;
	virtual void AddTextOverlay( const Vector& origin, int line_offset, float duration, const char* format, ... ) = 0;
	virtual void AddScreenTextOverlay( float flXPos, float flYPos, int line_offset, float flDuration, int r, int g, int b, int a, char const* text ) = 0; //float,float,float,int,int,int,int,char const*
	virtual void AddSweptBoxOverlay( const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const Vector& angles, int r, int g, int b, int a, float flDuration ) = 0;
	virtual void AddGridOverlay( const Vector& origin ) = 0;
	virtual int ScreenPosition( const Vector& point, Vector& screen ) = 0;
	virtual int ScreenPosition( float flXPos, float flYPos, Vector& screen ) = 0;
	virtual OverlayText_t* GetFirst( void ) = 0;
	virtual OverlayText_t* GetNext( OverlayText_t* current ) = 0;
	virtual void ClearDeadOverlays( void ) = 0;
	virtual void ClearAllOverlays( ) = 0;
	virtual void AddTextOverlayRGB( const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char* format, ... ) = 0;
	virtual void AddTextOverlayRGB( const Vector& origin, int line_offset, float duration, int r, int g, int b, int a, const char* format, ... ) = 0;
	virtual void Unknown( ) = 0;
	virtual void AddLineOverlayAlpha( const Vector& origin, const Vector& dest, int r, int g, int b, int a, bool noDepthTest, float duration ) = 0;
	virtual void AddBoxOverlay2( const Vector& origin, const Vector& mins, const Vector& max, Vector const& orientation, const Color& faceColor, const Color& edgeColor, float duration ) = 0;
};