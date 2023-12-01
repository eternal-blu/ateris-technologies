#pragma once
#define MAX_EVENT_NAME_LENGTH 32
#define MAX_EVENT_BITS 9
#define MAX_EVENT_NUMBER (1 << MAX_EVENT_BITS)
#define MAX_EVENT_BYTES 1024

#define EVENT_DEBUG_ID_INIT			42
#define EVENT_DEBUG_ID_SHUTDOWN		13

#include <deque>

class IGameEvent
{
public:
	virtual ~IGameEvent( ) {};
	virtual const char* GetName( ) const = 0;
	virtual bool IsReliable( ) const = 0;
	virtual bool IsLocal( ) const = 0;
	virtual bool IsEmpty( const char* keyName = 0 ) = 0;
	virtual bool GetBool( const char* keyName = 0, bool defaultValue = false ) = 0;
	virtual int GetInt( const char* keyName = 0, int defaultValue = 0 ) = 0;
	virtual float GetFloat( const char* keyName = 0, float defaultValue = 0.0f ) = 0;
	virtual const char* GetString( const char* keyName = 0, const char* defaultValue = "" ) = 0;
	virtual void SetBool( const char* keyName, bool value ) = 0;
	virtual void SetInt( const char* keyName, int value ) = 0;
	virtual void SetFloat( const char* keyName, float value ) = 0;
	virtual void SetString( const char* keyName, const char* value ) = 0;
};

class IGameEventListener
{
public:
	virtual	~IGameEventListener( ) {};
	virtual void FireGameEvent( IGameEvent* pEvent ) = 0;
};

class CSVCMsg_GameEvent;
class IGameEventManager
{
public:
	virtual				~IGameEventManager( ) {}
	virtual int			LoadEventsFromFile( const char* szFileName ) = 0;
	virtual void		Reset( ) = 0;
	virtual bool		AddListener( IGameEventListener* pListener, const char* szName, bool bServerSide ) = 0;
	virtual bool		FindListener( IGameEventListener* pListener, const char* szName ) = 0;
	virtual void		RemoveListener( IGameEventListener* pListener ) = 0;
	virtual void		AddListenerGlobal( IGameEventListener* pListener, bool bServerSide ) = 0;
	virtual IGameEvent* CreateNewEvent( const char* szName, bool bForce = false, int* unknown = nullptr ) = 0;
	virtual bool		FireEvent( IGameEvent* pEvent, bool bDontBroadcast = false ) = 0;
	virtual bool		FireEventClientSide( IGameEvent* pEvent ) = 0;
	virtual IGameEvent* DuplicateEvent( IGameEvent* pEvent ) = 0;
	virtual void		FreeEvent( IGameEvent* pEvent ) = 0;
	virtual bool		SerializeEvent( IGameEvent* pEvent, CSVCMsg_GameEvent* pEventMsg ) = 0;
	virtual IGameEvent* UnserializeEvent( const CSVCMsg_GameEvent& eventMsg ) = 0;
	virtual void* GetEventDataTypes( IGameEvent* pEvent ) = 0;
};