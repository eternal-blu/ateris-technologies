#include "DrawTools.h"
#include <cstdarg>
#include "../../../Globals.h"

void CRendering::InitFonts( )
{
	//maybe directx fonts
	gSurface->SetFontGlyphSet( Fonts[ FONT_ESP ] = gSurface->CreateFont( ), "Verdana", 12, FW_BOLD, 0, 0, 0 );
	gSurface->SetFontGlyphSet( Fonts[ FONT_MENU ] = gSurface->CreateFont( ), "Tahoma", 12, FW_DONTCARE, 0, 0, 0 );
	gSurface->SetFontGlyphSet( Fonts[ FONT_SMALL ] = gSurface->CreateFont( ), "Small Fonts", 8, FW_DONTCARE, 0, 0, 0 );
	gSurface->SetFontGlyphSet( Fonts[ FONT_RIJIN ] = gSurface->CreateFont( ), "Fixedsys", 14, FW_DONTCARE, 0, 0, 0 );
	gSurface->SetFontGlyphSet( Fonts[ FONT_WATERMARK ] = gSurface->CreateFont( ), "Tahoma", 14, FW_DONTCARE, 0, 0, 0 );
	gSurface->SetFontGlyphSet( Fonts[ FONT_INDICATOR ] = gSurface->CreateFont( ), "Verdana", 12, FW_DONTCARE, 0, 0, 0 );
	gSurface->SetFontGlyphSet( Fonts[ FONT_INDICATOR2 ] = gSurface->CreateFont( ), "Verdana", 28, FW_BOLD, 0, 0, FONTFLAG_DROPSHADOW | FONTFLAG_ANTIALIAS );
}

void CRendering::Line( int x, int y, int x_, int y_, const Color& Clr, bool Outlined, bool AlphaOutline )
{
	if ( Outlined )
	{
		gSurface->DrawSetColor( 0, 0, 0, ( AlphaOutline ? Clr.a : 255 ) );
		gSurface->DrawLine( x + 1, y, x_ + 1, y_ );
		gSurface->DrawLine( x - 1, y, x_ - 1, y_ );
		gSurface->DrawLine( x, y + 1, x_, y_ + 1 );
		gSurface->DrawLine( x, y - 1, x_, y_ - 1 );
	}

	gSurface->DrawSetColor( Clr );
	gSurface->DrawLine( x, y, x_, y_ );
}

void CRendering::Line( int x, int y, int x_, int y_, const Color& Clr, int Height, bool Outlined )
{
	if ( Outlined )
	{
		gSurface->DrawSetColor( 0, 0, 0, 255 );
		//gSurface->DrawLine( x + 1, y + Height, x_ + 1, y_ + Height );
		//gSurface->DrawLine( x - 1, y - Height, x_ - 1, y_ - Height );
		gSurface->DrawLine( x, y + 1 + Height, x_, y_ + 1 + Height );
		gSurface->DrawLine( x + 1 + Height, y, x_ + 1 + Height, y_ );
		gSurface->DrawLine( x, y - 1 - Height, x_, y_ - 1 - Height );
		gSurface->DrawLine( x - 1 - Height, y, x_ - 1 - Height, y_ );
	}

	gSurface->DrawSetColor( Clr );
	gSurface->DrawLine( x, y, x_, y_ );
}

void CRendering::Line( int x, int y, int x_, int y_, const Color& Clr1, const Color& Clr2 )
{
	gSurface->DrawSetColor( Clr2 );
	gSurface->DrawLine( x + 1, y, x_ + 1, y_ );
	gSurface->DrawLine( x - 1, y, x_ - 1, y_ );
	gSurface->DrawLine( x, y + 1, x_, y_ + 1 );
	gSurface->DrawLine( x, y - 1, x_, y_ - 1 );
	
	gSurface->DrawSetColor( Clr1 );
	gSurface->DrawLine( x, y, x_, y_ );
}

void CRendering::Rectangle( int x, int y, int w, int h, const Color& Clr, bool Outlined, bool Filled )
{
	if ( Filled )
	{
		if ( Outlined )
		{
			gSurface->DrawSetColor( 0, 0, 0, 255 );
			gSurface->DrawOutlinedRect( x - 1, y - 1, x + w + 1, y + h + 1 );
		}

		gSurface->DrawSetColor( Clr );
		gSurface->DrawFilledRect( x, y, x + w, y + h );
	}
	else
	{
		if ( Outlined )
		{
			gSurface->DrawSetColor( 0, 0, 0, 255 );
			gSurface->DrawOutlinedRect( x - 1, y - 1, x + w + 1, y + h + 1 );
			gSurface->DrawOutlinedRect( x + 1, y + 1, x + w - 1, y + h - 1 );
		}

		gSurface->DrawSetColor( Clr );
		gSurface->DrawOutlinedRect( x, y, x + w, y + h );
	}
}

void CRendering::FadeRectangle( int x, int y, int w, int h, const Color& Clr1, const Color& Clr2, bool Horizontal )
{
	gSurface->DrawSetColor( Clr1 );
	gSurface->DrawFilledRectFade( x, y, x + w, y + h, 255, 0, Horizontal );
	gSurface->DrawSetColor( Clr2 );
	gSurface->DrawFilledRectFade( x, y, x + w, y + h, 0, 255, Horizontal );
}

void CRendering::FadeRectangle( int x, int y, int w, int h, const Color& Clr, int alpha0, int alpha1, bool Horizontal )
{
	gSurface->DrawSetColor( Clr );
	gSurface->DrawFilledRectFade( x, y, x + w, y + h, alpha0, alpha1, Horizontal );
}

void CRendering::GetTextSize( const EFonts& Font, int& w, int& h, const char* str, ... )
{
	char buf[ 1024 ];
	wchar_t wbuf[ 1024 ];

	va_list vlist;
	va_start( vlist, str );
	vsprintf_s( buf, str, vlist );
	va_end( vlist );

	wsprintfW( wbuf, L"%hs", buf );
	gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
}

void CRendering::GetTextSize( const EFonts& Font, int& w, int& h, const wchar_t* str, ... )
{
	wchar_t wbuf[ 1024 ];

	va_list vlist;
	va_start( vlist, str );
	vswprintf_s( wbuf, str, vlist );
	va_end( vlist );

	gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
}

void CRendering::String( int x, int y, const EFonts& Font, unsigned int AlignFlags, const Color& Clr, bool Outlined, const char* str, ... )
{
	if ( !str )
	{
		return;
	}

	char buf[ 1024 ] = { '\0' };
	wchar_t wbuf[ 1024 ] = { '\0' };

	va_list vlist;
	va_start( vlist, str );
	vsprintf_s( buf, str, vlist );
	va_end( vlist );

	//MultiByteToWideChar( CP_UTF8, 0, buf, 1024, wbuf, 1024 );
	wsprintfW( wbuf, L"%hs", buf );

	if ( AlignFlags & Center_X )
	{
		int w = 0, h = 0;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		x -= w * 0.5f;
	}

	if ( AlignFlags & Center_Y )
	{
		int w = 0, h = 0;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		y -= h * 0.5f;
	}

	if ( AlignFlags & Reverse_X )
	{
		int w = 0, h = 0;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		x -= w;
	}

	gSurface->DrawSetTextFont( Fonts[ Font ] );

	if ( Outlined )
	{
		gSurface->DrawSetTextColor( Color( ) );
		if ( Config->Visuals->OptimizedFonts )
		{
			gSurface->DrawSetTextPos( x + 1, y + 1 );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
		}
		else
		{
			gSurface->DrawSetTextPos( x - 1, y );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

			gSurface->DrawSetTextPos( x + 1, y );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

			gSurface->DrawSetTextPos( x, y - 1 );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

			gSurface->DrawSetTextPos( x, y + 1 );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
		}
	}

	gSurface->DrawSetTextColor( Clr );
	gSurface->DrawSetTextPos( x, y );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
}

void CRendering::String( int x, int y, const EFonts& Font, unsigned int AlignFlags, const Color& Clr, const char* str, ... )
{
	if ( !str )
	{
		return;
	}

	char buf[ 1024 ] = { '\0' };
	wchar_t wbuf[ 1024 ] = { '\0' };

	va_list vlist;
	va_start( vlist, str );
	vsprintf_s( buf, str, vlist );
	va_end( vlist );

	//MultiByteToWideChar( CP_UTF8, 0, buf, 1024, wbuf, 1024 );
	wsprintfW( wbuf, L"%hs", buf );

	if ( AlignFlags & Center_X )
	{
		int w = 0, h = 0;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		x -= w * 0.5f;
	}

	if ( AlignFlags & Center_Y )
	{
		int w = 0, h = 0;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		y -= h * 0.5f;
	}

	if ( AlignFlags & Reverse_X )
	{
		int w = 0, h = 0;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		x -= w;
	}

	gSurface->DrawSetTextFont( Fonts[ Font ] );

	gSurface->DrawSetTextColor( Color( ) );
	gSurface->DrawSetTextPos( x - 1, y );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

	gSurface->DrawSetTextPos( x + 1, y );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

	gSurface->DrawSetTextPos( x, y - 1 );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

	gSurface->DrawSetTextPos( x, y + 1 );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

	gSurface->DrawSetTextColor( Clr );
	gSurface->DrawSetTextPos( x, y );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
}

void CRendering::String( int x, int y, const EFonts& Font, unsigned int AlignFlags, const Color& Clr, bool ApplyAlphaToOutline, bool Outlined, const char* str, ... )
{
	if ( !str )
	{
		return;
	}

	char buf[ 1024 ] = { '\0' };
	wchar_t wbuf[ 1024 ] = { '\0' };

	va_list vlist;
	va_start( vlist, str );
	vsprintf_s( buf, str, vlist );
	va_end( vlist );

	//MultiByteToWideChar( CP_UTF8, 0, buf, 1024, wbuf, 1024 );
	wsprintfW( wbuf, L"%hs", buf );

	if ( AlignFlags & Center_X )
	{
		int w = 0, h = 0;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		x -= w * 0.5f;
	}

	if ( AlignFlags & Center_Y )
	{
		int w = 0, h = 0;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		y -= h * 0.5f;
	}

	if ( AlignFlags & Reverse_X )
	{
		int w = 0, h = 0;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		x -= w;
	}

	gSurface->DrawSetTextFont( Fonts[ Font ] );

	if ( Outlined )
	{
		gSurface->DrawSetTextColor( Color( 0, 0, 0, ApplyAlphaToOutline ? Clr.a : 255 ) );
		if ( Config->Visuals->OptimizedFonts )
		{
			gSurface->DrawSetTextPos( x + 1, y + 1 );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
		}
		else
		{
			gSurface->DrawSetTextPos( x - 1, y );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

			gSurface->DrawSetTextPos( x + 1, y );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

			gSurface->DrawSetTextPos( x, y - 1 );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

			gSurface->DrawSetTextPos( x, y + 1 );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
		}
	}

	gSurface->DrawSetTextColor( Clr );
	gSurface->DrawSetTextPos( x, y );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
}

void CRendering::String( int x, int y, const EFonts& Font, unsigned int AlignFlags, const Color& Clr, bool Outlined, const wchar_t* str, ... )
{
	wchar_t wbuf[ 1024 ];

	va_list vlist;
	va_start( vlist, str );
	vswprintf_s( wbuf, str, vlist );
	va_end( vlist );

	if ( AlignFlags & Center_X )
	{
		int w, h;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		x -= w * 0.5f;
	}

	if ( AlignFlags & Center_Y )
	{
		int w, h;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		y -= h * 0.5f;
	}

	if ( AlignFlags & Reverse_X )
	{
		int w, h;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		x -= w;
	}

	gSurface->DrawSetTextFont( Fonts[ Font ] );
	if ( Outlined )
	{
		gSurface->DrawSetTextColor( Color( ) );
		if ( Config->Visuals->OptimizedFonts )
		{
			gSurface->DrawSetTextPos( x + 1, y + 1 );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
		}
		else
		{
			gSurface->DrawSetTextPos( x - 1, y );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

			gSurface->DrawSetTextPos( x + 1, y );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

			gSurface->DrawSetTextPos( x, y - 1 );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
			gSurface->DrawSetTextPos( x, y + 1 );
			gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
		}
	}

	gSurface->DrawSetTextColor( Clr );
	gSurface->DrawSetTextPos( x, y );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
}

void CRendering::String( int x, int y, const EFonts& Font, unsigned int AlignFlags, const Color& Clr, const wchar_t* str, ... )
{
	wchar_t wbuf[ 1024 ];

	va_list vlist;
	va_start( vlist, str );
	vswprintf_s( wbuf, str, vlist );
	va_end( vlist );

	if ( AlignFlags & Center_X )
	{
		int w, h;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		x -= w * 0.5f;
	}

	if ( AlignFlags & Center_Y )
	{
		int w, h;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		y -= h * 0.5f;
	}

	if ( AlignFlags & Reverse_X )
	{
		int w, h;
		gSurface->GetTextSize( Fonts[ Font ], wbuf, w, h );
		x -= w;
	}

	gSurface->DrawSetTextFont( Fonts[ Font ] );

	gSurface->DrawSetTextColor( Color( ) );
	gSurface->DrawSetTextPos( x - 1, y );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

	gSurface->DrawSetTextPos( x + 1, y );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

	gSurface->DrawSetTextPos( x, y - 1 );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

	gSurface->DrawSetTextPos( x, y + 1 );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );

	gSurface->DrawSetTextColor( Clr );
	gSurface->DrawSetTextPos( x, y );
	gSurface->DrawPrintText( wbuf, wcslen( wbuf ) );
}

bool CRendering::TextureRectangle( int x, int y, int w, int h, int ID, const Color& Clr )
{
	if ( !gSurface->IsTextureIDValid( ID ) )
		return false;

	gSurface->DrawSetColor( Clr );
	gSurface->DrawSetTexture( ID );
	gSurface->DrawTexturedRect( x, y, x + w, y + h );
	return true;
}

bool CRendering::Timer( )
{
	int t = clock( );
	static int i = 0;

	if ( t > i )
	{
		i += 1;
		return true;
	}
	return false;
}

float CRendering::EaseIn( float start, float end, float speed )
{
	if ( start < end || speed >= 1 )
		return end;

	if ( Timer( ) )
	{
		if ( start > end )
			return start * speed;
		return end;
	}
	return start;
}

float CRendering::Lerp( float start, float end, float speed )
{
	return start + ( end - start ) * speed;
}

void CRendering::TexturedPolygon( int Count, Vertex_t* Vertex, const Color& Clr )
{
	static int Texture = gSurface->CreateNewTextureID( true );
	gSurface->DrawSetColor( Clr ); // keep this full color and opacity use the RGBA @top to set values.
	gSurface->DrawSetTexture( Texture ); // bind texture
	gSurface->DrawTexturedPolygon( Count, Vertex );
}

int CRendering::CreateTexture( const unsigned char* array, int width, int height, bool avatar )
{
	int iTextureOut = gSurface->CreateNewTextureID( true );
	gSurface->DrawSetTextureRGBAEx( iTextureOut, array, width, height, avatar ? IMAGE_FORMAT_RGBA8888 : IMAGE_FORMAT_BGRA8888 );
	m_vecCreatedTextures.push_back( iTextureOut );
	return iTextureOut;
}

void CRendering::DeleteTexturesFromMemory( )
{
	for ( const auto& texture : m_vecCreatedTextures )
	{
		gSurface->DeleteTextureByID( texture );
	}
}

void CRendering::OutlinedCircle( int x, int y, float radius, int segments, const Color& clr )
{
	/*float Step = PI * 2.0f / segments;

	for ( float a = 0.0f; a < ( PI * 2.0f ); a += Step )
	{
		float x1 = radius * cosf( a ) + x;
		float y1 = radius * sinf( a ) + y;
		float x2 = radius * cosf( a + Step ) + x;
		float y2 = radius * sinf( a + Step ) + y;
		Line( x1, y1, x2, y2, clr, false );
	}*/
	gSurface->DrawSetColor( clr );
	gSurface->DrawOutlinedCircle( x, y, radius, segments );
}

void CRendering::FilledCircle( const int& x, const int& y, const float& radius, const int& segments, const Color& clr )
{
	CUtlVector<Vertex_t> vecVertixes;

	const float flStep = ( 6.28318530718f / static_cast< float >( segments ) );

	for ( float n = 0.0f; n < 6.28318530718f; n += flStep )
		vecVertixes.AddToTail( Vertex_t( { ( static_cast< float >( radius ) * cos( n ) + x ), ( static_cast< float >( radius ) * sinf( n ) + y ) }, { 0.0f, 0.0f } ) );

	if ( vecVertixes.Count( ) > 0 )
	{
		gSurface->DrawSetColor( clr.r, clr.g, clr.b, clr.a );
		gSurface->DrawTexturedPolygon( segments, vecVertixes.Base( ) );
	}
}

void CRendering::RenderLine( const Vector& v1, const Vector& v2, Color Clr, bool ZBuffer )
{
	static auto RenderLine = reinterpret_cast< void( __cdecl* )( const Vector&, const Vector&, Color, bool ) >( G->RenderLineAddress );
	RenderLine( v1, v2, Clr, ZBuffer );
}

void CRendering::RenderBox( const Vector& Origin, const Vector& Angles, const Vector& Mins, const Vector& Maxs, Color c, bool ZBuffer, bool InsideOut )
{
	static auto RenderBox = reinterpret_cast< void( __cdecl* )( const Vector&, const Vector&, const Vector&, const Vector&, Color, bool, bool ) >( G->RenderBoxAddress );
	RenderBox( Origin, Angles, Mins, Maxs, c, ZBuffer, InsideOut );
}

void CRendering::RenderWireframeBox( const Vector& Origin, const Vector& Angles, const Vector& Mins, const Vector& Maxs, Color c, bool ZBuffer )
{
	static auto RenderWireframeBox = reinterpret_cast< void( __cdecl* )( const Vector&, const Vector&, const Vector&, const Vector&, Color, bool ) > ( G->RenderWireframeBoxAddress );
	RenderWireframeBox( Origin, Angles, Mins, Maxs, c, ZBuffer );
}

void CRendering::RenderTriangle( const Vector& p1, const Vector& p2, const Vector& p3, Color Clr, bool ZBuffer )
{
	static auto RenderTriangle = reinterpret_cast< void( __cdecl* )( const Vector&, const Vector&, const Vector&, Color, bool ) >( G->RenderTriangleAddress );
	RenderTriangle( p1, p2, p3, Clr, ZBuffer );
}