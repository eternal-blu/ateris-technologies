#include "Input.h"

bool CInput::InArea( int x, int y, int w, int h )
{
	if ( mouse_x >= x && mouse_x <= x + w && mouse_y >= y && mouse_y <= y + h )
	{
		return true;
	}

	return false;
}

void CInput::Update( )
{
	for ( int n = 0; n < 256; n++ )
	{
		if ( GetAsyncKeyState( n ) & 0x8000 )
		{
			if ( keys[ n ] == PRESSED )
				keys[ n ] = HELD;

			else if ( keys[ n ] != HELD )
				keys[ n ] = PRESSED;
		}

		else keys[ n ] = NONE;
	}

	gSurface->SurfaceGetCursorPos( mouse_x, mouse_y );
}

bool CInput::IsPressed( short key )
{
	if ( PreventInputs )
	{
		return false;

	}

	return ( keys[ key ] == PRESSED );
}

bool CInput::IsHeld( short key )
{
	return ( keys[ key ] == HELD );
}

#include <chrono>

bool CInput::IsPressedAndHeld( short key )
{
	key_state_e key_state = keys[ key ];

	static std::chrono::time_point<std::chrono::steady_clock> start[ 256 ] = { std::chrono::steady_clock::now( ) };

	if ( key_state == PRESSED )
	{
		start[ key ] = std::chrono::steady_clock::now( );
		return true;
	}

	if ( key_state == HELD && std::chrono::duration_cast< std::chrono::milliseconds >(
		std::chrono::steady_clock::now( ) - start[ key ] ).count( ) > 400 )
		return true;

	return false;
}

void CInput::Drag( int& x, int& y, int w, int h, int offsety )
{
	static POINT delta;
	static bool drag = false;
	static bool move = false;
	bool held = IsHeld( VK_LBUTTON );
	bool pressed = IsPressed( VK_LBUTTON );

	auto mousex = mouse_x;
	auto mousey = mouse_y;

	if ( ( mousex > x && mousex < x + w && mousey > y - offsety && mousey < y - offsety + h ) )
	{
		if ( pressed ) {
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
		else if ( held ) {
			drag = true;
			dragging = true;

			if ( !move )
			{
				delta.x = mousex - x;
				delta.y = mousey - y;
				move = true;
			}
		}
	}

	if ( drag )
	{
		x = std::clamp( mousex - ( int )delta.x, 0, TF2->ScreenWidth - 457 );
		y = std::clamp( mousey - ( int )delta.y, 0, TF2->ScreenHeight - 451 );
	}

	if ( !held )
	{
		drag = false;
		move = false;
	}
}

bool CInput::AreaPressed( int x, int y, int w, int h )
{
	static POINT delta;
	static bool heldInArea = false;
	bool held = IsHeld( VK_LBUTTON );

	auto mousex = mouse_x;
	auto mousey = mouse_y;

	if ( ( mousex > x && mousex < x + w && mousey > y && mousey < y + h ) && held )
	{
		heldInArea = true;
	}

	if ( !held )
	{
		heldInArea = false;
	}

	return heldInArea;
}

void CInput::NullKey( short key )
{
	keys[ key ] = key_state_e::NONE;
}

bool CInput::InFocus( )
{
	static HWND hwGame = 0;

	while ( !hwGame )
	{
		hwGame = FindWindowA( 0, "Team Fortress 2" );
		return false;
	}

	return ( GetForegroundWindow( ) == hwGame );
}