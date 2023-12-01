#pragma once
#include "../../SDK/TF2/TF2.h"
#include "Wave/Wave.h"

class CMisc
{
	struct GlobalsBackup_t
	{
		float CLFrameTime = 0.f;
		float FrameTime = 0.f;
		float CurrentTime = 0.f;
		int TickCount = 0;

		inline void Store( )
		{
			CLFrameTime = gClientState->m_frameTime;
			FrameTime = gGlobals->frametime;
			CurrentTime = gGlobals->curtime;
			TickCount = gGlobals->tickcount;
		}

		inline void Restore( )
		{
			gClientState->m_frameTime = CLFrameTime;
			gGlobals->frametime = FrameTime;
			gGlobals->curtime = CurrentTime;
			gGlobals->tickcount = TickCount;
		}
	};

	struct WaveFileHeader_t
	{
		char ChunkId[ 4 ];
		long ChunkSize;
		char Format[ 4 ];
		char Subchunk1ID[ 4 ];
		long SubchunkSize1;
		short AudioFormat;
		short NumChannels;
		long SampleRate;
		long ByteRate;
		short BlockAlign;
		short BitsPerSample;
		char Subchunk2ID[ 4 ];
		long Subchunk2Size;
	};

	class StoredData_t {
	public:
		int TickBase {};
		Vector PunchAngle {};
		Vector  PunchVelocity {};
		Vector ViewOffset {};
		Vector Origin {};
		float m_flLastFireTime {};
		float m_flTimeWeaponIdle {};
		float m_flNextPrimaryAttack {};
		std::unordered_map<int, Vector> Origins {};
		std::unordered_map<int, float> SimTimes {};
		std::unordered_map<int, float> OldSimTimes {};

	public:
		StoredData_t( ) : TickBase{}, PunchAngle{}, PunchVelocity{}, ViewOffset{}, Origin{} {};
	};

public:
	GlobalsBackup_t State;
	void RemoveInputDelay( bool FinalTick );
	bool ShouldReadPackets( );
	void BunnyHoppenz(CUserCmd* Cmd);
	void AirStrafer(CUserCmd* Cmd);
	void AntiWarp(CUserCmd* Cmd);
	void HandleDelayDT();
	bool Warp( float extra_sample, bool final_tick, std::function< void( float, bool ) > f );
	void FastStop(CUserCmd* Cmd);
	void ScoutJump( CUserCmd* Cmd );
	void JitterMove( CUserCmd* Cmd );
	void FakeLag( CUserCmd* Cmd, bool* SendPacket );
	bool IsFakeLagToggled = true;
	void FakeDuck( CUserCmd* Cmd, bool* SendPacket );
	void RocketJump( CUserCmd* Cmd );
	bool AllowedToDoubleTap();
	void FastBackup(CUserCmd* pCmd);
	void PlayFile( const char* file );
	void AntiBackstab(CUserCmd* pCmd);
	void InitializeSound( );
	void Play( std::string filename );
	void Reset( );
	void LoadSoundFiles( );
	void ShootStickyAtCharge( CUserCmd* Cmd );
	void NetStore( );
	void NetApply( );
	void NetReset( );

	void OnEvent( IGameEvent* Event );
	bool CanDoRagdollEffects( CTFRagdoll* ECX );
	void RunTest( );
	void AutoRetreat( CUserCmd* pCmd );
	void EdgeJump( CUserCmd* Cmd, bool WasOnGround );

	int LastLatency = 0;
	bool IsClientLagging = false;
	float TimeSinceLag = 0.f;
	void CalcLatencyDifference( );
	bool IsFakeLatencyToggled = false;

	std::unordered_map<int, bool> KilledByLocal {};

	WaveOut waveOut {};
	HWND hwnd {};
	int deviceID = WAVE_MAPPER;
	char* current_file_data {};
	int current_file_bytes {};

	std::string sound_folder {};
	int selected_sound = 0;
	std::vector<std::string> sound_files {};
	std::vector<std::string> devices {};
	std::array< StoredData_t, MULTIPLAYER_BACKUP > NetData {};

	void VoiceSay( );
	void KillSay( IGameEvent* Event );
	void StopRecording( );
};

inline CMisc* Misc = new CMisc;