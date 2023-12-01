#include "Menu.h"
#include "../Visuals/Visuals.h"

#define SAVECOLOR(col)\
Save["Colors"][#col]["Red"] = Config->Colors->col.r;\
Save["Colors"][#col]["Green"] = Config->Colors->col.g;\
Save["Colors"][#col]["Blue"] = Config->Colors->col.b;\
Save["Colors"][#col]["Alpha"] = 255;

#define LOADCOLOR(col)\
if ( !Load["Colors"][#col]["Red"].isNull( ) && !Load["Colors"][#col]["Green"].isNull( ) && !Load["Colors"][#col]["Blue"].isNull( ) )\
{\
	Config->Colors->col.r = char(Load["Colors"][#col]["Red"].asInt( ));\
	Config->Colors->col.g = char(Load["Colors"][#col]["Green"].asInt( ));\
	Config->Colors->col.b = char(Load["Colors"][#col]["Blue"].asInt( ));\
	Config->Colors->col.a = char(255);\
}

void CMenu::Accent( )
{
	static int OldTickCount = gGlobals->tickcount;

	if ( OldTickCount == gGlobals->tickcount )
	{
		return;
	}

	if ( OldTickCount != gGlobals->tickcount )
	{
		OldTickCount = gGlobals->tickcount;
	}

	if ( gGlobals->tickcount % 255 < 128 )
	{
		if ( Accent1Left > 2 )
		{
			Accent1Left -= 2;
		}

		if ( Accent2Left < 253 )
		{
			Accent2Left += 2;
		}

		if ( Accent2Right > 2 )
		{
			Accent2Right -= 2;
		}

		if ( Accent1Right < 253 )
		{
			Accent1Right += 2;
		}

		if ( gGlobals->tickcount % 2 == 0 )
		{
			if ( Config->Colors->MenuAccent.r < Config->Colors->MenuFade2.r )
			{
				Config->Colors->MenuAccent.r += 2;
			}

			if ( Config->Colors->MenuAccent.g > Config->Colors->MenuFade2.g )
			{
				Config->Colors->MenuAccent.g -= 2;
			}

			if ( Config->Colors->MenuAccent.b > Config->Colors->MenuFade2.b )
			{
				Config->Colors->MenuAccent.b -= 2;
			}
		}
	}
	else
	{
		if ( Accent2Left > 2 )
		{
			Accent2Left -= 2;
		}

		if ( Accent1Left < 253 )
		{
			Accent1Left += 2;
		}

		if ( Accent1Right > 2 )
		{
			Accent1Right -= 2;
		}

		if ( Accent2Right < 253 )
		{
			Accent2Right += 2;
		}

		if ( gGlobals->tickcount % 2 == 0 )
		{
			if ( Config->Colors->MenuAccent.r > Config->Colors->MenuFade1.r )
			{
				Config->Colors->MenuAccent.r -= 2;
			}

			if ( Config->Colors->MenuAccent.g < Config->Colors->MenuFade1.g )
			{
				Config->Colors->MenuAccent.g += 2;
			}

			if ( Config->Colors->MenuAccent.b < Config->Colors->MenuFade1.b )
			{
				Config->Colors->MenuAccent.b += 2;
			}
		}
	}
}

void CMenu::RenderWindow( const std::string& label, const Rect_t& size, const Color& background, const Color& Edge1, const Color& Edge2 )
{
	//background
	//Draw->Rectangle( size.x, size.y, size.w, size.h, background, false, true );
	EnableClipping( { WindowSize.x - 1, WindowSize.y - 1, WindowSize.w + 2, WindowSize.h + 2 } );
	//Draw->TextureRectangle( size.x + 1, size.y + 1, size.w - 2, 650, MenuBackground, { 255, 255, 255, 255 } );
	DisableClipping( );
	//Draw->Rectangle( size.x + 1, size.y + 1, size.w - 2, 20, Config->Colors->MenuBackground, false, true );
	Draw->Rectangle( size.x + 1, size.y + 1, size.w - 2, size.h - 2, Config->Colors->MenuBackground, false, true );
	Draw->TextureRectangle( size.x + 2, size.y + 2, 18, 18, AterisLogo18px, Config->Colors->MenuAccent );

	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );
	//Draw->Line( size.x + size.w * 0.5f - wide * 0.5f - 1, size.y + 15, size.x + size.w * 0.5f + wide * 0.5f - 1, size.y + 15, Config->Colors->MenuAccent, false );
	Draw->FadeRectangle( size.x + size.w * 0.5f - wide * 0.5f, size.y + 15 + 2, wide, 1, Config->Colors->MenuAccent, 255, 0, true );
	Draw->String( size.x + size.w * 0.5f, size.y + 9 + 2, FONT_MENU, Center_X | Center_Y, Config->Colors->MenuAccent, true, label.c_str( ) );

	//edge
	Draw->Rectangle( size.x, size.y, size.w, size.h, Edge1, false );
	//Draw->Rectangle( size.x - 1, size.y - 1, size.w + 2, size.h + 2, { 60, 60, 60, 255 }, false );
	//Draw->Rectangle( size.x - 2, size.y - 2, size.w + 4, size.h + 4, { 60, 60, 60, 255 }, false );
	//Draw->Rectangle( size.x - 3, size.y - 3, size.w + 6, size.h + 6, Edge1, false );
	Draw->Rectangle( size.x - 1, size.y - 1, size.w + 2, size.h + 2, background, false );

	//Draw->FadeRectangle( size.x + 2, size.y + 2, size.w - 4, 1, { 80, 188, 188, 255 }, green_left, green_right, true );
	//Draw->FadeRectangle( size.x + 2, size.y + 2, size.w - 4, 1, { 40, 102, 162, 255 }, blue_left, blue_right, true );

	Draw->FadeRectangle( size.x + 2, size.y + 2, size.w - 4, 1, Config->Colors->MenuFade1, Accent1Left, Accent1Right, true );
	Draw->FadeRectangle( size.x + 2, size.y + 2, size.w - 4, 1, Config->Colors->MenuFade2, Accent2Left, Accent2Right, true );

	//horizontal line
	Draw->Line( size.x, size.y + 19 + 2, size.x + size.w - 1, size.y + 19 + 2, Edge1, false );

	//vertical line
	Draw->Line( size.x + 90, size.y + 20 + 2, size.x + 90, size.y + size.h - 1, Edge1, false );

	//top
	//Draw->FadeRectangle( size.x, size.y, size.w + 1, 1, Edge1, Edge2, true );

	//bottom
	//Draw->FadeRectangle( size.x, size.y + size.h, size.w + 1, 1, Edge2, Edge1, true );

	//left
	//Draw->FadeRectangle( size.x, size.y, 1, size.h, Edge1, Edge2, false );

	//right
	//Draw->FadeRectangle( size.x + size.w, size.y, 1, size.h, Edge2, Edge1, false );

	//horizontal line
	//Draw->FadeRectangle( size.x, size.y + 20, size.w + 1, 1, Edge1, Edge2, true );

	//vertical line
	//Draw->FadeRectangle( size.x + 90, size.y + 20, 1, size.h - 20, Edge1, Edge2, false );
}

bool CMenu::MenuTab( const std::string& label, bool active )
{
	int x = LastTabPos.x + 1;
	int y = LastTabPos.y + 1;
	int w = LastTabPos.w - 1;
	int h = LastTabPos.h;

	bool callback = false;
	Color string = { 180, 180, 180, 255 };

	if ( Input->InArea( x, y, w - 1, h ) )
	{
		string = Config->Colors->MenuAccent;
		if ( Input->IsPressed( VK_LBUTTON ) )
		{
			gSurface->PlaySound( "ui/buttonclick.wav" );
			callback = true;
		}

		if ( !active )
			Draw->Rectangle( x + 1, y + 1, w - 2, h - 2, Color( 64, 64, 64 ), false, true );
	}

	Draw->Rectangle( x, y, w, h, Config->Colors->MenuBorders, false );
	Draw->Rectangle( x + 1, y + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );

	if ( ( !Input->InArea( x, y, w, h ) || ( !Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) ) ) && !active )
	{
		Draw->FadeRectangle( x + 1, y + 1, w - 2, h - 2, Config->Colors->MenuShine, 200, 0 );
	}

	if ( active )
	{
		string = Config->Colors->MenuAccent;

		int wide, tall;
		Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );
		Draw->FadeRectangle( x + w * 0.5f - wide * 0.5f - 1, y + ( h * 0.5f ) + 6, wide, 1, Config->Colors->MenuAccent, 255, 0, true );
	}

	Draw->String( x + ( w * 0.5f ), y + ( h * 0.5f ), FONT_MENU, Center_X | Center_Y, string, true, label.c_str( ) );
	//draw.line( x + w, y, x + w, y + h, Config->Colors->m_ui_accent );

	LastTabPos.y = y + h;

	return callback;
}

bool CMenu::MenuSubtab( const std::string& label, bool active )
{
	int x = LastSubtabPos.x;
	int y = LastSubtabPos.y;
	int w = LastSubtabPos.w;
	int h = LastSubtabPos.h;

	bool callback = false;
	Color string = { 180, 180, 180, 255 };

	if ( Input->InArea( x, y, w - 1, h ) )
	{
		string = Config->Colors->MenuAccent;
		if ( Input->IsPressed( VK_LBUTTON ) )
		{
			gSurface->PlaySound( "ui/buttonclick.wav" );
			callback = true;
		}

		if ( !active )
			Draw->Rectangle( x + 1, y + 1, w - 2, h - 1, Color( 64, 64, 64 ), false, true );
	}

	if ( active )
	{
		string = Config->Colors->MenuAccent;
		Draw->Rectangle( x + 1, y + 1, w - 2, h - 2, Color( Config->Colors->MenuBorders.r, Config->Colors->MenuBorders.g, Config->Colors->MenuBorders.b, 80 ), false, true );
	}

	Draw->Rectangle( x, y, w, h, Config->Colors->MenuBorders, false );
	Draw->Rectangle( x + 1, y + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );

	if ( ( !Input->InArea( x, y, w, h ) || ( !Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) ) ) && !active )
	{
		Draw->FadeRectangle( x + 1, y + 1, w - 2, h - 2, Config->Colors->MenuShine, 200, 0 );
	}

	Draw->String( x + ( w * 0.5f ), y + ( h * 0.5f ), FONT_MENU, Center_X | Center_Y, string, true, label.c_str( ) );
	//draw.line( x + w, y, x + w, y + h, Config->Colors->m_ui_accent );

	LastSubtabPos.x = x + w + 1;

	return callback;
}

void CMenu::MenuGroupStart( int offset )
{
	/*LastGroupPos.x = ( right ? WindowSize.x + 90 + 206 : LastGroupPos.x );
	LastGroupY = ( up ? WindowSize.y + 24 : LastGroupPos.y + 2 );
	LastGroupPos.y = ( up ? WindowSize.y + 34 : LastGroupPos.y + 12 );*/
	LastGroupY = LastGroupPos.y + offset;
	MenuSpace( 13 );
}

void CMenu::PushGroupX( int x )
{
	PreviousX = LastGroupPos.x;
	LastGroupPos.x = x;
}

void CMenu::PopGroupX( bool right )
{
	LastGroupPos.x = ( right ? WindowSize.x + 90 + 207 : WindowSize.x + 93 );
}

void CMenu::PushGroupY( int y )
{
	PreviousY = LastGroupPos.y;
	LastGroupPos.y = y;
}

void CMenu::PopGroupY( bool up )
{
	LastGroupPos.y = ( up ? WindowSize.y + 24 : LastGroupPos.y + 1 );
}

void CMenu::MenuSpace( int y )
{
	LastGroupPos.y += y;
}

void CMenu::MenuGroup( const std::string& label, bool unstable )
{
	int x = LastGroupPos.x;
	int y = LastGroupY + 1;
	int w = LastGroupPos.w;
	int h = LastGroupPos.y - LastGroupY - 3;

	Color string;
	if ( unstable )
	{
		string = { 180, 90, 90, 255 };
	}
	else
	{
		string = { 180, 180, 180, 255 };
	}
	/*if ( h > 195 )
	{
		h -= ( ( m_last_group.y - last_group_y ) - 195 );
	}*/

	if ( Input->InArea( x, y, w, h ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		if ( unstable )
		{
			string = { 255, 128, 128, 255 };
		}
		else
		{
			string = Config->Colors->MenuAccent;
		}
	}
	//draw.line( x, y + custoy, x + w, y + custoy, Config->Colors->m_ui_accent );
	Draw->Rectangle( x, y, w, h, Config->Colors->MenuBorders, false );

	int text_w, text_h;
	Draw->GetTextSize( FONT_MENU, text_w, text_h, TF2->Utf8ToWide( label ).c_str( ) );

	Draw->Line( x + ( w / 2 ) - ( text_w / 2 ) - 5, y, x + ( w / 2 ) + ( text_w / 2 ) + 6, y, Config->Colors->MenuBackground, false );
	Draw->String( x + ( w * 0.5f ), y + 1, FONT_MENU, Center_X | Center_Y, string, true, TF2->Utf8ToWide( label ).c_str( ) );
	//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTER, label.c_str( ) );

	LastGroupPos.x = x;
	LastGroupPos.y = y + text_h + h - 10;
}

bool CMenu::Button( const std::string& label, std::function<void( )> callback_func, int width )
{
	bool callback = false;

	int x = LastGroupPos.x;
	int y = LastGroupPos.y - 5;
	int w = width;
	int h = 15;

	float time_pressed = 0.0f;
	Color string = { 180, 180, 180, 255 };

	if ( Input->InArea( x + 5, y, w, h ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		string = Config->Colors->MenuAccent;
	}

	Draw->Rectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );
	Draw->Rectangle( x + 5, y, w, h, Config->Colors->MenuBorders, false );

	if ( !Input->InArea( x + 5, y, w, h ) || ( !Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) ) )
	{
		Draw->FadeRectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuShine, 200, 0 );
	}

	Draw->String( x + ( w * 0.5f ) + 5, y + ( h * 0.5f ), FONT_MENU, Center_X | Center_Y, string, true, label.c_str( ) );

	if ( Input->InArea( x + 5, y, w, h ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		float time = gEngineClient->Time( );
		if ( Input->IsPressed( VK_LBUTTON ) )
		{
			callback_func( );
			callback = true;
			time_pressed = time;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
		else if ( time > time_pressed + 0.1f )
		{
			time_pressed = 0.0f;
		}
	}
	else
	{
		time_pressed = 0.0f;
	}

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 10;

	return callback;
}

bool CMenu::Button( const std::string& label, int width )
{
	bool callback = false;

	int x = LastGroupPos.x;
	int y = LastGroupPos.y - 5;
	int w = width;
	int h = 15;

	float time_pressed = 0.0f;
	Color string = { 180, 180, 180, 255 };

	if ( Input->InArea( x + 5, y, w, h ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		string = Config->Colors->MenuAccent;
	}

	Draw->Rectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );
	Draw->Rectangle( x + 5, y, w, h, Config->Colors->MenuBorders, false );

	if ( !Input->InArea( x + 5, y, w, h ) || ( !Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) ) )
	{
		Draw->FadeRectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuShine, 200, 0 );
	}

	Draw->String( x + ( w * 0.5f ) + 5, y + ( h * 0.5f ), FONT_MENU, Center_X | Center_Y, string, true, label.c_str( ) );

	if ( Input->InArea( x + 5, y, w, h ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		float time = gEngineClient->Time( );
		if ( Input->IsPressed( VK_LBUTTON ) )
		{
			callback = true;
			time_pressed = time;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
		else if ( time > time_pressed + 0.1f )
		{
			time_pressed = 0.0f;
		}
	}
	else
	{
		time_pressed = 0.0f;
	}

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 10;

	return callback;
}

bool CMenu::CheckBox( const std::string& label, bool& var )
{
	int x = LastGroupPos.x;
	int y = LastGroupPos.y;
	int w = 10;
	int h = 10;
	bool callback = false;

	Color string = { 180, 180, 180, 255 };
	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );

	Draw->Rectangle( x + 5, y - 5, w, h, Config->Colors->MenuBorders, false );
	Draw->Rectangle( x + 5 + 1, y - 5 + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );

	if ( Input->InArea( x + 5, y - 5, w + wide + 5, h ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		string = Config->Colors->MenuAccent;
		if ( Input->IsPressed( VK_LBUTTON ) )
		{
			var = !var;
			callback = true;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
	}

	if ( var )
	{
		Draw->Rectangle( x + 5 + 2, y - 5 + 2, w - 4, h - 4, Config->Colors->MenuAccent, false, true );
		Draw->FadeRectangle( x + 5 + 2, y - 5 + 2, w - 4, h - 4, { 0, 0, 0, 255 }, 0, 120 );
	}

	Draw->String( x + 5 + w + 5, y - 5 + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, label.c_str( ) );
	//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_DEFAULT, label.c_str( ) );

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 5;

	return callback;
}

bool CMenu::ToggleableText( const std::wstring& label, bool& var, const Color& selected )
{
	int x = LastGroupPos.x;
	int y = LastGroupPos.y;
	bool callback = false;

	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );

	if ( label.length( ) <= 3 )
		wide *= 3;

	if ( Input->InArea( x + 2, y - tall + 5, wide + 5, tall + 3 ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->IsPressed( VK_LBUTTON ) )
		{
			var = !var;
			callback = true;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
	}

	Draw->String( x + 5, y - 1, FONT_MENU, Center_Y, selected, true, label.c_str( ) );

	LastGroupPos.x = x;
	LastGroupPos.y = y + tall + 6;

	if ( LastGroupPos.y > LowestGroupPos )
	{
		LowestGroupPos = LastGroupPos.y;
	}

	return callback;
}

bool CMenu::Text( const std::string& label, const Color& text )
{
	int x = LastGroupPos.x;
	int y = LastGroupPos.y;
	bool callback = false;

	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );
	Draw->String( x + 5, y - 1, FONT_MENU, Center_Y, text, true, label.c_str( ) );

	LastGroupPos.x = x;
	LastGroupPos.y = y + tall + 6;

	return callback;
}

bool CMenu::CheckBoxKey( const std::string& label, bool& var, int& key )
{
	int x = LastGroupPos.x;
	int y = LastGroupPos.y;
	int w = 10;
	int h = 10;
	bool callback = false;

	Color string = { 180, 180, 180, 255 };
	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );

	Draw->Rectangle( x + 5, y - 5, w, h, Config->Colors->MenuBorders, false );
	Draw->Rectangle( x + 5 + 1, y - 5 + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );

	if ( Input->InArea( x + 5, y - 5, w + wide + 5, h ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		string = Config->Colors->MenuAccent;
		if ( Input->IsPressed( VK_LBUTTON ) )
		{
			var = !var;
			callback = true;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
	}

	if ( var )
	{
		Draw->Rectangle( x + 5 + 2, y - 5 + 2, w - 4, h - 4, Config->Colors->MenuAccent, false, true );
		Draw->FadeRectangle( x + 5 + 2, y - 5 + 2, w - 4, h - 4, { 0, 0, 0, 255 }, 0, 120 );
	}

	Draw->String( x + 5 + w + 5, y - 5 + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, label.c_str( ) );

	auto VK2STR = [ & ] ( const short key ) -> std::string {
		switch ( key ) {
			case VK_LBUTTON: return ( "M1" );
			case VK_RBUTTON: return ( "M2" );
			case VK_MBUTTON: return ( "M3" );
			case VK_XBUTTON1: return ( "M4" );
			case VK_XBUTTON2: return ( "M5" );
			case VK_SHIFT:
			case VK_RSHIFT:
			case VK_LSHIFT: return ( "SHIFT" );
			case VK_MENU: return ( "ALT" );
			case VK_NUMPAD0: return "NUM0";
			case VK_NUMPAD1: return "NUM1";
			case VK_NUMPAD2: return "NUM2";
			case VK_NUMPAD3: return "NUM3";
			case VK_NUMPAD4: return "NUM4";
			case VK_NUMPAD5: return "NUM5";
			case VK_NUMPAD6: return "NUM6";
			case VK_NUMPAD7: return "NUM7";
			case VK_NUMPAD8: return "NUM8";
			case VK_NUMPAD9: return "NUM9";
			case VK_MULTIPLY: return "*";
			case VK_ADD: return "+";
			case VK_SUBTRACT: return "-";
			case VK_DECIMAL: return "DEL";
			case VK_DIVIDE: return "/";
			case 0x0: return ( "-" );
			default: break;
		}

		CHAR output[ 16 ] = { "\0" };

		if ( GetKeyNameTextA( MapVirtualKeyA( key, MAPVK_VK_TO_VSC ) << 16, output, 16 ) )
			return output;

		return ( "-" );
	};

	std::string keystr = "[" + VK2STR( key ) + "]";
	int keywide, keytall;
	Draw->GetTextSize( FONT_SMALL, keywide, keytall, keystr.c_str( ) );

	static float time = gEngineClient->Time( );
	float elapsed = gEngineClient->Time( ) - time;

	static int* curr = nullptr, * prevv = curr;
	if ( curr != prevv )
	{
		time = gEngineClient->Time( );
		prevv = curr;
	}

	if ( curr == nullptr && elapsed > 0.1f && ( Input->IsPressed( VK_LBUTTON ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) )
		 && !SliderIntInfo->Var && !SliderFloatInfo->Var && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->InArea( x + 197 - keywide, y - 5, keywide, keytall ) )
		{
			curr = &key;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
	}

	static float time_notactive = 0.0f;

	if ( curr == &key && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		IsTyping = true;
		if ( gEngineClient->Time( ) - time_notactive > 0.1f )
		{
			for ( short n = 0; n < 256; n++ ) {
				if ( ( n > 0x0 && n < 0x7 ) || ( n > 'A' - 1 && n < 'Z' + 1 ) || ( n > VK_SLEEP && n < VK_F13 ) || n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT || n == VK_ESCAPE || n == VK_INSERT || n == VK_MENU ) {
					if ( Input->IsPressed( n ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) {
						if ( n == VK_INSERT ) {
							curr = nullptr;
							break;
						}

						if ( n == VK_ESCAPE ) {
							key = 0x0;
							curr = nullptr;
							break;
						}

						key = n;
						curr = nullptr;
						callback = true;
						break;
					}
				}
			}
		}

		Draw->String( x + 197, y, FONT_SMALL, Reverse_X |Center_Y, Config->Colors->MenuAccent, true, "[]" );
		//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTER, "Press a Key" );
	}

	else {
		if ( curr == nullptr )
		{
			time_notactive = gEngineClient->Time( );
		}
		Draw->String( x + 197, y, FONT_SMALL, Reverse_X | Center_Y, Config->Colors->MenuAccent, true, keystr.c_str( ) );
		//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTER, "%s", VK2STR( var ).c_str( ) );
	}

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 5;

	return callback;
}

bool CMenu::CheckBoxColor( const std::string& label, bool& var, Color& col )
{
	int x = LastGroupPos.x;
	int y = LastGroupPos.y;
	int w = 10;
	int h = 10;
	bool callback = false;

	Color string = { 180, 180, 180, 255 };
	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );

	Draw->Rectangle( x + 5, y - 5, w, h, Config->Colors->MenuBorders, false );
	Draw->Rectangle( x + 5 + 1, y - 5 + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );

	if ( Input->InArea( x + 5, y - 5, w + wide + 5, h ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		string = Config->Colors->MenuAccent;
		if ( Input->IsPressed( VK_LBUTTON ) )
		{
			var = !var;
			callback = true;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
	}

	if ( var )
	{
		Draw->Rectangle( x + 5 + 2, y - 5 + 2, w - 4, h - 4, Config->Colors->MenuAccent, false, true );
		Draw->FadeRectangle( x + 5 + 2, y - 5 + 2, w - 4, h - 4, { 0, 0, 0, 255 }, 0, 120 );
	}

	Draw->String( x + 5 + w + 5, y - 5 + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, label.c_str( ) );

	int colorw = 15;
	int colorh = 7;

	static float time = gEngineClient->Time( );
	float elapsed = gEngineClient->Time( ) - time;

	static Color* curr = nullptr, * prevv = curr;
	if ( curr != prevv )
	{
		time = gEngineClient->Time( );
		prevv = curr;
	}

	if ( curr == &col )
	{
		if ( elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && IsColorPickerOpen )
		{
			if ( !Input->InArea( ColorInfo->Area.x, ColorInfo->Area.y, 160, 160 ) )
			{
				time = gEngineClient->Time( );
				ColorInfo->Time = gEngineClient->Time( );
				Input->NullKey( VK_LBUTTON );
				IsColorPickerOpen = false;
				ColorInfo->Col = nullptr;
				ColorInfo->HSV[ 0 ] = 0.f;
				ColorInfo->HSV[ 1 ] = 0.f;
				ColorInfo->HSV[ 2 ] = 0.f;
				curr = nullptr;
				gSurface->PlaySound( "ui/buttonclickrelease.wav" );
			}
		}
	}

	if ( curr == nullptr && elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->InArea( x + 195 - colorw, y - 4, colorw, colorh ) )
		{
			time = gEngineClient->Time( );
			ColorInfo->Time = gEngineClient->Time( );
			Input->NullKey( VK_LBUTTON );
			ColorInfo->Col = &col;
			TF2->ColorToHSV( col, ColorInfo->HSV );
			curr = &col;
			ColorInfo->Area = { Input->mouse_x, Input->mouse_y, colorw, colorh };
			IsColorPickerOpen = true;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
	}

	if ( Input->IsPressed( VK_INSERT ) || Input->IsPressed( VK_ESCAPE ) )
	{
		IsColorPickerOpen = false;
		ColorInfo->Col = nullptr;
		ColorInfo->HSV[ 0 ] = 0.f;
		ColorInfo->HSV[ 1 ] = 0.f;
		ColorInfo->HSV[ 2 ] = 0.f;
		curr = nullptr;
	}

	Draw->Rectangle( x + 195 - colorw, y - 4, colorw, colorh, col, false, true );
	Draw->Rectangle( x + 195 - colorw - 1, y - 4 - 1, colorw + 2, colorh + 2, Config->Colors->MenuBorders, false );

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 5;

	return callback;
}

bool CMenu::ColorPicker( const std::string& label, Color& col )
{
	int x = LastGroupPos.x;
	int y = LastGroupPos.y;
	int w = 10;
	int h = 10;
	bool callback = false;

	Color string = { 180, 180, 180, 255 };
	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );

	if ( Input->InArea( x + 5, y - 5, wide + 5, h ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		string = Config->Colors->MenuAccent;
	}

	int colorw = 15;
	int colorh = 7;

	static float time = gEngineClient->Time( );
	float elapsed = gEngineClient->Time( ) - time;

	static Color* curr = nullptr, * prevv = curr;
	if ( curr != prevv )
	{
		time = gEngineClient->Time( );
		prevv = curr;
	}

	if ( curr == &col )
	{
		if ( elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && IsColorPickerOpen )
		{
			if ( !Input->InArea( ColorInfo->Area.x, ColorInfo->Area.y, 160, 160 ) )
			{
				time = gEngineClient->Time( );
				ColorInfo->Time = gEngineClient->Time( );
				Input->NullKey( VK_LBUTTON );
				IsColorPickerOpen = false;
				ColorInfo->Col = nullptr;
				ColorInfo->HSV[ 0 ] = 0.f;
				ColorInfo->HSV[ 1 ] = 0.f;
				ColorInfo->HSV[ 2 ] = 0.f;
				curr = nullptr;
				gSurface->PlaySound( "ui/buttonclickrelease.wav" );
			}
		}
	}

	if ( curr == nullptr && elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->InArea( x + 195 - colorw, y - 4, colorw, colorh ) )
		{
			time = gEngineClient->Time( );
			ColorInfo->Time = gEngineClient->Time( );
			Input->NullKey( VK_LBUTTON );
			ColorInfo->Col = &col;
			TF2->ColorToHSV( col, ColorInfo->HSV );
			curr = &col;
			ColorInfo->Area = { Input->mouse_x, Input->mouse_y, colorw, colorh };
			IsColorPickerOpen = true;
			gSurface->PlaySound( "ui/buttonclick.wav" );
			callback = true;
		}
	}

	if ( Input->IsPressed( VK_INSERT ) || Input->IsPressed( VK_ESCAPE ) )
	{
		IsColorPickerOpen = false;
		ColorInfo->Col = nullptr;
		ColorInfo->HSV[ 0 ] = 0.f;
		ColorInfo->HSV[ 1 ] = 0.f;
		ColorInfo->HSV[ 2 ] = 0.f;
		curr = nullptr;
	}

	Draw->String( x + 5, y - 5 + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, label.c_str( ) );
	Draw->Rectangle( x + 195 - colorw, y - 4, colorw, colorh, col, false, true );
	Draw->Rectangle( x + 195 - colorw - 1, y - 4 - 1, colorw + 2, colorh + 2, Config->Colors->MenuBorders, false );

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 5;

	return callback;
}

//todo: add hex/rgb values to this pasted dogshit
bool CMenu::UpdateColor( )
{
	bool callback = false;
	int w = ColorInfo->Area.w;
	int h = ColorInfo->Area.h;
	int x = ColorInfo->Area.x;
	int y = ColorInfo->Area.y;

	float elapsed = gEngineClient->Time( ) - ColorInfo->Time;
	if ( ColorInfo->Col && elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && IsColorPickerOpen )
	{
		if ( !Input->InArea( x, y, 160, 160 ) )
		{
			ColorInfo->Time = gEngineClient->Time( );
			Input->NullKey( VK_LBUTTON );
			IsColorPickerOpen = false;
			ColorInfo->Col = nullptr;
			gSurface->PlaySound( "ui/buttonclickrelease.wav" );
		}
	}

	//I FUCKING LOVE USING STATIC OFFSETS
	if ( ColorInfo->Col && !IsMultiComboboxOpen && !IsComboboxOpen && IsColorPickerOpen )
	{
		Draw->Rectangle( x, y, 156, 140, Config->Colors->MenuBackground, false, true );
		Draw->Rectangle( x - 1, y - 1, 156 + 2, 140 + 2, Config->Colors->MenuBorders, false );
		Draw->Rectangle( x - 2, y - 2, 156 + 4, 140 + 4, Config->Colors->MenuBackground, false );

		auto& square = *ColorInfo->Col;
		const auto squareclr = TF2->HSVToColor( ColorInfo->HSV[ 0 ], 100, 100 );
		square.r = squareclr.r;
		square.g = squareclr.g;
		square.b = squareclr.b;

		//Main colorpicker rect
		{
			//white top left
			gSurface->DrawSetColor( { 255, 255, 255, 255 } );
			gSurface->DrawFilledRectFade( x + 4, y + 4, x + 136, y + 136, 255, 0, true );

			//colored top right
			gSurface->DrawSetColor( { square.r, square.g, square.b, 255 } );
			gSurface->DrawFilledRectFade( x + 4, y + 4, x + 136, y + 136, 0, 255, true );
			gSurface->DrawFilledRectFade( x + 4, y + 4, x + 136, y + 136, 0, 128, true );

			//black bottom
			gSurface->DrawSetColor( { 0, 0, 0, 255 } );
			gSurface->DrawFilledRectFade( x + 4, y + 4, x + 136, y + 136, 0, 255, false );
			gSurface->DrawFilledRectFade( x + 4, y + 4, x + 136, y + 136, 0, 255, false );
			gSurface->DrawFilledRectFade( x + 4, y + 4, x + 136, y + 136, 0, 128, false );
			Draw->Rectangle( x + 3, y + 3, 134, 134, Config->Colors->MenuBorders, false );

			Vector2D picker;

			int width = 131;
			int height = 131;
			const float ratiox = width / 100.f;
			const float ratioy = height / 100.f;
			picker.x = ColorInfo->HSV[ 1 ] * ratiox;
			picker.y = 132.f - ( ColorInfo->HSV[ 2 ] * ratioy );
			Draw->Rectangle( x + 5 + picker.x - 1, y + 5 + picker.y - 1, 4, 4, { 128, 128, 128, 255 }, false );

			if ( Input->InArea( x + 5, y + 5, 132, 132 ) )
			{
				if ( Input->IsHeld( VK_LBUTTON ) )
				{
					picker.x = std::clamp( Input->mouse_x, ( x + 5 ), ( x + 132 ) );
					picker.y = std::clamp( Input->mouse_y, ( y + 5 ), ( y + 132 ) );
					ColorInfo->HSV[ 1 ] = ( ( Input->mouse_x - ( x + 5.f ) ) / ( 132 ) ) * ( 100.f );
					ColorInfo->HSV[ 2 ] = 100.f - ( ( Input->mouse_y - ( y + 5.f ) ) / ( 132 ) ) * ( 100.f );
					callback = true;
				}
			}
		}

		//Hue Slider
		{
			static Color hueColors[ 7 ] =
			{
				{ 255, 0, 0, 255 },
				{ 255, 200, 0, 255 },
				{ 0, 255, 0, 255 },
				{ 0, 255, 255, 255 },
				{ 0, 0, 255, 255 },
				{ 255, 0, 255, 255 },
				{ 255, 0, 0, 255 }
			};

			int hueheight = 132;
			for ( auto i = 0; i < 6; i++ )
			{
				Draw->FadeRectangle( x + 141, ( y + 4 ) + i * ( hueheight / 6.f ), 11, 22, hueColors[ i ], hueColors[ i + 1 ], false );
			}
			Draw->Rectangle( x + 140, y + 3, 13, hueheight + 2, Config->Colors->MenuBorders, false );

			int height = hueheight - 1;
			const float ratio = height / 359.f;
			float hueslidery = ColorInfo->HSV[ 0 ] * ratio;

			//_x + 9, _y + 9, 121

			Draw->Rectangle( x + 140, y + 4 + hueslidery, 13, 3, { 0, 0, 0, 255 }, false );
			Draw->Rectangle( x + 141, y + 5 + hueslidery, 11, 1, { 255, 255, 255, 255 }, false );
			if ( Input->InArea( x + 141, y + 5, 11, hueheight ) )
			{
				if ( Input->IsHeld( VK_LBUTTON ) )
				{
					hueslidery = std::clamp( Input->mouse_y, ( y + 5 ), ( y + hueheight ) );
					ColorInfo->HSV[ 0 ] = ( ( ( ( float )Input->mouse_y - ( float )( y + 5 ) ) / ( ( float )( hueheight ) ) ) * ( 359.f ) );
					callback = true;
				}
			}
		}

		if ( Input->IsHeld( VK_LCONTROL ) || Input->IsPressedAndHeld( VK_LCONTROL ) )
		{
			if ( Input->IsPressed( 0x43 ) )
			{
				Copied = true;
				ColorInfo->Clipboard[ 0 ] = ColorInfo->HSV[ 0 ];
				ColorInfo->Clipboard[ 1 ] = ColorInfo->HSV[ 1 ];
				ColorInfo->Clipboard[ 2 ] = ColorInfo->HSV[ 2 ];
			}

			if ( Input->IsPressed( 0x56 ) && Copied )
			{
				ColorInfo->HSV[ 0 ] = ColorInfo->Clipboard[ 0 ];
				ColorInfo->HSV[ 1 ] = ColorInfo->Clipboard[ 1 ];
				ColorInfo->HSV[ 2 ] = ColorInfo->Clipboard[ 2 ];
				callback = true;
			}
		}

		auto squareclr2 = TF2->HSVToColor( ColorInfo->HSV[ 0 ], ColorInfo->HSV[ 1 ], ColorInfo->HSV[ 2 ] );

		square.r = squareclr2.r;
		square.g = squareclr2.g;
		square.b = squareclr2.b;
	}

	return callback;
}

bool CMenu::SliderInt( const std::string& label, int& var, int min, int max, int width, int step, const char* format )
{
	int x = LastGroupPos.x;
	int y = LastGroupPos.y + 7;
	int w = width;
	int h = 7;

	bool callback = false;
	Color string = { 180, 180, 180, 255 };

	auto fraction = gMath->ReMapValClamped( ( float )var / step * step, min, max, 0.0f, 1.0f );

	int sliderx = ( fraction * w ) + 1;
	if ( sliderx > w - 1 )
		sliderx = w - 1;

	Draw->Rectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );
	Draw->Rectangle( x + 5 + 1, y + 1, sliderx - 1, h - 2, Config->Colors->MenuAccent, false, true );
	Draw->FadeRectangle( x + 5 + 1, y + 1 + h * 0.5f, sliderx - 1, h * 0.5f - 2, { 0, 0, 0, 255 }, 0, 120 );

	if ( !Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && SliderIntInfo->Var )
	{
		SliderIntInfo->Area = Rect_t();
		SliderIntInfo->Var = nullptr;
	}

	if ( Input->InArea( ( x + 5 ), y, w, h ) && !SliderIntInfo->Var && !SliderFloatInfo->Var && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->IsPressed( VK_LBUTTON ) )
		{
			gSurface->PlaySound( "ui/buttonclickrelease.wav" );
			SliderIntInfo->Area = { ( x + 5 ), y, w, h };
			SliderIntInfo->Var = &var;
		}
		string = Config->Colors->MenuAccent;
	}

	if ( Input->IsHeld( VK_LBUTTON ) && SliderIntInfo->Var == &var )
	{
		float fraction = std::clamp( ( float )( Input->mouse_x - ( x + 5 ) ) / w, 0.0f, 1.0f );

		int newval = ( int )( fraction * ( max - min ) ) / step * step + min;
		if ( newval != var )
		{
			var = newval;
			callback = true;
		}
	}

	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );
	if ( Input->InArea( x + 5, y - 13, wide, tall ) )
	{
		string = Config->Colors->MenuAccent;
	}

	Draw->String( x + 5, y - 13, FONT_MENU, 0, string, true, label.c_str( ) );
	Draw->Rectangle( x + 5, y, w, h, Config->Colors->MenuBorders, false );

	Draw->String( x + w + 5, y - 13, FONT_MENU, Reverse_X, string, true, format ? format : "%i", var );
	//Draw->Rectangle( x + sliderx + 3, y - 1, 2, 9, Config->Colors->MenuAccent, false );

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 10;

	return callback;
}

bool CMenu::SliderFloat( const std::string& label, float& var, float min, float max, int width, float step, const char* format )
{
	int x = LastGroupPos.x;
	int y = LastGroupPos.y + 7;
	int w = width;
	int h = 7;

	bool callback = false;
	Color string = { 180, 180, 180, 255 };

	auto fraction = gMath->ReMapValClamped( ( float )var / ( 1 / step ) * ( 1 / step ), min, max, 0.0f, 1.0f );

	int sliderx = ( fraction * w ) + 1;
	if ( sliderx > w - 1 )
		sliderx = w - 1;

	Draw->Rectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );
	Draw->Rectangle( x + 5 + 1, y + 1, sliderx - 1, h - 2, Config->Colors->MenuAccent, false, true );
	Draw->FadeRectangle( x + 5 + 1, y + 1 + h * 0.5f, sliderx - 1, h * 0.5f - 2, { 0, 0, 0, 255 }, 0, 120 );

	if ( !Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && SliderFloatInfo->Var )
	{
		SliderFloatInfo->Area = Rect_t( );
		SliderFloatInfo->Var = nullptr;
	}

	if ( Input->InArea( ( x + 5 ), y, w, h ) && !SliderIntInfo->Var && !SliderFloatInfo->Var && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->IsPressed( VK_LBUTTON ) )
		{
			gSurface->PlaySound( "ui/buttonclickrelease.wav" );
			SliderFloatInfo->Area = { ( x + 5 ), y, w, h };
			SliderFloatInfo->Var = &var;
		}
		string = Config->Colors->MenuAccent;
	}

	if ( Input->IsHeld( VK_LBUTTON ) && SliderFloatInfo->Var == &var )
	{
		float fraction = std::clamp( ( float )( Input->mouse_x - ( x + 5 ) ) / w, 0.0f, 1.0f );

		float newval = ( fraction * ( max - min ) ) / ( 1 / step ) * ( 1 / step ) + min;
		if ( newval != var )
		{
			var = newval;
			callback = true;
		}
	}

	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );
	if ( Input->InArea( x + 5, y - 13, wide, tall ) )
	{
		string = Config->Colors->MenuAccent;
	}

	Draw->String( x + 5, y - 13, FONT_MENU, 0, string, true, label.c_str( ) );
	Draw->Rectangle( x + 5, y, w, h, Config->Colors->MenuBorders, false );

	Draw->String( x + w + 5, y - 13, FONT_MENU, Reverse_X, string, true, format, var );
	//Draw->Rectangle( x + sliderx + 3, y - 1, 2, 9, Config->Colors->MenuAccent, false );

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 10;

	return callback;
}

bool CMenu::KeyBind( const std::string& label, int& var )
{
	int x = LastGroupPos.x;
	int y = LastGroupPos.y;
	int w = 10;
	int h = 10;
	bool callback = false;

	Color string = Color( 180, 180, 180 );
	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );

	if ( Input->InArea( x + 5, y - 5, wide + 5, h ) )
	{
		string = Config->Colors->MenuAccent;
	}

	auto VK2STR = [ & ] ( const short key ) -> std::string {
		switch ( key ) {
			case VK_LBUTTON: return "M1";
			case VK_RBUTTON: return "M2";
			case VK_MBUTTON: return "M3";
			case VK_XBUTTON1: return "M4";
			case VK_XBUTTON2: return "M5";
			case VK_SHIFT:
			case VK_RSHIFT:
			case VK_LSHIFT: return "SHIFT";
			case VK_MENU: return "ALT";
			case VK_NUMPAD0: return "NUM0";
			case VK_NUMPAD1: return "NUM1";
			case VK_NUMPAD2: return "NUM2";
			case VK_NUMPAD3: return "NUM3";
			case VK_NUMPAD4: return "NUM4";
			case VK_NUMPAD5: return "NUM5";
			case VK_NUMPAD6: return "NUM6";
			case VK_NUMPAD7: return "NUM7";
			case VK_NUMPAD8: return "NUM8";
			case VK_NUMPAD9: return "NUM9";
			case VK_MULTIPLY: return "*";
			case VK_ADD: return "+";
			case VK_SUBTRACT: return "-";
			case VK_DECIMAL: return "DEL";
			case VK_DIVIDE: return "/";
			case 0x0: return ( "-" );
			default: break;
		}

		CHAR output[ 16 ] = { "\0" };

		if ( GetKeyNameTextA( MapVirtualKeyA( key, MAPVK_VK_TO_VSC ) << 16, output, 16 ) )
			return output;

		return ( "-" );
	};

	std::string keystr = "[" + VK2STR( var ) + "]";
	int keywide, keytall;
	Draw->GetTextSize( FONT_SMALL, keywide, keytall, keystr.c_str( ) );

	static float time = gEngineClient->Time( );
	float elapsed = gEngineClient->Time( ) - time;

	static int* curr = nullptr, * prevv = curr;
	if ( curr != prevv )
	{
		time = gEngineClient->Time( );
		prevv = curr;
	}

	if ( curr == nullptr && elapsed > 0.1f && ( Input->IsPressed( VK_LBUTTON ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) 
		 && !SliderIntInfo->Var && !SliderFloatInfo->Var && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->InArea( x + 197 - keywide, y - 5, keywide, keytall ) )
		{
			curr = &var;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
	}

	static float time_notactive = 0.0f;

	if ( curr == &var && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		IsTyping = true;
		if ( gEngineClient->Time( ) - time_notactive > 0.1f )
		{
			for ( short n = 0; n < 256; n++ ) {
				if ( ( n > 0x0 && n < 0x7 ) || ( n > 'A' - 1 && n < 'Z' + 1 ) || ( n > VK_SLEEP && n < VK_F13 ) || n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT || n == VK_ESCAPE || n == VK_INSERT || n == VK_MENU ) {
					if ( Input->IsPressed( n ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) {
						if ( n == VK_INSERT ) {
							curr = nullptr;
							break;
						}

						if ( n == VK_ESCAPE ) {
							var = 0x0;
							curr = nullptr;
							break;
						}

						var = n;
						curr = nullptr;
						callback = true;
						break;
					}
				}
			}
		}

		Draw->String( x + 197, y, FONT_SMALL, Reverse_X | Center_Y, Config->Colors->MenuAccent, true, "[]" );
		//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTER, "Press a Key" );
	}

	else {
		if ( curr == nullptr )
		{
			time_notactive = gEngineClient->Time( );
		}
		Draw->String( x + 197, y, FONT_SMALL, Reverse_X | Center_Y, Config->Colors->MenuAccent, true, keystr.c_str( ) );
		//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTER, "%s", VK2STR( var ).c_str( ) );
	}

	if ( Input->InArea( x + 197 - keywide, y - 5, keywide, keytall ) )
	{
		string = Color( 255, 255, 255 );
	}

	Draw->String( x + 5, y - 5 + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, label.c_str( ) );

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 5;

	return callback;
}

/*bool CMenu::KeyBind( const std::string& label, int& var )
{
	auto VK2STR = [ & ] ( const short key ) -> std::string {
		switch ( key ) {
			case VK_LBUTTON: return ( "Left Mouse" );
			case VK_RBUTTON: return ( "Right Mouse" );
			case VK_MBUTTON: return ( "Middle Mouse" );
			case VK_XBUTTON1: return ( "Mouse4" );
			case VK_XBUTTON2: return ( "Mouse5" );
			case VK_MENU: return ( "Alt" );
			case 0x0: return ( "None" );
			default: break;
		}

		CHAR output[ 16 ] = { "\0" };

		if ( GetKeyNameTextA( MapVirtualKeyA( key, MAPVK_VK_TO_VSC ) << 16, output, 16 ) )
			return output;

		return ( "None" );
	};

	bool callback = false;
	Color string = { 180, 180, 180, 255 };

	int x = LastGroupPos.x;
	int y = LastGroupPos.y - 5;
	int w = 90;
	int h = 15;

	static float time = gEngineClient->Time( );
	float elapsed = gEngineClient->Time( ) - time;

	static int* curr = nullptr, * prevv = curr;
	if ( curr != prevv )
	{
		time = gEngineClient->Time( );
		prevv = curr;
	}

	if ( curr == nullptr && elapsed > 0.1f && ( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->InArea( x + 5, y, w, h ) )
		{
			curr = &var;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
	}

	static float time_notactive = 0.0f;

	Draw->Rectangle( x + 5, y, w, h, Config->Colors->MenuBorders, false );
	Draw->Rectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );
	if ( ( Input->InArea( x + 5, y, w, h ) || curr == &var ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		string = Config->Colors->MenuAccent;
	}

	if ( curr != &var )
	{
		if ( !Input->InArea( x + 5, y, w, h ) )
		{
			Draw->FadeRectangle( x + 5 + 1, y + 1, w - 2, h - 2, { 255, 255, 255, 255 }, 5, 0 );
		}
		else if ( !( GetAsyncKeyState( VK_LBUTTON ) & 0x8000 ) )
		{
			Draw->FadeRectangle( x + 5 + 1, y + 1, w - 2, h - 2, { 255, 255, 255, 255 }, 5, 0 );
		}
	}

	//if ( curr == &var )
	//{
	//	Draw->Rectangle( x + 6, y + 1, w - 2, h - 2, { Config->Colors->MenuAccent.r, Config->Colors->MenuAccent.g, Config->Colors->MenuAccent.b, 80 }, false, true );
	//}

	if ( curr == &var && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		IsTyping = true;
		if ( gEngineClient->Time( ) - time_notactive > 0.1f )
		{
			for ( short n = 0; n < 256; n++ ) {
				if ( ( n > 0x0 && n < 0x7 ) || ( n > 'A' - 1 && n < 'Z' + 1 ) || n == VK_LSHIFT || n == VK_RSHIFT || n == VK_SHIFT || n == VK_ESCAPE || n == VK_INSERT || n == VK_MENU) {
					if ( GetAsyncKeyState( n ) & 0x8000 ) {
						if ( n == VK_INSERT ) {
							curr = nullptr;
							break;
						}

						if ( n == VK_ESCAPE ) {
							var = 0x0;
							curr = nullptr;
							break;
						}

						var = n;
						curr = nullptr;
						callback = true;
						break;
					}
				}
			}
		}

		Draw->String( x + ( w * 0.5f ) + 5, y + ( h * 0.5f ), FONT_MENU, Center_X | Center_Y, string, true, "Press a Key" );
		//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTER, "Press a Key" );
	}

	else {
		if ( curr == nullptr )
		{
			time_notactive = gEngineClient->Time( );
		}
		Draw->String( x + ( w * 0.5f ) + 5, y + ( h * 0.5f ), FONT_MENU, Center_X | Center_Y, string, true, VK2STR( var ).c_str( ) );
		//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTER, "%s", VK2STR( var ).c_str( ) );
	}

	Draw->String( x + w + 10, y + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, label.c_str( ) );
	//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTERVERTICAL, label.c_str( ) );

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 10;

	return callback;
}*/

bool CMenu::TextBox( const std::string& label, std::string& var, int width )
{
	bool callback = false;
	Color string = { 180, 180, 180, 255 };

	int x = LastGroupPos.x;
	int y = LastGroupPos.y - 5;
	int w = width;
	int h = 15;

	static float time = gEngineClient->Time( );
	float elapsed = gEngineClient->Time( ) - time;

	static std::string* curr = nullptr, * prevv = curr;
	std::string preview;
	if ( curr != prevv )
	{
		time = gEngineClient->Time( );
		prevv = curr;
	}

	if ( curr == nullptr && elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->InArea( x + 5, y, w, h ) )
		{
			time = gEngineClient->Time( );
			gSurface->PlaySound( "ui/buttonclick.wav" );
			curr = &var;
			Input->NullKey( VK_LBUTTON );
		}
	}

	static float time_notactive = 0.0f;

	Draw->Rectangle( x + 5, y, w, h, Config->Colors->MenuBorders, false );
	Draw->Rectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );
	if ( ( Input->InArea( x + 5, y, w, h ) || curr == &var ) && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen )
	{
		string = Config->Colors->MenuAccent;
	}

	if ( curr != &var )
	{
		if ( !Input->InArea( x + 5, y, w, h ) )
		{
			Draw->FadeRectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuShine, 200, 0 );
		}
		else if ( !Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) )
		{
			Draw->FadeRectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuShine, 200, 0 );
		}
	}

	if ( curr == &var && !IsComboboxOpen && !IsMultiComboboxOpen && !IsColorPickerOpen && IsOpen )
	{
		IsTyping = true;
		if ( ( Input->IsPressed( VK_INSERT ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) || ( Input->IsPressed( VK_ESCAPE ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) )
		{
			curr = nullptr;
		}

		if ( var.length( ) < 32 )
		{
			if ( Input->IsPressed( VK_SPACE ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) )
				var += char( VK_SPACE );

			if ( Input->IsPressed( VK_OEM_MINUS ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) )
			{
				if ( ( Input->IsPressed( VK_LSHIFT ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) || ( Input->IsHeld( VK_RSHIFT ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) )
					var += '_';
				else
					var += '-';
			}

			for ( int16_t key = 'A'; key < 'Z' + 1; key++ )
			{
				if ( Input->IsPressed( key ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) )
				{
					if ( ( Input->IsPressed( VK_LSHIFT ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) || ( Input->IsHeld( VK_RSHIFT ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) || ( ( GetKeyState( VK_CAPITAL ) & 0x1 ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) )
						var += char( key );

					else var += char( key + 32 );
				}
			}

			for ( int16_t key = '0'; key < '9' + 1; key++ )
			{
				if ( Input->IsPressed( key ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) )
				{
					var += char( key );
				}
			}
		}

		if ( ( Input->IsPressedAndHeld( VK_BACK ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) ) && !var.empty( ) )
			var.erase( var.end( ) - 1 );

		if ( Input->IsPressed( VK_RETURN ) && Input->InFocus( ) && gSurface->IsCursorVisible( ) )
		{
			curr = nullptr;
			callback = true;
		}

		preview = var;
		int text_w, text_h;
		Draw->GetTextSize( FONT_MENU, text_w, text_h, TF2->Utf8ToWide( preview ).c_str( ) );

		if ( text_w > w - 5 )
		{
			preview.erase( preview.end( ) - 10, preview.end( ) );
			preview.append( "..." );
		}

		Draw->Rectangle( x + 6, y + 1, w - 2, h - 2, { Config->Colors->MenuBorders.r, Config->Colors->MenuBorders.g, Config->Colors->MenuBorders.b, 80 }, false, true );
		Draw->String( x + ( w * 0.5f ) + 5, y + ( h * 0.5f ), FONT_MENU, Center_X | Center_Y, string, true, preview.empty( ) ? ( "Enter text" ) : preview.c_str( ) );
		//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTER, preview.empty( ) ? ( "Enter text" ) : preview.c_str( ) );
	}

	else
	{
		preview = var;

		int text_w, text_h;
		Draw->GetTextSize( FONT_MENU, text_w, text_h, TF2->Utf8ToWide( preview ).c_str( ) );

		if ( text_w > w - 5 )
		{
			const int count = preview.length( ) - 7;
			preview.erase( preview.end( ) - ( int )count, preview.end( ) );
			preview.append( "..." );
		}

		Draw->String( x + ( w * 0.5f ) + 5, y + ( h * 0.5f ), FONT_MENU, Center_X | Center_Y, string, true, preview.empty( ) ? ( "Enter text" ) : preview.c_str( ) );
		//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTER, preview.empty( ) ? ( "Enter text" ) : preview.c_str( ) );
	}

	Draw->String( x + w + 10, y + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, label.c_str( ) );
	//draw.outlined_string( ESP, , , { 255, 255, 255, 255 }, ALIGN_CENTERVERTICAL, label.c_str( ) );

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 10;

	return callback;
}

bool CMenu::ComboBox( const std::string& label, int& var, std::vector< const char* > items, int width )
{
	bool callback = false;
	Color string = { 180, 180, 180, 255 };

	int x = LastGroupPos.x;
	int y = LastGroupPos.y - 5;
	int w = width;
	int h = 15;

	int size = items.size( ) ? items.size( ) : 0;
	int real_height = size * 15 - size - 1;

	static float time = gEngineClient->Time( );
	float elapsed = gEngineClient->Time( ) - time;

	static int* curr = nullptr, * prev = curr;
	std::string preview;
	if ( curr != prev )
	{
		time = gEngineClient->Time( );
		prev = curr;
	}

	if ( curr == &var )
	{
		Draw->Rectangle( x + 6, y + 1, w - 2, h - 2, { Config->Colors->MenuBorders.r, Config->Colors->MenuBorders.g, Config->Colors->MenuBorders.b, 40 }, false, true );
		if ( elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && IsComboboxOpen )
		{
			if ( !Input->InArea( x + 5, y + h, w, real_height ) )
			{
				time = gEngineClient->Time( );
				ComboInfo->Time = gEngineClient->Time( );
				Input->NullKey( VK_LBUTTON );
				IsComboboxOpen = false;
				ComboInfo->Var = nullptr;
				curr = nullptr;
				gSurface->PlaySound( "ui/buttonclickrelease.wav" );
			}
		}
	}

	if ( curr == nullptr && elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->InArea( x + 5, y, w, h ) )
		{
			time = gEngineClient->Time( );
			ComboInfo->Time = gEngineClient->Time( );
			Input->NullKey( VK_LBUTTON );
			ComboInfo->Var = &var;
			curr = &var;
			ComboInfo->Area = { x + 5, y, w, h };
			ComboInfo->Items = items;
			IsComboboxOpen = true;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
	}

	if ( Input->IsPressed( VK_INSERT ) || Input->IsPressed( VK_ESCAPE ) )
	{
		IsComboboxOpen = false;
		ComboInfo->Var = nullptr;
		curr = nullptr;
	}

	Draw->Rectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );
	Draw->Rectangle( x + 5, y, w, h, Config->Colors->MenuBorders, false );

	if ( !Input->InArea( x + 5, y, w, h ) )
	{
		Draw->FadeRectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuShine, 200, 0 );
	}
	else if ( !Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) )
	{
		Draw->FadeRectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuShine, 200, 0 );
	}

	int wide = 0, tall = 0;

	if ( !label.empty( ) )
	{
		Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );
	}

	if ( Input->InArea( x + 5, y, w + wide, h ) || curr == &var )
	{
		string = Config->Colors->MenuAccent;
	}

	//Draw->Rectangle( x + 5 + w - 15, y, 15, 15, Config->Colors->m_ui_accent, false );
	Draw->Line( x + 5 + w - 15, y, x + 5 + w - 15, y + 14, Config->Colors->MenuBorders, false );
	Draw->Rectangle( x + 5 + w - 14, y + 1, 13, 13, Config->Colors->MenuBackground, false, true );

	if ( Input->InArea( x + 5, y, w, h ) || curr == &var )
	{
		Draw->Rectangle( x + 5 + w - 14, y + 1, 13, 13, { Config->Colors->MenuBorders.r, Config->Colors->MenuBorders.g, Config->Colors->MenuBorders.b, 20 }, false, true );
	}

	/*Draw->Line( x + 5 + w - 11,
				y + 6,
				x + 5 + 7 + w - 15,
				y + 3 + 6, Config->Colors->MenuBorders, false );

	Draw->Line( x + 5 + w - 5,
				y + 6,
				x + 5 + 7 + w - 15,
				y + 3 + 6, Config->Colors->MenuBorders, false );

	Draw->Line( x + 5 + w - 11,
				y + 6,
				x + 6 + 7 + w - 15,
				y + 4 + 6, Config->Colors->MenuBorders, false );

	Draw->Line( x + 6 + 7 + w - 16,
				y + 3 + 6,
				x + 5 + w - 4,
				y + 5, Config->Colors->MenuBorders, false );*/

	Draw->TextureRectangle( x + 4 + w - 14, y + 1, 13, 13, ComboArrow, Config->Colors->MenuBorders );

	if ( !items.empty( ) )
	{
		preview = items.at( var );

		if ( wide > w - 5 )
		{
			const int count = preview.length( ) - 5;
			preview.erase( preview.end( ) - ( int )count, preview.end( ) );
			preview.append( "..." );
		}

		Draw->String( x + 5 + 5, y + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, preview.c_str( ) );
	}
	Draw->String( x + w + 10, y + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, label.c_str( ) );

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 10;

	//m_combo_info.m_var = prev;

	return callback;
}

bool CMenu::UpdateCombo( )
{
	bool callback = false;
	int x = ComboInfo->Area.x;
	int y = ComboInfo->Area.y - 1;
	int w = ComboInfo->Area.w;
	int h = ComboInfo->Area.h;
	int real_height = ComboInfo->Items.size( ) * 15 - ComboInfo->Items.size( );
	Color string = { 180, 180, 180, 255 };

	float elapsed = gEngineClient->Time( ) - ComboInfo->Time;
	if ( ComboInfo->Var && elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && IsComboboxOpen )
	{
		if ( !Input->InArea( x, y + h, w, real_height ) )
		{
			ComboInfo->Time = gEngineClient->Time( );
			Input->NullKey( VK_LBUTTON );
			IsComboboxOpen = false;
			ComboInfo->Var = nullptr;
			gSurface->PlaySound( "ui/buttonclickrelease.wav" );
		}
	}

	if ( ComboInfo->Var && !IsMultiComboboxOpen && !IsColorPickerOpen && IsComboboxOpen )
	{
		if ( ComboInfo->Items.empty( ) )
			return callback;

		for ( size_t i = 0; i < ComboInfo->Items.size( ); i++ )
		{
			int addedheight = ( ( i + 1 ) * 15 ) - i;
			const char* item = ComboInfo->Items.at( i );

			if ( Input->InArea( x + 1, y + addedheight + 1, w - 2, h - 2 ) )
			{
				string = Config->Colors->MenuAccent;
				if ( Input->IsPressed( VK_LBUTTON ) && elapsed > 0.1f && !Input->IsHeld( VK_LBUTTON ) && IsComboboxOpen )
				{
					ComboInfo->Time = gEngineClient->Time( );
					Input->NullKey( VK_LBUTTON );
					*ComboInfo->Var = i;
					gSurface->PlaySound( "ui/buttonclick.wav" );
					callback = true;
				}
			}
			else if ( *ComboInfo->Var == i )
			{
				string = Config->Colors->MenuAccent;
			}
			else
			{
				string = { 180, 180, 180, 255 };
			}

			Draw->Rectangle( x + 1, y + addedheight + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );
			if ( *ComboInfo->Var == i )
			{
				Draw->Rectangle( x + w - 11, y + addedheight + 4, 7, 7, Config->Colors->MenuAccent, false, true );
				Draw->FadeRectangle( x + w - 11, y + addedheight + 4, 7, 7, { 0, 0, 0, 255 }, 0, 120 );
				//Draw->Rectangle( x + 1, y + addedheight + 1, w - 2 - 15, h - 2, { Config->Colors->m_ui_accent.r, Config->Colors->m_ui_accent.g, Config->Colors->m_ui_accent.b, 40 }, false, true );
			}

			/*if ( Input->InArea( x + 1, y + addedheight + 1, w - 2, h - 2 ) )
			{
				Draw->Rectangle( x + 1, y + addedheight + 1, w - 2, h - 2, { Config->Colors->m_ui_accent.r, Config->Colors->m_ui_accent.g, Config->Colors->m_ui_accent.b, 40 }, false, true );
			}*/
			Draw->Rectangle( x, y + addedheight, w, h, Config->Colors->MenuBorders, false );
			Draw->Rectangle( x + w - 13, y + addedheight + 2, 11, 11, Config->Colors->MenuBorders, false, false );

			std::string preview = item;
			int wide, tall;
			Draw->GetTextSize( FONT_MENU, wide, tall, TF2->Utf8ToWide( item ).c_str( ) );

			if ( wide > w - 5 )
			{
				const int count = preview.length( ) - 5;
				preview.erase( preview.end( ) - ( int )count, preview.end( ) );
				preview.append( "..." );
			}
			Draw->String( x + ( w * 0.5f ), y + addedheight + ( h * 0.5f ), FONT_MENU, Center_X | Center_Y, string, true, preview.c_str( ) );
		}
	}

	return callback;
}

bool CMenu::MultiComboBox( const std::string& label, std::vector< std::pair<const char*, bool*> > items )
{
	bool callback = false;
	Color string = { 180, 180, 180, 255 };

	int x = LastGroupPos.x;
	int y = LastGroupPos.y - 5;
	int w = 120;
	int h = 15;

	int real_height = items.size( ) * 15 - items.size( ) - 1;

	static float time = gEngineClient->Time( );
	float elapsed = gEngineClient->Time( ) - time;

	static bool* curr = nullptr, * prev = curr;
	std::string preview;
	if ( curr != prev )
	{
		time = gEngineClient->Time( );
		prev = curr;
	}

	//the reason we're doing this retarded shit is because strings decide to go fuck themselves when it comes to pointers
	//so they come out as the same address which completely fucks multiple multicombos
	//idk why and i dont fucking care tbh this works so fuck it
	if ( curr == items.at( 0 ).second )
	{
		Draw->Rectangle( x + 6, y + 1, w - 2, h - 2, { Config->Colors->MenuBorders.r, Config->Colors->MenuBorders.g, Config->Colors->MenuBorders.b, 40 }, false, true );
		if ( elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && IsMultiComboboxOpen )
		{
			if ( !Input->InArea( x + 5, y + h, w, real_height ) )
			{
				time = gEngineClient->Time( );
				MultiComboInfo->Time = gEngineClient->Time( );
				Input->NullKey( VK_LBUTTON );
				IsMultiComboboxOpen = false;
				MultiComboInfo->Var = nullptr;
				curr = nullptr;
				gSurface->PlaySound( "ui/buttonclickrelease.wav" );
			}
		}
	}

	if ( curr == nullptr && elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
	{
		if ( Input->InArea( x + 5, y, w, h ) )
		{
			time = gEngineClient->Time( );
			MultiComboInfo->Time = gEngineClient->Time( );
			Input->NullKey( VK_LBUTTON );
			MultiComboInfo->Var = items.at( 0 ).second;
			curr = items.at( 0 ).second;
			MultiComboInfo->Area = { x + 5, y, w, h };
			MultiComboInfo->Items = items;
			IsMultiComboboxOpen = true;
			gSurface->PlaySound( "ui/buttonclick.wav" );
		}
	}

	if ( Input->IsPressed( VK_INSERT ) || Input->IsPressed( VK_ESCAPE ) )
	{
		IsMultiComboboxOpen = false;
		MultiComboInfo->Var = nullptr;
		curr = nullptr;
	}

	Draw->Rectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );
	Draw->Rectangle( x + 5, y, w, h, Config->Colors->MenuBorders, false );

	if ( !Input->InArea( x + 5, y, w, h ) )
	{
		Draw->FadeRectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuShine, 200, 0 );
	}
	else if ( !Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) )
	{
		Draw->FadeRectangle( x + 5 + 1, y + 1, w - 2, h - 2, Config->Colors->MenuShine, 200, 0 );
	}

	Draw->Line( x + 5 + w - 15, y, x + 5 + w - 15, y + 14, Config->Colors->MenuBorders, false );
	Draw->Rectangle( x + 5 + w - 14, y + 1, 13, 13, Config->Colors->MenuBackground, false, true );

	if ( Input->InArea( x + 5, y, w, h ) || curr == items.at( 0 ).second )
	{
		Draw->Rectangle( x + 5 + w - 14, y + 1, 13, 13, { Config->Colors->MenuBorders.r, Config->Colors->MenuBorders.g, Config->Colors->MenuBorders.b, 20 }, false, true );
	}

	/*Draw->Line( x + 5 + w - 11,
				y + 6,
				x + 5 + 7 + w - 15,
				y + 3 + 6, Config->Colors->MenuBorders, false );

	Draw->Line( x + 5 + w - 5,
				y + 6,
				x + 5 + 7 + w - 15,
				y + 3 + 6, Config->Colors->MenuBorders, false );

	Draw->Line( x + 5 + w - 11,
				y + 6,
				x + 6 + 7 + w - 15,
				y + 4 + 6, Config->Colors->MenuBorders, false );

	Draw->Line( x + 6 + 7 + w - 16,
				y + 3 + 6,
				x + 5 + w - 4,
				y + 5, Config->Colors->MenuBorders, false );*/

	Draw->TextureRectangle( x + 4 + w - 14, y + 1, 13, 13, ComboArrow, Config->Colors->MenuBorders );

	int wide, tall;
	Draw->GetTextSize( FONT_MENU, wide, tall, label.c_str( ) );

	if ( Input->InArea( x + 5, y, w + wide + 5, h ) || curr == items.at( 0 ).second )
	{
		string = Config->Colors->MenuAccent;
	}

	std::string str;
	int itemcount = 0;
	for ( const auto& item : items )
	{
		if ( *item.second )
		{
			itemcount++;
			//str = ( str + item.first + ( itemcount == 1 ? "" : ", " ) );
		}
	}

	int currentitem = 0;
	for ( const auto& item : items )
	{
		if ( *item.second )
		{
			currentitem++;
			if ( itemcount == currentitem )
			{
				str = ( str + item.first + " " );
			}
			else if ( itemcount > currentitem )
			{
				str = ( str + item.first + ", " );
			}
		}
	}

	if ( str.empty( ) )
	{
		str = "None";
	}
	else
	{
		str.pop_back( );
		//str.pop_back( );
	}

	if ( str.length( ) > 20 )
	{
		const int count = str.length( ) - ( 20 - 3 );
		str.erase( str.end( ) - ( int )count, str.end( ) );
		str.append( "..." );
	}

	Draw->String( x + 5 + 5, y + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, str.c_str( ) );

	preview = label;
	int previeww, previewh;
	Draw->GetTextSize( FONT_MENU, previeww, previewh, TF2->Utf8ToWide( label.c_str( ) ).c_str( ) );

	if ( previeww > w - 5 )
	{
		const int count = preview.length( ) - 5;
		preview.erase( preview.end( ) - ( int )count, preview.end( ) );
		preview.append( "..." );
	}

	Draw->String( x + w + 10, y + ( h * 0.5f ), FONT_MENU, Center_Y, string, true, preview.c_str( ) );

	LastGroupPos.x = x;
	LastGroupPos.y = y + h + 10;

	//MultiComboInfo->Label = curr;

	return callback;
}

bool CMenu::UpdateMultiCombo( )
{
	bool callback = false;
	int x = MultiComboInfo->Area.x;
	int y = MultiComboInfo->Area.y - 1;
	int w = MultiComboInfo->Area.w;
	int h = MultiComboInfo->Area.h;
	int real_height = MultiComboInfo->Items.size( ) * 15 - MultiComboInfo->Items.size( );
	Color string = { 180, 180, 180, 255 };

	float elapsed = gEngineClient->Time( ) - MultiComboInfo->Time;
	if ( MultiComboInfo->Var && elapsed > 0.1f && Input->IsPressed( VK_LBUTTON ) && !Input->IsHeld( VK_LBUTTON ) && IsMultiComboboxOpen )
	{
		if ( !Input->InArea( x, y + h, w, real_height ) )
		{
			MultiComboInfo->Time = gEngineClient->Time( );
			Input->NullKey( VK_LBUTTON );
			IsMultiComboboxOpen = false;
			MultiComboInfo->Var = nullptr;
			gSurface->PlaySound( "ui/buttonclickrelease.wav" );
		}
	}

	if ( MultiComboInfo->Var && !IsComboboxOpen && !IsColorPickerOpen && IsMultiComboboxOpen )
	{
		for ( size_t i = 0; i < MultiComboInfo->Items.size( ); i++ )
		{
			int addedheight = ( ( i + 1 ) * 15 ) - i;
			const char* item = MultiComboInfo->Items.at( i ).first;

			if ( Input->InArea( x + 1, y + addedheight + 1, w - 2, h - 2 ) )
			{
				string = Config->Colors->MenuAccent;
				if ( Input->IsPressed( VK_LBUTTON ) && elapsed > 0.1f && !Input->IsHeld( VK_LBUTTON ) && IsMultiComboboxOpen )
				{
					MultiComboInfo->Time = gEngineClient->Time( );
					Input->NullKey( VK_LBUTTON );
					*MultiComboInfo->Items.at( i ).second = !*MultiComboInfo->Items.at( i ).second;
					gSurface->PlaySound( "ui/buttonclick.wav" );
					callback = true;
				}
			}
			else if ( *MultiComboInfo->Items.at( i ).second )
			{
				string = Config->Colors->MenuAccent;
			}
			else
			{
				string = { 180, 180, 180, 255 };
			}

			Draw->Rectangle( x + 1, y + addedheight + 1, w - 2, h - 2, Config->Colors->MenuBackground, false, true );
			if ( *MultiComboInfo->Items.at( i ).second )
			{
				Draw->Rectangle( x + w - 11, y + addedheight + 4, 7, 7, Config->Colors->MenuAccent, false, true );
				Draw->FadeRectangle( x + w - 11, y + addedheight + 4, 7, 7, { 0, 0, 0, 255 }, 0, 120 );
				//Draw->Rectangle( x + 1, y + addedheight + 1, w - 2, h - 2, { Config->Colors->m_ui_accent.r, Config->Colors->m_ui_accent.g, Config->Colors->m_ui_accent.b, 40 }, false, true );
			}

			/*if ( Input->InArea( x + 1, y + addedheight + 1, w - 2, h - 2 ) )
			{
				Draw->Rectangle( x + 1, y + addedheight + 1, w - 2, h - 2, { Config->Colors->m_ui_accent.r, Config->Colors->m_ui_accent.g, Config->Colors->m_ui_accent.b, 40 }, false, true );
			}*/
			Draw->Rectangle( x, y + addedheight, w, h, Config->Colors->MenuBorders, false );
			Draw->Rectangle( x + w - 13, y + addedheight + 2, 11, 11, Config->Colors->MenuBorders, false, false );

			std::string preview = item;
			int wide, tall;
			Draw->GetTextSize( FONT_MENU, wide, tall, TF2->Utf8ToWide( item ).c_str( ) );

			if ( wide > w - 5 )
			{
				const int count = preview.length( ) - 5;
				preview.erase( preview.end( ) - ( int )count, preview.end( ) );
				preview.append( "..." );
			}
			Draw->String( x + ( w * 0.5f ), y + addedheight + ( h * 0.5f ), FONT_MENU, Center_X | Center_Y, string, true, preview.c_str( ) );
		}
	}

	return callback;
}

void CMenu::EnableClipping( const Rect_t& size )
{
	gSurface->GetClippingRect( ClippingBackupL, ClippingBackupT, ClippingBackupR, ClippingBackupB, ClippingBackupDisabled );

	gSurface->DisableClipping( false );
	gSurface->SetClippingRect( size.x, size.y, size.x + size.w, size.y + size.h );
}

void CMenu::DisableClipping( )
{
	gSurface->DisableClipping( ClippingBackupDisabled );
	gSurface->SetClippingRect( ClippingBackupL, ClippingBackupT, ClippingBackupR, ClippingBackupB );
}

void CMenu::SaveConfig( const char* name )
{
	if ( !name )
	{
		return;
	}

	std::ofstream Output( ConfigPath + "\\" + name + ".json" );

	if ( Output.is_open( ) )
	{
		Json::Value Save;

		Save[ "Aimbot" ][ "Global" ][ "Enabled" ] = Config->Aimbot->Global->Enabled;
		Save[ "Aimbot" ][ "Global" ][ "AimKey" ] = Config->Aimbot->Global->AimKey;
		Save[ "Aimbot" ][ "Global" ][ "FOV" ] = Config->Aimbot->Global->FOV;
		Save[ "Aimbot" ][ "Global" ][ "AutoSecondarySwitch" ] = Config->Aimbot->Global->AutoSecondarySwitch;
		Save[ "Aimbot" ][ "Hitscan" ][ "TapFire" ] = Config->Aimbot->Hitscan->TapFire;
		Save[ "Aimbot" ][ "Hitscan" ][ "WaitForHeadshot" ] = Config->Aimbot->Hitscan->WaitForHeadshot;
		Save[ "Aimbot" ][ "Hitscan" ][ "LagCompMethod" ] = Config->Aimbot->Hitscan->LagCompMethod;
		Save[ "Aimbot" ][ "Hitscan" ][ "FakeLatency" ] = Config->Aimbot->Hitscan->FakeLatency;
		Save[ "Aimbot" ][ "Hitscan" ][ "DynamicLatency" ] = Config->Aimbot->Hitscan->DynamicLatency;
		Save[ "Aimbot" ][ "Hitscan" ][ "FakeLatencyAmount" ] = Config->Aimbot->Hitscan->FakeLatencyAmount;
		Save[ "Aimbot" ][ "Hitscan" ][ "FakeLatencyKey" ] = Config->Aimbot->Hitscan->FakeLatencyKey;
		Save[ "Aimbot" ][ "Hitscan" ][ "ReduceScoreboardPing" ] = Config->Aimbot->Hitscan->ReduceScoreboardPing;
		Save[ "Aimbot" ][ "Hitscan" ][ "ScoreboardPingGoal" ] = Config->Aimbot->Hitscan->ScoreboardPingGoal;
		Save[ "Aimbot" ][ "Hitscan" ][ "PointScale" ] = Config->Aimbot->Hitscan->PointScale;
		Save[ "Aimbot" ][ "Hitscan" ][ "HeavyPointScale" ] = Config->Aimbot->Hitscan->HeavyPointScale;
		Save[ "Aimbot" ][ "Hitscan" ][ "PreventBait" ] = Config->Aimbot->Hitscan->PreventBait;
		Save[ "Aimbot" ][ "Hitscan" ][ "WaitForCharge" ] = Config->Aimbot->Hitscan->WaitForCharge;
		Save[ "Aimbot" ][ "Hitscan" ][ "Preference" ] = Config->Aimbot->Hitscan->Preference;
		Save[ "Aimbot" ][ "Hitscan" ][ "LegitBob" ][ "Enabled" ] = Config->Aimbot->Hitscan->LegitBob->Enabled;
		Save[ "Aimbot" ][ "Hitscan" ][ "LegitBob" ][ "DeadZone" ] = Config->Aimbot->Hitscan->LegitBob->DeadZone;
		Save[ "Aimbot" ][ "Hitscan" ][ "LegitBob" ][ "Fov" ] = Config->Aimbot->Hitscan->LegitBob->FOV;
		Save[ "Aimbot" ][ "Hitscan" ][ "LegitBob" ][ "Strenght" ] = Config->Aimbot->Hitscan->LegitBob->Strenght;
		Save[ "Aimbot" ][ "Hitscan" ][ "TargetHead" ] = Config->Aimbot->Hitscan->TargetHead;
		Save[ "Aimbot" ][ "Hitscan" ][ "TargetChest" ] = Config->Aimbot->Hitscan->TargetChest;
		Save[ "Aimbot" ][ "Hitscan" ][ "TargetStomach" ] = Config->Aimbot->Hitscan->TargetStomach;
		Save[ "Aimbot" ][ "Hitscan" ][ "TargetPelvis" ] = Config->Aimbot->Hitscan->TargetPelvis;
		Save[ "Aimbot" ][ "Hitscan" ][ "TargetArms" ] = Config->Aimbot->Hitscan->TargetArms;
		Save[ "Aimbot" ][ "Projectile" ][ "PredictStrafing" ] = Config->Aimbot->Projectile->PredictStrafing;
		Save[ "Aimbot" ][ "Projectile" ][ "PredictJumping" ] = Config->Aimbot->Projectile->PredictJumping;
		Save[ "Aimbot" ][ "Projectile" ][ "MaxProjectileTime" ] = Config->Aimbot->Projectile->MaxProjectileTime;
		Save[ "Aimbot" ][ "Projectile" ][ "MaxTime" ] = Config->Aimbot->Projectile->MaxTime;
		Save[ "Aimbot" ][ "Projectile" ][ "HitChance" ] = Config->Aimbot->Projectile->HitChance;
		Save[ "Aimbot" ][ "Projectile" ][ "MaxStickyCharge" ] = Config->Aimbot->Projectile->MaxStickyCharge;
		Save[ "Aimbot" ][ "Projectile" ][ "ChangePositionKey" ] = Config->Aimbot->Projectile->ChangePositionKey;
		Save[ "Aimbot" ][ "Projectile" ][ "ResetPositionKey" ] = Config->Aimbot->Projectile->ResetPositionKey;
		Save[ "Aimbot" ][ "Projectile" ][ "GazaBombingKey" ] = Config->Aimbot->Projectile->GazaBombingKey;
		Save[ "Aimbot" ][ "Projectile" ][ "MaxGroundDebug" ] = Config->Aimbot->Projectile->MaxGroundDebug;
		Save[ "Aimbot" ][ "Projectile" ][ "WaitForHeadshot" ] = Config->Aimbot->Projectile->WaitForHeadshot;
		Save[ "Aimbot" ][ "Projectile" ][ "DelayShot" ] = Config->Aimbot->Projectile->DelayShot;
		Save[ "Aimbot" ][ "Projectile" ][ "SplashBot" ] = Config->Aimbot->Projectile->SplashBot;
		Save[ "Aimbot" ][ "Projectile" ][ "SplashRadius" ] = Config->Aimbot->Projectile->SplashRadius;
		Save[ "Aimbot" ][ "Projectile" ][ "SplashPoints" ] = Config->Aimbot->Projectile->SplashPoints;
		Save[ "Aimbot" ][ "Targetting" ][ "TargetPlayers" ] = Config->Aimbot->Targetting->TargetPlayers;
		Save[ "Aimbot" ][ "Targetting" ][ "TargetBonk" ] = Config->Aimbot->Targetting->TargetBonk;
		Save[ "Aimbot" ][ "Targetting" ][ "TargetUber" ] = Config->Aimbot->Targetting->TargetUber;
		Save[ "Aimbot" ][ "Targetting" ][ "TargetVacc" ] = Config->Aimbot->Targetting->TargetVacc;
		Save[ "Aimbot" ][ "Targetting" ][ "TargetBuildings" ] = Config->Aimbot->Targetting->TargetBuildings;
		Save[ "Aimbot" ][ "Targetting" ][ "TargetStickies" ] = Config->Aimbot->Targetting->TargetStickies;
		Save[ "Aimbot" ][ "Targetting" ][ "TargetFriends" ] = Config->Aimbot->Targetting->TargetFriends;
		Save[ "Aimbot" ][ "Targetting" ][ "TargetLethalBombs" ] = Config->Aimbot->Targetting->TargetLethalBombs;
		Save[ "Aimbot" ][ "Priorities" ][ "Scout" ] = Config->Aimbot->Priorities->Scout;
		Save[ "Aimbot" ][ "Priorities" ][ "Soldier" ] = Config->Aimbot->Priorities->Soldier;
		Save[ "Aimbot" ][ "Priorities" ][ "Pyro" ] = Config->Aimbot->Priorities->Pyro;
		Save[ "Aimbot" ][ "Priorities" ][ "Demoman" ] = Config->Aimbot->Priorities->Demoman;
		Save[ "Aimbot" ][ "Priorities" ][ "Heavy" ] = Config->Aimbot->Priorities->Heavy;
		Save[ "Aimbot" ][ "Priorities" ][ "Engineer" ] = Config->Aimbot->Priorities->Engineer;
		Save[ "Aimbot" ][ "Priorities" ][ "Medic" ] = Config->Aimbot->Priorities->Medic;
		Save[ "Aimbot" ][ "Priorities" ][ "Sniper" ] = Config->Aimbot->Priorities->Sniper;
		Save[ "Aimbot" ][ "Priorities" ][ "Spy" ] = Config->Aimbot->Priorities->Spy;
		Save[ "Aimbot" ][ "Priorities" ][ "OtherBuildings" ] = Config->Aimbot->Priorities->OtherBuildings;
		Save[ "Aimbot" ][ "Priorities" ][ "Stickies" ] = Config->Aimbot->Priorities->Stickies;
		Save[ "Aimbot" ][ "Priorities" ][ "Cheaters" ] = Config->Aimbot->Priorities->Cheaters;
		Save[ "Aimbot" ][ "Priorities" ][ "Friends" ] = Config->Aimbot->Priorities->Friends;
		Save[ "Aimbot" ][ "Priorities" ][ "Cloaked" ] = Config->Aimbot->Priorities->Cloaked;
		Save[ "Aimbot" ][ "Priorities" ][ "Scoped" ] = Config->Aimbot->Priorities->Scoped;
		Save[ "Aimbot" ][ "Priorities" ][ "Revved" ] = Config->Aimbot->Priorities->Revved;
		Save[ "Aimbot" ][ "Priorities" ][ "Sentry" ] = Config->Aimbot->Priorities->Sentry;
		Save[ "Aimbot" ][ "Priorities" ][ "Other" ] = Config->Aimbot->Priorities->Other;
		Save[ "Aimbot" ][ "Melee" ][ "Enabled" ] = Config->Aimbot->Melee->Enabled;
		Save[ "Aimbot" ][ "Melee" ][ "RequireAimKey" ] = Config->Aimbot->Melee->RequireAimKey;
		Save[ "Aimbot" ][ "Melee" ][ "WarpPrediction" ] = Config->Aimbot->Melee->WarpPrediction;

		Save[ "ESP" ][ "Global" ][ "Enabled" ] = Config->ESP->Global->Enabled;
		Save[ "ESP" ][ "Global" ][ "ColorScheme" ] = Config->ESP->Global->ColorScheme;
		Save[ "ESP" ][ "Buildings" ][ "Enabled" ] = Config->ESP->Buildings->Enabled;
		Save[ "ESP" ][ "Buildings" ][ "Enemy" ] = Config->ESP->Buildings->Enemy;
		Save[ "ESP" ][ "Buildings" ][ "Team" ] = Config->ESP->Buildings->Team;
		Save[ "ESP" ][ "Buildings" ][ "Friends" ] = Config->ESP->Buildings->Friends;
		Save[ "ESP" ][ "Buildings" ][ "Local" ] = Config->ESP->Buildings->Local;
		Save[ "ESP" ][ "Buildings" ][ "Name" ] = Config->ESP->Buildings->Name;
		Save[ "ESP" ][ "Buildings" ][ "EnabledBox" ] = Config->ESP->Buildings->EnabledBox;
		Save[ "ESP" ][ "Buildings" ][ "Flags" ] = Config->ESP->Buildings->Flags;
		Save[ "ESP" ][ "Buildings" ][ "HealthBar" ] = Config->ESP->Buildings->HealthBar;
		Save[ "ESP" ][ "Buildings" ][ "Box" ] = Config->ESP->Buildings->Box;
		Save[ "ESP" ][ "Buildings" ][ "PHealthBar" ][ "Lines" ] = Config->ESP->Buildings->PHealthBar->Lines;
		Save[ "ESP" ][ "Buildings" ][ "PHealthBar" ][ "Animated" ] = Config->ESP->Buildings->PHealthBar->Animated;
		Save[ "ESP" ][ "Buildings" ][ "PHealthBar" ][ "CustomColor" ] = Config->ESP->Buildings->PHealthBar->CustomColor;
		Save[ "ESP" ][ "Buildings" ][ "PHealthBar" ][ "LinesCount" ] = Config->ESP->Buildings->PHealthBar->LinesCount;
		Save[ "ESP" ][ "Players" ][ "Enabled" ] = Config->ESP->Players->Enabled;
		Save[ "ESP" ][ "Players" ][ "Enemy" ] = Config->ESP->Players->Enemy;
		Save[ "ESP" ][ "Players" ][ "Team" ] = Config->ESP->Players->Team;
		Save[ "ESP" ][ "Players" ][ "Friends" ] = Config->ESP->Players->Friends;
		Save[ "ESP" ][ "Players" ][ "Local" ] = Config->ESP->Players->Local;
		Save[ "ESP" ][ "Players" ][ "Name" ] = Config->ESP->Players->Name;
		Save[ "ESP" ][ "Players" ][ "EnabledBox" ] = Config->ESP->Players->EnabledBox;
		Save[ "ESP" ][ "Players" ][ "Flags" ] = Config->ESP->Players->Flags;
		Save[ "ESP" ][ "Players" ][ "Weapon" ] = Config->ESP->Players->Weapon;
		Save[ "ESP" ][ "Players" ][ "HealthBar" ] = Config->ESP->Players->HealthBar;
		Save[ "ESP" ][ "Players" ][ "UberBar" ] = Config->ESP->Players->UberBar;
		Save[ "ESP" ][ "Players" ][ "Avatar" ] = Config->ESP->Players->Avatar;
		Save[ "ESP" ][ "Players" ][ "Box" ] = Config->ESP->Players->Box;
		Save[ "ESP" ][ "Players" ][ "PHealthBar" ][ "Lines" ] = Config->ESP->Players->PHealthBar->Lines;
		Save[ "ESP" ][ "Players" ][ "PHealthBar" ][ "Animated" ] = Config->ESP->Players->PHealthBar->Animated;
		Save[ "ESP" ][ "Players" ][ "PHealthBar" ][ "CustomColor" ] = Config->ESP->Players->PHealthBar->CustomColor;
		Save[ "ESP" ][ "Players" ][ "PHealthBar" ][ "LinesCount" ] = Config->ESP->Players->PHealthBar->LinesCount;
		Save[ "ESP" ][ "World" ][ "Enabled" ] = Config->ESP->World->Enabled;
		Save[ "ESP" ][ "World" ][ "Enemy" ] = Config->ESP->World->Enemy;
		Save[ "ESP" ][ "World" ][ "Team" ] = Config->ESP->World->Team;
		Save[ "ESP" ][ "World" ][ "Friends" ] = Config->ESP->World->Friends;
		Save[ "ESP" ][ "World" ][ "Local" ] = Config->ESP->World->Local;
		Save[ "ESP" ][ "World" ][ "Healthpacks" ] = Config->ESP->World->Healthpacks;
		Save[ "ESP" ][ "World" ][ "Ammopacks" ] = Config->ESP->World->Ammopacks;
		Save[ "ESP" ][ "World" ][ "Intels" ] = Config->ESP->World->Intels;
		Save[ "ESP" ][ "World" ][ "Bombs" ] = Config->ESP->World->Bombs;
		Save[ "ESP" ][ "World" ][ "MvMCash" ] = Config->ESP->World->MvMCash;
		Save[ "ESP" ][ "World" ][ "MvMTanks" ] = Config->ESP->World->MvMTanks;
		Save[ "ESP" ][ "World" ][ "MvMSentryBusters" ] = Config->ESP->World->MvMSentryBusters;
		Save[ "ESP" ][ "World" ][ "MPPowerups" ] = Config->ESP->World->MPPowerups;
		Save[ "ESP" ][ "World" ][ "Projectiles" ] = Config->ESP->World->Projectiles;
		Save[ "ESP" ][ "World" ][ "Name" ] = Config->ESP->World->Name;
		Save[ "ESP" ][ "World" ][ "EnabledBox" ] = Config->ESP->World->EnabledBox;
		Save[ "ESP" ][ "World" ][ "Distance" ] = Config->ESP->World->Distance;
		Save[ "ESP" ][ "World" ][ "Box" ] = Config->ESP->World->Box;

		Save[ "Models" ][ "Global" ][ "Enabled" ] = Config->Models->Global->Enabled;
		Save[ "Models" ][ "Global" ][ "ColorScheme" ] = Config->Models->Global->ColorScheme;
		Save[ "Models" ][ "Global" ][ "RenderingOrder" ] = Config->Models->Global->RenderingOrder;
		Save[ "Models" ][ "Buildings" ][ "Enabled" ] = Config->Models->Buildings->Enabled;
		Save[ "Models" ][ "Buildings" ][ "Enemy" ] = Config->Models->Buildings->Enemy;
		Save[ "Models" ][ "Buildings" ][ "Team" ] = Config->Models->Buildings->Team;
		Save[ "Models" ][ "Buildings" ][ "Friends" ] = Config->Models->Buildings->Friends;
		Save[ "Models" ][ "Buildings" ][ "Local" ] = Config->Models->Buildings->Local;
		Save[ "Models" ][ "Buildings" ][ "TwoModels" ] = Config->Models->Buildings->TwoModels;
		Save[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "Overlay" ] = Config->Models->Buildings->Unoccluded->Overlay;
		Save[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "Alpha" ] = Config->Models->Buildings->Unoccluded->Alpha;
		Save[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "OverlayAlpha" ] = Config->Models->Buildings->Unoccluded->OverlayAlpha;
		Save[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "Model" ] = Config->Models->Buildings->Unoccluded->Model;
		Save[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "OverlayModel" ] = Config->Models->Buildings->Unoccluded->OverlayModel;
		Save[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "OverlayWireframe" ] = Config->Models->Buildings->Unoccluded->OverlayWireframe;
		Save[ "Models" ][ "Buildings" ][ "Occluded" ][ "Overlay" ] = Config->Models->Buildings->Occluded->Overlay;
		Save[ "Models" ][ "Buildings" ][ "Occluded" ][ "Alpha" ] = Config->Models->Buildings->Occluded->Alpha;
		Save[ "Models" ][ "Buildings" ][ "Occluded" ][ "OverlayAlpha" ] = Config->Models->Buildings->Occluded->OverlayAlpha;
		Save[ "Models" ][ "Buildings" ][ "Occluded" ][ "Model" ] = Config->Models->Buildings->Occluded->Model;
		Save[ "Models" ][ "Buildings" ][ "Occluded" ][ "OverlayModel" ] = Config->Models->Buildings->Occluded->OverlayModel;
		Save[ "Models" ][ "Buildings" ][ "Occluded" ][ "OverlayWireframe" ] = Config->Models->Buildings->Occluded->OverlayWireframe;
		Save[ "Models" ][ "Buildings" ][ "IgnoreZ" ] = Config->Models->Buildings->IgnoreZ;
		Save[ "Models" ][ "Buildings" ][ "Overlay" ] = Config->Models->Buildings->Overlay;
		Save[ "Models" ][ "Buildings" ][ "Alpha" ] = Config->Models->Buildings->Alpha;
		Save[ "Models" ][ "Buildings" ][ "OverlayAlpha" ] = Config->Models->Buildings->OverlayAlpha;
		Save[ "Models" ][ "Buildings" ][ "Model" ] = Config->Models->Buildings->Model;
		Save[ "Models" ][ "Buildings" ][ "OverlayModel" ] = Config->Models->Buildings->OverlayModel;
		Save[ "Models" ][ "Buildings" ][ "OverlayWireframe" ] = Config->Models->Buildings->OverlayWireframe;

		Save[ "Models" ][ "Players" ][ "Enabled" ] = Config->Models->Players->Enabled;
		Save[ "Models" ][ "Players" ][ "Enemy" ] = Config->Models->Players->Enemy;
		Save[ "Models" ][ "Players" ][ "Team" ] = Config->Models->Players->Team;
		Save[ "Models" ][ "Players" ][ "Friends" ] = Config->Models->Players->Friends;
		Save[ "Models" ][ "Players" ][ "Local" ] = Config->Models->Players->Local;
		Save[ "Models" ][ "Players" ][ "TwoModels" ] = Config->Models->Players->TwoModels;
		Save[ "Models" ][ "Players" ][ "Unoccluded" ][ "Overlay" ] = Config->Models->Players->Unoccluded->Overlay;
		Save[ "Models" ][ "Players" ][ "Unoccluded" ][ "Alpha" ] = Config->Models->Players->Unoccluded->Alpha;
		Save[ "Models" ][ "Players" ][ "Unoccluded" ][ "OverlayAlpha" ] = Config->Models->Players->Unoccluded->OverlayAlpha;
		Save[ "Models" ][ "Players" ][ "Unoccluded" ][ "Model" ] = Config->Models->Players->Unoccluded->Model;
		Save[ "Models" ][ "Players" ][ "Unoccluded" ][ "OverlayType" ] = Config->Models->Players->Unoccluded->OverlayType;
		Save[ "Models" ][ "Players" ][ "Unoccluded" ][ "OverlayModel" ] = Config->Models->Players->Unoccluded->OverlayModel;
		Save[ "Models" ][ "Players" ][ "Unoccluded" ][ "OverlayWireframe" ] = Config->Models->Players->Unoccluded->OverlayWireframe;
		Save[ "Models" ][ "Players" ][ "Occluded" ][ "Overlay" ] = Config->Models->Players->Occluded->Overlay;
		Save[ "Models" ][ "Players" ][ "Occluded" ][ "Alpha" ] = Config->Models->Players->Occluded->Alpha;
		Save[ "Models" ][ "Players" ][ "Occluded" ][ "OverlayAlpha" ] = Config->Models->Players->Occluded->OverlayAlpha;
		Save[ "Models" ][ "Players" ][ "Occluded" ][ "Model" ] = Config->Models->Players->Occluded->Model;
		Save[ "Models" ][ "Players" ][ "Occluded" ][ "OverlayType" ] = Config->Models->Players->Occluded->OverlayType;
		Save[ "Models" ][ "Players" ][ "Occluded" ][ "OverlayModel" ] = Config->Models->Players->Occluded->OverlayModel;
		Save[ "Models" ][ "Players" ][ "Occluded" ][ "OverlayWireframe" ] = Config->Models->Players->Occluded->OverlayWireframe;
		Save[ "Models" ][ "Players" ][ "IgnoreZ" ] = Config->Models->Players->IgnoreZ;
		Save[ "Models" ][ "Players" ][ "Overlay" ] = Config->Models->Players->Overlay;
		Save[ "Models" ][ "Players" ][ "Alpha" ] = Config->Models->Players->Alpha;
		Save[ "Models" ][ "Players" ][ "OverlayAlpha" ] = Config->Models->Players->OverlayAlpha;
		Save[ "Models" ][ "Players" ][ "Model" ] = Config->Models->Players->Model;
		Save[ "Models" ][ "Players" ][ "OverlayType" ] = Config->Models->Players->OverlayType;
		Save[ "Models" ][ "Players" ][ "OverlayModel" ] = Config->Models->Players->OverlayModel;
		Save[ "Models" ][ "Players" ][ "OverlayWireframe" ] = Config->Models->Players->OverlayWireframe;

		Save[ "Models" ][ "World" ][ "Enabled" ] = Config->Models->World->Enabled;
		Save[ "Models" ][ "World" ][ "Enemy" ] = Config->Models->World->Enemy;
		Save[ "Models" ][ "World" ][ "Team" ] = Config->Models->World->Team;
		Save[ "Models" ][ "World" ][ "Friends" ] = Config->Models->World->Friends;
		Save[ "Models" ][ "World" ][ "Local" ] = Config->Models->World->Local;
		Save[ "Models" ][ "World" ][ "Healthpacks" ] = Config->Models->World->Healthpacks;
		Save[ "Models" ][ "World" ][ "Ammopacks" ] = Config->Models->World->Ammopacks;
		Save[ "Models" ][ "World" ][ "Intels" ] = Config->Models->World->Intels;
		Save[ "Models" ][ "World" ][ "Bombs" ] = Config->Models->World->Bombs;
		Save[ "Models" ][ "World" ][ "MvMCash" ] = Config->Models->World->MvMCash;
		Save[ "Models" ][ "World" ][ "MvMTanks" ] = Config->Models->World->MvMTanks;
		Save[ "Models" ][ "World" ][ "MvMSentryBusters" ] = Config->Models->World->MvMSentryBusters;
		Save[ "Models" ][ "World" ][ "MPPowerups" ] = Config->Models->World->MPPowerups;
		Save[ "Models" ][ "World" ][ "Projectiles" ] = Config->Models->World->Projectiles;
		Save[ "Models" ][ "World" ][ "TwoModels" ] = Config->Models->World->TwoModels;
		Save[ "Models" ][ "World" ][ "Unoccluded" ][ "Overlay" ] = Config->Models->World->Unoccluded->Overlay;
		Save[ "Models" ][ "World" ][ "Unoccluded" ][ "Alpha" ] = Config->Models->World->Unoccluded->Alpha;
		Save[ "Models" ][ "World" ][ "Unoccluded" ][ "OverlayAlpha" ] = Config->Models->World->Unoccluded->OverlayAlpha;
		Save[ "Models" ][ "World" ][ "Unoccluded" ][ "Model" ] = Config->Models->World->Unoccluded->Model;
		Save[ "Models" ][ "World" ][ "Unoccluded" ][ "OverlayModel" ] = Config->Models->World->Unoccluded->OverlayModel;
		Save[ "Models" ][ "World" ][ "Unoccluded" ][ "OverlayWireframe" ] = Config->Models->World->Unoccluded->OverlayWireframe;
		Save[ "Models" ][ "World" ][ "Occluded" ][ "Overlay" ] = Config->Models->World->Occluded->Overlay;
		Save[ "Models" ][ "World" ][ "Occluded" ][ "Alpha" ] = Config->Models->World->Occluded->Alpha;
		Save[ "Models" ][ "World" ][ "Occluded" ][ "OverlayAlpha" ] = Config->Models->World->Occluded->OverlayAlpha;
		Save[ "Models" ][ "World" ][ "Occluded" ][ "Model" ] = Config->Models->World->Occluded->Model;
		Save[ "Models" ][ "World" ][ "Occluded" ][ "OverlayModel" ] = Config->Models->World->Occluded->OverlayModel;
		Save[ "Models" ][ "World" ][ "Occluded" ][ "OverlayWireframe" ] = Config->Models->World->Occluded->OverlayWireframe;
		Save[ "Models" ][ "World" ][ "IgnoreZ" ] = Config->Models->World->IgnoreZ;
		Save[ "Models" ][ "World" ][ "Overlay" ] = Config->Models->World->Overlay;
		Save[ "Models" ][ "World" ][ "Alpha" ] = Config->Models->World->Alpha;
		Save[ "Models" ][ "World" ][ "OverlayAlpha" ] = Config->Models->World->OverlayAlpha;
		Save[ "Models" ][ "World" ][ "Model" ] = Config->Models->World->Model;
		Save[ "Models" ][ "World" ][ "OverlayModel" ] = Config->Models->World->OverlayModel;
		Save[ "Models" ][ "World" ][ "OverlayWireframe" ] = Config->Models->World->OverlayWireframe;

		Save[ "Outlines" ][ "Global" ][ "Enabled" ] = Config->Outlines->Global->Enabled;
		Save[ "Outlines" ][ "Global" ][ "ColorScheme" ] = Config->Outlines->Global->ColorScheme; //0 - Enemy/Team, 1 - Red/Blu
		Save[ "Outlines" ][ "Global" ][ "BlurOutline" ] = Config->Outlines->Global->BlurOutline;
		Save[ "Outlines" ][ "Global" ][ "StencilOutline" ] = Config->Outlines->Global->StencilOutline;
		Save[ "Outlines" ][ "Global" ][ "CheapStencil" ] = Config->Outlines->Global->CheapStencil;
		Save[ "Outlines" ][ "Global" ][ "BlurScale" ] = Config->Outlines->Global->BlurScale;
		Save[ "Outlines" ][ "Global" ][ "StencilScale" ] = Config->Outlines->Global->StencilScale;

		Save[ "Outlines" ][ "Players" ][ "Enabled" ] = Config->Outlines->Players->Enabled;
		Save[ "Outlines" ][ "Players" ][ "Enemy" ] = Config->Outlines->Players->Enemy;
		Save[ "Outlines" ][ "Players" ][ "Team" ] = Config->Outlines->Players->Team;
		Save[ "Outlines" ][ "Players" ][ "Friends" ] = Config->Outlines->Players->Friends;
		Save[ "Outlines" ][ "Players" ][ "Local" ] = Config->Outlines->Players->Local;
		Save[ "Outlines" ][ "Players" ][ "Alpha" ] = Config->Outlines->Players->Alpha;

		Save[ "Outlines" ][ "Buildings" ][ "Enabled" ] = Config->Outlines->Buildings->Enabled;
		Save[ "Outlines" ][ "Buildings" ][ "Enemy" ] = Config->Outlines->Buildings->Enemy;
		Save[ "Outlines" ][ "Buildings" ][ "Team" ] = Config->Outlines->Buildings->Team;
		Save[ "Outlines" ][ "Buildings" ][ "Friends" ] = Config->Outlines->Buildings->Friends;
		Save[ "Outlines" ][ "Buildings" ][ "Local" ] = Config->Outlines->Buildings->Local;
		Save[ "Outlines" ][ "Buildings" ][ "Alpha" ] = Config->Outlines->Buildings->Alpha;

		Save[ "Outlines" ][ "World" ][ "Enabled" ] = Config->Outlines->World->Enabled;
		Save[ "Outlines" ][ "World" ][ "Enemy" ] = Config->Outlines->World->Enemy;
		Save[ "Outlines" ][ "World" ][ "Team" ] = Config->Outlines->World->Team;
		Save[ "Outlines" ][ "World" ][ "Friends" ] = Config->Outlines->World->Friends;
		Save[ "Outlines" ][ "World" ][ "Local" ] = Config->Outlines->World->Local;
		Save[ "Outlines" ][ "World" ][ "Healthpacks" ] = Config->Outlines->World->Healthpacks;
		Save[ "Outlines" ][ "World" ][ "Ammopacks" ] = Config->Outlines->World->Ammopacks;
		Save[ "Outlines" ][ "World" ][ "Intels" ] = Config->Outlines->World->Intels;
		Save[ "Outlines" ][ "World" ][ "Bombs" ] = Config->Outlines->World->Bombs;
		Save[ "Outlines" ][ "World" ][ "MvMCash" ] = Config->Outlines->World->MvMCash;
		Save[ "Outlines" ][ "World" ][ "MvMTanks" ] = Config->Outlines->World->MvMTanks;
		Save[ "Outlines" ][ "World" ][ "MvMSentryBusters" ] = Config->Outlines->World->MvMSentryBusters;
		Save[ "Outlines" ][ "World" ][ "MPPowerups" ] = Config->Outlines->World->MPPowerups;
		Save[ "Outlines" ][ "World" ][ "Projectiles" ] = Config->Outlines->World->Projectiles;
		Save[ "Outlines" ][ "World" ][ "Alpha" ] = Config->Outlines->World->Alpha;

		Save[ "Visuals" ][ "FOV" ] = Config->Visuals->FOV;
		Save[ "Visuals" ][ "ScopedFOV" ] = Config->Visuals->ScopedFOV;
		Save[ "Visuals" ][ "RemoveViewPunch" ] = Config->Visuals->RemoveViewPunch;
		Save[ "Visuals" ][ "RemoveShake" ] = Config->Visuals->RemoveShake;
		Save[ "Visuals" ][ "RemoveFade" ] = Config->Visuals->RemoveFade;
		Save[ "Visuals" ][ "RemovePostProcessing" ] = Config->Visuals->RemovePostProcessing;
		Save[ "Visuals" ][ "RemoveScope" ] = Config->Visuals->RemoveScope;
		Save[ "Visuals" ][ "ThirdPersonKey" ] = Config->Visuals->ThirdPersonKey;
		Save[ "Visuals" ][ "ThirdPersonOffset" ] = Config->Visuals->ThirdPersonOffset;
		Save[ "Visuals" ][ "ThirdPersonSidewaysCycleKey" ] = Config->Visuals->ThirdPersonSidewaysCycleKey;
		Save[ "Visuals" ][ "ThirdPersonForward" ] = Config->Visuals->ThirdPersonForward;
		Save[ "Visuals" ][ "ThirdPersonSideways" ] = Config->Visuals->ThirdPersonSideways;
		Save[ "Visuals" ][ "ThirdPersonUpward" ] = Config->Visuals->ThirdPersonUpward;
		Save[ "Visuals" ][ "AspectRatio" ] = Config->Visuals->AspectRatio;
		Save[ "Visuals" ][ "BloomScale" ] = Config->Visuals->BloomScale;
		Save[ "Visuals" ][ "HitMarker2D" ] = Config->Visuals->HitMarker2D;
		Save[ "Visuals" ][ "HitscanHitboxes" ] = Config->Visuals->HitscanHitboxes;
		Save[ "Visuals" ][ "HitboxesCustomColor" ] = Config->Visuals->HitboxesCustomColor;
		Save[ "Visuals" ][ "HitboxesAlpha" ] = Config->Visuals->HitboxesAlpha;
		Save[ "Visuals" ][ "HitboxesTime" ] = Config->Visuals->HitboxesTime;
		Save[ "Visuals" ][ "ProjectileBoundingBox" ] = Config->Visuals->ProjectileBoundingBox;
		Save[ "Visuals" ][ "BoundingBoxCustomColor" ] = Config->Visuals->BoundingBoxCustomColor;
		Save[ "Visuals" ][ "BoundingBoxAlpha" ] = Config->Visuals->BoundingBoxAlpha;
		Save[ "Visuals" ][ "BoundingBoxTime" ] = Config->Visuals->BoundingBoxTime;
		//Save[ "Visuals" ][ "HitMarker3D" ] = Config->Visuals->HitMarker3D;
		//Save[ "Visuals" ][ "HitboxHitMarker" ] = Config->Visuals->HitboxHitMarker;
		Save[ "Visuals" ][ "DrawBackTrack" ] = Config->Visuals->DrawBackTrack;
		Save[ "Visuals" ][ "LastTickOnly" ] = Config->Visuals->LastTickOnly;
		Save[ "Visuals" ][ "ViewmodelSway" ] = Config->Visuals->ViewmodelSway;
		Save[ "Visuals" ][ "ViewmodelSwayAmount" ] = Config->Visuals->ViewmodelSwayAmount;
		Save[ "Visuals" ][ "ViewmodelPos" ] = Config->Visuals->ViewmodelPos;
		Save[ "Visuals" ][ "ViewmodelFlip" ] = Config->Visuals->ViewmodelFlip;
		Save[ "Visuals" ][ "ViewmodelFOV" ] = Config->Visuals->ViewmodelFOV;
		Save[ "Visuals" ][ "ViewmodelX" ] = Config->Visuals->ViewmodelX;
		Save[ "Visuals" ][ "ViewmodelY" ] = Config->Visuals->ViewmodelY;
		Save[ "Visuals" ][ "ViewmodelZ" ] = Config->Visuals->ViewmodelZ;
		Save[ "Visuals" ][ "ViewmodelRoll" ] = Config->Visuals->ViewmodelRoll;
		Save[ "Visuals" ][ "HandsModels" ] = Config->Visuals->HandsModels;
		Save[ "Visuals" ][ "HandsModel" ] = Config->Visuals->HandsModel;
		Save[ "Visuals" ][ "DisableHands" ] = Config->Visuals->DisableHands;
		Save[ "Visuals" ][ "DisableWeapon" ] = Config->Visuals->DisableWeapon;
		Save[ "Visuals" ][ "HandsOverlay" ] = Config->Visuals->HandsOverlay;
		Save[ "Visuals" ][ "HandsOverlayModel" ] = Config->Visuals->HandsOverlayModel;
		Save[ "Visuals" ][ "HandsAlpha" ] = Config->Visuals->HandsAlpha;
		Save[ "Visuals" ][ "HandsOverlayAlpha" ] = Config->Visuals->HandsOverlayAlpha;
		Save[ "Visuals" ][ "HandsOverlayWireframe" ] = Config->Visuals->HandsOverlayWireframe;
		Save[ "Visuals" ][ "PlayerArrows" ] = Config->Visuals->PlayerArrows;
		Save[ "Visuals" ][ "ArrowsScreenDistance" ] = Config->Visuals->ArrowsScreenDistance;
		Save[ "Visuals" ][ "ArrowsScreenShape" ] = Config->Visuals->ArrowsScreenShape;
		Save[ "Visuals" ][ "ProjectileLine" ] = Config->Visuals->ProjectileLine;
		Save[ "Visuals" ][ "ProjectileLineAlpha" ] = Config->Visuals->ProjectileLineAlpha;
		Save[ "Visuals" ][ "ProjectileCamera" ] = Config->Visuals->ProjectileCamera;
		Save[ "Visuals" ][ "ProjectileCameraFOV" ] = Config->Visuals->ProjectileCameraFOV;
		Save[ "Visuals" ][ "ProjectileCamX" ] = Visuals->ProjectileCamX;
		Save[ "Visuals" ][ "ProjectileCamY" ] = Visuals->ProjectileCamY;
		Save[ "Visuals" ][ "ProjectileCameraWidth" ] = Config->Visuals->ProjectileCameraWidth;
		Save[ "Visuals" ][ "ProjectileCameraHeight" ] = Config->Visuals->ProjectileCameraHeight;
		Save[ "Visuals" ][ "BackTrackModel" ] = Config->Visuals->BackTrackModel;
		Save[ "Visuals" ][ "BackTrackOutline" ] = Config->Visuals->BackTrackOutline;
		Save[ "Visuals" ][ "BackTrackAlpha" ] = Config->Visuals->BackTrackAlpha;
		Save[ "Visuals" ][ "RagdollEffects" ] = Config->Visuals->RagdollEffects;
		Save[ "Visuals" ][ "RagdollBurning" ] = Config->Visuals->RagdollBurning;
		Save[ "Visuals" ][ "RagdollElectrocuted" ] = Config->Visuals->RagdollElectrocuted;
		Save[ "Visuals" ][ "RagdollDissolving" ] = Config->Visuals->RagdollDissolving;
		Save[ "Visuals" ][ "RagdollBecomeAsh" ] = Config->Visuals->RagdollBecomeAsh;
		Save[ "Visuals" ][ "RagdollBaseType" ] = Config->Visuals->RagdollBaseType;
		Save[ "Visuals" ][ "RagdollEnemy" ] = Config->Visuals->RagdollEnemy;
		Save[ "Visuals" ][ "RagdollTeam" ] = Config->Visuals->RagdollTeam;
		Save[ "Visuals" ][ "RagdollLocal" ] = Config->Visuals->RagdollLocal;
		Save[ "Visuals" ][ "BulletTracers" ] = Config->Visuals->BulletTracers;
		Save[ "Visuals" ][ "SkyboxName" ] = Config->Visuals->SkyboxName;
		Save[ "Visuals" ][ "StreamerMode" ] = Config->Visuals->StreamerMode;
		Save[ "Visuals" ][ "HideLocal" ] = Config->Visuals->HideLocal;
		Save[ "Visuals" ][ "HideFriends" ] = Config->Visuals->HideFriends;
		Save[ "Visuals" ][ "UnlockEquipRegion" ] = Config->Visuals->UnlockEquipRegion;
		Save[ "Visuals" ][ "OptimizedFonts" ] = Config->Visuals->OptimizedFonts;
		Save[ "Visuals" ][ "MedalChanger" ] = Config->Visuals->MedalChanger;
		Save[ "Visuals" ][ "MedalLevel" ] = Config->Visuals->MedalLevel;
		Save[ "Visuals" ][ "MedalTier" ] = Config->Visuals->MedalTier;

		Save[ "Visuals" ][ "FakeTaunt" ][ "Enabled" ] = Config->Visuals->FakeTaunt->Enabled;
		Save[ "Visuals" ][ "WorldModulation" ][ "Enabled" ] = Config->Visuals->WorldModulation->Enabled;
		Save[ "Visuals" ][ "WorldModulation" ][ "World" ] = Config->Visuals->WorldModulation->World;
		Save[ "Visuals" ][ "WorldModulation" ][ "Skybox" ] = Config->Visuals->WorldModulation->Skybox;
		Save[ "Visuals" ][ "WorldModulation" ][ "Mode" ] = Config->Visuals->WorldModulation->Mode;
		Save[ "Visuals" ][ "WorldModulation" ][ "Amount" ] = Config->Visuals->WorldModulation->Amount;
		Save[ "Visuals" ][ "WorldModulation" ][ "Alpha" ] = Config->Visuals->WorldModulation->Alpha;
		Save[ "Visuals" ][ "Beams" ][ "Enabled" ] = Config->Visuals->Beams->Enabled;
		Save[ "Visuals" ][ "Beams" ][ "ColorScheme" ] = Config->Visuals->Beams->ColorScheme;
		Save[ "Visuals" ][ "Beams" ][ "Enemy" ] = Config->Visuals->Beams->Enemy;
		Save[ "Visuals" ][ "Beams" ][ "Team" ] = Config->Visuals->Beams->Team;
		Save[ "Visuals" ][ "Beams" ][ "Friends" ] = Config->Visuals->Beams->Friends;
		Save[ "Visuals" ][ "Beams" ][ "Local" ] = Config->Visuals->Beams->Local;

		Save[ "Misc" ][ "BHop" ] = Config->Misc->BHop;
		Save[ "Misc" ][ "AutoStrafer" ] = Config->Misc->AutoStrafer;
		Save[ "Misc" ][ "EdgeJump" ] = Config->Misc->EdgeJump;
		Save[ "Misc" ][ "EdgeJumpKey" ] = Config->Misc->EdgeJumpKey;
		Save[ "Misc" ][ "StraferScale" ] = Config->Misc->StraferScale;
		Save[ "Misc" ][ "AvoidTeamPush" ] = Config->Misc->AvoidTeamPush;
		Save[ "Misc" ][ "FastStop" ] = Config->Misc->FastStop;
		Save[ "Misc" ][ "HighJump" ] = Config->Misc->HighJump;
		Save[ "Misc" ][ "HighJumpKey" ] = Config->Misc->HighJumpKey;
		Save[ "Misc" ][ "AutoRetreat" ] = Config->Misc->AutoRetreat;
		Save[ "Misc" ][ "AutoRetreatKey" ] = Config->Misc->AutoRetreatKey;
		Save[ "Misc" ][ "FastBackup" ] = Config->Misc->FastBackup;
		Save[ "Misc" ][ "AntiBackstab" ] = Config->Misc->AntiBackstab;
		Save[ "Misc" ][ "VoiceSpam" ] = Config->Misc->VoiceSpam;
		Save[ "Misc" ][ "VoiceSpamMode" ] = Config->Misc->VoiceSpamMode;
		Save[ "Misc" ][ "AntiAutoBalance" ] = Config->Misc->AntiAutoBalance;
		Save[ "Misc" ][ "DT" ][ "Enabled" ] = Config->Misc->DT->Enabled;
		Save[ "Misc" ][ "DT" ][ "DTKey" ] = Config->Misc->DT->DTKey;
		Save[ "Misc" ][ "DT" ][ "DelayMode" ] = Config->Misc->DT->DelayMode;
		Save[ "Misc" ][ "DT" ][ "DelayTicks" ] = Config->Misc->DT->DelayTicks;
		Save[ "Misc" ][ "DT" ][ "RechargeKey" ] = Config->Misc->DT->RechargeKey;
		Save[ "Misc" ][ "DT" ][ "WarpKey" ] = Config->Misc->DT->WarpKey;
		Save[ "Misc" ][ "DT" ][ "ShootWood" ] = Config->Misc->DT->ShootWood;
		Save[ "Misc" ][ "DT" ][ "AntiWarp" ] = Config->Misc->DT->AntiWarp;
		Save[ "Misc" ][ "DT" ][ "WarpMode" ] = Config->Misc->DT->WarpMode;
		Save[ "Misc" ][ "DT" ][ "WarpTicks" ] = Config->Misc->DT->WarpTicks;
		Save[ "Misc" ][ "DT" ][ "ActiveInAir" ] = Config->Misc->DT->ActiveInAir;
		Save[ "Misc" ][ "DT" ][ "ActiveOnlyAimbot" ] = Config->Misc->DT->ActiveOnlyAimbot;
		Save[ "Misc" ][ "DT" ][ "ActivateOnlyLethal" ] = Config->Misc->DT->ActivateOnlyLethal;
		Save[ "Misc" ][ "DT" ][ "Indicator" ][ "Enabled" ] = Config->Misc->DT->Indicator->Enabled;
		Save[ "Misc" ][ "DT" ][ "Indicator" ][ "CustomFade" ] = Config->Misc->DT->Indicator->CustomFade;
		Save[ "Misc" ][ "CritHack" ][ "Enabled" ] = Config->Misc->CritHack->Enabled;
		Save[ "Misc" ][ "CritHack" ][ "CritKey" ] = Config->Misc->CritHack->CritKey;
		Save[ "Misc" ][ "CritHack" ][ "SkipRandom" ] = Config->Misc->CritHack->SkipRandom;
		Save[ "Misc" ][ "CritHack" ][ "ForceOnMelee" ] = Config->Misc->CritHack->ForceOnMelee;
		Save[ "Misc" ][ "CritHack" ][ "ForceOnSuspicious" ] = Config->Misc->CritHack->ForceOnSuspicious;
		Save[ "Misc" ][ "CritHack" ][ "ForceOnCheater" ] = Config->Misc->CritHack->ForceOnCheater;
		Save[ "Misc" ][ "CritHack" ][ "ForceOnFriends" ] = Config->Misc->CritHack->ForceOnFriends;
		Save[ "Misc" ][ "CritHack" ][ "ForceOnStreamers" ] = Config->Misc->CritHack->ForceOnStreamers;
		Save[ "Misc" ][ "CritHack" ][ "Optimization" ] = Config->Misc->CritHack->Optimization;
		Save[ "Misc" ][ "CritHack" ][ "Indicator" ][ "Enabled" ] = Config->Misc->CritHack->Indicator->Enabled;
		Save[ "Misc" ][ "NoSpread" ][ "Enabled" ] = Config->Misc->NoSpread->Enabled;
		Save[ "Misc" ][ "NoSpread" ][ "Indicator" ] = Config->Misc->NoSpread->Indicator;

		Save[ "Misc" ][ "RegionSelector" ][ "Enabled" ] = Config->Misc->Matchmaking->RegionSelector->Enabled;
		Save[ "Misc" ][ "RegionSelector" ][ "EUEast" ] = Config->Misc->Matchmaking->RegionSelector->EUEast;
		Save[ "Misc" ][ "RegionSelector" ][ "EUWest" ] = Config->Misc->Matchmaking->RegionSelector->EUWest;
		Save[ "Misc" ][ "RegionSelector" ][ "Asia" ] = Config->Misc->Matchmaking->RegionSelector->Asia;
		Save[ "Misc" ][ "RegionSelector" ][ "MiddleEast" ] = Config->Misc->Matchmaking->RegionSelector->MiddleEast;
		Save[ "Misc" ][ "RegionSelector" ][ "HongKong" ] = Config->Misc->Matchmaking->RegionSelector->HongKong;
		Save[ "Misc" ][ "RegionSelector" ][ "Japan" ] = Config->Misc->Matchmaking->RegionSelector->Japan;
		Save[ "Misc" ][ "RegionSelector" ][ "India" ] = Config->Misc->Matchmaking->RegionSelector->India;
		Save[ "Misc" ][ "RegionSelector" ][ "Australia" ] = Config->Misc->Matchmaking->RegionSelector->Australia;
		Save[ "Misc" ][ "RegionSelector" ][ "Africa" ] = Config->Misc->Matchmaking->RegionSelector->Africa;
		Save[ "Misc" ][ "RegionSelector" ][ "Chile" ] = Config->Misc->Matchmaking->RegionSelector->Chile;
		Save[ "Misc" ][ "RegionSelector" ][ "Peru" ] = Config->Misc->Matchmaking->RegionSelector->Peru;
		Save[ "Misc" ][ "RegionSelector" ][ "Brazil" ] = Config->Misc->Matchmaking->RegionSelector->Brazil;
		Save[ "Misc" ][ "RegionSelector" ][ "USEast" ] = Config->Misc->Matchmaking->RegionSelector->USEast;
		Save[ "Misc" ][ "RegionSelector" ][ "USNorthWest" ] = Config->Misc->Matchmaking->RegionSelector->USNorthWest;
		Save[ "Misc" ][ "RegionSelector" ][ "USSouthWest" ] = Config->Misc->Matchmaking->RegionSelector->USSouthWest;

		Save[ "Misc" ][ "MatchMaking" ][ "AutoAccept" ] = Config->Misc->Matchmaking->AutoAccept;
		Save[ "Misc" ][ "MatchMaking" ][ "PreventAutoJoin" ] = Config->Misc->Matchmaking->PreventAutoJoin;

		Save[ "Misc" ][ "FakeLag" ][ "Enabled" ] = Config->Misc->FakeLag->Enabled;
		Save[ "Misc" ][ "FakeLag" ][ "DrawLag" ] = Config->Misc->FakeLag->DrawLag;
		Save[ "Misc" ][ "FakeLag" ][ "LagModel" ] = Config->Misc->FakeLag->LagModel;
		Save[ "Misc" ][ "FakeLag" ][ "LagOutline" ] = Config->Misc->FakeLag->LagOutline;
		Save[ "Misc" ][ "FakeLag" ][ "Value" ] = Config->Misc->FakeLag->Value;
		Save[ "Misc" ][ "FakeLag" ][ "FakeLagKey" ] = Config->Misc->FakeLag->FakeLagKey;
		Save[ "Misc" ][ "FakeLag" ][ "BreakLagComp" ] = Config->Misc->FakeLag->BreakLagComp;
		Save[ "Misc" ][ "FakeLag" ][ "BreakMoving" ] = Config->Misc->FakeLag->BreakMoving;
		Save[ "Misc" ][ "FakeLag" ][ "BreakInAir" ] = Config->Misc->FakeLag->BreakInAir;
		Save[ "Misc" ][ "FakeLag" ][ "UnchokeOnAttack" ] = Config->Misc->FakeLag->UnchokeOnAttack;
		Save[ "Misc" ][ "FakeDuck" ][ "Enabled" ] = Config->Misc->FakeDuck->Enabled;
		Save[ "Misc" ][ "FakeDuck" ][ "FakeDuckKey" ] = Config->Misc->FakeDuck->FakeDuckKey;

		Save[ "Misc" ][ "AntiAim" ][ "Enabled" ] = Config->Misc->AntiAim->Enabled;
		Save[ "Misc" ][ "AntiAim" ][ "DrawFake" ] = Config->Misc->AntiAim->DrawFake;
		Save[ "Misc" ][ "AntiAim" ][ "FakeModel" ] = Config->Misc->AntiAim->FakeModel;
		Save[ "Misc" ][ "AntiAim" ][ "FakeOutline" ] = Config->Misc->AntiAim->FakeOutline;
		Save[ "Misc" ][ "AntiAim" ][ "RageActive" ] = Config->Misc->AntiAim->RageActive;
		Save[ "Misc" ][ "AntiAim" ][ "ToggleKey" ] = Config->Misc->AntiAim->ToggleKey;
		Save[ "Misc" ][ "AntiAim" ][ "LegitYaw" ] = Config->Misc->AntiAim->LegitYaw;
		Save[ "Misc" ][ "AntiAim" ][ "CycleKey" ] = Config->Misc->AntiAim->CycleKey;
		Save[ "Misc" ][ "AntiAim" ][ "FakeYaw" ] = Config->Misc->AntiAim->FakeYaw;
		Save[ "Misc" ][ "AntiAim" ][ "FakeMode" ] = Config->Misc->AntiAim->FakeMode;
		Save[ "Misc" ][ "AntiAim" ][ "FakeRotate" ] = Config->Misc->AntiAim->FakeRotate;
		Save[ "Misc" ][ "AntiAim" ][ "FakeDelta" ] = Config->Misc->AntiAim->FakeDelta;
		Save[ "Misc" ][ "AntiAim" ][ "FakeSpin" ] = Config->Misc->AntiAim->FakeSpin;
		Save[ "Misc" ][ "AntiAim" ][ "FakeAngle1" ] = Config->Misc->AntiAim->FakeAngle1;
		Save[ "Misc" ][ "AntiAim" ][ "FakeAngle2" ] = Config->Misc->AntiAim->FakeAngle2;
		Save[ "Misc" ][ "AntiAim" ][ "RealYaw" ] = Config->Misc->AntiAim->RealYaw;
		Save[ "Misc" ][ "AntiAim" ][ "RealMode" ] = Config->Misc->AntiAim->RealMode;
		Save[ "Misc" ][ "AntiAim" ][ "RealRotate" ] = Config->Misc->AntiAim->RealRotate;
		Save[ "Misc" ][ "AntiAim" ][ "RealDelta" ] = Config->Misc->AntiAim->RealDelta;
		Save[ "Misc" ][ "AntiAim" ][ "RealSpin" ] = Config->Misc->AntiAim->RealSpin;
		Save[ "Misc" ][ "AntiAim" ][ "RealAngle1" ] = Config->Misc->AntiAim->RealAngle1;
		Save[ "Misc" ][ "AntiAim" ][ "RealAngle2" ] = Config->Misc->AntiAim->RealAngle2;
		Save[ "Misc" ][ "AntiAim" ][ "Pitch" ] = Config->Misc->AntiAim->Pitch;
		Save[ "Misc" ][ "AntiAim" ][ "FakePitch" ] = Config->Misc->AntiAim->FakePitch;
		Save[ "Misc" ][ "AntiAim" ][ "RealPitch" ] = Config->Misc->AntiAim->RealPitch;

		Save[ "AutoVacc" ][ "Enabled" ] = Config->AutoVacc->Enabled;
		Save[ "AutoVacc" ][ "LogReasons" ] = Config->AutoVacc->LogReasons;
		Save[ "AutoVacc" ][ "AutoPopExplosive" ] = Config->AutoVacc->AutoPopExplosive;
		Save[ "AutoVacc" ][ "AutoPopCrossbow" ] = Config->AutoVacc->AutoPopCrossbow;
		Save[ "AutoVacc" ][ "AutoPopHigherHealthHeavies" ] = Config->AutoVacc->AutoPopHigherHealthHeavies;
		Save[ "AutoVacc" ][ "AutoPopHeavies" ] = Config->AutoVacc->AutoPopHeavies;
		Save[ "AutoVacc" ][ "IgnoreFriendHeavies" ] = Config->AutoVacc->IgnoreFriendHeavies;
		Save[ "AutoVacc" ][ "AutoPopHeadshot" ] = Config->AutoVacc->AutoPopHeadshot;
		Save[ "AutoVacc" ][ "AutoPopShotgun" ] = Config->AutoVacc->AutoPopShotgun;
		Save[ "AutoVacc" ][ "AutoPopMinHealth" ] = Config->AutoVacc->AutoPopMinHealth;
		Save[ "AutoVacc" ][ "HeavyDistance" ] = Config->AutoVacc->HeavyDistance;

		Save[ "AutoMedic" ][ "Enabled" ] = Config->AutoMedic->Enabled;
		Save[ "AutoMedic" ][ "LockTarget" ] = Config->AutoMedic->LockTarget;
		Save[ "AutoMedic" ][ "TargetLock" ] = Config->AutoMedic->TargetLock;

		Save[ "OtherAuto" ][ "Enabled" ] = Config->OtherAuto->Enabled;
		Save[ "OtherAuto" ][ "AutoAirblast" ] = Config->OtherAuto->AutoAirblast;
		Save[ "OtherAuto" ][ "AutoAirblastPath" ] = Config->OtherAuto->AutoAirblastPath;
		Save[ "OtherAuto" ][ "BlastExplosive" ] = Config->OtherAuto->BlastExplosive;
		Save[ "OtherAuto" ][ "BlastArrows" ] = Config->OtherAuto->BlastArrows;
		Save[ "OtherAuto" ][ "BlastStickies" ] = Config->OtherAuto->BlastStickies;
		Save[ "OtherAuto" ][ "AutoDetonation" ] = Config->OtherAuto->AutoDetonation;
		Save[ "OtherAuto" ][ "TargetBonk" ] = Config->OtherAuto->TargetBonk;
		Save[ "OtherAuto" ][ "TargetBuildings" ] = Config->OtherAuto->TargetBuildings;
		Save[ "OtherAuto" ][ "TargetFriends" ] = Config->OtherAuto->TargetFriends;
		Save[ "OtherAuto" ][ "TargetPlayers" ] = Config->OtherAuto->TargetPlayers;
		Save[ "OtherAuto" ][ "TargetStickies" ] = Config->OtherAuto->TargetStickies;
		Save[ "OtherAuto" ][ "TargetUber" ] = Config->OtherAuto->TargetUber;
		Save[ "OtherAuto" ][ "TargetVacc" ] = Config->OtherAuto->TargetVacc;
		Save[ "OtherAuto" ][ "AutoDetonationMode" ] = Config->OtherAuto->AutoDetonationMode;

		if ( G->Authentication->IsDebugAllowed )
		{
			Save[ "Debug" ][ "Enabled" ] = Config->Debug->EnableInfo;
		}

		Save[ "Indicator" ][ "DT" ][ "X" ] = Visuals->DTIndicatorX;
		Save[ "Indicator" ][ "DT" ][ "Y" ] = Visuals->DTIndicatorY;
		Save[ "Indicator" ][ "Crits" ][ "X" ] = Visuals->CritIndicatorX;
		Save[ "Indicator" ][ "Crits" ][ "Y" ] = Visuals->CritIndicatorY;

		SAVECOLOR( MenuOutline );
		SAVECOLOR( MenuBackground );
		//SAVECOLOR( MenuForeground );
		SAVECOLOR( MenuBorders );
		//SAVECOLOR( MenuAccent );
		SAVECOLOR( MenuFade1 );
		SAVECOLOR( MenuFade2 );
		SAVECOLOR( MenuShine );
		SAVECOLOR( DTFade1 );
		SAVECOLOR( DTFade2 );

		SAVECOLOR( ESPPlayerRed );
		SAVECOLOR( ESPPlayerBlu );
		SAVECOLOR( ESPPlayerEnemy );
		SAVECOLOR( ESPPlayerTeam );
		SAVECOLOR( ESPPlayerLocal );
		SAVECOLOR( ESPPlayerFriend );

		SAVECOLOR( ESPBuildingRed );
		SAVECOLOR( ESPBuildingLocal );
		SAVECOLOR( ESPBuildingFriend );
		SAVECOLOR( ESPBuildingBlu );
		SAVECOLOR( ESPBuildingEnemy );
		SAVECOLOR( ESPBuildingTeam );

		SAVECOLOR( ESPWorldHealth );
		SAVECOLOR( ESPWorldLocal );
		SAVECOLOR( ESPWorldFriend );
		SAVECOLOR( ESPWorldAmmo );
		SAVECOLOR( ESPWorldBomb );
		SAVECOLOR( ESPWorldMoney );
		SAVECOLOR( ESPWorldPowerup );
		SAVECOLOR( ESPWorldRed );
		SAVECOLOR( ESPWorldBlu );
		SAVECOLOR( ESPWorldEnemy );
		SAVECOLOR( ESPWorldTeam );

		SAVECOLOR( ModelsPlayerRed );
		SAVECOLOR( ModelsPlayerBlu );
		SAVECOLOR( ModelsPlayerLocal );
		SAVECOLOR( ModelsPlayerFriend );
		SAVECOLOR( ModelsPlayerUber );
		SAVECOLOR( ModelsPlayerEnemy );
		SAVECOLOR( ModelsPlayerTeam );
		SAVECOLOR( ModelsPlayerLocalUnoccluded );
		SAVECOLOR( ModelsPlayerFriendUnoccluded );
		SAVECOLOR( ModelsPlayerUberUnoccluded );
		SAVECOLOR( ModelsPlayerRedUnoccluded );
		SAVECOLOR( ModelsPlayerBluUnoccluded );
		SAVECOLOR( ModelsPlayerEnemyUnoccluded );
		SAVECOLOR( ModelsPlayerTeamUnoccluded );
		SAVECOLOR( ModelsPlayerLocalOccluded );
		SAVECOLOR( ModelsPlayerFriendOccluded );
		SAVECOLOR( ModelsPlayerUberOccluded );
		SAVECOLOR( ModelsPlayerRedOccluded );
		SAVECOLOR( ModelsPlayerBluOccluded );
		SAVECOLOR( ModelsPlayerEnemyOccluded );
		SAVECOLOR( ModelsPlayerTeamOccluded );

		SAVECOLOR( ModelsBuildingRed );
		SAVECOLOR( ModelsBuildingLocal );
		SAVECOLOR( ModelsBuildingFriend );
		SAVECOLOR( ModelsBuildingBlu );
		SAVECOLOR( ModelsBuildingEnemy );
		SAVECOLOR( ModelsBuildingTeam );
		SAVECOLOR( ModelsBuildingRedUnoccluded );
		SAVECOLOR( ModelsBuildingBluUnoccluded );
		SAVECOLOR( ModelsBuildingEnemyUnoccluded );
		SAVECOLOR( ModelsBuildingTeamUnoccluded );
		SAVECOLOR( ModelsBuildingRedOccluded );
		SAVECOLOR( ModelsBuildingBluOccluded );
		SAVECOLOR( ModelsBuildingEnemyOccluded );
		SAVECOLOR( ModelsBuildingTeamOccluded );

		SAVECOLOR( ModelsWorldHealth );
		SAVECOLOR( ModelsWorldLocal );
		SAVECOLOR( ModelsWorldFriend );
		SAVECOLOR( ModelsWorldAmmo );
		SAVECOLOR( ModelsWorldBomb );
		SAVECOLOR( ModelsWorldMoney );
		SAVECOLOR( ModelsWorldPowerup );
		SAVECOLOR( ModelsWorldRed );
		SAVECOLOR( ModelsWorldBlu );
		SAVECOLOR( ModelsWorldEnemy );
		SAVECOLOR( ModelsWorldTeam );
		SAVECOLOR( ModelsWorldHealthUnoccluded );
		SAVECOLOR( ModelsWorldAmmoUnoccluded );
		SAVECOLOR( ModelsWorldBombUnoccluded );
		SAVECOLOR( ModelsWorldMoneyUnoccluded );
		SAVECOLOR( ModelsWorldPowerupUnoccluded );
		SAVECOLOR( ModelsWorldRedUnoccluded );
		SAVECOLOR( ModelsWorldBluUnoccluded );
		SAVECOLOR( ModelsWorldEnemyUnoccluded );
		SAVECOLOR( ModelsWorldTeamUnoccluded );
		SAVECOLOR( ModelsWorldHealthOccluded );
		SAVECOLOR( ModelsWorldAmmoOccluded );
		SAVECOLOR( ModelsWorldBombOccluded );
		SAVECOLOR( ModelsWorldMoneyOccluded );
		SAVECOLOR( ModelsWorldPowerupOccluded );
		SAVECOLOR( ModelsWorldRedOccluded );
		SAVECOLOR( ModelsWorldBluOccluded );
		SAVECOLOR( ModelsWorldEnemyOccluded );
		SAVECOLOR( ModelsWorldTeamOccluded );

		SAVECOLOR( OutlinesPlayerRed );
		SAVECOLOR( OutlinesPlayerLocal );
		SAVECOLOR( OutlinesPlayerFriend );
		SAVECOLOR( OutlinesPlayerUber );
		SAVECOLOR( OutlinesPlayerBlu );
		SAVECOLOR( OutlinesPlayerEnemy );
		SAVECOLOR( OutlinesPlayerTeam );

		SAVECOLOR( OutlinesBuildingRed );
		SAVECOLOR( OutlinesBuildingBlu );
		SAVECOLOR( OutlinesBuildingLocal );
		SAVECOLOR( OutlinesBuildingFriend );
		SAVECOLOR( OutlinesBuildingEnemy );
		SAVECOLOR( OutlinesBuildingTeam );

		SAVECOLOR( OutlinesWorldHealth );
		SAVECOLOR( OutlinesWorldLocal );
		SAVECOLOR( OutlinesWorldFriend );
		SAVECOLOR( OutlinesWorldAmmo );
		SAVECOLOR( OutlinesWorldBomb );
		SAVECOLOR( OutlinesWorldMoney );
		SAVECOLOR( OutlinesWorldPowerup );
		SAVECOLOR( OutlinesWorldRed );
		SAVECOLOR( OutlinesWorldBlu );
		SAVECOLOR( OutlinesWorldEnemy );
		SAVECOLOR( OutlinesWorldTeam );

		SAVECOLOR( WorldColor );
		SAVECOLOR( HandsColor );
		SAVECOLOR( HandsOverlayColor );
		SAVECOLOR( HitboxColor );
		SAVECOLOR( BoundingBoxColor );
		SAVECOLOR( ProjectileLineColor );

		SAVECOLOR( BeamsLocal );
		SAVECOLOR( BeamsFriend );
		SAVECOLOR( BeamsEnemy );
		SAVECOLOR( BeamsTeam );
		SAVECOLOR( BeamsRed );
		SAVECOLOR( BeamsBlu );

		SAVECOLOR( FakeModel );
		SAVECOLOR( FakeGlow );
		SAVECOLOR( LagModel );
		SAVECOLOR( LagGlow );

		Output << Save;
		Output.close( );
	}
}

void CMenu::LoadConfig( const char* name )
{
	if ( !name )
	{
		return;
	}

	std::ifstream Input( ConfigPath + "\\" + name + ".json" );

	if ( Input.is_open( ) )
	{
		Json::Value Load;
		Input >> Load;

		Config->Aimbot->Global->Enabled = Load[ "Aimbot" ][ "Global" ][ "Enabled" ].asBool( );
		Config->Aimbot->Global->AimKey = Load[ "Aimbot" ][ "Global" ][ "AimKey" ].asInt( );
		Config->Aimbot->Global->FOV = Load[ "Aimbot" ][ "Global" ][ "FOV" ].asInt( );
		Config->Aimbot->Global->AutoSecondarySwitch = Load[ "Aimbot" ][ "Global" ][ "AutoSecondarySwitch" ].asBool( );
		Config->Aimbot->Hitscan->TapFire = Load[ "Aimbot" ][ "Hitscan" ][ "TapFire" ].asBool( );
		Config->Aimbot->Hitscan->WaitForHeadshot = Load[ "Aimbot" ][ "Hitscan" ][ "WaitForHeadshot" ].asBool( );
		Config->Aimbot->Hitscan->PreventBait = Load[ "Aimbot" ][ "Hitscan" ][ "PreventBait" ].asBool( );
		Config->Aimbot->Hitscan->WaitForCharge = Load[ "Aimbot" ][ "Hitscan" ][ "WaitForCharge" ].asBool( );
		Config->Aimbot->Hitscan->Preference = Load[ "Aimbot" ][ "Hitscan" ][ "Preference" ].asInt( );
		Config->Aimbot->Hitscan->LagCompMethod = Load[ "Aimbot" ][ "Hitscan" ][ "LagCompMethod" ].asBool( );
		Config->Aimbot->Hitscan->FakeLatency = Load[ "Aimbot" ][ "Hitscan" ][ "FakeLatency" ].asBool( );
		Config->Aimbot->Hitscan->DynamicLatency = Load[ "Aimbot" ][ "Hitscan" ][ "DynamicLatency" ].asBool( );
		Config->Aimbot->Hitscan->FakeLatencyAmount = Load[ "Aimbot" ][ "Hitscan" ][ "FakeLatencyAmount" ].asInt( );
		Config->Aimbot->Hitscan->FakeLatencyKey = Load[ "Aimbot" ][ "Hitscan" ][ "FakeLatencyKey" ].asInt( );
		Config->Aimbot->Hitscan->ReduceScoreboardPing = Load[ "Aimbot" ][ "Hitscan" ][ "ReduceScoreboardPing" ].asBool( );
		Config->Aimbot->Hitscan->ScoreboardPingGoal = Load[ "Aimbot" ][ "Hitscan" ][ "ScoreboardPingGoal" ].asInt( );
		Config->Aimbot->Hitscan->PointScale = Load[ "Aimbot" ][ "Hitscan" ][ "PointScale" ].asInt( );
		Config->Aimbot->Hitscan->HeavyPointScale = Load[ "Aimbot" ][ "Hitscan" ][ "HeavyPointScale" ].asInt( );
		Config->Aimbot->Hitscan->LegitBob->Enabled = Load[ "Aimbot" ][ "Hitscan" ][ "LegitBob" ][ "Enabled" ].asBool( );
		Config->Aimbot->Hitscan->LegitBob->DeadZone = Load[ "Aimbot" ][ "Hitscan" ][ "LegitBob" ][ "DeadZone" ].asFloat( );
		Config->Aimbot->Hitscan->LegitBob->FOV = Load[ "Aimbot" ][ "Hitscan" ][ "LegitBob" ][ "Fov" ].asFloat( );
		Config->Aimbot->Hitscan->LegitBob->Strenght = Load[ "Aimbot" ][ "Hitscan" ][ "LegitBob" ][ "Strenght" ].asFloat( );
		Config->Aimbot->Hitscan->TargetHead = Load[ "Aimbot" ][ "Hitscan" ][ "TargetHead" ].asBool( );
		Config->Aimbot->Hitscan->TargetChest = Load[ "Aimbot" ][ "Hitscan" ][ "TargetChest" ].asBool( );
		Config->Aimbot->Hitscan->TargetStomach = Load[ "Aimbot" ][ "Hitscan" ][ "TargetStomach" ].asBool( );
		Config->Aimbot->Hitscan->TargetPelvis = Load[ "Aimbot" ][ "Hitscan" ][ "TargetPelvis" ].asBool( );
		Config->Aimbot->Hitscan->TargetArms = Load[ "Aimbot" ][ "Hitscan" ][ "TargetArms" ].asBool( );
		Config->Aimbot->Projectile->PredictStrafing = Load[ "Aimbot" ][ "Projectile" ][ "PredictStrafing" ].asBool( );
		Config->Aimbot->Projectile->PredictJumping = Load[ "Aimbot" ][ "Projectile" ][ "PredictJumping" ].asBool( );
		Config->Aimbot->Projectile->WaitForHeadshot = Load[ "Aimbot" ][ "Projectile" ][ "WaitForHeadshot" ].asBool( );
		Config->Aimbot->Projectile->DelayShot = Load[ "Aimbot" ][ "Projectile" ][ "DelayShot" ].asBool( );
		Config->Aimbot->Projectile->SplashBot = Load[ "Aimbot" ][ "Projectile" ][ "SplashBot" ].asBool( );
		Config->Aimbot->Projectile->SplashRadius = Load[ "Aimbot" ][ "Projectile" ][ "SplashRadius" ].asInt( );
		Config->Aimbot->Projectile->SplashPoints = Load[ "Aimbot" ][ "Projectile" ][ "SplashPoints" ].asInt( );
		Config->Aimbot->Projectile->MaxProjectileTime = Load[ "Aimbot" ][ "Projectile" ][ "MaxProjectileTime" ].asFloat( );
		Config->Aimbot->Projectile->MaxTime = Load[ "Aimbot" ][ "Projectile" ][ "MaxTime" ].asFloat( );
		Config->Aimbot->Projectile->HitChance = Load[ "Aimbot" ][ "Projectile" ][ "HitChance" ].asInt( );
		Config->Aimbot->Projectile->MaxStickyCharge = Load[ "Aimbot" ][ "Projectile" ][ "MaxStickyCharge" ].asInt( );
		Config->Aimbot->Projectile->ChangePositionKey = Load[ "Aimbot" ][ "Projectile" ][ "ChangePositionKey" ].asInt( );
		Config->Aimbot->Projectile->ResetPositionKey = Load[ "Aimbot" ][ "Projectile" ][ "ResetPositionKey" ].asInt( );
		Config->Aimbot->Projectile->GazaBombingKey = Load[ "Aimbot" ][ "Projectile" ][ "GazaBombingKey" ].asInt( );
		Config->Aimbot->Projectile->MaxGroundDebug = Load[ "Aimbot" ][ "Projectile" ][ "MaxGroundDebug" ].asFloat( );
		Config->Aimbot->Targetting->TargetPlayers = Load[ "Aimbot" ][ "Targetting" ][ "TargetPlayers" ].asBool( );
		Config->Aimbot->Targetting->TargetBonk = Load[ "Aimbot" ][ "Targetting" ][ "TargetBonk" ].asBool( );
		Config->Aimbot->Targetting->TargetUber = Load[ "Aimbot" ][ "Targetting" ][ "TargetUber" ].asBool( );
		Config->Aimbot->Targetting->TargetVacc = Load[ "Aimbot" ][ "Targetting" ][ "TargetVacc" ].asBool( );
		Config->Aimbot->Targetting->TargetBuildings = Load[ "Aimbot" ][ "Targetting" ][ "TargetBuildings" ].asBool( );
		Config->Aimbot->Targetting->TargetStickies = Load[ "Aimbot" ][ "Targetting" ][ "TargetStickies" ].asBool( );
		Config->Aimbot->Targetting->TargetFriends = Load[ "Aimbot" ][ "Targetting" ][ "TargetFriends" ].asBool( );
		Config->Aimbot->Targetting->TargetLethalBombs = Load[ "Aimbot" ][ "Targetting" ][ "TargetLethalBombs" ].asBool( );
		Config->Aimbot->Priorities->Scout = Load[ "Aimbot" ][ "Priorities" ][ "Scout" ].asInt( );
		Config->Aimbot->Priorities->Soldier = Load[ "Aimbot" ][ "Priorities" ][ "Soldier" ].asInt( );
		Config->Aimbot->Priorities->Pyro = Load[ "Aimbot" ][ "Priorities" ][ "Pyro" ].asInt( );
		Config->Aimbot->Priorities->Demoman = Load[ "Aimbot" ][ "Priorities" ][ "Demoman" ].asInt( );
		Config->Aimbot->Priorities->Heavy = Load[ "Aimbot" ][ "Priorities" ][ "Heavy" ].asInt( );
		Config->Aimbot->Priorities->Engineer = Load[ "Aimbot" ][ "Priorities" ][ "Engineer" ].asInt( );
		Config->Aimbot->Priorities->Medic = Load[ "Aimbot" ][ "Priorities" ][ "Medic" ].asInt( );
		Config->Aimbot->Priorities->Sniper = Load[ "Aimbot" ][ "Priorities" ][ "Sniper" ].asInt( );
		Config->Aimbot->Priorities->Spy = Load[ "Aimbot" ][ "Priorities" ][ "Spy" ].asInt( );
		Config->Aimbot->Priorities->OtherBuildings = Load[ "Aimbot" ][ "Priorities" ][ "OtherBuildings" ].asInt( );
		Config->Aimbot->Priorities->Stickies = Load[ "Aimbot" ][ "Priorities" ][ "Stickies" ].asInt( );
		Config->Aimbot->Priorities->Cheaters = Load[ "Aimbot" ][ "Priorities" ][ "Cheaters" ].asInt( );
		Config->Aimbot->Priorities->Friends = Load[ "Aimbot" ][ "Priorities" ][ "Friends" ].asInt( );
		Config->Aimbot->Priorities->Cloaked = Load[ "Aimbot" ][ "Priorities" ][ "Cloaked" ].asInt( );
		Config->Aimbot->Priorities->Scoped = Load[ "Aimbot" ][ "Priorities" ][ "Scoped" ].asInt( );
		Config->Aimbot->Priorities->Revved = Load[ "Aimbot" ][ "Priorities" ][ "Revved" ].asInt( );
		Config->Aimbot->Priorities->Sentry = Load[ "Aimbot" ][ "Priorities" ][ "Sentry" ].asInt( );
		Config->Aimbot->Priorities->Other = Load[ "Aimbot" ][ "Priorities" ][ "Other" ].asInt( );
		Config->Aimbot->Melee->Enabled = Load[ "Aimbot" ][ "Melee" ][ "Enabled" ].asBool( );
		Config->Aimbot->Melee->RequireAimKey = Load[ "Aimbot" ][ "Melee" ][ "RequireAimKey" ].asBool( );
		Config->Aimbot->Melee->WarpPrediction = Load[ "Aimbot" ][ "Melee" ][ "WarpPrediction" ].asBool( );

		Config->ESP->Global->Enabled = Load[ "ESP" ][ "Global" ][ "Enabled" ].asBool( );
		Config->ESP->Global->ColorScheme = Load[ "ESP" ][ "Global" ][ "ColorScheme" ].asInt( );
		Config->ESP->Buildings->Enabled = Load[ "ESP" ][ "Buildings" ][ "Enabled" ].asBool( );
		Config->ESP->Buildings->Enemy = Load[ "ESP" ][ "Buildings" ][ "Enemy" ].asBool( );
		Config->ESP->Buildings->Team = Load[ "ESP" ][ "Buildings" ][ "Team" ].asBool( );
		Config->ESP->Buildings->Friends = Load[ "ESP" ][ "Buildings" ][ "Friends" ].asBool( );
		Config->ESP->Buildings->Local = Load[ "ESP" ][ "Buildings" ][ "Local" ].asBool( );
		Config->ESP->Buildings->Name = Load[ "ESP" ][ "Buildings" ][ "Name" ].asBool( );
		Config->ESP->Buildings->EnabledBox = Load[ "ESP" ][ "Buildings" ][ "EnabledBox" ].asBool( );
		Config->ESP->Buildings->Flags = Load[ "ESP" ][ "Buildings" ][ "Flags" ].asBool( );
		Config->ESP->Buildings->HealthBar = Load[ "ESP" ][ "Buildings" ][ "HealthBar" ].asBool( );
		Config->ESP->Buildings->Box = Load[ "ESP" ][ "Buildings" ][ "Box" ].asInt( );
		Config->ESP->Buildings->PHealthBar->Lines = Load[ "ESP" ][ "Buildings" ][ "PHealthBar" ][ "Lines" ].asBool( );
		Config->ESP->Buildings->PHealthBar->Animated = Load[ "ESP" ][ "Buildings" ][ "PHealthBar" ][ "Animated" ].asBool( );
		Config->ESP->Buildings->PHealthBar->CustomColor = Load[ "ESP" ][ "Buildings" ][ "PHealthBar" ][ "CustomColor" ].asBool( );
		Config->ESP->Buildings->PHealthBar->LinesCount = Load[ "ESP" ][ "Buildings" ][ "PHealthBar" ][ "LinesCount" ].asInt( );
		Config->ESP->Players->Enabled = Load[ "ESP" ][ "Players" ][ "Enabled" ].asBool( );
		Config->ESP->Players->Enemy = Load[ "ESP" ][ "Players" ][ "Enemy" ].asBool( );
		Config->ESP->Players->Team = Load[ "ESP" ][ "Players" ][ "Team" ].asBool( );
		Config->ESP->Players->Friends = Load[ "ESP" ][ "Players" ][ "Friends" ].asBool( );
		Config->ESP->Players->Local = Load[ "ESP" ][ "Players" ][ "Local" ].asBool( );
		Config->ESP->Players->Name = Load[ "ESP" ][ "Players" ][ "Name" ].asBool( );
		Config->ESP->Players->EnabledBox = Load[ "ESP" ][ "Players" ][ "EnabledBox" ].asBool( );
		Config->ESP->Players->Flags = Load[ "ESP" ][ "Players" ][ "Flags" ].asBool( );
		Config->ESP->Players->Weapon = Load[ "ESP" ][ "Players" ][ "Weapon" ].asBool( );
		Config->ESP->Players->HealthBar = Load[ "ESP" ][ "Players" ][ "HealthBar" ].asBool( );
		Config->ESP->Players->UberBar = Load[ "ESP" ][ "Players" ][ "UberBar" ].asBool( );
		Config->ESP->Players->Avatar = Load[ "ESP" ][ "Players" ][ "Avatar" ].asBool( );
		Config->ESP->Players->Box = Load[ "ESP" ][ "Players" ][ "Box" ].asInt( );
		Config->ESP->Players->PHealthBar->Lines = Load[ "ESP" ][ "Players" ][ "PHealthBar" ][ "Lines" ].asBool( );
		Config->ESP->Players->PHealthBar->Animated = Load[ "ESP" ][ "Players" ][ "PHealthBar" ][ "Animated" ].asBool( );
		Config->ESP->Players->PHealthBar->CustomColor = Load[ "ESP" ][ "Players" ][ "PHealthBar" ][ "CustomColor" ].asBool( );
		Config->ESP->Players->PHealthBar->LinesCount = Load[ "ESP" ][ "Players" ][ "PHealthBar" ][ "LinesCount" ].asInt( );
		Config->ESP->World->Enabled = Load[ "ESP" ][ "World" ][ "Enabled" ].asBool( );
		Config->ESP->World->Enemy = Load[ "ESP" ][ "World" ][ "Enemy" ].asBool( );
		Config->ESP->World->Team = Load[ "ESP" ][ "World" ][ "Team" ].asBool( );
		Config->ESP->World->Friends = Load[ "ESP" ][ "World" ][ "Friends" ].asBool( );
		Config->ESP->World->Local = Load[ "ESP" ][ "World" ][ "Local" ].asBool( );
		Config->ESP->World->Healthpacks = Load[ "ESP" ][ "World" ][ "Healthpacks" ].asBool( );
		Config->ESP->World->Ammopacks = Load[ "ESP" ][ "World" ][ "Ammopacks" ].asBool( );
		Config->ESP->World->Intels = Load[ "ESP" ][ "World" ][ "Intels" ].asBool( );
		Config->ESP->World->Bombs = Load[ "ESP" ][ "World" ][ "Bombs" ].asBool( );
		Config->ESP->World->MvMCash = Load[ "ESP" ][ "World" ][ "MvMCash" ].asBool( );
		Config->ESP->World->MvMTanks = Load[ "ESP" ][ "World" ][ "MvMTanks" ].asBool( );
		Config->ESP->World->MvMSentryBusters = Load[ "ESP" ][ "World" ][ "MvMSentryBusters" ].asBool( );
		Config->ESP->World->MPPowerups = Load[ "ESP" ][ "World" ][ "MPPowerups" ].asBool( );
		Config->ESP->World->Projectiles = Load[ "ESP" ][ "World" ][ "Projectiles" ].asBool( );
		Config->ESP->World->Name = Load[ "ESP" ][ "World" ][ "Name" ].asBool( );
		Config->ESP->World->EnabledBox = Load[ "ESP" ][ "World" ][ "EnabledBox" ].asBool( );
		Config->ESP->World->Distance = Load[ "ESP" ][ "World" ][ "Distance" ].asBool( );
		Config->ESP->World->Box = Load[ "ESP" ][ "World" ][ "Box" ].asInt( );

		Config->Models->Global->Enabled = Load[ "Models" ][ "Global" ][ "Enabled" ].asBool( );
		Config->Models->Global->ColorScheme = Load[ "Models" ][ "Global" ][ "ColorScheme" ].asInt( );
		Config->Models->Global->RenderingOrder = Load[ "Models" ][ "Global" ][ "RenderingOrder" ].asInt( );
		Config->Models->Buildings->Enabled = Load[ "Models" ][ "Buildings" ][ "Enabled" ].asBool( );
		Config->Models->Buildings->Enemy = Load[ "Models" ][ "Buildings" ][ "Enemy" ].asBool( );
		Config->Models->Buildings->Team = Load[ "Models" ][ "Buildings" ][ "Team" ].asBool( );
		Config->Models->Buildings->Friends = Load[ "Models" ][ "Buildings" ][ "Friends" ].asBool( );
		Config->Models->Buildings->Local = Load[ "Models" ][ "Buildings" ][ "Local" ].asBool( );
		Config->Models->Buildings->TwoModels = Load[ "Models" ][ "Buildings" ][ "TwoModels" ].asBool( );
		Config->Models->Buildings->Unoccluded->Overlay = Load[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "Overlay" ].asBool( );
		Config->Models->Buildings->Unoccluded->Alpha = Load[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "Alpha" ].asFloat( );
		Config->Models->Buildings->Unoccluded->OverlayAlpha = Load[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "OverlayAlpha" ].asFloat( );
		Config->Models->Buildings->Unoccluded->Model = Load[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "Model" ].asInt( );
		Config->Models->Buildings->Unoccluded->OverlayModel = Load[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "OverlayModel" ].asInt( );
		Config->Models->Buildings->Unoccluded->OverlayWireframe = Load[ "Models" ][ "Buildings" ][ "Unoccluded" ][ "OverlayWireframe" ].asBool( );
		Config->Models->Buildings->Occluded->Overlay = Load[ "Models" ][ "Buildings" ][ "Occluded" ][ "Overlay" ].asBool( );
		Config->Models->Buildings->Occluded->Alpha = Load[ "Models" ][ "Buildings" ][ "Occluded" ][ "Alpha" ].asFloat( );
		Config->Models->Buildings->Occluded->OverlayAlpha = Load[ "Models" ][ "Buildings" ][ "Occluded" ][ "OverlayAlpha" ].asFloat( );
		Config->Models->Buildings->Occluded->Model = Load[ "Models" ][ "Buildings" ][ "Occluded" ][ "Model" ].asInt( );
		Config->Models->Buildings->Occluded->OverlayModel = Load[ "Models" ][ "Buildings" ][ "Occluded" ][ "OverlayModel" ].asInt( );
		Config->Models->Buildings->Occluded->OverlayWireframe = Load[ "Models" ][ "Buildings" ][ "Occluded" ][ "OverlayWireframe" ].asBool( );
		Config->Models->Buildings->IgnoreZ = Load[ "Models" ][ "Buildings" ][ "IgnoreZ" ].asBool( );
		Config->Models->Buildings->Overlay = Load[ "Models" ][ "Buildings" ][ "Overlay" ].asBool( );
		Config->Models->Buildings->Alpha = Load[ "Models" ][ "Buildings" ][ "Alpha" ].asFloat( );
		Config->Models->Buildings->OverlayAlpha = Load[ "Models" ][ "Buildings" ][ "OverlayAlpha" ].asFloat( );
		Config->Models->Buildings->Model = Load[ "Models" ][ "Buildings" ][ "Model" ].asInt( );
		Config->Models->Buildings->OverlayModel = Load[ "Models" ][ "Buildings" ][ "OverlayModel" ].asInt( );
		Config->Models->Buildings->OverlayWireframe = Load[ "Models" ][ "Buildings" ][ "OverlayWireframe" ].asBool( );

		Config->Models->Players->Enabled = Load[ "Models" ][ "Players" ][ "Enabled" ].asBool( );
		Config->Models->Players->Enemy = Load[ "Models" ][ "Players" ][ "Enemy" ].asBool( );
		Config->Models->Players->Team = Load[ "Models" ][ "Players" ][ "Team" ].asBool( );
		Config->Models->Players->Friends = Load[ "Models" ][ "Players" ][ "Friends" ].asBool( );
		Config->Models->Players->Local = Load[ "Models" ][ "Players" ][ "Local" ].asBool( );
		Config->Models->Players->TwoModels = Load[ "Models" ][ "Players" ][ "TwoModels" ].asBool( );
		Config->Models->Players->Unoccluded->Overlay = Load[ "Models" ][ "Players" ][ "Unoccluded" ][ "Overlay" ].asBool( );
		Config->Models->Players->Unoccluded->Alpha = Load[ "Models" ][ "Players" ][ "Unoccluded" ][ "Alpha" ].asFloat( );
		Config->Models->Players->Unoccluded->OverlayAlpha = Load[ "Models" ][ "Players" ][ "Unoccluded" ][ "OverlayAlpha" ].asFloat( );
		Config->Models->Players->Unoccluded->Model = Load[ "Models" ][ "Players" ][ "Unoccluded" ][ "Model" ].asInt( );
		Config->Models->Players->Unoccluded->OverlayType = Load[ "Models" ][ "Players" ][ "Unoccluded" ][ "OverlayType" ].asInt( );
		Config->Models->Players->Unoccluded->OverlayModel = Load[ "Models" ][ "Players" ][ "Unoccluded" ][ "OverlayModel" ].asInt( );
		Config->Models->Players->Unoccluded->OverlayWireframe = Load[ "Models" ][ "Players" ][ "Unoccluded" ][ "OverlayWireframe" ].asBool( );
		Config->Models->Players->Occluded->Overlay = Load[ "Models" ][ "Players" ][ "Occluded" ][ "Overlay" ].asBool( );
		Config->Models->Players->Occluded->Alpha = Load[ "Models" ][ "Players" ][ "Occluded" ][ "Alpha" ].asFloat( );
		Config->Models->Players->Occluded->OverlayAlpha = Load[ "Models" ][ "Players" ][ "Occluded" ][ "OverlayAlpha" ].asFloat( );
		Config->Models->Players->Occluded->Model = Load[ "Models" ][ "Players" ][ "Occluded" ][ "Model" ].asInt( );
		Config->Models->Players->Occluded->OverlayType = Load[ "Models" ][ "Players" ][ "Occluded" ][ "OverlayType" ].asInt( );
		Config->Models->Players->Occluded->OverlayModel = Load[ "Models" ][ "Players" ][ "Occluded" ][ "OverlayModel" ].asInt( );
		Config->Models->Players->Occluded->OverlayWireframe = Load[ "Models" ][ "Players" ][ "Occluded" ][ "OverlayWireframe" ].asBool( );
		Config->Models->Players->IgnoreZ = Load[ "Models" ][ "Players" ][ "IgnoreZ" ].asBool( );
		Config->Models->Players->Overlay = Load[ "Models" ][ "Players" ][ "Overlay" ].asBool( );
		Config->Models->Players->Alpha = Load[ "Models" ][ "Players" ][ "Alpha" ].asFloat( );
		Config->Models->Players->OverlayAlpha = Load[ "Models" ][ "Players" ][ "OverlayAlpha" ].asFloat( );
		Config->Models->Players->Model = Load[ "Models" ][ "Players" ][ "Model" ].asInt( );
		Config->Models->Players->OverlayType = Load[ "Models" ][ "Players" ][ "OverlayType" ].asInt( );
		Config->Models->Players->OverlayModel = Load[ "Models" ][ "Players" ][ "OverlayModel" ].asInt( );
		Config->Models->Players->OverlayWireframe = Load[ "Models" ][ "Players" ][ "OverlayWireframe" ].asBool( );

		Config->Models->World->Enabled = Load[ "Models" ][ "World" ][ "Enabled" ].asBool( );
		Config->Models->World->Enemy = Load[ "Models" ][ "World" ][ "Enemy" ].asBool( );
		Config->Models->World->Team = Load[ "Models" ][ "World" ][ "Team" ].asBool( );
		Config->Models->World->Friends = Load[ "Models" ][ "World" ][ "Friends" ].asBool( );
		Config->Models->World->Local = Load[ "Models" ][ "World" ][ "Local" ].asBool( );
		Config->Models->World->Healthpacks = Load[ "Models" ][ "World" ][ "Healthpacks" ].asBool( );
		Config->Models->World->Ammopacks = Load[ "Models" ][ "World" ][ "Ammopacks" ].asBool( );
		Config->Models->World->Intels = Load[ "Models" ][ "World" ][ "Intels" ].asBool( );
		Config->Models->World->Bombs = Load[ "Models" ][ "World" ][ "Bombs" ].asBool( );
		Config->Models->World->MvMCash = Load[ "Models" ][ "World" ][ "MvMCash" ].asBool( );
		Config->Models->World->MvMTanks = Load[ "Models" ][ "World" ][ "MvMTanks" ].asBool( );
		Config->Models->World->MvMSentryBusters = Load[ "Models" ][ "World" ][ "MvMSentryBusters" ].asBool( );
		Config->Models->World->MPPowerups = Load[ "Models" ][ "World" ][ "MPPowerups" ].asBool( );
		Config->Models->World->Projectiles = Load[ "Models" ][ "World" ][ "Projectiles" ].asBool( );
		Config->Models->World->TwoModels = Load[ "Models" ][ "World" ][ "TwoModels" ].asBool( );
		Config->Models->World->Unoccluded->Overlay = Load[ "Models" ][ "World" ][ "Unoccluded" ][ "Overlay" ].asBool( );
		Config->Models->World->Unoccluded->Alpha = Load[ "Models" ][ "World" ][ "Unoccluded" ][ "Alpha" ].asFloat( );
		Config->Models->World->Unoccluded->OverlayAlpha = Load[ "Models" ][ "World" ][ "Unoccluded" ][ "OverlayAlpha" ].asFloat( );
		Config->Models->World->Unoccluded->Model = Load[ "Models" ][ "World" ][ "Unoccluded" ][ "Model" ].asInt( );
		Config->Models->World->Unoccluded->OverlayModel = Load[ "Models" ][ "World" ][ "Unoccluded" ][ "OverlayModel" ].asInt( );
		Config->Models->World->Unoccluded->OverlayWireframe = Load[ "Models" ][ "World" ][ "Unoccluded" ][ "OverlayWireframe" ].asBool( );
		Config->Models->World->Occluded->Overlay = Load[ "Models" ][ "World" ][ "Occluded" ][ "Overlay" ].asBool( );
		Config->Models->World->Occluded->Alpha = Load[ "Models" ][ "World" ][ "Occluded" ][ "Alpha" ].asFloat( );
		Config->Models->World->Occluded->OverlayAlpha = Load[ "Models" ][ "World" ][ "Occluded" ][ "OverlayAlpha" ].asFloat( );
		Config->Models->World->Occluded->Model = Load[ "Models" ][ "World" ][ "Occluded" ][ "Model" ].asInt( );
		Config->Models->World->Occluded->OverlayModel = Load[ "Models" ][ "World" ][ "Occluded" ][ "OverlayModel" ].asInt( );
		Config->Models->World->Occluded->OverlayWireframe = Load[ "Models" ][ "World" ][ "Occluded" ][ "OverlayWireframe" ].asBool( );
		Config->Models->World->IgnoreZ = Load[ "Models" ][ "World" ][ "IgnoreZ" ].asBool( );
		Config->Models->World->Overlay = Load[ "Models" ][ "World" ][ "Overlay" ].asBool( );
		Config->Models->World->Alpha = Load[ "Models" ][ "World" ][ "Alpha" ].asFloat( );
		Config->Models->World->OverlayAlpha = Load[ "Models" ][ "World" ][ "OverlayAlpha" ].asFloat( );
		Config->Models->World->Model = Load[ "Models" ][ "World" ][ "Model" ].asInt( );
		Config->Models->World->OverlayModel = Load[ "Models" ][ "World" ][ "OverlayModel" ].asInt( );
		Config->Models->World->OverlayWireframe = Load[ "Models" ][ "World" ][ "OverlayWireframe" ].asBool( );

		Config->Outlines->Global->Enabled = Load[ "Outlines" ][ "Global" ][ "Enabled" ].asBool( );
		Config->Outlines->Global->ColorScheme = Load[ "Outlines" ][ "Global" ][ "ColorScheme" ].asInt( );
		Config->Outlines->Global->BlurOutline = Load[ "Outlines" ][ "Global" ][ "BlurOutline" ].asBool( );
		Config->Outlines->Global->StencilOutline = Load[ "Outlines" ][ "Global" ][ "StencilOutline" ].asBool( );
		Config->Outlines->Global->CheapStencil = Load[ "Outlines" ][ "Global" ][ "CheapStencil" ].asBool( );
		Config->Outlines->Global->BlurScale = Load[ "Outlines" ][ "Global" ][ "BlurScale" ].asInt( );
		Config->Outlines->Global->StencilScale = Load[ "Outlines" ][ "Global" ][ "StencilScale" ].asInt( );

		Config->Outlines->Players->Enabled = Load[ "Outlines" ][ "Players" ][ "Enabled" ].asBool( );
		Config->Outlines->Players->Enemy = Load[ "Outlines" ][ "Players" ][ "Enemy" ].asBool( );
		Config->Outlines->Players->Team = Load[ "Outlines" ][ "Players" ][ "Team" ].asBool( );
		Config->Outlines->Players->Friends = Load[ "Outlines" ][ "Players" ][ "Friends" ].asBool( );
		Config->Outlines->Players->Local = Load[ "Outlines" ][ "Players" ][ "Local" ].asBool( );
		Config->Outlines->Players->Alpha = Load[ "Outlines" ][ "Players" ][ "Alpha" ].asFloat( );

		Config->Outlines->Buildings->Enabled = Load[ "Outlines" ][ "Buildings" ][ "Enabled" ].asBool( );
		Config->Outlines->Buildings->Enemy = Load[ "Outlines" ][ "Buildings" ][ "Enemy" ].asBool( );
		Config->Outlines->Buildings->Team = Load[ "Outlines" ][ "Buildings" ][ "Team" ].asBool( );
		Config->Outlines->Buildings->Friends = Load[ "Outlines" ][ "Buildings" ][ "Friends" ].asBool( );
		Config->Outlines->Buildings->Local = Load[ "Outlines" ][ "Buildings" ][ "Local" ].asBool( );
		Config->Outlines->Buildings->Alpha = Load[ "Outlines" ][ "Buildings" ][ "Alpha" ].asFloat( );

		Config->Outlines->World->Enabled = Load[ "Outlines" ][ "World" ][ "Enabled" ].asBool( );
		Config->Outlines->World->Enemy = Load[ "Outlines" ][ "World" ][ "Enemy" ].asBool( );
		Config->Outlines->World->Team = Load[ "Outlines" ][ "World" ][ "Team" ].asBool( );
		Config->Outlines->World->Friends = Load[ "Outlines" ][ "World" ][ "Friends" ].asBool( );
		Config->Outlines->World->Local = Load[ "Outlines" ][ "World" ][ "Local" ].asBool( );
		Config->Outlines->World->Healthpacks = Load[ "Outlines" ][ "World" ][ "Healthpacks" ].asBool( );
		Config->Outlines->World->Ammopacks = Load[ "Outlines" ][ "World" ][ "Ammopacks" ].asBool( );
		Config->Outlines->World->Intels = Load[ "Outlines" ][ "World" ][ "Intels" ].asBool( );
		Config->Outlines->World->Bombs = Load[ "Outlines" ][ "World" ][ "Bombs" ].asBool( );
		Config->Outlines->World->MvMCash = Load[ "Outlines" ][ "World" ][ "MvMCash" ].asBool( );
		Config->Outlines->World->MvMTanks = Load[ "Outlines" ][ "World" ][ "MvMTanks" ].asBool( );
		Config->Outlines->World->MvMSentryBusters = Load[ "Outlines" ][ "World" ][ "MvMSentryBusters" ].asBool( );
		Config->Outlines->World->MPPowerups = Load[ "Outlines" ][ "World" ][ "MPPowerups" ].asBool( );
		Config->Outlines->World->Projectiles = Load[ "Outlines" ][ "World" ][ "Projectiles" ].asBool( );
		Config->Outlines->World->Alpha = Load[ "Outlines" ][ "World" ][ "Alpha" ].asFloat( );

		Config->Visuals->FOV = Load[ "Visuals" ][ "FOV" ].asInt( );
		Config->Visuals->ScopedFOV = Load[ "Visuals" ][ "ScopedFOV" ].asInt( );
		Config->Visuals->RemoveViewPunch = Load[ "Visuals" ][ "RemoveViewPunch" ].asBool( );
		Config->Visuals->RemoveShake = Load[ "Visuals" ][ "RemoveShake" ].asBool( );
		Config->Visuals->RemovePostProcessing = Load[ "Visuals" ][ "RemovePostProcessing" ].asBool( );
		Config->Visuals->RemoveFade = Load[ "Visuals" ][ "RemoveFade" ].asBool( );
		Config->Visuals->RemoveScope = Load[ "Visuals" ][ "RemoveScope" ].asBool( );
		Config->Visuals->ThirdPersonKey = Load[ "Visuals" ][ "ThirdPersonKey" ].asInt( );
		Config->Visuals->ThirdPersonOffset = Load[ "Visuals" ][ "ThirdPersonOffset" ].asBool( );
		Config->Visuals->ThirdPersonSidewaysCycleKey = Load[ "Visuals" ][ "ThirdPersonSidewaysCycleKey" ].asInt( );
		Config->Visuals->ThirdPersonForward = Load[ "Visuals" ][ "ThirdPersonForward" ].asInt( );
		Config->Visuals->ThirdPersonSideways = Load[ "Visuals" ][ "ThirdPersonSideways" ].asInt( );
		Config->Visuals->ThirdPersonUpward = Load[ "Visuals" ][ "ThirdPersonUpward" ].asInt( );
		Config->Visuals->AspectRatio = Load[ "Visuals" ][ "AspectRatio" ].asFloat( );
		Config->Visuals->BloomScale = Load[ "Visuals" ][ "BloomScale" ].asFloat( );
		Config->Visuals->HitMarker2D = Load[ "Visuals" ][ "HitMarker2D" ].asBool( );
		Config->Visuals->HitscanHitboxes = Load[ "Visuals" ][ "HitscanHitboxes" ].asBool( );
		Config->Visuals->HitboxesCustomColor = Load[ "Visuals" ][ "HitboxesCustomColor" ].asBool( );
		Config->Visuals->HitboxesAlpha = Load[ "Visuals" ][ "HitboxesAlpha" ].asFloat( );
		Config->Visuals->HitboxesTime = Load[ "Visuals" ][ "HitboxesTime" ].asFloat( );
		Config->Visuals->ProjectileBoundingBox = Load[ "Visuals" ][ "ProjectileBoundingBox" ].asBool( );
		Config->Visuals->BoundingBoxCustomColor = Load[ "Visuals" ][ "BoundingBoxCustomColor" ].asBool( );
		Config->Visuals->BoundingBoxAlpha = Load[ "Visuals" ][ "BoundingBoxAlpha" ].asFloat( );
		Config->Visuals->BoundingBoxTime = Load[ "Visuals" ][ "BoundingBoxTime" ].asFloat( );
		//Config->Visuals->HitMarker3D = Load[ "Visuals" ][ "HitMarker3D" ].asBool( );
		//Config->Visuals->HitboxHitMarker = Load[ "Visuals" ][ "HitboxHitMarker" ].asBool( );
		Config->Visuals->DrawBackTrack = Load[ "Visuals" ][ "DrawBackTrack" ].asBool( );
		Config->Visuals->LastTickOnly = Load[ "Visuals" ][ "LastTickOnly" ].asBool( );
		Config->Visuals->ViewmodelSway = Load[ "Visuals" ][ "ViewmodelSway" ].asBool( );
		Config->Visuals->ViewmodelSwayAmount = Load[ "Visuals" ][ "ViewmodelSwayAmount" ].asFloat( );
		Config->Visuals->ViewmodelPos = Load[ "Visuals" ][ "ViewmodelPos" ].asBool( );
		Config->Visuals->ViewmodelFlip = Load[ "Visuals" ][ "ViewmodelFlip" ].asBool( );
		Config->Visuals->ViewmodelFOV = Load[ "Visuals" ][ "ViewmodelFOV" ].asInt( );
		Config->Visuals->ViewmodelX = Load[ "Visuals" ][ "ViewmodelX" ].asInt( );
		Config->Visuals->ViewmodelY = Load[ "Visuals" ][ "ViewmodelY" ].asInt( );
		Config->Visuals->ViewmodelZ = Load[ "Visuals" ][ "ViewmodelZ" ].asInt( );
		Config->Visuals->ViewmodelRoll = Load[ "Visuals" ][ "ViewmodelRoll" ].asInt( );
		Config->Visuals->HandsModels = Load[ "Visuals" ][ "HandsModels" ].asBool( );
		Config->Visuals->HandsModel = Load[ "Visuals" ][ "HandsModel" ].asInt( );
		Config->Visuals->DisableHands = Load[ "Visuals" ][ "DisableHands" ].asBool( );
		Config->Visuals->DisableWeapon = Load[ "Visuals" ][ "DisableWeapon" ].asBool( );
		Config->Visuals->HandsOverlay = Load[ "Visuals" ][ "HandsOverlay" ].asBool( );
		Config->Visuals->HandsOverlayModel = Load[ "Visuals" ][ "HandsOverlayModel" ].asInt( );
		Config->Visuals->HandsAlpha = Load[ "Visuals" ][ "HandsAlpha" ].asFloat( );
		Config->Visuals->HandsOverlayAlpha = Load[ "Visuals" ][ "HandsOverlayAlpha" ].asFloat( );
		Config->Visuals->HandsOverlayWireframe = Load[ "Visuals" ][ "HandsOverlayWireframe" ].asBool( );
		Config->Visuals->PlayerArrows = Load[ "Visuals" ][ "PlayerArrows" ].asBool( );
		Config->Visuals->ArrowsScreenDistance = Load[ "Visuals" ][ "ArrowsScreenDistance" ].asFloat( );
		Config->Visuals->ArrowsScreenShape = Load[ "Visuals" ][ "ArrowsScreenShape" ].asInt( );
		Config->Visuals->ProjectileLine = Load[ "Visuals" ][ "ProjectileLine" ].asBool( );
		Config->Visuals->ProjectileLineAlpha = Load[ "Visuals" ][ "ProjectileLineAlpha" ].asFloat( );
		Config->Visuals->ProjectileCamera = Load[ "Visuals" ][ "ProjectileCamera" ].asBool( );
		Config->Visuals->ProjectileCameraFOV = Load[ "Visuals" ][ "ProjectileCameraFOV" ].asInt( );
		Visuals->ProjectileCamX = Load[ "Visuals" ][ "ProjectileCamX" ].asInt( );
		Visuals->ProjectileCamY = Load[ "Visuals" ][ "ProjectileCamY" ].asInt( );
		Config->Visuals->ProjectileCameraWidth = Load[ "Visuals" ][ "ProjectileCameraWidth" ].asInt( );
		Config->Visuals->ProjectileCameraHeight = Load[ "Visuals" ][ "ProjectileCameraHeight" ].asInt( );
		Config->Visuals->BackTrackModel = Load[ "Visuals" ][ "BackTrackModel" ].asBool( );
		Config->Visuals->BackTrackOutline = Load[ "Visuals" ][ "BackTrackOutline" ].asBool( );
		Config->Visuals->BackTrackAlpha = Load[ "Visuals" ][ "BackTrackAlpha" ].asFloat( );
		Config->Visuals->RagdollEffects = Load[ "Visuals" ][ "RagdollEffects" ].asBool( );
		Config->Visuals->RagdollBurning = Load[ "Visuals" ][ "RagdollBurning" ].asBool( );
		Config->Visuals->RagdollElectrocuted = Load[ "Visuals" ][ "RagdollElectrocuted" ].asBool( );
		Config->Visuals->RagdollDissolving = Load[ "Visuals" ][ "RagdollDissolving" ].asBool( );
		Config->Visuals->RagdollBecomeAsh = Load[ "Visuals" ][ "RagdollBecomeAsh" ].asBool( );
		Config->Visuals->RagdollBaseType = Load[ "Visuals" ][ "RagdollBaseType" ].asInt( );
		Config->Visuals->RagdollEnemy = Load[ "Visuals" ][ "RagdollEnemy" ].asBool( );
		Config->Visuals->RagdollTeam = Load[ "Visuals" ][ "RagdollTeam" ].asBool( );
		Config->Visuals->RagdollLocal = Load[ "Visuals" ][ "RagdollLocal" ].asBool( );
		Config->Visuals->BulletTracers = Load[ "Visuals" ][ "BulletTracers" ].asInt( );
		Config->Visuals->SkyboxName = Load[ "Visuals" ][ "SkyboxName" ].asString( );
		Config->Visuals->StreamerMode = Load[ "Visuals" ][ "StreamerMode" ].asBool( );
		Config->Visuals->HideLocal = Load[ "Visuals" ][ "HideLocal" ].asBool( );
		Config->Visuals->HideFriends = Load[ "Visuals" ][ "HideFriends" ].asBool( );
		Config->Visuals->UnlockEquipRegion = Load[ "Visuals" ][ "UnlockEquipRegion" ].asBool( );
		Config->Visuals->OptimizedFonts = Load[ "Visuals" ][ "OptimizedFonts" ].asBool( );
		Config->Visuals->MedalChanger = Load[ "Visuals" ][ "MedalChanger" ].asBool( );
		Config->Visuals->MedalLevel = Load[ "Visuals" ][ "MedalLevel" ].asInt( );
		Config->Visuals->MedalTier = Load[ "Visuals" ][ "MedalTier" ].asInt( );

		Config->Visuals->FakeTaunt->Enabled = Load[ "Visuals" ][ "FakeTaunt" ][ "Enabled" ].asBool( );
		Config->Visuals->WorldModulation->Enabled = Load[ "Visuals" ][ "WorldModulation" ][ "Enabled" ].asBool( );
		Config->Visuals->WorldModulation->World = Load[ "Visuals" ][ "WorldModulation" ][ "World" ].asBool( );
		Config->Visuals->WorldModulation->Skybox = Load[ "Visuals" ][ "WorldModulation" ][ "Skybox" ].asBool( );
		Config->Visuals->WorldModulation->Mode = Load[ "Visuals" ][ "WorldModulation" ][ "Mode" ].asInt( );
		Config->Visuals->WorldModulation->Amount = Load[ "Visuals" ][ "WorldModulation" ][ "Amount" ].asFloat( );
		Config->Visuals->WorldModulation->Alpha = Load[ "Visuals" ][ "WorldModulation" ][ "Alpha" ].asFloat( );
		Config->Visuals->Beams->Enabled = Load[ "Visuals" ][ "Beams" ][ "Enabled" ].asBool( );
		Config->Visuals->Beams->ColorScheme = Load[ "Visuals" ][ "Beams" ][ "ColorScheme" ].asInt( );
		Config->Visuals->Beams->Enemy = Load[ "Visuals" ][ "Beams" ][ "Enemy" ].asBool( );
		Config->Visuals->Beams->Team = Load[ "Visuals" ][ "Beams" ][ "Team" ].asBool( );
		Config->Visuals->Beams->Friends = Load[ "Visuals" ][ "Beams" ][ "Friends" ].asBool( );
		Config->Visuals->Beams->Local = Load[ "Visuals" ][ "Beams" ][ "Local" ].asBool( );

		Config->Misc->BHop = Load[ "Misc" ][ "BHop" ].asBool( );
		Config->Misc->AutoStrafer = Load[ "Misc" ][ "AutoStrafer" ].asBool( );
		Config->Misc->EdgeJump = Load[ "Misc" ][ "EdgeJump" ].asBool( );
		Config->Misc->EdgeJumpKey = Load[ "Misc" ][ "EdgeJumpKey" ].asInt( );
		Config->Misc->StraferScale = Load[ "Misc" ][ "StraferScale" ].asInt( );
		Config->Misc->AvoidTeamPush = Load[ "Misc" ][ "AvoidTeamPush" ].asBool( );
		Config->Misc->FastStop = Load[ "Misc" ][ "AvoidTeamPush" ].asBool( );
		Config->Misc->HighJump = Load[ "Misc" ][ "HighJump" ].asBool( );
		Config->Misc->HighJumpKey = Load[ "Misc" ][ "HighJumpKey" ].asInt( );
		Config->Misc->AutoRetreat = Load[ "Misc" ][ "AutoRetreat" ].asBool( );
		Config->Misc->AutoRetreatKey = Load[ "Misc" ][ "AutoRetreatKey" ].asInt( );
		Config->Misc->FastBackup = Load[ "Misc" ][ "FastBackup" ].asBool( );
		Config->Misc->AntiBackstab = Load[ "Misc" ][ "AntiBackstab" ].asBool( );
		Config->Misc->VoiceSpam = Load[ "Misc" ][ "VoiceSpam" ].asBool( );
		Config->Misc->VoiceSpamMode = Load[ "Misc" ][ "VoiceSpamMode" ].asInt( );
		Config->Misc->AntiAutoBalance = Load[ "Misc" ][ "AntiAutoBalance" ].asBool( );
		Config->Misc->DT->Enabled = Load[ "Misc" ][ "DT" ][ "Enabled" ].asBool( );
		Config->Misc->DT->DTKey = Load[ "Misc" ][ "DT" ][ "DTKey" ].asInt( );
		Config->Misc->DT->DelayMode = Load[ "Misc" ][ "DT" ][ "DelayMode" ].asInt( );
		Config->Misc->DT->DelayTicks = Load[ "Misc" ][ "DT" ][ "DelayTicks" ].asInt( );
		Config->Misc->DT->RechargeKey = Load[ "Misc" ][ "DT" ][ "RechargeKey" ].asInt( );
		Config->Misc->DT->WarpKey = Load[ "Misc" ][ "DT" ][ "WarpKey" ].asInt( );
		Config->Misc->DT->ShootWood = Load[ "Misc" ][ "DT" ][ "ShootWood" ].asBool( );
		Config->Misc->DT->AntiWarp = Load[ "Misc" ][ "DT" ][ "AntiWarp" ].asBool( );
		Config->Misc->DT->WarpMode = Load[ "Misc" ][ "DT" ][ "WarpMode" ].asInt( );
		Config->Misc->DT->WarpTicks = Load[ "Misc" ][ "DT" ][ "WarpTicks" ].asInt( );
		Config->Misc->DT->ActiveInAir = Load[ "Misc" ][ "DT" ][ "ActiveInAir" ].asBool( );
		Config->Misc->DT->ActiveOnlyAimbot = Load[ "Misc" ][ "DT" ][ "ActiveOnlyAimbot" ].asBool( );
		Config->Misc->DT->ActivateOnlyLethal = Load[ "Misc" ][ "DT" ][ "ActivateOnlyLethal" ].asBool( );
		Config->Misc->DT->Indicator->Enabled = Load[ "Misc" ][ "DT" ][ "Indicator" ][ "Enabled" ].asBool( );
		Config->Misc->DT->Indicator->CustomFade = Load[ "Misc" ][ "DT" ][ "Indicator" ][ "CustomFade" ].asBool( );
		Config->Misc->CritHack->Enabled = Load[ "Misc" ][ "CritHack" ][ "Enabled" ].asBool( );
		Config->Misc->CritHack->CritKey = Load[ "Misc" ][ "CritHack" ][ "CritKey" ].asInt( );
		Config->Misc->CritHack->SkipRandom = Load[ "Misc" ][ "CritHack" ][ "SkipRandom" ].asBool( );
		Config->Misc->CritHack->ForceOnMelee = Load[ "Misc" ][ "CritHack" ][ "ForceOnMelee" ].asBool();
		Config->Misc->CritHack->ForceOnSuspicious = Load[ "Misc" ][ "CritHack" ][ "ForceOnSuspicious" ].asBool();
		Config->Misc->CritHack->ForceOnCheater = Load[ "Misc" ][ "CritHack" ][ "ForceOnCheater" ].asBool();
		Config->Misc->CritHack->ForceOnFriends = Load[ "Misc" ][ "CritHack" ][ "ForceOnFriends" ].asBool();
		Config->Misc->CritHack->ForceOnStreamers = Load[ "Misc" ][ "CritHack" ][ "ForceOnStreamers" ].asBool();
		Config->Misc->CritHack->Optimization = Load[ "Misc" ][ "CritHack" ][ "Optimization" ].asInt( );
		Config->Misc->CritHack->Indicator->Enabled = Load[ "Misc" ][ "CritHack" ][ "Indicator" ][ "Enabled" ].asBool( );
		Config->Misc->NoSpread->Enabled = Load[ "Misc" ][ "NoSpread" ][ "Enabled" ].asBool( );
		Config->Misc->NoSpread->Indicator = Load[ "Misc" ][ "NoSpread" ][ "Indicator" ].asBool( );

		Config->Misc->Matchmaking->RegionSelector->Enabled = Load[ "Misc" ][ "RegionSelector" ][ "Enabled" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->EUEast = Load[ "Misc" ][ "RegionSelector" ][ "EUEast" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->EUWest = Load[ "Misc" ][ "RegionSelector" ][ "EUWest" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->Asia = Load[ "Misc" ][ "RegionSelector" ][ "Asia" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->MiddleEast = Load[ "Misc" ][ "RegionSelector" ][ "MiddleEast" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->HongKong = Load[ "Misc" ][ "RegionSelector" ][ "HongKong" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->Japan = Load[ "Misc" ][ "RegionSelector" ][ "Japan" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->India = Load[ "Misc" ][ "RegionSelector" ][ "India" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->Australia = Load[ "Misc" ][ "RegionSelector" ][ "Australia" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->Africa = Load[ "Misc" ][ "RegionSelector" ][ "Africa" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->Chile = Load[ "Misc" ][ "RegionSelector" ][ "Chile" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->Peru = Load[ "Misc" ][ "RegionSelector" ][ "Peru" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->Brazil = Load[ "Misc" ][ "RegionSelector" ][ "Brazil" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->USEast = Load[ "Misc" ][ "RegionSelector" ][ "USEast" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->USNorthWest = Load[ "Misc" ][ "RegionSelector" ][ "USNorthWest" ].asBool( );
		Config->Misc->Matchmaking->RegionSelector->USSouthWest = Load[ "Misc" ][ "RegionSelector" ][ "USSouthWest" ].asBool( );

		Config->Misc->Matchmaking->AutoAccept = Load[ "Misc" ][ "MatchMaking" ][ "AutoAccept" ].asBool( );
		Config->Misc->Matchmaking->PreventAutoJoin = Load[ "Misc" ][ "MatchMaking" ][ "PreventAutoJoin" ].asBool( );

		Config->Misc->FakeLag->Enabled = Load[ "Misc" ][ "FakeLag" ][ "Enabled" ].asBool( );
		Config->Misc->FakeLag->DrawLag = Load[ "Misc" ][ "FakeLag" ][ "DrawLag" ].asBool( );
		Config->Misc->FakeLag->LagModel = Load[ "Misc" ][ "FakeLag" ][ "LagModel" ].asBool( );
		Config->Misc->FakeLag->LagOutline = Load[ "Misc" ][ "FakeLag" ][ "LagOutline" ].asBool( );
		Config->Misc->FakeLag->Value = Load[ "Misc" ][ "FakeLag" ][ "Value" ].asInt( );
		Config->Misc->FakeLag->FakeLagKey = Load[ "Misc" ][ "FakeLag" ][ "FakeLagKey" ].asInt( );
		Config->Misc->FakeLag->BreakLagComp = Load[ "Misc" ][ "FakeLag" ][ "BreakLagComp" ].asBool( );
		Config->Misc->FakeLag->BreakMoving = Load[ "Misc" ][ "FakeLag" ][ "BreakMoving" ].asBool( );
		Config->Misc->FakeLag->BreakInAir = Load[ "Misc" ][ "FakeLag" ][ "BreakInAir" ].asBool( );
		Config->Misc->FakeLag->UnchokeOnAttack = Load[ "Misc" ][ "FakeLag" ][ "UnchokeOnAttack" ].asBool( );
		Config->Misc->FakeDuck->Enabled = Load[ "Misc" ][ "FakeDuck" ][ "Enabled" ].asBool( );
		Config->Misc->FakeDuck->FakeDuckKey = Load[ "Misc" ][ "FakeDuck" ][ "FakeDuckKey" ].asInt( );

		Config->Misc->AntiAim->Enabled = Load[ "Misc" ][ "AntiAim" ][ "Enabled" ].asBool( );
		Config->Misc->AntiAim->DrawFake = Load[ "Misc" ][ "AntiAim" ][ "DrawFake" ].asBool( );
		Config->Misc->AntiAim->FakeModel = Load[ "Misc" ][ "AntiAim" ][ "FakeModel" ].asBool( );
		Config->Misc->AntiAim->FakeOutline = Load[ "Misc" ][ "AntiAim" ][ "FakeOutline" ].asBool( );
		Config->Misc->AntiAim->RageActive = Load[ "Misc" ][ "AntiAim" ][ "RageActive" ].asBool( );
		Config->Misc->AntiAim->ToggleKey = Load[ "Misc" ][ "AntiAim" ][ "ToggleKey" ].asInt( );
		Config->Misc->AntiAim->LegitYaw = Load[ "Misc" ][ "AntiAim" ][ "LegitYaw" ].asInt( );
		Config->Misc->AntiAim->CycleKey = Load[ "Misc" ][ "AntiAim" ][ "CycleKey" ].asInt( );
		Config->Misc->AntiAim->FakeYaw = Load[ "Misc" ][ "AntiAim" ][ "FakeYaw" ].asInt( );
		Config->Misc->AntiAim->FakeMode = Load[ "Misc" ][ "AntiAim" ][ "FakeMode" ].asInt( );
		Config->Misc->AntiAim->FakeRotate = Load[ "Misc" ][ "AntiAim" ][ "FakeRotate" ].asInt( );
		Config->Misc->AntiAim->FakeDelta = Load[ "Misc" ][ "AntiAim" ][ "FakeDelta" ].asInt( );
		Config->Misc->AntiAim->FakeSpin = Load[ "Misc" ][ "AntiAim" ][ "FakeSpin" ].asInt( );
		Config->Misc->AntiAim->FakeAngle1 = Load[ "Misc" ][ "AntiAim" ][ "FakeAngle1" ].asInt( );
		Config->Misc->AntiAim->FakeAngle2 = Load[ "Misc" ][ "AntiAim" ][ "FakeAngle2" ].asInt( );
		Config->Misc->AntiAim->RealYaw = Load[ "Misc" ][ "AntiAim" ][ "RealYaw" ].asInt( );
		Config->Misc->AntiAim->RealMode = Load[ "Misc" ][ "AntiAim" ][ "RealMode" ].asInt( );
		Config->Misc->AntiAim->RealRotate = Load[ "Misc" ][ "AntiAim" ][ "RealRotate" ].asInt( );
		Config->Misc->AntiAim->RealDelta = Load[ "Misc" ][ "AntiAim" ][ "RealDelta" ].asInt( );
		Config->Misc->AntiAim->RealSpin = Load[ "Misc" ][ "AntiAim" ][ "RealSpin" ].asInt( );
		Config->Misc->AntiAim->RealAngle1 = Load[ "Misc" ][ "AntiAim" ][ "RealAngle1" ].asInt( );
		Config->Misc->AntiAim->RealAngle2 = Load[ "Misc" ][ "AntiAim" ][ "RealAngle2" ].asInt( );
		Config->Misc->AntiAim->Pitch = Load[ "Misc" ][ "AntiAim" ][ "Pitch" ].asInt( );
		Config->Misc->AntiAim->FakePitch = Load[ "Misc" ][ "AntiAim" ][ "FakePitch" ].asBool( );
		Config->Misc->AntiAim->RealPitch = Load[ "Misc" ][ "AntiAim" ][ "RealPitch" ].asInt( );

		Config->AutoVacc->Enabled = Load[ "AutoVacc" ][ "Enabled" ].asBool( );
		Config->AutoVacc->LogReasons = Load[ "AutoVacc" ][ "LogReasons" ].asBool( );
		Config->AutoVacc->AutoPopExplosive = Load[ "AutoVacc" ][ "AutoPopExplosive" ].asBool( );
		Config->AutoVacc->AutoPopCrossbow = Load[ "AutoVacc" ][ "AutoPopCrossbow" ].asBool( );
		Config->AutoVacc->AutoPopHigherHealthHeavies = Load[ "AutoVacc" ][ "AutoPopHigherHealthHeavies" ].asBool( );
		Config->AutoVacc->AutoPopHeavies = Load[ "AutoVacc" ][ "AutoPopHeavies" ].asBool( );
		Config->AutoVacc->IgnoreFriendHeavies = Load[ "AutoVacc" ][ "IgnoreFriendHeavies" ].asBool( );
		Config->AutoVacc->AutoPopHeadshot = Load[ "AutoVacc" ][ "AutoPopHeadshot" ].asBool( );
		Config->AutoVacc->AutoPopShotgun = Load[ "AutoVacc" ][ "AutoPopShotgun" ].asBool( );
		Config->AutoVacc->AutoPopMinHealth = Load[ "AutoVacc" ][ "AutoPopMinHealth" ].asFloat( );
		Config->AutoVacc->HeavyDistance = Load[ "AutoVacc" ][ "HeavyDistance" ].asFloat( );

		Config->AutoMedic->Enabled = Load[ "AutoMedic" ][ "Enabled" ].asBool( );
		Config->AutoMedic->LockTarget = Load[ "AutoMedic" ][ "LockTarget" ].asInt( );
		Config->AutoMedic->TargetLock = Load[ "AutoMedic" ][ "TargetLock" ].asBool( );

		Config->OtherAuto->Enabled = Load["OtherAuto"]["Enabled"].asBool();
		Config->OtherAuto->AutoAirblast = Load["OtherAuto"]["AutoAirblast"].asBool();
		Config->OtherAuto->AutoAirblastPath = Load["OtherAuto"]["AutoAirblastPath"].asBool();
		Config->OtherAuto->BlastStickies = Load["OtherAuto"]["BlastStickies"].asBool();
		Config->OtherAuto->BlastArrows = Load["OtherAuto"]["BlastArrows"].asBool();
		Config->OtherAuto->BlastExplosive = Load["OtherAuto"]["BlastExplosive"].asBool();
		Config->OtherAuto->AutoDetonation = Load["OtherAuto"]["AutoDetonation"].asBool();
		Config->OtherAuto->TargetBonk = Load["OtherAuto"]["TargetBonk"].asBool();
		Config->OtherAuto->TargetBuildings = Load["OtherAuto"]["TargetBuildings"].asBool();
		Config->OtherAuto->TargetFriends = Load["OtherAuto"]["TargetFriends"].asBool();
		Config->OtherAuto->TargetPlayers = Load["OtherAuto"]["TargetPlayers"].asBool();
		Config->OtherAuto->TargetStickies = Load["OtherAuto"]["TargetStickies"].asBool();
		Config->OtherAuto->TargetUber = Load["OtherAuto"]["TargetUber"].asBool();
		Config->OtherAuto->TargetVacc = Load["OtherAuto"]["TargetVacc"].asBool();
		Config->OtherAuto->AutoDetonationMode = Load["OtherAuto"]["AutoDetonationMode"].asInt();

		if ( G->Authentication->IsDebugAllowed )
		{
			Config->Debug->EnableInfo = Load[ "Debug" ][ "Enabled" ].asBool( );
		}

		Visuals->DTIndicatorX = Load["Indicator" ]["DT"]["X"].asInt( );
		Visuals->DTIndicatorY = Load[ "Indicator" ][ "DT" ][ "Y" ].asInt( );
		Visuals->CritIndicatorX = Load[ "Indicator" ][ "Crits" ][ "X" ].asInt( );
		Visuals->CritIndicatorY = Load[ "Indicator" ][ "Crits" ][ "Y" ].asInt( );

		LOADCOLOR( MenuOutline );
		LOADCOLOR( MenuBackground );
		//LOADCOLOR( MenuForeground );
		LOADCOLOR( MenuBorders );
		//LOADCOLOR( MenuAccent );
		LOADCOLOR( MenuFade1 );
		LOADCOLOR( MenuFade2 );
		LOADCOLOR( MenuShine );
		LOADCOLOR( DTFade1 );
		LOADCOLOR( DTFade2 );

		LOADCOLOR( ESPPlayerRed );
		LOADCOLOR( ESPPlayerBlu );
		LOADCOLOR( ESPPlayerEnemy );
		LOADCOLOR( ESPPlayerTeam );
		LOADCOLOR( ESPPlayerLocal );
		LOADCOLOR( ESPPlayerFriend );

		LOADCOLOR( ESPBuildingRed );
		LOADCOLOR( ESPBuildingBlu );
		LOADCOLOR( ESPBuildingEnemy );
		LOADCOLOR( ESPBuildingTeam );
		LOADCOLOR( ESPBuildingLocal );
		LOADCOLOR( ESPBuildingFriend );

		LOADCOLOR( ESPWorldHealth );
		LOADCOLOR( ESPWorldAmmo );
		LOADCOLOR( ESPWorldBomb );
		LOADCOLOR( ESPWorldMoney );
		LOADCOLOR( ESPWorldPowerup );
		LOADCOLOR( ESPWorldRed );
		LOADCOLOR( ESPWorldBlu );
		LOADCOLOR( ESPWorldEnemy );
		LOADCOLOR( ESPWorldTeam );
		LOADCOLOR( ESPWorldLocal );
		LOADCOLOR( ESPWorldFriend );

		LOADCOLOR( ModelsPlayerRed );
		LOADCOLOR( ModelsPlayerBlu );
		LOADCOLOR( ModelsPlayerEnemy );
		LOADCOLOR( ModelsPlayerTeam );
		LOADCOLOR( ModelsPlayerLocal );
		LOADCOLOR( ModelsPlayerFriend );
		LOADCOLOR( ModelsPlayerRedUnoccluded );
		LOADCOLOR( ModelsPlayerBluUnoccluded );
		LOADCOLOR( ModelsPlayerEnemyUnoccluded );
		LOADCOLOR( ModelsPlayerTeamUnoccluded );
		LOADCOLOR( ModelsPlayerRedOccluded );
		LOADCOLOR( ModelsPlayerBluOccluded );
		LOADCOLOR( ModelsPlayerEnemyOccluded );
		LOADCOLOR( ModelsPlayerTeamOccluded );

		LOADCOLOR( ModelsBuildingRed );
		LOADCOLOR( ModelsBuildingLocal );
		LOADCOLOR( ModelsBuildingFriend );
		LOADCOLOR( ModelsBuildingBlu );
		LOADCOLOR( ModelsBuildingEnemy );
		LOADCOLOR( ModelsBuildingTeam );
		LOADCOLOR( ModelsBuildingRedUnoccluded );
		LOADCOLOR( ModelsBuildingBluUnoccluded );
		LOADCOLOR( ModelsBuildingEnemyUnoccluded );
		LOADCOLOR( ModelsBuildingTeamUnoccluded );
		LOADCOLOR( ModelsBuildingRedOccluded );
		LOADCOLOR( ModelsBuildingBluOccluded );
		LOADCOLOR( ModelsBuildingEnemyOccluded );
		LOADCOLOR( ModelsBuildingTeamOccluded );

		LOADCOLOR( ModelsWorldHealth );
		LOADCOLOR( ModelsWorldLocal );
		LOADCOLOR( ModelsWorldFriend );
		LOADCOLOR( ModelsWorldAmmo );
		LOADCOLOR( ModelsWorldBomb );
		LOADCOLOR( ModelsWorldMoney );
		LOADCOLOR( ModelsWorldPowerup );
		LOADCOLOR( ModelsWorldRed );
		LOADCOLOR( ModelsWorldBlu );
		LOADCOLOR( ModelsWorldEnemy );
		LOADCOLOR( ModelsWorldTeam );
		LOADCOLOR( ModelsWorldHealthUnoccluded );
		LOADCOLOR( ModelsWorldAmmoUnoccluded );
		LOADCOLOR( ModelsWorldBombUnoccluded );
		LOADCOLOR( ModelsWorldMoneyUnoccluded );
		LOADCOLOR( ModelsWorldPowerupUnoccluded );
		LOADCOLOR( ModelsWorldRedUnoccluded );
		LOADCOLOR( ModelsWorldBluUnoccluded );
		LOADCOLOR( ModelsWorldEnemyUnoccluded );
		LOADCOLOR( ModelsWorldTeamUnoccluded );
		LOADCOLOR( ModelsWorldHealthOccluded );
		LOADCOLOR( ModelsWorldAmmoOccluded );
		LOADCOLOR( ModelsWorldBombOccluded );
		LOADCOLOR( ModelsWorldMoneyOccluded );
		LOADCOLOR( ModelsWorldPowerupOccluded );
		LOADCOLOR( ModelsWorldRedOccluded );
		LOADCOLOR( ModelsWorldBluOccluded );
		LOADCOLOR( ModelsWorldEnemyOccluded );
		LOADCOLOR( ModelsWorldTeamOccluded );

		LOADCOLOR( OutlinesPlayerRed );
		LOADCOLOR( OutlinesPlayerBlu );
		LOADCOLOR( OutlinesPlayerEnemy );
		LOADCOLOR( OutlinesPlayerTeam );
		LOADCOLOR( OutlinesPlayerLocal );
		LOADCOLOR( OutlinesPlayerFriend);

		LOADCOLOR( OutlinesBuildingRed );
		LOADCOLOR( OutlinesBuildingFriend );
		LOADCOLOR( OutlinesBuildingLocal );
		LOADCOLOR( OutlinesBuildingBlu );
		LOADCOLOR( OutlinesBuildingEnemy );
		LOADCOLOR( OutlinesBuildingTeam );

		LOADCOLOR( OutlinesWorldHealth );
		LOADCOLOR( OutlinesWorldLocal );
		LOADCOLOR( OutlinesWorldFriend );
		LOADCOLOR( OutlinesWorldAmmo );
		LOADCOLOR( OutlinesWorldBomb );
		LOADCOLOR( OutlinesWorldMoney );
		LOADCOLOR( OutlinesWorldPowerup );
		LOADCOLOR( OutlinesWorldRed );
		LOADCOLOR( OutlinesWorldBlu );
		LOADCOLOR( OutlinesWorldEnemy );
		LOADCOLOR( OutlinesWorldTeam );

		LOADCOLOR( WorldColor );
		LOADCOLOR( HandsColor );
		LOADCOLOR( HandsOverlayColor );
		LOADCOLOR( HitboxColor );
		LOADCOLOR( BoundingBoxColor );
		LOADCOLOR( ProjectileLineColor );

		LOADCOLOR( BeamsLocal );
		LOADCOLOR( BeamsFriend );
		LOADCOLOR( BeamsEnemy );
		LOADCOLOR( BeamsTeam );
		LOADCOLOR( BeamsRed );
		LOADCOLOR( BeamsBlu );

		LOADCOLOR( FakeModel );
		LOADCOLOR( FakeGlow );
		LOADCOLOR( LagModel );
		LOADCOLOR( LagGlow );

		Visuals->UpdateWorldModulation( );
		Input.close( );
	}
}