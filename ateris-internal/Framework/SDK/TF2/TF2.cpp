#include "TF2.h"
#include "../../../Globals.h"
#include "../TF2/BaseClientDLL.h"
#include <wininet.h>

#pragma comment( lib, "winhttp.lib" )
#pragma comment( lib, "Wininet.lib" )

void CTF2::UpdateWorldToScreen( )
{
	CViewSetup View;
	if ( gClient->GetPlayerView( View ) )
	{
		ViewMatrix WorldToView, ViewToProjection, WorldToPixels;
		gRenderView->GetMatricesForView( View, &WorldToView, &ViewToProjection, &WorldToProjection, &WorldToPixels );
	}
}

bool CTF2::WorldToScreen( const Vector& origin, Vector& screen )
{
	const auto& WorldToProjection3x4 =  * ( ( const Matrix3x4* )WorldToProjection );
	auto w = WorldToProjection3x4[ 3 ][ 0 ] * origin[ 0 ] + WorldToProjection3x4[ 3 ][ 1 ] * origin[ 1 ] + WorldToProjection3x4[ 3 ][ 2 ] * origin[ 2 ] + WorldToProjection3x4[ 3 ][ 3 ];
	screen.z = 0.0f;

	if ( w > 0.001f ) {
		auto inverse_w = ( 1.0f / w );
		screen.x = ( static_cast< float >( ScreenWidth ) * 0.5f ) + ( 0.5f * ( ( WorldToProjection3x4[ 0 ][ 0 ] * origin[ 0 ] + WorldToProjection3x4[ 0 ][ 1 ] * origin[ 1 ] + WorldToProjection3x4[ 0 ][ 2 ] * origin[ 2 ] + WorldToProjection3x4[ 0 ][ 3 ] ) * inverse_w ) * static_cast< float >( ScreenWidth ) + 0.5f );
		screen.y = ( static_cast< float >( ScreenHeight ) * 0.5f ) - ( 0.5f * ( ( WorldToProjection3x4[ 1 ][ 0 ] * origin[ 0 ] + WorldToProjection3x4[ 1 ][ 1 ] * origin[ 1 ] + WorldToProjection3x4[ 1 ][ 2 ] * origin[ 2 ] + WorldToProjection3x4[ 1 ][ 3 ] ) * inverse_w ) * static_cast< float >( ScreenHeight ) + 0.5f );
		return true;
	}

	return false;
}

bool CTF2::ComputeBoundingBox( CBaseEntity* Entity, Vector* Transformed, Rect_t& BoundingBox )
{
	const auto& Mins = Entity->Mins( );
	const auto& Maxs = Entity->Maxs( );

	std::array< Vector, 8 > Points = {
		Vector( Mins.x, Mins.y, Mins.z ),
		Vector( Mins.x, Maxs.y, Mins.z ),
		Vector( Maxs.x, Maxs.y, Mins.z ),
		Vector( Maxs.x, Mins.y, Mins.z ),
		Vector( Maxs.x, Maxs.y, Maxs.z ),
		Vector( Mins.x, Maxs.y, Maxs.z ),
		Vector( Mins.x, Mins.y, Maxs.z ),
		Vector( Maxs.x, Mins.y, Maxs.z )
	};

	auto& Transform = ( Matrix3x4& )Entity->RenderableToWorldTransform( );

	if ( Entity && Entity == gLocalPlayer ) {
		auto Angles = gEngineClient->GetViewAngles( );
		Angles.x = Angles.z = 0.0f;
		gMath->AngleMatrix( Angles, Transform );
		gMath->MatrixSetColumn( Entity->GetRenderOrigin( ), 3, Transform );
	}

	auto Left = std::numeric_limits< float >::max( );
	auto Top = std::numeric_limits< float >::max( );
	auto Right = std::numeric_limits< float >::lowest( );
	auto Bottom = std::numeric_limits< float >::lowest( );

	std::array< Vector, 8 > Screen;

	for ( std::size_t i = 0; i < 8; i++ ) {
		gMath->VectorTransform( Points.at( i ), Transform, Transformed[ i ] );
		if ( !WorldToScreen( Transformed[ i ], Screen.at( i ) ) )
		{
			return false;
		}

		Left = std::min( Left, Screen.at( i ).x );
		Top = std::min( Top, Screen.at( i ).y );
		Right = std::max( Right, Screen.at( i ).x );
		Bottom = std::max( Bottom, Screen.at( i ).y );
	}

	auto x_ = Left;
	auto y_ = Top;
	auto w_ = Right - Left;
	auto h_ = Bottom - Top;

	if ( Entity->IsPlayer( ) ) {
		x_ += ( ( Right - Left ) / 8.0f );
		w_ -= ( ( ( Right - Left ) / 8.0f ) * 2.0f );
	}

	BoundingBox.x = static_cast< int >( x_ );
	BoundingBox.y = static_cast< int >( y_ );
	BoundingBox.w = static_cast< int >( w_ );
	BoundingBox.h = static_cast< int >( h_ );

	return true;
}

std::wstring CTF2::Utf8ToWide( const std::string_view& str ) {
	auto count = MultiByteToWideChar( CP_UTF8, 0, str.data( ), str.length( ), NULL, 0 );
	std::wstring wstr( count, 0 );
	MultiByteToWideChar( CP_UTF8, 0, str.data( ), str.length( ), &wstr[ 0 ], count );
	return wstr;
}

bool CTF2::IsVisible(CBasePlayer* Skip, CBasePlayer* Entity, const Vector& from, const Vector& to, unsigned int Mask)
{
	CGameTrace trace = {};
	CTraceFilterHitscan filter = {};
	filter.Skip = Skip;
	Ray_t ray;
	ray.Init(from, to);
	gEngineTrace->TraceRay(ray, Mask, &filter, &trace);
	return ((trace.Ent && trace.Ent == Entity) || trace.Fraction > 0.99f);
}

bool CTF2::IsVisible(CBasePlayer* skip, CBasePlayer* target, const Vector& from, const Vector& to, unsigned int mask, const Vector& mins, const Vector& maxs)
{
	CGameTrace trace;
	CTraceFilterHitscan filter;
	filter.Skip = skip;
	Ray_t ray;
	ray.Init(from, to, mins, maxs);
	gEngineTrace->TraceRay(ray, mask, &filter, &trace);

	return ((trace.Ent && trace.Ent == target) || trace.Fraction > 0.99f);
}

bool CTF2::IsVisible( const Vector& from, const Vector& to, const Vector& mins, const Vector& maxs )
{
	CGameTrace trace;
	CTraceFilterWorldAndPropsOnly filter;
	Ray_t ray;
	ray.Init( from, to, mins, maxs );
	gEngineTrace->TraceRay( ray, MASK_SOLID, &filter, &trace );

	return trace.Fraction > 0.99f;
}

void CTF2::TraceHull( const Vector& from, const Vector& to, const Vector& mins, const Vector& maxs, CTraceFilter* Filter, CGameTrace* Trace, unsigned int MASK )
{
	Ray_t ray;
	ray.Init( from, to, mins, maxs );
	gEngineTrace->TraceRay( ray, MASK, Filter, Trace );
}

void CTF2::Trace( const Vector& from, const Vector& to, CTraceFilter* Filter, CGameTrace* Trace, unsigned int MASK )
{
	Ray_t ray;
	ray.Init( from, to );
	gEngineTrace->TraceRay( ray, MASK, Filter, Trace );
}

float CTF2::LerpTime() {
	static auto cl_interp = gCvar->FindVar("cl_interp");
	static auto cl_interp_ratio = gCvar->FindVar("cl_interp_ratio");
	static auto cl_updaterate = gCvar->FindVar("cl_updaterate");
	static auto min_ud_rate = gCvar->FindVar("sv_minupdaterate");
	static auto max_ud_rate = gCvar->FindVar("sv_maxupdaterate");
	static auto c_min_ratio = gCvar->FindVar("sv_client_min_interp_ratio");
	static auto c_max_ratio = gCvar->FindVar("sv_client_max_interp_ratio");

	int ud_rate = cl_updaterate->GetInt();

	if (min_ud_rate && max_ud_rate)
		ud_rate = max_ud_rate->GetInt();

	float ratio = cl_interp_ratio->GetFloat();

	if (ratio == 0)
		ratio = 1.f;

	float lerp = cl_interp->GetFloat();

	if (c_min_ratio && c_max_ratio && c_min_ratio->GetFloat() != 1)
		ratio = std::clamp(ratio, c_min_ratio->GetFloat(), c_max_ratio->GetFloat());

	return std::max(lerp, (ratio / ud_rate));
}

int CTF2::ServerTick( )
{
	if ( !gClientState || !gEngineClient )
	{
		return 0;
	}

	auto Channel = gEngineClient->GetNetChannelInfo( );
	if ( !Channel )
	{
		return 0;
	}

	return gClientState->m_ClockDriftMgr.server_tick + 1 + TIME_TO_TICKS( Channel->GetLatency( FLOW_OUTGOING ) );
}

bool CTF2::IsNonAimable(CBaseCombatWeapon* Weapon)
{
	if ( !Weapon || Weapon->IsDormant( ) )
		return false;

	switch (Weapon->ItemDefinitionIndex()) {
	case Soldier_s_TheBuffBanner:
	case Soldier_s_FestiveBuffBanner:
	case Soldier_s_TheBattalionsBackup:
	case Soldier_s_TheConcheror: return true;
	default: {
		switch ( G->Aimbot.SavedWeaponID ) {
		case TF_WEAPON_MEDIGUN:
		case TF_WEAPON_PDA:
		case TF_WEAPON_PDA_ENGINEER_BUILD:
		case TF_WEAPON_PDA_ENGINEER_DESTROY:
		case TF_WEAPON_PDA_SPY:
		case TF_WEAPON_PDA_SPY_BUILD:
		case TF_WEAPON_BUILDER:
		case TF_WEAPON_GRAPPLINGHOOK: return true;
		default: return false;
		}
		break;
	}
	}
}

bool CTF2::IsProjectileWeapon(CBaseCombatWeapon* Weapon)
{
	if (!Weapon || Weapon->IsDormant( ))
		return false;

	if (Weapon->Slot() == 2)
		return false;

	switch (Weapon->ItemDefinitionIndex()) {
	case Soldier_s_TheRighteousBison:
	case Engi_m_ThePomson6000:
	case Scout_s_TheFlyingGuillotine:
	case Scout_s_TheFlyingGuillotineG:
	case Engi_m_TheRescueRanger: return true;
	}

	switch ( G->Aimbot.SavedWeaponID ) {
	case TF_WEAPON_CANNON:
	case TF_WEAPON_COMPOUND_BOW:
	case TF_WEAPON_LUNCHBOX:
	case TF_WEAPON_CROSSBOW:
	case TF_WEAPON_FLAMETHROWER:
	case TF_WEAPON_FLAME_BALL:
	case TF_WEAPON_FLAREGUN:
	case TF_WEAPON_DRG_POMSON:
	case TF_WEAPON_GRENADELAUNCHER:
	case TF_WEAPON_NAILGUN:
	case TF_WEAPON_PIPEBOMBLAUNCHER:
	case TF_WEAPON_STICKY_BALL_LAUNCHER:
	case TF_WEAPON_ROCKETLAUNCHER:
	case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
	case TF_WEAPON_JAR:
	case TF_WEAPON_JAR_MILK:
	case TF_WEAPON_SYRINGEGUN_MEDIC: return true;
	}

	return false;
}

bool CTF2::IsHitscanWeapon(CBaseCombatWeapon* Weapon)
{
	if (!Weapon || Weapon->IsDormant( ))
		return false;

	return !IsNonAimable(Weapon) && !IsProjectileWeapon( Weapon );
}

void CTF2::GetProjectileFireSetup(CBasePlayer* Player, const Vector& Angles, Vector Offset, Vector& PosOut, Vector& AngOut, bool Pipers = false) {
	if (G->ConVars->FlipViewmodels && G->ConVars->FlipViewmodels->GetBool())
		Offset.y *= -1.0f;

	auto forward = Vector(), right = Vector(), up = Vector();
	gMath->AngleVectors(Angles, &forward, &right, &up);

	PosOut = Player->EyePosition() + (forward * Offset.x) + (right * Offset.y) + (up * Offset.z);

	if (Pipers) {
		AngOut = Angles;
	}

	else {
		auto end_position = PosOut + (forward * 2000.0f);
		gMath->VectorAngles(end_position - PosOut, AngOut);
	}
}

void CTF2::GetProjectileFireSetup( CBasePlayer* Player, const Vector& Angles, Vector Offset, Vector* Out )
{
	if ( G->ConVars->FlipViewmodels && G->ConVars->FlipViewmodels->GetBool( ) )
		Offset.y *= -1.0f;

	auto Forward = Vector( ), Right = Vector( ), Up = Vector( );
	gMath->AngleVectors( Angles, &Forward, &Right, &Up );

	*Out += ( Forward * Offset.x ) + ( Right * Offset.y ) + ( Up * Offset.z );
}

float CTF2::AttribHookFloat(float Base, const char* Str, CBaseEntity* Entity, void* Buffer = nullptr, bool IsGlobalString) {
	typedef float(__cdecl* attrib_hook_float_fn)(float, const char*, CBaseEntity*, void*, bool);
	static auto fn = Memory->FindSignature< attrib_hook_float_fn >("client.dll", "55 8B EC 83 EC 0C 8B 0D ? ? ? ? 53 56 57 33 F6 33 FF 89 75 F4 89 7D F8 8B 41 08 85 C0 74 38");
	return fn(Base, Str, Entity, Buffer, IsGlobalString);
}

void CTF2::FixMovement(CUserCmd* pCmd, Vector Old) {
	auto forward = Vector(), right = Vector(), up = Vector();
	gMath->AngleVectors(Old, &forward, &right, &up);

	forward.z = right.z = up.x = up.y = 0.0f;

	forward.NormalizeInPlace();
	right.NormalizeInPlace();
	up.NormalizeInPlace();

	auto OldForward = Vector(), OldRight = Vector(), OldUp = Vector();
	gMath->AngleVectors(pCmd->viewangles, &OldForward, &OldRight, &OldUp);

	OldForward.z = OldRight.z = OldUp.x = OldUp.y = 0.0f;

	OldForward.NormalizeInPlace();
	OldRight.NormalizeInPlace();
	OldUp.NormalizeInPlace();

	const float pForward = forward.x * pCmd->forwardmove;
	const float yForward = forward.y * pCmd->forwardmove;
	const float pSide = right.x * pCmd->sidemove;
	const float ySide = right.y * pCmd->sidemove;
	const float rUp = up.z * pCmd->upmove;

	const float x = OldForward.x * pSide + OldForward.y * ySide + OldForward.x * pForward + OldForward.y * yForward + OldForward.z * rUp;
	const float y = OldRight.x * pSide + OldRight.y * ySide + OldRight.x * pForward + OldRight.y * yForward + OldRight.z * rUp;
	const float z = OldUp.x * ySide + OldUp.y * pSide + OldUp.x * yForward + OldUp.y * pForward + OldUp.z * rUp;

	pCmd->forwardmove = std::clamp(x, -450.f, 450.f);
	pCmd->sidemove = std::clamp(y, -450.f, 450.f);
	pCmd->upmove = std::clamp(z, -450.f, 450.f);
}

void CTF2::RandomSeed(int iSeed)
{
	static auto RandomSeedFn = reinterpret_cast<void(__cdecl*)(int)>(reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandleW(L"vstdlib.dll"), "RandomSeed")));
	RandomSeedFn(iSeed);
}

int CTF2::RandomInt(int min, int max)
{
	static auto RandomIntFn = reinterpret_cast<int(__cdecl*)(int, int)>(reinterpret_cast<DWORD>(GetProcAddress(GetModuleHandleW(L"vstdlib.dll"), "RandomInt")));
	return RandomIntFn(min, max);
}

float CTF2::RandomFloat( float min, float max )
{
	static auto RandomFloatFn = reinterpret_cast< float( __cdecl* )( float, float ) >( reinterpret_cast< DWORD >( GetProcAddress( GetModuleHandleW( L"vstdlib.dll" ), "RandomFloat" ) ) );
	return RandomFloatFn( min, max );
}

Color CTF2::GetHealthColor( int Health, int MaxHealth )
{
	int HP = std::max( 0, std::min( Health, MaxHealth ) );
	int r = std::min( ( ( 510 * ( MaxHealth - HP ) ) / MaxHealth ) + 130, 255 );
	int g = std::min( ( ( 510 * HP ) / MaxHealth ), 255 );
	return { r, g, 40, 180 };
}

bool CTF2::IsAttacking(CUserCmd* pCmd)
{
	if (!gLocalWeapon)
		return false;

	if (G->Aimbot.SavedWeaponID == TF_WEAPON_COMPOUND_BOW 
		|| G->Aimbot.SavedWeaponID == TF_WEAPON_PIPEBOMBLAUNCHER)
	{
		static bool bCharging = false;
		const float Charge = gLocalWeapon->ChargeBeginTime();

		if (Charge > 0.0f)
			bCharging = true;

		if (!(pCmd->buttons & IN_ATTACK) && bCharging) {
			bCharging = false;
			return true;
		}
	}
	else if ( G->Aimbot.SavedWeaponID == TF_WEAPON_CANNON)
	{
		static bool Charging = false;

		if (gLocalWeapon->ChargeBeginTime() > 0.0f)
			Charging = true;

		if (!(pCmd->buttons & IN_ATTACK) && Charging)
		{
			Charging = false;
			return true;
		}
	}
	else
	{
		if (gLocalWeapon->Slot() == SLOT_MELEE && gLocalPlayer->Class() != CLASS_SPY)
			return fabs(gLocalWeapon->SmackTime() - TICKS_TO_TIME(gLocalPlayer->TickBase())) < gGlobals->interval_per_tick * 2;

		if ( G->Aimbot.SavedWeaponID == TF_WEAPON_FLAME_BALL && ( *reinterpret_cast< float* >( reinterpret_cast<DWORD>( gLocalPlayer ) + 0x1B40 ) >= 100.0f ) )
			return pCmd->buttons & IN_ATTACK;

		if (pCmd->buttons & IN_ATTACK && gLocalWeapon->CanFire() )
			return true;
	}

	return false;
}

void CTF2::UpdateIncomingSequences( )
{
	if ( !gClientState->m_NetChannel )
		return;

	if ( G->Sequences.empty( ) || gClientState->m_NetChannel->InSequenceNr > G->Sequences.front( ).seq )
	{
		G->Sequences.emplace_front( gGlobals->curtime, gClientState->m_NetChannel->InReliableState, gClientState->m_NetChannel->InSequenceNr );
	}

	while ( G->Sequences.size( ) > 2048 )
		G->Sequences.pop_back( );
}

void CTF2::AddLatencyToNetChannel( CNetChannel* NetChan, float Latency )
{
	for ( auto& seq : G->Sequences )
	{
		if ( gGlobals->curtime - seq.time >= Latency )
		{
			NetChan->InReliableState = seq.state;
			NetChan->InSequenceNr = seq.seq;
			break;
		}
	}
}

//straight outta net_chan.cpp
void CTF2::FlowNewPacket( CNetChannel* chan, int flow, int seqnr, int acknr, int nChoked, int nDropped, int nSize )
{
	//netflow_t * m_DataFlow = (netflow_t*)((DWORD)chan + 0x5C0);
	CNetChannel::netflow_t* pflow = &chan->m_DataFlow[ flow ];

	// if frame_number != ( current + 1 ) mark frames between as invalid

	CNetChannel::netframe_t* pframe = NULL;

	if ( seqnr > pflow->currentindex )
	{
		int framecount = 0;
		for ( int i = pflow->currentindex + 1, numPacketFramesOverflow = 0;
			  ( i <= seqnr ) && ( numPacketFramesOverflow < NET_FRAMES_BACKUP );
			  ++i, ++numPacketFramesOverflow )
		{
			int nBackTrack = seqnr - i;

			pframe = &pflow->frames[ i & NET_FRAMES_MASK ];

			pframe->time = chan->m_fClearTime;	// now
			pframe->valid = false;
			pframe->size = 0;
			pframe->latency = -1.0f; // not acknowledged yet
			pframe->avg_latency = chan->GetAverageLatency( FLOW_OUTGOING );
			pframe->choked = 0; // not acknowledged yet
			pframe->dropped = 0;
			pframe->m_flInterpolationAmount = 0.0f;

			pframe = &pflow->frames[ i & NET_FRAMES_MASK ];
			memset( &pframe->msggroups, 0, sizeof( pframe->msggroups ) );

			if ( nBackTrack < ( nChoked + nDropped ) )
			{
				if ( nBackTrack < nChoked )
				{
					pframe->choked = 1;
				}
				else
				{
					pframe->dropped = 1;
				}
			}
			framecount++;
		}

		pframe->dropped = nDropped;
		pframe->choked = nChoked;
		pframe->size = nSize;
		pframe->valid = true;
		pframe->avg_latency = chan->GetAverageLatency( FLOW_OUTGOING );
		pframe->m_flInterpolationAmount = chan->m_flInterpolationAmount;
	}
	else
	{
		//Assert( demoplayer->IsPlayingBack() || seqnr > pflow->currentindex );
	}

	pflow->totalpackets++;
	pflow->currentindex = seqnr;
	pflow->currentframe = pframe;

	// updated ping for acknowledged packet

	int aflow = ( flow == FLOW_OUTGOING ) ? FLOW_INCOMING : FLOW_OUTGOING;

	if ( acknr <= ( chan->m_DataFlow[ aflow ].currentindex - NET_FRAMES_BACKUP ) )
		return;	// acknowledged packet isn't in backup buffer anymore

	CNetChannel::netframe_t* aframe = &chan->m_DataFlow[ aflow ].frames[ acknr & NET_FRAMES_MASK ];

	if ( aframe->valid && aframe->latency == -1.0f )
	{
		// update ping for acknowledged packet, if not already acknowledged before

		aframe->latency = chan->m_fClearTime - aframe->time;

		if ( aframe->latency < 0.0f )
			aframe->latency = 0.0f;
	}
}

#define FLOW_INTERVAL 0.25
#define FLOW_AVG ( 3.0 / 4.0 )
void CTF2::FlowUpdate( CNetChannel* chan, int flow, int addbytes )
{
	CNetChannel::netflow_t* pflow = &chan->m_DataFlow[ flow ];
	pflow->totalBYTEs += addbytes;

	if ( pflow->nextcompute > chan->m_fClearTime )
		return;

	pflow->nextcompute = chan->m_fClearTime + FLOW_INTERVAL;

	int		totalvalid = 0;
	int		totalinvalid = 0;
	int		totalbytes = 0;
	float	totallatency = 0.0f;
	int		totallatencycount = 0;
	int		totalchoked = 0;

	float   starttime = FLT_MAX;
	float	endtime = 0.0f;

	CNetChannel::netframe_t *pprev = &pflow->frames[NET_FRAMES_BACKUP - 1];

	for ( int i = 0; i < NET_FRAMES_BACKUP; i++ )
	{
		// Most recent message then backward from there
		CNetChannel::netframe_t* pcurr = &pflow->frames[ i ];

		if ( pcurr->valid )
		{
			if ( pcurr->time < starttime )
				starttime = pcurr->time;

			if ( pcurr->time > endtime )
				endtime = pcurr->time;

			totalvalid++;
			totalchoked += pcurr->choked;
			totalbytes += pcurr->size;

			if ( pcurr->latency > -1.0f )
			{
				totallatency += pcurr->latency;
				totallatencycount++;
			}
		}
		else
		{
			totalinvalid++;
		}

		pprev = pcurr;
	}

	float totaltime = endtime - starttime;

	if ( totaltime > 0.0f )
	{
		pflow->avgBYTEspersec *= FLOW_AVG;
		pflow->avgBYTEspersec += ( 1.0f - FLOW_AVG ) * ( ( float )totalbytes / totaltime );

		pflow->avgpacketspersec *= FLOW_AVG;
		pflow->avgpacketspersec += ( 1.0f - FLOW_AVG ) * ( ( float )totalvalid / totaltime );
	}

	int totalPackets = totalvalid + totalinvalid;

	if ( totalPackets > 0 )
	{
		pflow->avgloss *= FLOW_AVG;
		pflow->avgloss += ( 1.0f - FLOW_AVG ) * ( ( float )( totalinvalid - totalchoked ) / totalPackets );

		if ( pflow->avgloss < 0 )
			pflow->avgloss = 0;

		pflow->avgchoke *= FLOW_AVG;
		pflow->avgchoke += ( 1.0f - FLOW_AVG ) * ( ( float )totalchoked / totalPackets );
	}
	else
	{
		pflow->avgloss = 0.0f;
	}

	if ( totallatencycount > 0 )
	{
		float newping = totallatency / totallatencycount;
		pflow->latency = newping;
		pflow->avglatency *= FLOW_AVG;
		pflow->avglatency += ( 1.0f - FLOW_AVG ) * newping;
	}
}

Color CTF2::HSVToColor( float H, float S, float V )
{
	if ( H > 360 || H < 0 || S>100 || S < 0 || V>100 || V < 0 )
	{
		return { 0, 0, 0, 0 };
	}
	float s = S / 100;
	float v = V / 100;
	float C = s * v;
	float X = C * ( 1 - ( float )fabs( fmod( H / 60.0f, 2 ) - 1 ) );
	float m = v - C;
	float r, g, b;
	if ( H >= 0 && H < 60 )
	{
		r = C, g = X, b = 0;
	}
	else if ( H >= 60 && H < 120 )
	{
		r = X, g = C, b = 0;
	}
	else if ( H >= 120 && H < 180 )
	{
		r = 0, g = C, b = X;
	}
	else if ( H >= 180 && H < 240 )
	{
		r = 0, g = X, b = C;
	}
	else if ( H >= 240 && H < 300 )
	{
		r = X, g = 0, b = C;
	}
	else
	{
		r = C, g = 0, b = X;
	}
	unsigned char R = static_cast< unsigned char >( ( r + m ) * 255 );
	unsigned char G = static_cast< unsigned char >( ( g + m ) * 255 );
	unsigned char B = static_cast< unsigned char >( ( b + m ) * 255 );

	return { R, G, B, 255 };
}

void CTF2::ColorToHSV( const Color& Col, float* HSV )
{
	float r = Col.r / 255.f;
	float g = Col.g / 255.f;
	float b = Col.b / 255.f;

	float cmax = std::max( r, std::max( g, b ) );
	float cmin = std::min( r, std::min( g, b ) );
	float diff = cmax - cmin;
	float h = -1, s = -1;

	if ( cmax == cmin )
		h = 0;

	// if cmax equal r then compute h
	else if ( cmax == r )
		h = fmod( 60 * ( ( g - b ) / diff ) + 360, 360 );

	// if cmax equal g then compute h
	else if ( cmax == g )
		h = fmod( 60 * ( ( b - r ) / diff ) + 120, 360 );

	// if cmax equal b then compute h
	else if ( cmax == b )
		h = fmod( 60 * ( ( r - g ) / diff ) + 240, 360 );

	// if cmax equal zero
	if ( cmax == 0 )
		s = 0;
	else
		s = ( diff / cmax ) * 100;

	float v = cmax * 100.f;

	HSV[ 0 ] = h;
	HSV[ 1 ] = s;
	HSV[ 2 ] = v;
}

std::string CTF2::GetUserName( unsigned long long SteamID )
{
	switch ( SteamID )
	{
		//source access
		case 76561198316334587: { return "cleric"; }
		case 76561199123546700: { return "Mad?"; }
		case 76561199545760040: { return "jenga"; }

		//beta access
		case 76561199097087535: { return "Yani"; }
		case 76561199239697704: { return "relaxtakenotes"; }
		case 76561198055938080: { return "Teki"; }
		case 76561199466680140: { return "Ken"; }
		case 76561198141149000:
		case 76561199476288533: { return "Maurice"; }
		case 76561199024591783:	// DoesColder (main account)
		case 76561199021322150: { return "DoesColder"; } // DoesColder (alt account)
		case 76561198289945886: { return "Godl3ss"; }
		case 76561199236564495: { return "Dafo"; }
		case 76561199071704588: { return "Zphr"; }
		case 76561198839612863: { return "M-Fed"; }
		default: { return ""; }
	}
	return "";
}

std::string CTF2::GetBuildName( unsigned long long SteamID )
{
	switch ( SteamID )
	{
		case 76561198316334587:
		case 76561199123546700:
		case 76561199545760040: 
		{ 
			G->Authentication->IsDebugAllowed = true;
			std::string build = "";

/*#ifdef _DEBUG
			build = "Debug - AVX2";
#else
			build = "Release - AVX2";
#endif*/
			build = "Debug";
			return build; 
		}

		case 76561199097087535:
		case 76561199239697704:
		case 76561198055938080:
		case 76561199466680140:
		case 76561198141149000:
		case 76561199476288533:
		case 76561199024591783:
		case 76561199021322150:
		case 76561198289945886:
		case 76561199236564495:
		case 76561199071704588:
		case 76561198839612863: { G->Authentication->IsDebugAllowed = false; return "Beta"; }
		default: { return ""; }
	}
	return "";
}

int CTF2::SeedFileLineHash( int seedvalue, const char* sharedname, int additionalSeed )
{
	CRC32_t retval;

	CRC32_Init( &retval );

	CRC32_ProcessBuffer( &retval, ( void* )&seedvalue, sizeof( int ) );
	CRC32_ProcessBuffer( &retval, ( void* )&additionalSeed, sizeof( int ) );
	CRC32_ProcessBuffer( &retval, ( void* )sharedname, strlen( sharedname ) );

	CRC32_Final( &retval );

	return static_cast< int >( retval );
}

int CTF2::SharedRandomInt( unsigned int iSeed, const char* sharedname, int iMinVal, int iMaxVal, int additionalSeed )
{
	const int seed = SeedFileLineHash( iSeed, sharedname, additionalSeed );
	gRandomStream->SetSeed( seed );
	return gRandomStream->RandomInt( iMinVal, iMaxVal );

	//return Memory->FindSignature<int( __cdecl* )( const char*, int, int, int )>( "client.dll", "55 8B EC 83 EC ? 8B 45 ? 89 45 ? A1 ? ? ? ? 89 45 ? 8D 45 ? 56 50 E8 ? ? ? ? 6A ? 8D 45 ? 50" )( sharedname, iMinVal, iMaxVal, additionalSeed );
}

double CTF2::PlatFloatTime( )
{
	return reinterpret_cast< double( __cdecl* ) ( ) >( G->Plat_FloatTimeAddress )( );
}

bool CTF2::IsRegionAllowed( int ID )
{
	switch ( ID )
	{
		case ( 1 << 6 ):
		case ( 1 << 25 ):
		case ( 1 << 29 ):
		case ( 1 << 28 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->EUEast;
			break;
		}
		case ( 1 << 0 ):
		case ( 1 << 13 ):
		case ( 1 << 15 ):
		case ( 1 << 17 ):
		case ( 1 << 21 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->EUWest;
			break;
		}
		case ( 1 << 18 ):
		case ( 1 << 24 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->Asia;
			break;
		}
		case ( 1 << 3 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->MiddleEast;
			break;
		}
		case ( 1 << 9 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->HongKong;
			break;
		}
		case ( 1 << 7 ):
		case ( 1 << 27 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->Japan;
			break;
		}
		case ( 1 << 2 ):
		case ( 1 << 16 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->India;
			break;
		}
		case ( 1 << 26 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->Australia;
			break;
		}
		case ( 1 << 11 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->Africa;
			break;
		}
		case ( 1 << 22 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->Chile;
			break;
		}
		case ( 1 << 14 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->Peru;
			break;
		}
		case ( 1 << 8 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->Brazil;
			break;
		}
		case ( 1 << 1 ):
		case ( 1 << 10 ):
		case ( 1 << 20 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->USEast;
			break;
		}
		case ( 1 << 4 ):
		case ( 1 << 5 ):
		case ( 1 << 23 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->USNorthWest;
			break;
		}
		case ( 1 << 12 ):
		case ( 1 << 19 ):
		{
			return Config->Misc->Matchmaking->RegionSelector->USSouthWest;
			break;
		}
		default: return true;
	}

	return true;
}

void CTF2::DrawHitboxes( CBasePlayer* Player, Color Face, Color Edge, float Duration )
{
	const model_t* model = Player->GetModel( );
	if ( !model )
		return;

	const studiohdr_t* hdr = gModelInfo->GetStudioModel( model );
	if ( !hdr )
		return;

	const mstudiohitboxset_t* set = hdr->GetHitboxSet( Player->HitboxSet( ) );
	if ( !set )
		return;

	for ( int i{}; i < set->numhitboxes; ++i )
	{
		const mstudiobbox_t* bbox = set->hitbox( i );
		if ( !bbox )
		{
			continue;
		}

		/*if (bbox->m_radius <= 0.f) {*/
		Matrix3x4 RotatedMatrix;
		gMath->AngleMatrix( bbox->angle, RotatedMatrix );

		Matrix3x4 Matrix;
		Matrix3x4 Bones[ 128 ];
		Player->SetupBones( Bones, 128, 0x0007FF00, gGlobals->curtime );
		gMath->ConcatTransforms( Bones[ bbox->bone ], RotatedMatrix, Matrix );

		Vector BBoxAngle;
		gMath->MatrixAngles( Matrix, BBoxAngle );

		Vector MatrixOrigin;
		gMath->GetMatrixOrigin( Matrix, MatrixOrigin );

		gDebugOverlay->AddBoxOverlay2( MatrixOrigin, bbox->bbmin, bbox->bbmax, BBoxAngle, Face, Edge, Duration );
	}
}

inline std::vector<int> SplitByNewLine( const std::string& content )
{
	std::vector<int> numbers;
	std::istringstream iss( content );

	std::string line;
	while ( std::getline( iss, line ) ) 
	{
		if ( strlen( line.c_str( ) ) > 12 || line.empty( ) )
			continue;

		numbers.push_back( std::stoi( line ) );
	}

	return numbers;
}

bool CTF2::DownloadFile( const std::string& url, const std::string& output_file_path )
{
	HINTERNET h_internet = InternetOpenA( "FireFoxMyBlackMen", INTERNET_OPEN_TYPE_DIRECT, NULL, NULL, 0 );
	if ( h_internet == NULL ) {
		return false;
	}

	HINTERNET h_connection = InternetOpenUrlA( h_internet, url.c_str( ), NULL, 0, INTERNET_FLAG_RELOAD, 0 );
	if ( h_connection == NULL ) {
		InternetCloseHandle( h_internet );
		return false;
	}

	std::ofstream output_file( output_file_path, std::ios::binary );
	if ( !output_file ) {
		InternetCloseHandle( h_connection );
		InternetCloseHandle( h_internet );
		return false;
	}

	constexpr int buffer_size = 4096;
	char buffer[ buffer_size ];
	DWORD bytes_read = 0;

	while ( InternetReadFile( h_connection, buffer, buffer_size, &bytes_read ) && bytes_read > 0 ) {
		output_file.write( buffer, bytes_read );
	}

	output_file.close( );
	InternetCloseHandle( h_connection );
	InternetCloseHandle( h_internet );


	return true;
}

bool CTF2::IsInLethalDTRange( CBasePlayer* Player )
{
	if ( !gLocalPlayer || !Player || Player == gLocalPlayer )
		return false;

	// TODO: Handle other classes like soldier
	// TODO: Handle secondary weapons of scout (pistol)
	if ( gLocalPlayer->Class( ) == CLASS_HEAVY || gLocalPlayer->Class( ) == CLASS_SCOUT )
	{
		const float Distance = Player->WorldSpaceCenter( ).DistTo( gLocalPlayer->EyePosition( ) );
		if ( ( Player->Health( ) <= 200 && Distance <= 147.f ) 
			|| ( Player->Health( ) <= 150 && Distance <= 195.f ) ||
			( Player->Health( ) <= 125 && Distance <= 300.f ) || 
			( Player->Health( ) <= 100 && Distance <= 400.f ) ||
			( Player->Health( ) <= 60 && Distance <= 560.f ) || 
			( Player->Health( ) <= 30 && Distance <= 700.f ) ||
			( Player->Health( ) <= 15 && Distance <= 900.f ) || 
			( Player->Health( ) <= 5 && Distance <= 1300.f ) )
		{
			return true;
		}
	}

	return false;
}

Vector CTF2::GetVelocityVectorForCleaver( const Vector& vecForward, const Vector& vecUp )
{
	Vector vecVelocity;

	// Calculate the initial impulse on the item.
	vecVelocity = Vector( 0.0f, 0.0f, 0.0f );
	vecVelocity += vecForward * 10;
	vecVelocity += vecUp * 1;
	gMath->NormalizeVector( vecVelocity );
	vecVelocity *= 3000;

	return vecVelocity;
}