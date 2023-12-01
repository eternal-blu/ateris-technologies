#include "Notifications.h"
#include "../../../Globals.h"

void CNotifications::Render( )
{
	// needed
	Update( );

	for ( size_t i = 0; i < Notifications.size( ); i++ )
	{

	}
}

void CNotifications::Update( )
{
	for ( size_t i = 0; i < Notifications.size( ); i++ )
	{
		auto& Notification = Notifications[ i ];
		Notification->Update( );

		if ( Notification->Invalid( ) )
			Notifications.erase( Notifications.begin( ) + i );
	}
}

void CNotifications::Push( CNotification* Notification )
{
	Notifications.push_front( Notification );
}

CNotification::CNotification( std::wstring vMessage, CNotificationType vType, float Time )
{
	StartTime = gEngineClient->Time( );
	EndTime = StartTime + START_TRANSITION_TIME + Time + END_TRANSITION_TIME;

	Type = vType;
	Message = vMessage;
	WantsDeletion = false;
}

void CNotification::Update( )
{
	TimeRemaining = EndTime - gEngineClient->Time( );

	if ( TimeRemaining <= 0.f )
		WantsDeletion = true;
}