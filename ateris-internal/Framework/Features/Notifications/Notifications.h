#pragma once
#include "../../SDK/TF2/TF2.h"
#include <deque>

#define START_TRANSITION_TIME 0.5f
#define END_TRANSITION_TIME 0.5f

enum class CNotificationType : int
{
	NOTIF_ERROR = 0,
	NOTIF_SUCCESS = 1,
	NOTIF_WARN = 2,
	NOTIF_INFO = 3
};

class CNotification
{
public:
	CNotification( std::wstring Message, CNotificationType Type, float Time = 5.0f );
	void Update( );

	bool Invalid( )
	{
		return WantsDeletion;
	}

	float RemainingTime( )
	{
		return TimeRemaining;
	}

private:
	float StartTime = 0.f;
	float EndTime = 0.f;
	float TimeRemaining = 0.f;
	bool WantsDeletion = false;

	CNotificationType Type = CNotificationType::NOTIF_ERROR;
	std::wstring Message = L"NOT FOUND";
};

class CNotifications
{
public:
	void Render( );

	void Update( );
	void Push( CNotification* Notif );

private:
	std::deque<CNotification*> Notifications = {};
};

inline CNotifications* Notifications = new CNotifications;