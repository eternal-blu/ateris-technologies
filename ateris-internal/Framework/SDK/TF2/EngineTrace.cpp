#include "EngineTrace.h"
#include "Entities.h"

bool CTraceFilterHitscan::ShouldHitEntity( void* ServerEntity, int ContentsMask )
{
	auto entity = reinterpret_cast< IClientEntity* >( ServerEntity );

	switch ( entity->GetClientClass( )->ClassID ) {
		case EClassID::CFuncAreaPortalWindow:
		case EClassID::CFuncRespawnRoomVisualizer:
		case EClassID::CSniperDot: return false;
		default: return ( entity != this->Skip );
	}
}