#pragma once
#include "../../SDK/TF2/TF2.h"

enum EFonts
{
	FONT_ESP = 0,
	FONT_MENU,
	FONT_SMALL,
	FONT_RIJIN,
	FONT_WATERMARK,
	FONT_INDICATOR,
	FONT_INDICATOR2
};

#define	Center_X ( 1 << 0 )
#define	Center_Y ( 1 << 1 )
#define	Reverse_X ( 1 << 2 )

inline std::unordered_map< EFonts, HFont > Fonts;
class CRendering
{
public:
	void Line( int x, int y, int x_, int y_, const Color& Clr, bool Outlined = true, bool AlphaOutline = false );
	void Line( int x, int y, int x_, int y_, const Color& Clr, int Height, bool Outlined = true );
	void Line( int x, int y, int x_, int y_, const Color& Clr, const Color& Clr2 );
	void Rectangle( int x, int y, int w, int h, const Color& Clr, bool Outlined = true, bool Filled = false );
	void FadeRectangle( int x, int y, int w, int h, const Color& Clr1, const Color& Clr2, bool Horizontal = false );
	void FadeRectangle( int x, int y, int w, int h, const Color& Clr, int alpha0, int alpha1, bool Horizontal = false );
	void GetTextSize( const EFonts& Font, int& w, int& h, const char* str, ... );
	void GetTextSize( const EFonts& Font, int& w, int& h, const wchar_t* str, ... );
	void String( int x, int y, const EFonts& Font, unsigned int AlignFlags, const Color& Clr, const char* str, ... );
	void String( int x, int y, const EFonts& Font, unsigned int AlignFlags, const Color& Clr, bool Outlined, const char* str, ... );
	void String( int x, int y, const EFonts& Font, unsigned int AlignFlags, const Color& Clr, bool ApplyAlphaToOutline, bool Outlined, const char* str, ... );
	void String( int x, int y, const EFonts& Font, unsigned int AlignFlags, const Color& Clr, const wchar_t* str, ... );
	void String( int x, int y, const EFonts& Font, unsigned int AlignFlags, const Color& Clr, bool Outlined, const wchar_t* str, ... );
	bool TextureRectangle( int x, int y, int w, int h, int ID, const Color& Clr );
	bool Timer( );
	float EaseIn( float start, float end, float speed );
	float Lerp( float start, float end, float speed );
	void TexturedPolygon( int Count, Vertex_t* Vertex, const Color& Clr );
	int CreateTexture( const unsigned char* array, int width, int height, bool avatar = false );
	void DeleteTexturesFromMemory( );
	void OutlinedCircle( int x, int y, float radius, int segments, const Color& clr );
	void FilledCircle( const int& x, const int& y, const float& radius, const int& segments, const Color& clr );
	void RenderLine( const Vector& v1, const Vector& v2, Color Clr, bool ZBuffer );
	void RenderBox( const Vector& Origin, const Vector& Angles, const Vector& Mins, const Vector& Maxs, Color c, bool ZBuffer, bool InsideOut );
	void RenderWireframeBox( const Vector& Origin, const Vector& Angles, const Vector& Mins, const Vector& Maxs, Color c, bool ZBuffer );
	void RenderTriangle( const Vector& p1, const Vector& p2, const Vector& p3, Color Clr, bool ZBuffer );
	void InitFonts( );

	std::vector<int> m_vecCreatedTextures = { };
};

inline CRendering* Draw = new CRendering;