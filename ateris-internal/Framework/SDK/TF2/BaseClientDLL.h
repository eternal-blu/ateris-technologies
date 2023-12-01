#pragma once
#include "const.h"
#include "bitbuf.h"
#include <Windows.h>
#include "CUtlVector.h"

typedef enum
{
	DPT_Int = 0,
	DPT_Float,
	DPT_Vector,
	DPT_VectorXY,
	DPT_String,
	DPT_Array,
	DPT_DataTable,
	DPT_NUMSendPropTypes
} SendPropType;

enum class ClientFrameStage_t
{
	FRAME_UNDEFINED = -1,
	FRAME_START,
	FRAME_NET_UPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_START,
	FRAME_NET_UPDATE_POSTDATAUPDATE_END,
	FRAME_NET_UPDATE_END,
	FRAME_RENDER_START,
	FRAME_RENDER_END
};

class DVariant
{
public:
	DVariant( ) { Type = DPT_Float; }
	DVariant( float val ) { Type = DPT_Float; Float = val; }
	union
	{
		float	Float;
		int		Int;
		const char* String;
		void* Data;
		float	Vector[ 3 ];
	};
	SendPropType	Type {};
};

#undef GetProp

class RecvTable;
class RecvProp;
class SendProp;
class CSendTablePrecalc;
class CRecvDecoder;
class CViewSetup;

class CRecvProxyData
{
public:
	const RecvProp* Prop;
	DVariant Value;
	int	Element;
	int	ID;
};

typedef void ( *RecvVarProxyFn )( const CRecvProxyData* Data, void* Struct, void* Out );
typedef void ( *ArrayLengthRecvProxyFn )( void* Struct, int ID, int Length );
typedef void ( *DataTableRecvVarProxyFn )( const RecvProp* Prop, void** Out, void* Data, int ID );

class SendTable
{
public:

	typedef SendProp PropType;
	SendProp* Props;
	int	PropsCount;
	const char* NetTableName;
	CSendTablePrecalc* Precalc;


protected:
	bool		Initialized : 1;
	bool		HasBeenWritten : 1;
	bool		HasPropsEncodedAgainstCurrentTickCount : 1;
};

class RecvProp
{
public:
	const char* VarName;
	SendPropType RecvType;
	int	Flags;
	int	StringBufferSize;
	bool InsideArray;
	const void* ExtraData;
	RecvProp* ArrayProp;
	ArrayLengthRecvProxyFn ArrayLengthProxy;
	RecvVarProxyFn ProxyFn;
	DataTableRecvVarProxyFn	DataTableProxyFn;
	RecvTable* DataTable;
	int	Offset;
	int	ElementStride;
	int Elements;
	const char* ParentArrayPropName;
};

class RecvTable
{
public:
	RecvProp* Props;
	int	PropsCount;
	CRecvDecoder* Decoder;
	const char* NetTableName;
	bool Initialized;
	bool InMainList;
};

#undef CreateEvent

class RecvTable;
typedef void* ( *CreateClientClassFn )( int EntNum, int Serial );
typedef void* ( *CreateEventFn )( );

class CClientClass
{
public:
	CreateClientClassFn CreateClientClass;
	CreateEventFn CreateEvent;
	const char* NetworkTable;
	RecvTable* RecvTable;
	CClientClass* Next;
	EClassID ClassID;
};

class IBaseClientDLL
{
public:
	CClientClass* GetAllClasses( )
	{
		return Memory->GetVFunc< CClientClass * ( __thiscall* )( IBaseClientDLL* ) >( this, 8u )( this );
	}

	bool WriteUsercmdDeltaToBuffer( bf_write* buf, int from, int to, bool isnewcommand )
	{
		return Memory->GetVFunc< bool( __thiscall* )( IBaseClientDLL*, bf_write*, int, int, bool ) >( this, 23u )( this, buf, from, to, isnewcommand );
	}

	bool GetPlayerView( CViewSetup& view )
	{
		return Memory->GetVFunc< bool( __thiscall* )( IBaseClientDLL*, CViewSetup& ) >( this, 59u )( this, view );
	}
};

#define FLOW_OUTGOING 0
#define FLOW_INCOMING 1

typedef struct netadr_s
{
public:
	netadr_s( )
	{
		SetIP( 0 );
		SetPort( 0 );
		//SetType(NA_IP);
	}
	netadr_s( const char* pch )
	{
		SetFromString( pch );
	}
	void Clear( ); // invalids Address

	//void SetType(netadrtype_t type);
	void SetPort( unsigned short port );
	bool SetFromSockadr( const struct sockaddr* s );
	void SetIP( unsigned int unIP ); // Sets IP.  unIP is in host order (little-endian)
	void SetIPAndPort( unsigned int unIP, unsigned short usPort )
	{
		SetIP( unIP );
		SetPort( usPort );
	}
	void SetFromString( const char* pch, bool bUseDNS = false ); // if bUseDNS is true then do a DNS lookup if needed

	bool CompareAdr( const netadr_s& a, bool onlyBase = false ) const;
	bool CompareClassBAdr( const netadr_s& a ) const;
	bool CompareClassCAdr( const netadr_s& a ) const;

	//netadrtype_t GetType() const;
	unsigned short GetPort( ) const;
	const char* ToString( bool onlyBase = false ) const; // returns xxx.xxx.xxx.xxx:ppppp
	void ToSockadr( struct sockaddr* s ) const;
	unsigned int GetIP( ) const;

	bool IsLocalhost( ) const;   // true, if this is the localhost IP
	bool IsLoopback( ) const;	// true if engine loopback buffers are used
	bool IsReservedAdr( ) const; // true, if this is a private LAN IP
	bool IsValid( ) const;		// ip & port != 0
	void SetFromSocket( int hSocket );
	// These function names are decorated because the Xbox360 defines macros for ntohl and htonl
	unsigned long addr_ntohl( ) const;
	unsigned long addr_htonl( ) const;
	bool operator==( const netadr_s& netadr ) const
	{
		return ( CompareAdr( netadr ) );
	}
	bool operator<( const netadr_s& netadr ) const;

public: // members are public to avoid to much changes
	//netadrtype_t type;
	unsigned char ip[ 4 ];
	unsigned short port;
} netadr_t;


class INetMessage;
class INetChannelHandler;
class INetChannelInfo
{
public:

	enum
	{
		GENERIC = 0,	// must be first and is default group
		LOCALPLAYER,	// bytes for local player entity update
		OTHERPLAYERS,	// bytes for other players update
		ENTITIES,		// all other entity bytes
		SOUNDS,			// game sounds
		EVENTS,			// event messages
		TEMPENTS,		// temp entities
		USERMESSAGES,	// user messages
		ENTMESSAGES,	// entity messages
		VOICE,			// voice data
		STRINGTABLE,	// a stringtable update
		MOVE,			// client move cmds
		STRINGCMD,		// string command
		SIGNON,			// various signondata
		TOTAL,			// must be last and is not a real group
	};

	virtual const char* GetName( void ) const = 0;	// get channel name
	virtual const char* GetAddress( void ) const = 0; // get channel IP address as string
	virtual float		GetTime( void ) const = 0;	// current net time
	virtual float		GetTimeConnected( void ) const = 0;	// get connection time in seconds
	virtual int			GetBufferSize( void ) const = 0;	// netchannel packet history size
	virtual int			GetDataRate( void ) const = 0; // send data rate in byte/sec

	virtual bool		IsLoopback( void ) const = 0;	// true if loopback channel
	virtual bool		IsTimingOut( void ) const = 0;	// true if timing out
	virtual bool		IsPlayback( void ) const = 0;	// true if demo playback

	virtual float		GetLatency( int flow ) const = 0;	 // current latency (RTT), more accurate but jittering
	virtual float		GetAverageLatency( int flow ) const = 0; // average packet latency in seconds
	virtual float		GetAverageLoss( int flow ) const = 0;	 // avg packet loss[0..1]
	virtual float		GetAverageChoke( int flow ) const = 0;	 // avg packet choke[0..1]
	virtual float		GetAverageData( int flow ) const = 0;	 // data flow in bytes/sec
	virtual float		GetAveragePackets( int flow ) const = 0; // avg packets/sec
	virtual int			GetTotalData( int flow ) const = 0;	 // total flow in/out in bytes
	virtual int			GetTotalPackets( int flow ) const = 0;
	virtual int			GetSequenceNumber( int flow ) const = 0;	// last send seq number
	virtual bool		IsValidPacket( int flow, int frame_number ) const = 0; // true if packet was not lost/dropped/chocked/flushed
	virtual float		GetPacketTime( int flow, int frame_number ) const = 0; // time when packet was send
	virtual int			GetPacketBytes( int flow, int frame_number, int group ) const = 0; // group size of this packet
	virtual bool		GetStreamProgess( int flow, int* received, int* total ) const = 0;  // TCP progress if transmitting
	virtual float		GetTimeSinceLastRecieved( void ) const = 0;	// get time since last recieved packet in seconds
	virtual	float		GetCommandInterpolationAmount( int flow, int frame_number ) const = 0;
	virtual void		GetPacketResponseLatency( int flow, int frame_number, int* pnLatencyMsecs, int* pnChoke ) const = 0;
	virtual void		GetRemoveFramerate( float* pflFrameTime, float* pflFrameTimeStdDeviation, float* pflFrameStartTimeStdDeviation ) const = 0;

	virtual float		GetTimeoutSeconds( ) const = 0;
};


#define MULTIPLAYER_BACKUP 90
#define WEAPON_SUBTYPE_BITS	6
#define	MAX_OSPATH		260			// max length of a filesystem pathname
#define MAX_STREAMS			2    
#define NET_FRAMES_BACKUP	64		// must be power of 2
#define NET_FRAMES_MASK		(NET_FRAMES_BACKUP-1)
#define MAX_SUBCHANNELS		8		// we have 8 alternative send&wait bits
#define SUBCHANNEL_FREE		0	// subchannel is free to use
#define SUBCHANNEL_TOSEND	1	// subchannel has data, but not send yet
#define SUBCHANNEL_WAITING	2   // sbuchannel sent data, waiting for ACK
#define SUBCHANNEL_DIRTY	3	// subchannel is marked as dirty during changelevel
#define DELTA_OFFSET_BITS 5
#define DELTA_OFFSET_MAX ( ( 1 << DELTA_OFFSET_BITS ) - 1 )
#define DELTASIZE_BITS 20	
#define NUM_NEW_COMMAND_BITS		4
#define MAX_NEW_COMMANDS			((1 << NUM_NEW_COMMAND_BITS)-1)
#define NUM_BACKUP_COMMAND_BITS		3
#define MAX_BACKUP_COMMANDS			((1 << NUM_BACKUP_COMMAND_BITS)-1)
#define MAX_COMMANDS				MAX_NEW_COMMANDS + MAX_BACKUP_COMMANDS
#define MAX_PLAYER_NAME_LENGTH		32
#define MAX_CUSTOM_FILES		4		// max 4 files
#define NETMSG_TYPE_BITS	6	// must be 2^NETMSG_TYPE_BITS > SVC_LASTMSG
#define net_NOP 0        // nop command used for padding
#define net_Disconnect 1 // disconnect, last message in connection
#define net_File 2       // file transmission message request/deny
#define net_Tick 3        // send last world tick
#define net_StringCmd 4   // a string command
#define net_SetConVar 5   // sends one/multiple convar settings
#define net_SignonState 6 // signals current signon state
#define clc_ClientInfo 8       
#define clc_Move 9             
#define clc_VoiceData 10       
#define clc_BaselineAck 11     
#define clc_ListenEvents 12    
#define clc_RespondCvarValue 13
#define clc_FileCRCCheck 14    
#define clc_CmdKeyValues 16    
#define FLOW_OUTGOING	0		
#define FLOW_INCOMING	1
#define MAX_FLOWS		2		// in & out

#define DECLARE_BASE_MESSAGE(msgtype)       \
public:                                     \
    bool ReadFromBuffer(bf_read &buffer);   \
    bool WriteToBuffer(bf_write &buffer);   \
    const char *ToString() const;           \
    int GetType() const { return msgtype; } \
    const char *GetName() const { return #msgtype; }

#define DECLARE_NET_MESSAGE(name)     \
    DECLARE_BASE_MESSAGE(net_##name); \
    void *m_pMessageHandler;

#define DECLARE_CLC_MESSAGE(name)     \
    DECLARE_BASE_MESSAGE(clc_##name); \
    void *m_pMessageHandler;

#define DECLARE_NET_MESSAGE(name)     \
    DECLARE_BASE_MESSAGE(net_##name); \
    void *m_pMessageHandler;

#define DECLARE_SVC_MESSAGE( name )		\
	DECLARE_BASE_MESSAGE( svc_##name );	\
	void *m_pMessageHandler;

class INetMessage;
class INetChannel : public INetChannelInfo
{
public:
	virtual ~INetChannel( void )
	{
	};

	virtual void SetDataRate( float rate ) = 0;
	virtual bool RegisterMessage( INetMessage* msg ) = 0;
	virtual bool StartStreaming( unsigned int challengeNr ) = 0;
	virtual void ResetStreaming( void ) = 0;
	virtual void SetTimeout( float seconds ) = 0;
	//virtual void SetDemoRecorder(IDemoRecorder *recorder) = 0;
	virtual void SetChallengeNr( unsigned int chnr ) = 0;

	virtual void Reset( void ) = 0;
	virtual void Clear( void ) = 0;
	virtual void Shutdown( const char* reason ) = 0;

	virtual void ProcessPlayback( void ) = 0;
	virtual bool ProcessStream( void ) = 0;
	virtual void ProcessPacket( struct netpacket_s* packet, bool bHasHeader ) = 0;

	virtual bool SendNetMsg( INetMessage& msg, bool bForceReliable = false, bool bVoice = false ) = 0;
	virtual bool SendData(bf_write &msg, bool bReliable = true) = 0;
	virtual bool SendFile( const char* filename, unsigned int transferID ) = 0;
	virtual void DenyFile( const char* filename, unsigned int transferID ) = 0;
	virtual void RequestFile_OLD( const char* filename, unsigned int transferID ) = 0; // get rid of this function when we version the
	virtual void SetChoked( void ) = 0;
	virtual int SendDatagram(bf_write *data) = 0;
	virtual bool Transmit( bool onlyReliable = false ) = 0;

	virtual const netadr_t& GetRemoteAddress( void ) const = 0;
	virtual INetChannelHandler* GetMsgHandler( void ) const = 0;
	virtual int GetDropNumber( void ) const = 0;
	virtual int GetSocket( void ) const = 0;
	virtual unsigned int GetChallengeNr( void ) const = 0;
	virtual void GetSequenceData( int& nOutSequenceNr, int& nInSequenceNr, int& nOutSequenceNrAck ) = 0;
	virtual void SetSequenceData( int nOutSequenceNr, int nInSequenceNr, int nOutSequenceNrAck ) = 0;

	virtual void UpdateMessageStats( int msggroup, int bits ) = 0;
	virtual bool CanPacket( void ) const = 0;
	virtual bool IsOverflowed( void ) const = 0;
	virtual bool IsTimedOut( void ) const = 0;
	virtual bool HasPendingReliableData( void ) = 0;

	virtual void SetFileTransmissionMode( bool bBackgroundMode ) = 0;
	virtual void SetCompressionMode( bool bUseCompression ) = 0;
	virtual unsigned int RequestFile( const char* filename ) = 0;
	virtual float GetTimeSinceLastReceived( void ) const = 0; // get time since last received packet in seconds

	virtual void SetMaxBufferSize( bool bReliable, int nBYTEs, bool bVoice = false ) = 0;

	virtual bool IsNull( ) const = 0;
	virtual int GetNumBitsWritten( bool bReliable ) = 0;
	virtual void SetInterpolationAmount( float flInterpolationAmount ) = 0;
	virtual void SetRemoteFramerate( float flFrameTime, float flFrameTimeStdDeviation ) = 0;

	// Max # of payload BYTEs before we must split/fragment the packet
	virtual void SetMaxRoutablePayloadSize( int nSplitSize ) = 0;
	virtual int GetMaxRoutablePayloadSize( ) = 0;

	virtual int GetProtocolVersion( ) = 0;
};

class INetMessage
{
public:
	virtual ~INetMessage( ) {};

	// Use these to setup who can hear whose voice.
	// Pass in client indices (which are their ent indices - 1).

	virtual void SetNetChannel( INetChannel* netchan ) = 0; // netchannel this message is from/for
	virtual void SetReliable( bool state ) = 0;             // set to true if it's a reliable message

	virtual bool Process( void ) = 0; // calles the recently set handler to process this message

	virtual bool ReadFromBuffer( bf_read& buffer ) = 0; // returns true if parsing was OK
	virtual bool WriteToBuffer( bf_write& buffer ) = 0; // returns true if writing was OK

	virtual bool IsReliable( void ) const = 0; // true, if message needs reliable handling

	virtual int GetType( void ) const = 0;         // returns module specific header tag eg svc_serverinfo
	virtual int GetGroup( void ) const = 0;        // returns net message group of this message
	virtual const char* GetName( void ) const = 0; // returns network message name, eg "svc_serverinfo"
	virtual INetChannel* GetNetChannel( void ) const = 0;
	virtual const char* ToString( void ) const = 0; // returns a human readable string about message content
};

typedef void* FileHandle_t;
#define MAX_OSPATH 260
#define UDP_HEADER_SIZE 28

class CNetChannel : public INetChannel
{

public: // netchan structurs
	typedef struct dataFragments_s
	{
		FileHandle_t file;                 // open file handle
		char         filename[ MAX_OSPATH ]; // filename
		char* buffer;               // if NULL it's a file
		unsigned int BYTEs;                // size in BYTEs
		unsigned int bits;                 // size in bits
		unsigned int transferID;           // only for files
		bool         isCompressed;         // true if data is bzip compressed
		unsigned int nUncompressedSize;    // full size in BYTEs
		bool         asTCP;                // send as TCP stream
		int          numFragments;         // number of total fragments
		int          ackedFragments;       // number of fragments send & acknowledged
		int          pendingFragments;     // number of fragments send, but not acknowledged yet
	} dataFragments_t;

	struct subChannel_s
	{
		int startFraggment[ MAX_STREAMS ];
		int numFragments[ MAX_STREAMS ];
		int sendSeqNr;
		int state; // 0 = free, 1 = scheduled to send, 2 = send & waiting, 3 = dirty
		int index; // index in m_SubChannels[]

		void Free( )
		{
			state = SUBCHANNEL_FREE;
			sendSeqNr = -1;
			for ( int i = 0; i < MAX_STREAMS; i++ )
			{
				numFragments[ i ] = 0;
				startFraggment[ i ] = -1;
			}
		}
	};

	typedef struct netframe_s
	{
		float          time;        // net_time received/send
		int            size;        // total size in BYTEs
		float          latency;     // raw ping for this packet, not cleaned. set when acknowledged otherwise -1.
		float          avg_latency; // averaged ping for this packet
		bool           valid;       // false if dropped, lost, flushed
		int            choked;      // number of previously chocked packets
		int            dropped;
		float          m_flInterpolationAmount;
		unsigned short msggroups[ INetChannelInfo::TOTAL ]; // received BYTEs for each message group
	} netframe_t;
	typedef struct
	{
		float       nextcompute;      // Time when we should recompute k/sec data
		float       avgBYTEspersec;   // average BYTEs/sec
		float       avgpacketspersec; // average packets/sec
		float       avgloss;          // average packet loss [0..1]
		float       avgchoke;         // average packet choke [0..1]
		float       avglatency;       // average ping, not cleaned
		float       latency;          // current ping, more accurate also more jittering
		int         totalpackets;     // total processed packets
		int         totalBYTEs;       // total processed BYTEs
		int         currentindex;     // current frame index
		netframe_t  frames[ 64 ];       // frame history
		netframe_t* currentframe;     // current frame
	} netflow_t;
	typedef struct netpacket_s
	{
		netadr_t		from;		// sender IP
		int				source;		// received source 
		double			received;	// received time
		unsigned char* data;		// pointer to raw packet data
		bf_read			message;	// easy bitbuf data access
		int				size;		// size in bytes
		int				wiresize;   // size in bytes before decompression
		bool			stream;		// was send as stream
		struct netpacket_s* pNext;	// for internal use, should be NULL in public
	} netpacket_t;

public:
	bool ProcessingMessages;
	bool ShouldDelete;

	int OutSequenceNr;
	int InSequenceNr;
	int OutSequenceNrAck;
	int OutReliableState;
	int InReliableState;
	int ChokedPackets;

	// Reliable data buffer, send which each packet (or put in waiting list)
	bf_write	m_StreamReliable;
	CUtlMemory<byte> m_ReliableDataBuffer;

	// unreliable message buffer, cleared which each packet
	bf_write	m_StreamUnreliable;
	CUtlMemory<byte> m_UnreliableDataBuffer;

	bf_write	m_StreamVoice;
	CUtlMemory<byte> m_VoiceDataBuffer;

	int			m_Socket;   // NS_SERVER or NS_CLIENT index, depending on channel.
	int			m_StreamSocket;	// TCP socket handle

	unsigned int m_MaxReliablePayloadSize;	// max size of reliable payload in a single packet	

	// Address this channel is talking to.
	netadr_t	remote_address;

	// For timeouts.  Time last message was received.
	float		last_received;
	// Time when channel was connected.
	double      connect_time;

	// Bandwidth choke
	// Bytes per second
	int			m_Rate;
	// If realtime > cleartime, free to send next packet
	double		m_fClearTime;

	CUtlVector<dataFragments_t*>	m_WaitingList[ MAX_STREAMS ];	// waiting list for reliable data and file transfer
	dataFragments_t					m_ReceiveList[ MAX_STREAMS ]; // receive buffers for streams
	subChannel_s					m_SubChannels[ MAX_SUBCHANNELS ];

	unsigned int	m_FileRequestCounter;	// increasing counter with each file request
	bool			m_bFileBackgroundTranmission; // if true, only send 1 fragment per packet
	bool			m_bUseCompression;	// if true, larger reliable data will be bzip compressed

	// TCP stream state maschine:
	bool		m_StreamActive;		// true if TCP is active
	int			m_SteamType;		// STREAM_CMD_*
	int			m_StreamSeqNr;		// each blob send of TCP as an increasing ID
	int			m_StreamLength;		// total length of current stream blob
	int			m_StreamReceived;	// length of already received bytes
	char		m_SteamFile[ MAX_OSPATH ];	// if receiving file, this is it's name
	CUtlMemory<byte> m_StreamData;			// Here goes the stream data (if not file). Only allocated if we're going to use it.

	// packet history
	netflow_t		m_DataFlow[ MAX_FLOWS ];

	int				m_MsgStats[ INetChannelInfo::TOTAL ];	// total bytes for each message group


	int				m_PacketDrop;	// packets lost before getting last update (was global net_drop)

	char			m_Name[ 32 ];		// channel name

	unsigned int	m_ChallengeNr;	// unique, random challenge number 

	float		m_Timeout;		// in seconds 

	INetChannelHandler* m_MessageHandler;	// who registers and processes messages
	CUtlVector<INetMessage*>	m_NetMessages;		// list of registered message
	void* m_DemoRecorder;			// if != NULL points to a recording/playback demo object
	int							m_nQueuedPackets;

	float						m_flInterpolationAmount;
	float						m_flRemoteFrameTime;
	float						m_flRemoteFrameTimeStdDeviation;
	int							m_nMaxRoutablePayloadSize;

	int							m_nSplitPacketSequence;
	bool						m_bStreamContainsChallenge;  // true if PACKET_FLAG_CHALLENGE was set when receiving packets from the sender

	int							m_nProtocolVersion;
};

class CNetMessage : public INetMessage {
public:
	CNetMessage() {
		m_bReliable = true;
		m_NetChannel = nullptr;
	}

	virtual ~CNetMessage() {};

	virtual int GetGroup() const { return INetChannelInfo::GENERIC; }
	INetChannel* GetNetChannel() const { return m_NetChannel; }

	virtual void SetReliable(bool state) { m_bReliable = state; };
	virtual bool IsReliable() const { return m_bReliable; };
	virtual void SetNetChannel(INetChannel* netchan) { m_NetChannel = netchan; }
	virtual bool Process() { return false; }; // no handler set

protected:
	bool m_bReliable;          // true if message should be send reliable
	INetChannel* m_NetChannel; // netchannel this message is from/for
};

class CLC_Move : public CNetMessage
{
public:
	bool ReadFromBuffer(bf_read& buffer)
	{
		m_nNewCommands = buffer.ReadUBitLong(NUM_NEW_COMMAND_BITS);
		m_nBackupCommands = buffer.ReadUBitLong(NUM_BACKUP_COMMAND_BITS);
		m_nLength = buffer.ReadWord();
		m_DataIn = buffer;
		return buffer.SeekRelative(m_nLength);
	}

	bool WriteToBuffer(bf_write& buffer)
	{
		buffer.WriteUBitLong(GetType(), NETMSG_TYPE_BITS);
		m_nLength = m_DataOut.GetNumBitsWritten();

		buffer.WriteUBitLong(m_nNewCommands, NUM_NEW_COMMAND_BITS);
		buffer.WriteUBitLong(m_nBackupCommands, NUM_BACKUP_COMMAND_BITS);

		buffer.WriteWord(m_nLength);

		return buffer.WriteBits(m_DataOut.GetData(), m_nLength);
	}

	const char* ToString() const { return "CLC_Move"; }
	int GetType() const { return clc_Move; }
	const char* GetName() const { return "clc_Move"; }
	void* m_pMessageHandler;
	int GetGroup() const { return INetChannelInfo::MOVE; }
	CLC_Move() { m_bReliable = false; }

public:
	int m_nBackupCommands;
	int m_nNewCommands;
	int m_nLength;
	bf_read m_DataIn;
	bf_write m_DataOut;
};

class NET_SetConVar : public CNetMessage
{
public:
	int GetGroup( ) const
	{
		return INetChannelInfo::STRINGCMD;
	}

	bool ReadFromBuffer( bf_read& buffer )
	{
		int numvars = buffer.ReadByte( );

		for ( int i = 0; i < numvars; i++ )
		{
			cvar_t var;
			buffer.ReadString( var.Name, sizeof( var.Name ) );
			buffer.ReadString( var.Value, sizeof( var.Value ) );

		}
		return !buffer.IsOverflowed( );
	}

	bool WriteToBuffer( bf_write& buffer )
	{
		buffer.WriteUBitLong( GetType( ), NETMSG_TYPE_BITS );

		const int numvars = 1;

		buffer.WriteByte( numvars );

		buffer.WriteString( ConVar.Name );
		buffer.WriteString( ConVar.Value );

		return !buffer.IsOverflowed( );
	}

	const char* ToString( ) const { return "NET_SetConVar"; }
	int GetType( ) const { return net_SetConVar; }
	const char* GetName( ) const { return "net_SetConVar"; }
	
	NET_SetConVar( ) { }
	NET_SetConVar( const char* name, const char* value )
	{
		cvar_t cvar;
		strncpy_s( cvar.Name, name, MAX_OSPATH );
		strncpy_s( cvar.Value, value, MAX_OSPATH );
		ConVar = cvar;
	}

public:
	typedef struct cvar_s
	{
		char Name[ MAX_OSPATH ];
		char Value[ MAX_OSPATH ];
	} cvar_t;
	cvar_t ConVar;
};