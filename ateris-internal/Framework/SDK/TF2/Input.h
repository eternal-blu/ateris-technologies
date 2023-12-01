#pragma once
#include "Prediction.h"
#include "Surface.h"

struct kbutton_t;

enum GameActionSet_t
{
	GAME_ACTION_SET_NONE = -1,
	GAME_ACTION_SET_MENUCONTROLS = 0,
	GAME_ACTION_SET_FPSCONTROLS,
	GAME_ACTION_SET_IN_GAME_HUD,
	GAME_ACTION_SET_SPECTATOR,
};

enum GameActionSetFlags_t
{
	GAME_ACTION_SET_FLAGS_NONE = 0,
	GAME_ACTION_SET_FLAGS_TAUNTING = ( 1 << 0 ),
};

struct CameraThirdData_t
{
	float	Pitch;
	float	Yaw;
	float	Dist;
	float	Lag;
	Vector	HullMin;
	Vector	HullMax;
};

enum ESteamControllerPadIInput;
class CBaseCombatWeapon;
class IInput
{
public:
	/*virtual	void		Init_All( void ) = 0;
	virtual void		Shutdown_All( void ) = 0;
	virtual int			GetButtonBits( int ) = 0;
	virtual void		CreateMove( int sequence_number, float input_sample_frametime, bool active ) = 0;
	virtual void		ExtraMouseSample( float frametime, bool active ) = 0;
	virtual bool		WriteUsercmdDeltaToBuffer( bf_write* buf, int from, int to, bool isnewcommand ) = 0;
	virtual void		EncodeUserCmdToBuffer( bf_write& buf, int slot ) = 0;
	virtual void		DecodeUserCmdFromBuffer( bf_read& buf, int slot ) = 0;
	virtual CUserCmd* GetUserCmd( int sequence_number ) = 0;
	virtual void		MakeWeaponSelection( CBaseCombatWeapon* weapon ) = 0;
	virtual float		KeyState( kbutton_t* key ) = 0;
	virtual void		stupid_shit(void) = 0;
	virtual kbutton_t* FindKey( const char* name ) = 0;
	virtual void		ControllerCommands( void ) = 0;
	virtual void		Joystick_Advanced( void ) = 0;
	virtual void		Joystick_SetSampleTime( float frametime ) = 0;
	virtual void		IN_SetSampleTime( float frametime ) = 0;
	virtual void		AccumulateMouse( void ) = 0;
	virtual void		ActivateMouse( void ) = 0;
	virtual void		DeactivateMouse( void ) = 0;
	virtual void		ClearStates( void ) = 0;
	virtual float		GetLookSpring( void ) = 0;
	virtual void		GetFullscreenMousePos( int* mx, int* my, int* unclampedx = 0, int* unclampedy = 0 ) = 0;
	virtual void		SetFullscreenMousePos( int mx, int my ) = 0;
	virtual void		ResetMouse( void ) = 0;
	virtual	float		GetLastForwardMove( void ) = 0;
	virtual	float		Joystick_GetForward( void ) = 0;
	virtual	float		Joystick_GetSide( void ) = 0;
	virtual	float		Joystick_GetPitch( void ) = 0;
	virtual	float		Joystick_GetYaw( void ) = 0;
	virtual void		CAM_Think( void ) = 0;
	virtual int			CAM_IsThirdPerson( void ) = 0;*/
	int CAM_IsThirdPerson( )
	{
		return Memory->GetVFunc<int( __thiscall* )( IInput* )>( this, 31u )( this );
	}

	void CAM_ToThirdPerson( )
	{
		return Memory->GetVFunc<void( __thiscall* )( IInput* )>( this, 32u )( this );
	}

	void CAM_ToFirstPerson( )
	{
		return Memory->GetVFunc<void( __thiscall* )( IInput* )>( this, 33u )( this );
	}
	/*virtual void		CAM_ToThirdPerson( void ) = 0;
	virtual void		CAM_ToFirstPerson( void ) = 0;
	virtual void		CAM_StartMouseMove( void ) = 0;
	virtual void		CAM_EndMouseMove( void ) = 0;
	virtual void		CAM_StartDistance( void ) = 0;
	virtual void		CAM_EndDistance( void ) = 0;
	virtual int			CAM_InterceptingMouse( void ) = 0;
	virtual void		CAM_ToOrthographic( ) = 0;
	virtual	bool		CAM_IsOrthographic( ) const = 0;
	virtual	void		CAM_OrthographicSize( float& w, float& h ) const = 0;
	virtual void		SetPreferredGameActionSet( GameActionSet_t action_set ) = 0;
	virtual GameActionSet_t GetPreferredGameActionSet( ) = 0;
	virtual void		SetGameActionSetFlags( GameActionSetFlags_t action_set_flags ) = 0;
	virtual void		LevelInit( void ) = 0;
	virtual void		ClearInputButton( int bits ) = 0;
	virtual	void		CAM_SetCameraThirdData( CameraThirdData_t* pCameraData, const QAngle& vecCameraOffset ) = 0;
	virtual void		CAM_CameraThirdThink( void ) = 0;
	virtual	bool		EnableJoystickMode( ) = 0;
	virtual bool		IsSteamControllerActive( ) = 0;*/
public:
	typedef struct
	{
		unsigned int AxisFlags;
		unsigned int AxisMap;
		unsigned int ControlMap;
	} joy_axis_t;

	void		DescribeJoystickAxis( char const* axis, joy_axis_t* mapping );
	char const* DescribeAxis( int index );

	enum
	{
		GAME_AXIS_NONE = 0,
		GAME_AXIS_FORWARD,
		GAME_AXIS_PITCH,
		GAME_AXIS_SIDE,
		GAME_AXIS_YAW,
		MAX_GAME_AXES
	};

	enum
	{
		CAM_COMMAND_NONE = 0,
		CAM_COMMAND_TOTHIRDPERSON = 1,
		CAM_COMMAND_TOFIRSTPERSON = 2
	};

	enum
	{
		MOUSE_ACCEL_THRESHHOLD1 = 0,	// if mouse moves > this many mickey's double it
		MOUSE_ACCEL_THRESHHOLD2,		// if mouse moves > this many mickey's double it a second time
		MOUSE_SPEED_FACTOR,				// 0 = disabled, 1 = threshold 1 enabled, 2 = threshold 2 enabled

		NUM_MOUSE_PARAMS,
	};

	// Has the mouse been initialized?
	bool		MouseInitialized;
	// Is the mosue active?
	bool		MouseActive;
	// Has the joystick advanced initialization been run?
	bool		JoystickAdvancedInit;
	// Used to support hotplugging by reinitializing the advanced joystick system when we toggle between some/none joysticks.
	bool		HadJoysticks;

	// Accumulated mouse deltas
	float		AccumulatedMouseXMovement;
	float		AccumulatedMouseYMovement;
	float		PreviousMouseXPosition;
	float		PreviousMouseYPosition;
	float		RemainingJoystickSampleTime;
	float		KeyboardSampleTime;

	// Flag to restore systemparameters when exiting
	bool		RestoreSPI;
	// Original mouse parameters
	int			OrigMouseParms[ NUM_MOUSE_PARAMS ];
	// Current mouse parameters.
	int			NewMouseParms[ NUM_MOUSE_PARAMS ];
	bool		CheckMouseParam[ NUM_MOUSE_PARAMS ];
	// Are the parameters valid
	bool		MouseParmsValid;
	// Joystick Axis data
	joy_axis_t Axes[ 6 ];
	// List of queryable keys
	void* Keys;

	// Is the 3rd person camera using the mouse?
	bool		CameraInterceptingMouse;
	// Are we in 3rd person view?
	bool		CameraInThirdPerson;
	// Should we move view along with mouse?
	bool		CameraMovingWithMouse;


	// Is the camera in distance moving mode?
	bool		CameraDistanceMove;
	// Old and current mouse position readings.
	int			CameraOldX;
	int			CameraOldY;
	int			CameraX;
	int			CameraY;

	// orthographic camera settings
	bool		CameraIsOrthographic;

	QAngle		PreviousViewAngles;

	float		LastForwardMove;

	float PreviousJoystickForward;
	float PreviousJoystickSide;
	float PreviousJoystickPitch;
	float PreviousJoystickYaw;

	// Steam controller stuff
	int ControllerType[ 1 + 8 ];

	GameActionSet_t PreferredGameActionSet;
	GameActionSetFlags_t GameActionSetFlags;

	bool SteamControllerGameActionsInitialized;

	CUserCmd* Commands;
	void* VerifiedCommands;

	CameraThirdData_t* CameraThirdData;

	inline CUserCmd* GetCmds( )
	{
		return *reinterpret_cast< CUserCmd** >( reinterpret_cast< uintptr_t >( this ) + 0xFC );
	}

};

enum AnalogCode_t
{
	ANALOG_CODE_INVALID = -1,
	MOUSE_X = 0,
	MOUSE_Y,
	MOUSE_XY, // invoked when either x or y changes
	MOUSE_WHEEL,
	ANALOG_CODE_LAST = 10,
};

enum ButtonCode_t
{
	BUTTON_CODE_INVALID = -1,
	BUTTON_CODE_NONE = 0,

	KEY_FIRST = 0,

	KEY_NONE = KEY_FIRST,
	KEY_0,
	KEY_1,
	KEY_2,
	KEY_3,
	KEY_4,
	KEY_5,
	KEY_6,
	KEY_7,
	KEY_8,
	KEY_9,
	KEY_A,
	KEY_B,
	KEY_C,
	KEY_D,
	KEY_E,
	KEY_F,
	KEY_G,
	KEY_H,
	KEY_I,
	KEY_J,
	KEY_K,
	KEY_L,
	KEY_M,
	KEY_N,
	KEY_O,
	KEY_P,
	KEY_Q,
	KEY_R,
	KEY_S,
	KEY_T,
	KEY_U,
	KEY_V,
	KEY_W,
	KEY_X,
	KEY_Y,
	KEY_Z,
	KEY_PAD_0,
	KEY_PAD_1,
	KEY_PAD_2,
	KEY_PAD_3,
	KEY_PAD_4,
	KEY_PAD_5,
	KEY_PAD_6,
	KEY_PAD_7,
	KEY_PAD_8,
	KEY_PAD_9,
	KEY_PAD_DIVIDE,
	KEY_PAD_MULTIPLY,
	KEY_PAD_MINUS,
	KEY_PAD_PLUS,
	KEY_PAD_ENTER,
	KEY_PAD_DECIMAL,
	KEY_LBRACKET,
	KEY_RBRACKET,
	KEY_SEMICOLON,
	KEY_APOSTROPHE,
	KEY_BACKQUOTE,
	KEY_COMMA,
	KEY_PERIOD,
	KEY_SLASH,
	KEY_BACKSLASH,
	KEY_MINUS,
	KEY_EQUAL,
	KEY_ENTER,
	KEY_SPACE,
	KEY_BACKSPACE,
	KEY_TAB,
	KEY_CAPSLOCK,
	KEY_NUMLOCK,
	KEY_ESCAPE,
	KEY_SCROLLLOCK,
	KEY_INSERT,
	KEY_DELETE,
	KEY_HOME,
	KEY_END,
	KEY_PAGEUP,
	KEY_PAGEDOWN,
	KEY_BREAK,
	KEY_LSHIFT,
	KEY_RSHIFT,
	KEY_LALT,
	KEY_RALT,
	KEY_LCONTROL,
	KEY_RCONTROL,
	KEY_LWIN,
	KEY_RWIN,
	KEY_APP,
	KEY_UP,
	KEY_LEFT,
	KEY_DOWN,
	KEY_RIGHT,
	KEY_F1,
	KEY_F2,
	KEY_F3,
	KEY_F4,
	KEY_F5,
	KEY_F6,
	KEY_F7,
	KEY_F8,
	KEY_F9,
	KEY_F10,
	KEY_F11,
	KEY_F12,
	KEY_CAPSLOCKTOGGLE,
	KEY_NUMLOCKTOGGLE,
	KEY_SCROLLLOCKTOGGLE,

	KEY_LAST = KEY_SCROLLLOCKTOGGLE,
	KEY_COUNT = KEY_LAST - KEY_FIRST + 1,

	// Mouse
	MOUSE_FIRST = KEY_LAST + 1,

	MOUSE_LEFT = MOUSE_FIRST,
	MOUSE_RIGHT,
	MOUSE_MIDDLE,
	MOUSE_4,
	MOUSE_5,
	MOUSE_WHEEL_UP,		// A fake button which is 'pressed' and 'released' when the wheel is moved up 
	MOUSE_WHEEL_DOWN,	// A fake button which is 'pressed' and 'released' when the wheel is moved down

	MOUSE_LAST = MOUSE_WHEEL_DOWN,
	MOUSE_COUNT = MOUSE_LAST - MOUSE_FIRST + 1,
};

class IInputSystem /*: public IAppSystem*/
{
public:
	/*virtual void AttachToWindow( void* hWnd ) = 0;
	virtual void DetachFromWindow( ) = 0;
	virtual void EnableInput( bool bEnable ) = 0;
	virtual void EnableMessagePump( bool bEnable ) = 0;
	virtual void PollInputState( ) = 0;
	virtual int GetPollTick( ) const = 0;
	virtual bool IsButtonDown( int code ) const = 0;*/
	bool IsButtonDown( int code )
	{
		return Memory->GetVFunc<bool( __thiscall* )( IInputSystem*, int )>( this, 11u )( this, code );
	}
	/*virtual int GetButtonPressedTick( ButtonCode_t code ) const = 0;
	virtual int GetButtonReleasedTick( ButtonCode_t code ) const = 0;
	virtual int GetAnalogValue( AnalogCode_t code ) const = 0;
	virtual int GetAnalogDelta( AnalogCode_t code ) const = 0;
	virtual int GetEventCount( ) const = 0;
	virtual const InputEvent_t* GetEventData( ) const = 0;
	virtual void PostUserEvent( const InputEvent_t& event ) = 0;
	virtual int GetJoystickCount( ) const = 0;
	virtual void EnableJoystickInput( int nJoystick, bool bEnable ) = 0;
	virtual void EnableJoystickDiagonalPOV( int nJoystick, bool bEnable ) = 0;
	virtual void SampleDevices( void ) = 0;
	virtual void SetRumble( float fLeftMotor, float fRightMotor, int userId ) = 0;
	virtual void StopRumble( void ) = 0;
	virtual void ResetInputState( ) = 0;*/
	void ResetInputState( )
	{
		return Memory->GetVFunc<void( __thiscall* )( IInputSystem* )>( this, 25u )( this );
	}
	/*virtual void SetPrimaryUserId( int userId ) = 0;
	virtual const char* ButtonCodeToString( int code ) const = 0;
	virtual const char* AnalogCodeToString( AnalogCode_t code ) const = 0;
	virtual ButtonCode_t StringToButtonCode( const char* pString ) const = 0;
	virtual AnalogCode_t StringToAnalogCode( const char* pString ) const = 0;
	virtual void SleepUntilInput( int nMaxSleepTimeMS = -1 ) = 0;
	virtual ButtonCode_t VirtualKeyToButtonCode( int nVirtualKey ) const = 0;
	virtual int ButtonCodeToVirtualKey( ButtonCode_t code ) const = 0;
	virtual ButtonCode_t ScanCodeToButtonCode( int lParam ) const = 0;
	virtual int GetPollCount( ) const = 0;
	virtual void SetCursorPosition( int x, int y ) = 0;
	virtual void* GetHapticsInterfaceAddress( ) const = 0;
	virtual void SetNovintPure( bool bPure ) = 0;
	virtual bool GetRawMouseAccumulators( int& accumX, int& accumY ) = 0;
	virtual void SetConsoleTextMode( bool bConsoleTextMode ) = 0;
	virtual void* SteamControllerInterface( ) = 0;
	virtual void* GetNumSteamControllersConnected( ) = 0;
	virtual bool IsSteamControllerActive( ) = 0;
	virtual bool IsSteamControllerConnected( ) = 0;
	virtual int GetSteamControllerIndexForSlot( int nSlot ) = 0;
	virtual bool GetRadialMenuStickValues( int nSlot, float& fX, float& fY ) = 0;
	virtual void ActivateSteamControllerActionSetForSlot( void* nSlot, GameActionSet_t eActionSet ) = 0;*/
};