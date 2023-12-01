#pragma once
#include "../../SDK/TF2/TF2.h"

class CEvents : public IGameEventListener
{
public:
	void Setup( const std::deque<const char*>& arrEvents );
	void Destroy( );

	virtual void FireGameEvent( IGameEvent* event ) override;
};
inline CEvents* Events = new CEvents;