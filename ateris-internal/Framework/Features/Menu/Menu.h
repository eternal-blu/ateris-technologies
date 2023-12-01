#pragma once
#include "Input/Input.h"
#include "../../Utils/DrawTools/DrawTools.h"
#include "../../SDK/Math/Math.h"
#include "Config.h"

#include "JSON/json.h"
#include "../../Hooks/HookList.h"
#include "../../../Globals.h"

#include <fstream>
#include <string>
#include <filesystem>

class CMenu
{
	enum EMenuTabs {
		TAB_AIM = 0,
		TAB_VISUALS,
		TAB_MISC,
		TAB_HVH,
		TAB_AUTO,
		TAB_PLAYERLIST,
		TAB_SKINCHANGER,
		TAB_DEBUG
	};

	enum EAimbotTabs {
		SUBTAB_AIMBOT = 0,
		SUBTAB_AUTOMATIONS,
	};

	enum EVisualsTabs {
		SUBTAB_ESP = 0,
		SUBTAB_MODELS,
		SUBTAB_MODELS2,
		SUBTAB_OUTLINES,
		SUBTAB_MISC_,
	};

	enum EHvHTabs {
		SUBTAB_ANTIAIM,
		SUBTAB_EXPLOITS
	};

	Rect_t WindowSize = { 200, 200, 503, 430 };

	int ClippingBackupL {}, ClippingBackupR {}, ClippingBackupT {}, ClippingBackupB {};
	bool ClippingBackupDisabled {};

	EMenuTabs CurrentMenuTab = TAB_AIM;
	EAimbotTabs CurrentAimSubtab = SUBTAB_AIMBOT;
	EVisualsTabs CurrentVisualsSubtab = SUBTAB_ESP;
	EHvHTabs CurrentHvHSubtab = SUBTAB_ANTIAIM;

	Rect_t LastTabPos {};
	Rect_t LastSubtabPos {};
	Rect_t LastGroupPos {};
	int LowestGroupPos = 0;

public:
	bool IsComboboxOpen = false;
	bool IsMultiComboboxOpen = false;
	bool IsColorPickerOpen = false;
private:
	bool IsRenderingGroup = false;
	int LastGroupY = 0;
	int PreviousX = 0;
	int PreviousY = 0;
	bool MenuTab( const std::string& label, bool active );
	bool MenuSubtab( const std::string& label, bool active );
	void MenuGroupStart( int offset = 0 );

	void PushGroupX( int x );
	void PopGroupX( bool right );
	void PushGroupY( int y );
	void PopGroupY( bool up );
	void MenuSpace( int y = 5 );

	void MenuGroup( const std::string& label, bool unstable = false );
	bool Button( const std::string& label, std::function<void( )> callback, int width = 90 );
	bool Button( const std::string& label, int width = 90 );
	bool CheckBox( const std::string& label, bool& var );
	bool ToggleableText( const std::wstring& label, bool& var, const Color& selected );
	bool Text( const std::string& label, const Color& text );
	bool CheckBoxKey( const std::string& label, bool& var, int& key );

	struct CColorInfo
	{
		Color* Col = nullptr;
		Rect_t Area = { };
		float Time = 0.f;
		float HSV[ 3 ] = { };
		float Clipboard[ 3 ] = { };
	};
	CColorInfo* ColorInfo = new CColorInfo;
	bool CheckBoxColor( const std::string& label, bool& var, Color& col );
	bool ColorPicker( const std::string& label, Color& col );
	bool UpdateColor( );

	bool Copied = false;

	struct CSliderIntInfo
	{
		int* Var = nullptr;
		Rect_t Area = { };
		float Time = 0.f;
	};
public:
	CSliderIntInfo* SliderIntInfo = new CSliderIntInfo;
private:
	bool SliderInt( const std::string& label, int& var, int min, int max, int width = 90, int step = 1, const char* format = "%i" );

	struct CSliderFloatInfo
	{
		float* Var = nullptr;
		Rect_t Area = { };
		float Time = 0.f;
	};
public:
	CSliderFloatInfo* SliderFloatInfo = new CSliderFloatInfo;
private:
	bool SliderFloat( const std::string& label, float& var, float min, float max, int width = 90, float step = 1.f, const char* format = "%.2f" );
	bool KeyBind(const std::string& label, int& var);
	bool TextBox( const std::string& label, std::string& var, int width = 90 );

	struct CComboInfo
	{
		int* Var = nullptr;
		std::vector< const char* > Items = { };
		Rect_t Area = { };
		float Time = 0.f;
	};
	CComboInfo* ComboInfo = new CComboInfo;
	bool ComboBox( const std::string& label, int& var, std::vector< const char* > items, int width = 120 );
	bool UpdateCombo( );

	struct CMultiComboInfo
	{
		bool* Var = nullptr;
		std::vector< std::pair<const char*, bool*> > Items = { };
		Rect_t Area = { };
		float Time = 0.f;
	};
	CMultiComboInfo* MultiComboInfo = new CMultiComboInfo;

	bool MultiComboBox( const std::string& label, std::vector< std::pair<const char*, bool*> > items );
	bool UpdateMultiCombo( );

	void SaveConfig( const char* name );
	void LoadConfig( const char* name );

	void RenderWindow( const std::string& label, const Rect_t& size, const Color& background, const Color& Edge1, const Color& Edge2 );
	void EnableClipping( const Rect_t& size );
	void DisableClipping( );

	void RenderPlayer( int idx, const PlayerInfo_t& info );

	std::string output = { };
	int TestCombo = 0;
	int ConfigSelected {};

	std::vector<std::string> ConfigList = { };
	bool test = false, test1 = false;
public:
	void SavePlayer( int FriendsID, const PriorityPlayer_t& Player );

	std::string SelectedSound = "";
	std::string SelectedDevice = "";
	std::string SoundFolder = { };
	std::string ConfigPath {};
	bool IsOpen = false;
	bool IsTyping = false;
	int AterisLogo18px = 0;
	int AterisLogo28px = 0;
	int ComboArrow = 0;
	int MenuBackground = 0;
	int Accent1Left = 0;
	int Accent1Right = 0;
	int Accent2Left = 0;
	int Accent2Right = 0;

	void Accent( );
	void Init( );
	void Run( );
	void Reset( );
	void RenderSkinChanger( );
};

inline CMenu* Menu = new CMenu;