#pragma once
#include "../../../SDK/TF2/TF2.h"

class CInput
{
public:
	enum key_state_e { NONE, PRESSED, HELD };
	key_state_e keys[ 256 ] {};

	int mouse_x {}, mouse_y { }; // Mouse X/Y

	bool PreventInputs = false;
	char key_pressed = 0x0;
	int scroll = 0;
	bool hovered = false;
	bool dragging = false;

	void Update( );
	void NullKey( short key );
	bool IsPressed( short key );
	bool IsHeld( short key );
	bool IsPressedAndHeld( short key );
	void Drag( int& x, int& y, int w, int h, int offsetY );
	bool AreaPressed( int x, int y, int w, int h );
	bool InArea( int x, int y, int w, int h );
	bool InFocus( );
};
inline CInput* Input = new CInput;