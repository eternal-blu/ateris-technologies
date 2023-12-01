#pragma once
#include "Entities.h"
#include "EngineTrace.h"

class CUserCmd
{
public:
	virtual ~CUserCmd( ) {}

	void Reset( )
	{
		command_number = 0;
		tick_count = 0;
		viewangles = { 0.0f, 0.0f, 0.0f };
		forwardmove = 0.0f;
		sidemove = 0.0f;
		upmove = 0.0f;
		buttons = 0;
		impulse = 0;
		weaponselect = 0;
		weaponsubtype = 0;
		random_seed = 0;
		mousedx = 0;
		mousedy = 0;
		hasbeenpredicted = false;
	}

	CUserCmd& operator =( const CUserCmd& src )
	{
		if ( this == &src )
			return *this;

		command_number = src.command_number;
		tick_count = src.tick_count;
		viewangles = src.viewangles;
		forwardmove = src.forwardmove;
		sidemove = src.sidemove;
		upmove = src.upmove;
		buttons = src.buttons;
		impulse = src.impulse;
		weaponselect = src.weaponselect;
		weaponsubtype = src.weaponsubtype;
		random_seed = src.random_seed;
		mousedx = src.mousedx;
		mousedy = src.mousedy;
		hasbeenpredicted = src.hasbeenpredicted;

		return *this;
	}

	void MakeInert( void )
	{
		viewangles = { 0.0f, 0.0f, 0.0f };
		forwardmove = 0.f;
		sidemove = 0.f;
		upmove = 0.f;
		buttons = 0;
		impulse = 0;
	}

	int	command_number;
	int	tick_count;
	Vector viewangles;
	float forwardmove;
	float sidemove;
	float upmove;
	int	buttons;
	unsigned char impulse;
	int	weaponselect;
	int	weaponsubtype;
	int	random_seed;
	short mousedx;
	short mousedy;
	bool hasbeenpredicted;
};

enum
{
	WL_NotInWater = 0,
	WL_Feet,
	WL_Waist,
	WL_Eyes
};

enum PLAYER_ANIM;
enum struct soundlevel_t;
class IPhysicsSurfaceProps;
using EntityHandle_t = CBaseHandle;
class CBaseEntity;
class CGameTrace;

class IMoveHelper
{
public:
	/*virtual	char const* GetName( EntityHandle_t handle ) const = 0;
	virtual void ResetTouchList( void ) = 0;
	virtual bool AddToTouched( const CGameTrace& tr, const Vector& impactvelocity ) = 0;
	virtual void ProcessImpacts( void ) = 0;*/
	void ProcessImpacts( )
	{
		return Memory->GetVFunc<void( __thiscall* )( IMoveHelper* )>( this, 3u )( this );
	}
	/*virtual void Con_NPrintf( int idx, char const* fmt, ... ) = 0;
	virtual void StartSound( const Vector& origin, int channel, char const* sample, float volume, soundlevel_t soundlevel, int fFlags, int pitch ) = 0;
	virtual void StartSound( const Vector& origin, const char* soundname ) = 0;
	virtual void PlaybackEventFull( int flags, int clientindex, unsigned short eventindex, float delay, Vector& origin, Vector& angles, float fparam1, float fparam2, int iparam1, int iparam2, int bparam1, int bparam2 ) = 0;
	virtual bool PlayerFallingDamage( void ) = 0;
	virtual void PlayerSetAnimation( PLAYER_ANIM playerAnim ) = 0;
	virtual IPhysicsSurfaceProps* GetSurfaceProps( void ) = 0;
	virtual bool IsWorldEntity( const CBaseHandle& handle ) = 0;
	virtual ~IMoveHelper( ) {}*/

	//void SetHost( CBaseEntity* host );
};

struct StartSoundParams_t
{
	bool staticsound;
	int	userdata;
	int	soundsource;
	int	entchannel;
	void* Sfx; //CSfxTable
	Vector			origin;
	Vector			direction;
	bool			UpdatePositions;
	float			vol;
	soundlevel_t	soundlevel;
	int				flags;
	int				pitch;
	int				specialdsp;
	bool			fromserver;
	float			delay;
	int				speakerentity;
	bool			suppressrecording;
	int				initialStreamPosition;
};

class CMoveData
{
public:
	bool FirstRunOfFunctions : 1;
	bool bGameCodeMovedPlayer : 1;
	EntityHandle_t PlayerHandle;
	int ImpulseCommand;
	QAngle ViewAngles;
	QAngle AbsViewAngles;
	int Buttons;
	int OldButtons;
	float ForwardMove;
	float OldForwardMove;
	float SideMove;
	float UpMove;
	float MaxSpeed;
	float ClientMaxSpeed;
	Vector Velocity;
	QAngle Angles;
	QAngle OldAngles;
	float StepHeight;
	Vector WishVel;
	Vector JumpVel;
	Vector ConstraintCenter;
	float ConstraintRadius;
	float ConstraintWidth;
	float ConstraintSpeedFactor;
	Vector AbsOrigin;
};

class IPrediction
{
public:
	virtual ~IPrediction( void ) {};
	virtual void Init( void ) = 0;
	virtual void Shutdown( void ) = 0;
	virtual void Update( int startframe, bool validframe, int incoming_acknowledged, int outgoing_command ) = 0;
	virtual void PreEntityPacketReceived( int commands_acknowledged, int current_world_update_packet ) = 0;
	virtual void PostEntityPacketReceived( void ) = 0;
	virtual void PostNetworkDataReceived( int commands_acknowledged ) = 0;
	virtual void OnReceivedUncompressedPacket( void ) = 0;
	virtual void GetViewOrigin( Vector& org ) = 0;
	virtual void SetViewOrigin( Vector& org ) = 0;
	virtual void GetViewAngles( QAngle& ang ) = 0;
	virtual void SetViewAngles( QAngle& ang ) = 0;
	virtual void GetLocalViewAngles( QAngle& ang ) = 0;
	virtual void SetLocalViewAngles( QAngle& ang ) = 0;
};

class CPrediction : public IPrediction
{
public:
	virtual ~CPrediction( );
	virtual void Init( );
	virtual void Shutdown( );
	virtual void Update( int startframe, bool validframe, int incoming_acknowledged, int outgoing_command );
	virtual void OnReceivedUncompressedPacket( );
	virtual void PreEntityPacketReceived( int commands_acknowledged, int current_world_update_packet );
	virtual void PostEntityPacketReceived( );
	virtual void PostNetworkDataReceived( int commands_acknowledged );
	virtual bool InPrediction( );
	virtual bool IsFirstTimePredicted( );
	virtual int	GetIncomingPacketNumber( );
	virtual void GetViewOrigin( Vector& org );
	virtual void SetViewOrigin( Vector& org );
	virtual void GetViewAngles( QAngle& ang );
	virtual void SetViewAngles( QAngle& ang );
	virtual void GetLocalViewAngles( QAngle& ang );
	virtual void SetLocalViewAngles( QAngle& ang );
	virtual void RunCommand( CBaseEntity* player, CUserCmd* ucmd, IMoveHelper* moveHelper );
	virtual void SetupMove( CBaseEntity* player, CUserCmd* ucmd, IMoveHelper* pHelper, CMoveData* move );
	virtual void FinishMove( CBaseEntity* player, CUserCmd* ucmd, CMoveData* move );
	virtual void SetIdealPitch( CBaseEntity* player, const Vector& origin, const Vector& angles, const Vector& viewheight );
	virtual void _Update( bool received_new_world_update, bool validframe, int incoming_acknowledged, int outgoing_command );

	uint32_t LastGround;
	bool bInPrediction;
	bool FirstTimePredicted;
	bool OldCLPredictValue;
	bool EnginePaused;
	int PreviousStartFrame;
	int CommandsPredicted;
	int ServerCommandsAcknowledged;
	int PreviousAckHadErrors;
	int IncomingPacketNumber;
	float IdealPitch;
};