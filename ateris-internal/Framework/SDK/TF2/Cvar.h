#pragma once
#include "Color.h"
#include "../../Utils/Memory/Memory.h"

class IConVar;
class CCommand;

//-----------------------------------------------------------------------------
// ConVar flags
//-----------------------------------------------------------------------------
// The default, no flags at all
#define FCVAR_NONE				0 

// Command to ConVars and ConCommands
// ConVar Systems
#define FCVAR_UNREGISTERED		(1<<0)	// If this is set, don't add to linked list, etc.
#define FCVAR_DEVELOPMENTONLY	(1<<1)	// Hidden in released products. Flag is removed automatically if ALLOW_DEVELOPMENT_CVARS is defined.
#define FCVAR_GAMEDLL			(1<<2)	// defined by the game DLL
#define FCVAR_CLIENTDLL			(1<<3)  // defined by the client DLL
#define FCVAR_HIDDEN			(1<<4)	// Hidden. Doesn't appear in find or autocomplete. Like DEVELOPMENTONLY, but can't be compiled out.

// ConVar only
#define FCVAR_PROTECTED			(1<<5)  // It's a server cvar, but we don't send the data since it's a password, etc.  Sends 1 if it's not bland/zero, 0 otherwise as value
#define FCVAR_SPONLY			(1<<6)  // This cvar cannot be changed by clients connected to a multiplayer server.
#define	FCVAR_ARCHIVE			(1<<7)	// set to cause it to be saved to vars.rc
#define	FCVAR_NOTIFY			(1<<8)	// notifies players when changed
#define	FCVAR_USERINFO			(1<<9)	// changes the client's info string
#define FCVAR_CHEAT				(1<<14) // Only useable in singleplayer / debug / multiplayer & sv_cheats

#define FCVAR_PRINTABLEONLY		(1<<10)  // This cvar's string cannot contain unprintable characters ( e.g., used for player name etc ).
#define FCVAR_UNLOGGED			(1<<11)  // If this is a FCVAR_SERVER, don't log changes to the log file / console if we are creating a log
#define FCVAR_NEVER_AS_STRING	(1<<12)  // never try to print that cvar

// It's a ConVar that's shared between the client and the server.
// At signon, the values of all such ConVars are sent from the server to the client (skipped for local
//  client, of course )
// If a change is requested it must come from the console (i.e., no remote client changes)
// If a value is changed while a server is active, it's replicated to all connected clients
#define FCVAR_REPLICATED		(1<<13)	// server setting enforced on clients, TODO rename to FCAR_SERVER at some time
#define FCVAR_DEMO				(1<<16)  // record this cvar when starting a demo file
#define FCVAR_DONTRECORD		(1<<17)  // don't record these command in demofiles
#define FCVAR_RELOAD_MATERIALS	(1<<20)	// If this cvar changes, it forces a material reload
#define FCVAR_RELOAD_TEXTURES	(1<<21)	// If this cvar changes, if forces a texture reload

#define FCVAR_NOT_CONNECTED		(1<<22)	// cvar cannot be changed by a client that is connected to a server
#define FCVAR_MATERIAL_SYSTEM_THREAD (1<<23)	// Indicates this cvar is read from the material system thread
#define FCVAR_ARCHIVE_XBOX		(1<<24) // cvar written to config.cfg on the Xbox

#define FCVAR_ACCESSIBLE_FROM_THREADS	(1<<25)	// used as a debugging tool necessary to check material system thread convars

#define FCVAR_SERVER_CAN_EXECUTE	(1<<28)// the server is allowed to execute this command on clients via ClientCommand/NET_StringCmd/CBaseClientState::ProcessStringCmd.
#define FCVAR_SERVER_CANNOT_QUERY	(1<<29)// If this is set, then the server is not allowed to query this cvar's value (via IServerPluginHelpers::StartQueryCvarValue).
#define FCVAR_CLIENTCMD_CAN_EXECUTE	(1<<30)	// IVEngineClient::ClientCmd is allowed to execute this command. 
											// Note: IVEngineClient::ClientCmd_Unrestricted can run any client command.
#define FCVAR_EXEC_DESPITE_DEFAULT	(1<<31) // -default causes a lot of commands to be ignored (but still be recorded as though they had run). 
											// This causes them to be executed anyways.

#define FCVAR_INTERNAL_USE		(1<<15)	// This var isn't archived, but is exposed to players--and its use is allowed in competitive play.
#define FCVAR_ALLOWED_IN_COMPETITIVE	(1<<18) // This convar can be changed in competitive (strict) settings mode even though it is not archived. Meant for one-offs like cl_showfps that are not user facing settings but are benign
// #define FCVAR_AVAILABLE			(1<<19)
// #define FCVAR_AVAILABLE			(1<<26)
// #define FCVAR_AVAILABLE			(1<<27)

#define FCVAR_MATERIAL_THREAD_MASK ( FCVAR_RELOAD_MATERIALS | FCVAR_RELOAD_TEXTURES | FCVAR_MATERIAL_SYSTEM_THREAD )	

typedef void ( *FnChangeCallback_t )( IConVar* var, const char* pOldValue, float flOldValue );

class IConVar
{
public:
	// Value set
	virtual void SetValue( const char* pValue ) = 0;
	virtual void SetValue( float flValue ) = 0;
	virtual void SetValue( int nValue ) = 0;
	virtual const char* GetName( void ) const = 0;
	virtual bool IsFlagSet( int nFlag ) const = 0;
};

#include <string>

class ConVar;
class CCommand;
class ConCommand;
class ConCommandBase;
struct characterset_t;

class IConCommandBaseAccessor
{
public:
	virtual bool RegisterConCommandBase( ConCommandBase* pVar ) = 0;
};

typedef void ( *FnCommandCallbackVoid_t )( void );
typedef void ( *FnCommandCallback_t )( const CCommand& command );

#define COMMAND_COMPLETION_MAXITEMS		64
#define COMMAND_COMPLETION_ITEM_LENGTH	64

typedef int  ( *FnCommandCompletionCallback )( const char* partial, char commands[ COMMAND_COMPLETION_MAXITEMS ][ COMMAND_COMPLETION_ITEM_LENGTH ] );

class ICommandCallback
{
public:
	virtual void CommandCallback( const CCommand& command ) = 0;
};

class ICommandCompletionCallback
{
public:
	virtual int  CommandCompletionCallback( const char* pPartial, void* commands ) = 0;
};

struct CVarDLLIdentifier_t;

class ConCommandBase
{
public:
	virtual ~ConCommandBase( void );
	virtual	bool IsCommand( void ) const;
	virtual bool IsFlagSet( int flag ) const;
	virtual void AddFlags( int flags );
	virtual const char* GetName( void ) const;
	virtual const char* GetHelpText( void ) const;
	virtual bool IsRegistered( void ) const;
	virtual CVarDLLIdentifier_t	GetDLLIdentifier( ) const;
	virtual void CreateBase( const char* pName, const char* pHelpString = 0, int flags = 0 );

	ConCommandBase* m_pNext;
	bool m_bRegistered;
	const char* m_pszName;
	const char* m_pszHelpString;
	int m_nFlags;

protected:
	static ConCommandBase* s_pConCommandBases;
	static IConCommandBaseAccessor* s_pAccessor;
};

class CCommand
{
public:
	enum
	{
		COMMAND_MAX_ARGC = 64,
		COMMAND_MAX_LENGTH = 512,
	};

	int	Argc;
	int	 Argv0Size;
	char ArgSBuffer[ COMMAND_MAX_LENGTH ];
	char ArgvBuffer[ COMMAND_MAX_LENGTH ];
	const char* Argv[ COMMAND_MAX_ARGC ];
};

class ConCommand : public ConCommandBase
{
public:
	typedef ConCommandBase BaseClass;
	virtual ~ConCommand( void );
	virtual	bool IsCommand( void ) const;
	virtual int AutoCompleteSuggest( const char* partial, void* commands );
	virtual bool CanAutoComplete( void );
	virtual void Dispatch( const CCommand& command );

private:
	union
	{
		FnCommandCallbackVoid_t CommandCallbackV1;
		FnCommandCallback_t FnCommandCallback;
		ICommandCallback* CommandCallback;
	};

	union
	{
		FnCommandCompletionCallback	CompletionCallback;
		ICommandCompletionCallback* CommandCompletionCallback;
	};

	bool HasCompletionCallback : 1;
	bool UsingNewCommandCallback : 1;
	bool UsingCommandCallbackInterface : 1;
};

class ConVar : public ConCommandBase, public IConVar
{
public:
	virtual ~ConVar( void );
	virtual bool IsFlagSet( int flag ) const;
	virtual const char* GetHelpText( void ) const;
	virtual bool IsRegistered( void ) const;
	virtual const char* GetName( void ) const;
	virtual void AddFlags( int flags );
	virtual	bool IsCommand( void ) const;
	virtual void SetValue( const char* value );
	virtual void SetValue( float value );
	virtual void SetValue( int value );
	virtual void InternalSetValue( const char* value );
	virtual void InternalSetFloatValue( float fNewValue, bool bForce = false );
	virtual void InternalSetIntValue( int nValue );
	virtual bool ClampValue( float& value );
	virtual void ChangeStringValue( const char* tempVal, float flOldValue );
	virtual void Init( );

	inline float GetFloat( void ) const;
	inline int GetInt( void ) const;
	inline bool	GetBool( ) const { return !!GetInt( ); }
	inline char const* GetString( void ) const;

	ConVar* Parent;
	const char* DefaultValue;
	char* String;
	int StringLength;
	float fValue;
	int iValue;
	bool HasMin;
	float MinVal;
	bool HasMax;
	float MaxVal;
	bool HasCompMin;
	float CompMinVal;
	bool HasCompMax;
	float CompMaxVal;
	bool CompetitiveRestrictions;
	FnChangeCallback_t ChangeCallback;
};

inline float ConVar::GetFloat( void ) const
{
	return Parent->fValue;
}

inline int ConVar::GetInt( void ) const
{
	return Parent->iValue;
}

inline const char* ConVar::GetString( void ) const
{
	if ( m_nFlags & FCVAR_NEVER_AS_STRING )
		return "FCVAR_NEVER_AS_STRING";

	return ( Parent->String ) ? Parent->String : "";
}

typedef void* ( *CreateInterfaceFn )( const char* pName, int* pReturnCode );
enum InitReturnVal_t
{
	INIT_FAILED = 0,
	INIT_OK,
	INIT_LAST_VAL,
};

class IAppSystem
{
public:
	virtual bool Connect( CreateInterfaceFn factory ) = 0;
	virtual void Disconnect( ) = 0;
	virtual void* QueryInterface( const char* pInterfaceName ) = 0;
	virtual InitReturnVal_t Init( ) = 0;
	virtual void Shutdown( ) = 0;
};

class IConsoleDisplayFunc
{
public:
	virtual void ColorPrint( const Color& clr, const char* pMessage ) = 0;
	virtual void Print( const char* pMessage ) = 0;
	virtual void DPrint( const char* pMessage ) = 0;
};

class ICvarQuery : public IAppSystem
{
public:
	virtual bool AreConVarsLinkable( const ConVar* child, const ConVar* parent ) = 0;
};

class ICvar : public IAppSystem
{
public:
	/*virtual CVarDLLIdentifier_t AllocateDLLIdentifier( ) = 0;
	virtual void RegisterConCommand( ConCommandBase* pCommandBase ) = 0;
	virtual void UnregisterConCommand( ConCommandBase* pCommandBase ) = 0;
	virtual void UnregisterConCommands( CVarDLLIdentifier_t id ) = 0;
	virtual const char* GetCommandLineValue( const char* pVariableName ) = 0;
	virtual ConCommandBase* FindCommandBase( const char* name ) = 0;
	virtual const ConCommandBase* FindCommandBase( const char* name ) const = 0;
	virtual ConVar* FindVar( const char* var_name ) = 0;*/
	ConVar* FindVar( const char* name )
	{
		return Memory->GetVFunc<ConVar * ( __thiscall* )( ICvar*, const char* )>( this, 12u )( this, name );
	}
	/*virtual const ConVar* FindVar( const char* var_name ) const = 0;
	virtual ConCommand* FindCommand( const char* name ) = 0;
	virtual const ConCommand* FindCommand( const char* name ) const = 0;
	virtual ConCommandBase* GetCommands( void ) = 0;
	virtual const ConCommandBase* GetCommands( void ) const = 0;
	virtual void InstallGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
	virtual void RemoveGlobalChangeCallback( FnChangeCallback_t callback ) = 0;
	virtual void CallGlobalChangeCallbacks( ConVar* var, const char* pOldString, float flOldValue ) = 0;
	virtual void InstallConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	virtual void RemoveConsoleDisplayFunc( IConsoleDisplayFunc* pDisplayFunc ) = 0;
	virtual void ConsoleColorPrintf( const Color& clr, const char* pFormat, ... ) const = 0;*/
	void ConsoleColorPrintf( const Color& clr, const char* Format, ... )
	{
		return Memory->GetVFunc< void( __cdecl* )( ICvar*, const Color&, const char*, ... )>( this, 23u )( this, clr, Format );
	}
	/*virtual void ConsolePrintf( const char* pFormat, ... ) const = 0;
	virtual void ConsoleDPrintf( const char* pFormat, ... ) const = 0;
	virtual void RevertFlaggedConVars( int nFlag ) = 0;
	virtual void InstallCVarQuery( ICvarQuery* pQuery ) = 0;
	virtual void SetMaxSplitScreenSlots( int nSlots ) = 0;
	virtual int	 GetMaxSplitScreenSlots( ) const = 0;
	virtual void AddSplitScreenConVars( ) = 0;
	virtual void RemoveSplitScreenConVars( CVarDLLIdentifier_t id ) = 0;
	virtual int	 GetConsoleDisplayFuncCount( ) const = 0;
	virtual void GetConsoleText( int nDisplayFuncIndex, char* pchText, size_t bufSize ) const = 0;
	virtual bool IsMaterialThreadSetAllowed( ) const = 0;
	virtual void QueueMaterialThreadSetValue( ConVar* pConVar, const char* pValue ) = 0;
	virtual void QueueMaterialThreadSetValue( ConVar* pConVar, int nValue ) = 0;
	virtual void QueueMaterialThreadSetValue( ConVar* pConVar, float flValue ) = 0;
	virtual bool HasQueuedMaterialThreadConVarSets( ) const = 0;
	virtual int ProcessQueuedMaterialThreadConVarSets( ) = 0;*/
};