#pragma once
#include "BaseClientDLL.h"
#include "CUtlVector.h"
#include "../Math/MD5.h"

class CSaveRestoreData;

class CGlobalVarsBase
{
public:
	float realtime;
	int framecount;
	float absoluteframetime;
	float curtime;
	float frametime;
	int maxClients;
	int tickcount;
	float interval_per_tick;
	float interpolation_amount;
	int simTicksThisFrame;
	int network_protocol;
	CSaveRestoreData* pSaveData;
	bool m_bClient;
	int nTimestampNetworkingBase;
	int nTimestampRandomizeWindow;
};

enum
{
	NUM_CLOCKDRIFT_SAMPLES = 16
};

class CClockDriftMgr
{
public:
	float clock_offsets[ NUM_CLOCKDRIFT_SAMPLES ];
	int current_offset = 0;
	int server_tick = 0;
	int client_tick = 0;
};

#define MAX_AREA_STATE_BYTES		32
#define MAX_AREA_PORTAL_STATE_BYTES 24

struct AddAngle
{
	float total;
	float starttime;
};

typedef unsigned int CRC32_t;
typedef struct CustomFile_s
{
	CRC32_t			crc;	//file CRC
	unsigned int	reqID;	// download request ID
} CustomFile_t;

class CEventInfo
{
public:
	enum
	{
		EVENT_INDEX_BITS = 8,
		EVENT_DATA_LEN_BITS = 11,
		MAX_EVENT_DATA = 192,  // ( 1<<8 bits == 256, but only using 192 below )
	};

	~CEventInfo( )
	{
		if ( pData )
		{
			delete pData;
		}
	}

	// 0 implies not in use
	short classID;

	// If non-zero, the delay time when the event should be fired ( fixed up on the client )
	float fire_delay;

	// send table pointer or NULL if send as full update
	const SendTable* pSendTable;
	const CClientClass* pClientClass;

	// Length of data bits
	int		bits;
	// Raw event data
	byte* pData;
	// CLIENT ONLY Reliable or not, etc.
	int		flags;
private:
	char pad[ 24 ];
public:
	CEventInfo* next;
};

class CBaseClientState
{
private:
	char pad_0000[ 8 ]; //0x0000
public:
	void* thisptr = nullptr; //0x0008
private:
	char pad_000C[ 4 ]; //0x000C
public:
	CNetChannel* m_NetChannel = nullptr; //0x0010
private:
	char pad_0014[ 316 ]; //0x0014
public:
	int m_nCurrentSequence = 0; //0x0150
	CClockDriftMgr m_ClockDriftMgr = { }; //0x0154
	int m_nDeltaTick = 0; //0x01A0
private:
	char pad_01A4[ 16 ]; //0x01A4
public:
	char m_szLevelFileName[ 128 ]; //0x01B4
	char m_szLevelBaseName[ 128 ]; //0x0234
	int m_nMaxClients = 0; //0x02B4
private:
	char pad_02B8[ 18528 ]; //0x02B8
public:
	int	oldtickcount = 0;		//0x4B18
	float m_tickRemainder = 0.f;	//0x4B1C
	float m_frameTime = 0.f; //0x4B20
	int lastoutgoingcommand = 0; //0x4B24
	int chokedcommands = 0; //0x4B28
	int last_command_ack = 0; //0x4B2C
	int command_ack = 0; //0x4B30
	int	m_nSoundSequence = 0;
	bool ishltv;
	MD5Value_t serverMD5;
	unsigned char m_chAreaBits[ MAX_AREA_STATE_BYTES ];
	unsigned char m_chAreaPortalBits[ MAX_AREA_PORTAL_STATE_BYTES ];
	bool m_bAreaBitsValid;
	QAngle viewangles;
	CUtlVector< AddAngle > addangle;
	float addangletotal;
	float prevaddangletotal;
	int	cdtrack;
	CustomFile_t m_nCustomFiles[ MAX_CUSTOM_FILES ];
	unsigned int m_nFriendsID;
	char m_FriendsName[ MAX_PLAYER_NAME_LENGTH ];
	CEventInfo* events;
};