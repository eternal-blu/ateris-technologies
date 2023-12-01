#pragma once

class IClientEntity;
class IClientNetworkable;
class IClientUnknown;
class IClientRenderable;
#include "CHandle.h"

class IClientEntityList
{
public:
	virtual IClientNetworkable* GetClientNetworkable( int entnum ) = 0;
	virtual IClientNetworkable* GetClientNetworkableFromHandle( CBaseHandle hEnt ) = 0;
	virtual IClientUnknown* GetClientUnknownFromHandle( CBaseHandle hEnt ) = 0;
	virtual IClientEntity* GetClientEntity( int entnum ) = 0;
	virtual IClientEntity* GetClientEntityFromHandle( CBaseHandle hEnt ) = 0;
	virtual int	NumberOfEntities( bool bIncludeNonNetworkable ) = 0;
	virtual int	GetHighestEntityIndex( void ) = 0;
	virtual void SetMaxEntities( int maxents ) = 0;
	virtual int	GetMaxEntities( ) = 0;
};