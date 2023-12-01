#pragma once

// handle to a communication pipe to the Steam client
typedef int HSteamPipe;
// handle to single instance of a steam user
typedef int HSteamUser;
// reference to a steam call, to filter results by
typedef int HSteamCall;

typedef void( *SteamAPIWarningMessageHook_t )( int hpipe, const char* message );

#if defined(__x86_64__) || defined(_WIN64)
#define X64BITS
#endif

typedef unsigned char uint8;
typedef signed char int8;

#if defined( _MSV_VER )

typedef __int16 int16;
typedef unsigned __int16 uint16;
typedef __int32 int32;
typedef unsigned __int32 uint32;
typedef __int64 int64;
typedef unsigned __int64 uint64;

#ifdef X64BITS
typedef __int64 intp;				// intp is an integer that can accomodate a pointer
typedef unsigned __int64 uintp;		// (ie, sizeof(intp) >= sizeof(int) && sizeof(intp) >= sizeof(void *)
#else
typedef __int32 intp;
typedef unsigned __int32 uintp;
#endif

#else // _MSV_VER

typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;
#ifdef X64BITS
typedef long long intp;
typedef unsigned long long uintp;
#else
typedef int intp;
typedef unsigned int uintp;
#endif

#endif // else _MSV_VER

// handle to a Steam API call
typedef uint64 SteamAPICall_t;

class CGameID
{
public:
	struct GameID_t
	{
#ifdef VALVE_BIG_ENDIAN
		unsigned int m_nModID : 32;
		unsigned int m_nType : 8;
		unsigned int m_nAppID : 24;
#else
		unsigned int m_nAppID : 24;
		unsigned int m_nType : 8;
		unsigned int m_nModID : 32;
#endif
	};

	union
	{
		uint64 m_ulGameID;
		GameID_t m_gameID;
	};
};

enum EAccountType
{
	k_EAccountTypeInvalid = 0,
	k_EAccountTypeIndividual = 1,		// single user account
	k_EAccountTypeMultiseat = 2,		// multiseat (e.g. cybercafe) account
	k_EAccountTypeGameServer = 3,		// game server account
	k_EAccountTypeAnonGameServer = 4,	// anonymous game server account
	k_EAccountTypePending = 5,			// pending
	k_EAccountTypeContentServer = 6,	// content server
	k_EAccountTypeClan = 7,
	k_EAccountTypeChat = 8,
	k_EAccountTypeConsoleUser = 9,		// Fake SteamID for local PSN account on PS3 or Live account on 360, etc.
	k_EAccountTypeAnonUser = 10,

	// Max of 16 items in this field
	k_EAccountTypeMax
};

enum EUniverse
{
	k_EUniverseInvalid = 0,
	k_EUniversePublic = 1,
	k_EUniverseBeta = 2,
	k_EUniverseInternal = 3,
	k_EUniverseDev = 4,
	//	k_EUniverseRC = 5, // Removed

	k_EUniverseMax
};

class CSteamID
{
public:
	CSteamID( )
	{
		m_steamid.m_comp.m_unAccountID = 0;
		m_steamid.m_comp.m_EAccountType = k_EAccountTypeInvalid;
		m_steamid.m_comp.m_EUniverse = k_EUniverseInvalid;
		m_steamid.m_comp.m_unAccountInstance = 0;
	}

	CSteamID( uint32 unAccountID, unsigned int unAccountInstance, EUniverse eUniverse, EAccountType eAccountType )
	{
		InstancedSet( unAccountID, unAccountInstance, eUniverse, eAccountType );
	}

	void InstancedSet( uint32 unAccountID, uint32 unInstance, EUniverse eUniverse, EAccountType eAccountType )
	{
		m_steamid.m_comp.m_unAccountID = unAccountID;
		m_steamid.m_comp.m_EUniverse = eUniverse;
		m_steamid.m_comp.m_EAccountType = eAccountType;
		m_steamid.m_comp.m_unAccountInstance = unInstance;
	}

	uint64_t ToUint64( ) const {
		return m_steamid.m_unAll64Bits;
	}

private:
	union SteamID_t
	{
		struct SteamIDComponent_t
		{
			unsigned int				m_unAccountID : 32;			// unique account identifier
			unsigned int		m_unAccountInstance : 20;	// dynamic instance ID
			unsigned int		m_EAccountType : 4;			// type of account - can't show as EAccountType, due to signed / unsigned difference
			EUniverse			m_EUniverse : 8;	// universe this account belongs to
		} m_comp;

		unsigned long long m_unAll64Bits;
	} m_steamid;
};

struct FriendGameInfo_t
{
	CGameID m_gameID;
	uint32 m_unGameIP;
	uint16 m_usGamePort;
	uint16 m_usQueryPort;
	CSteamID m_steamIDLobby;
};

enum DataCenter_t
{
	DC_AMS = ( 1 << 0 ),	//Amsterdam
	DC_ATL = ( 1 << 1 ),	//Atlanta
	DC_BOM = ( 1 << 2 ),	//Mumbai
	DC_DXB = ( 1 << 3 ),	//Dubai
	DC_EAT = ( 1 << 4 ),	//Moses Lake
	DC_MWH = ( 1 << 5 ),	//Washington
	DC_FRA = ( 1 << 6 ),	//Frankfurt
	DC_GNRT = ( 1 << 7 ),	//Tokyo (GNRT)
	DC_GRU = ( 1 << 8 ),	//Sao Paulo
	DC_HKG = ( 1 << 9 ),	//Hong Kong
	DC_IAD = ( 1 << 10 ),	//Virginia
	DC_JNB = ( 1 << 11 ),	//Johannesburg
	DC_LAX = ( 1 << 12 ),	//Los Angeles
	DC_LHR = ( 1 << 13 ),	//London
	DC_LIM = ( 1 << 14 ),	//Lima
	DC_LUX = ( 1 << 15 ),	//Luxembourg
	DC_MAA = ( 1 << 16 ),	//Chennai
	DC_MAD = ( 1 << 17 ),	//Madrid
	DC_MAN = ( 1 << 18 ),	//Manila
	DC_OKC = ( 1 << 19 ),	//Oklahoma City
	DC_ORD = ( 1 << 20 ),	//Chicago
	DC_PAR = ( 1 << 21 ),	//Paris
	DC_SCL = ( 1 << 22 ),	//Santiago
	DC_SEA = ( 1 << 23 ),	//Seattle
	DC_SGP = ( 1 << 24 ),	//Singapore
	DC_STO = ( 1 << 25 ),	//Stockholm
	DC_SYD = ( 1 << 26 ),	//Sydney
	DC_TYO = ( 1 << 27 ),	//Tokyo
	DC_VIE = ( 1 << 28 ),	//Vienna
	DC_WAW = ( 1 << 29 )	//Warsaw
};

static const std::map<std::string, int> DataCenterHashMap{
	{"ams", DC_AMS},
	{ "atl",  DC_ATL },
	{ "bom",  DC_BOM },
	{ "dxb",  DC_DXB },
	{ "eat",  DC_EAT },
	{ "mwh",  DC_MWH },
	{ "fra",  DC_FRA },
	{ "gnrt", DC_GNRT },
	{ "gru",  DC_GRU },
	{ "hkg",  DC_HKG },
	{ "iad",  DC_IAD },
	{ "jnb",  DC_JNB },
	{ "lax",  DC_LAX },
	{ "lhr",  DC_LHR },
	{ "lim",  DC_LIM },
	{ "lux",  DC_LUX },
	{ "maa",  DC_MAA },
	{ "mad",  DC_MAD },
	{ "man",  DC_MAN },
	{ "okc",  DC_OKC },
	{ "ord",  DC_ORD },
	{ "par",  DC_PAR },
	{ "scl",  DC_SCL },
	{ "sea",  DC_SEA },
	{ "sgp",  DC_SGP },
	{ "sto",  DC_STO },
	{ "sto2", DC_STO },
	{ "syd",  DC_SYD },
	{ "tyo",  DC_TYO },
	{ "tyo2", DC_TYO },
	{ "tyo1", DC_TYO },
	{ "vie",  DC_VIE },
	{ "waw",  DC_WAW }
};

struct SteamNetworkingPOPID
{
	unsigned v;
	/* 'out' must point to array with capacity at least 5 */
	inline void ToString( char* out )
	{
		out[ 0 ] = char( v >> 16 );
		out[ 1 ] = char( v >> 8 );
		out[ 2 ] = char( v );
		out[ 3 ] = char( v >> 24 );
		out[ 4 ] = 0;
	}
};

class ISteamNetworkingUtils
{
public:

};

enum EChatEntryType;
enum EPersonaState;
enum EFriendRelationship;
enum EUserRestriction;
enum EOverlayToStoreFlag;
enum ENotificationPosition;
enum ESteamAPICallFailure;

#define STEAMWORKS_STRUCT_RETURN_0(returnType, functionName) virtual returnType functionName() = 0;
#define STEAMWORKS_STRUCT_RETURN_1(returnType, functionName, arg1Type, arg1Name) virtual returnType functionName( arg1Type arg1Name ) = 0;
#define STEAMWORKS_STRUCT_RETURN_2(returnType, functionName, arg1Type, arg1Name, arg2Type, arg2Name) virtual returnType functionName( arg1Type arg1Name, arg2Type arg2Name ) = 0;
#define STEAMWORKS_STRUCT_RETURN_3(returnType, functionName, arg1Type, arg1Name, arg2Type, arg2Name, arg3Type, arg3Name) virtual returnType functionName( arg1Type arg1Name, arg2Type arg2Name, arg3Type arg3Name ) = 0;

class ISteamUser
{
public:
	// returns the HSteamUser this interface represents
	// this is only used internally by the API, and by a few select interfaces that support multi-user
	virtual HSteamUser GetHSteamUser( ) = 0;

	// returns true if the Steam client current has a live connection to the Steam servers.
	// If false, it means there is no active connection due to either a networking issue on the local machine, or the Steam server is down/busy.
	// The Steam client will automatically be trying to recreate the connection as often as possible.
	virtual bool BLoggedOn( ) = 0;

	// returns the CSteamID of the account currently logged into the Steam client
	// a CSteamID is a unique identifier for an account, and used to differentiate users in all parts of the Steamworks API
	STEAMWORKS_STRUCT_RETURN_0( CSteamID, GetSteamID )
};

class ISteamGameServer;
class ISteamFriends;
class ISteamUtils;
class ISteamClient
{
public:
	// Creates a communication pipe to the Steam client
	virtual HSteamPipe CreateSteamPipe( ) = 0;

	// Releases a previously created communications pipe
	virtual bool BReleaseSteamPipe( HSteamPipe hSteamPipe ) = 0;

	// connects to an existing global user, failing if none exists
	// used by the game to coordinate with the steamUI
	virtual HSteamUser ConnectToGlobalUser( HSteamPipe hSteamPipe ) = 0;

	// used by game servers, create a steam user that won't be shared with anyone else
	virtual HSteamUser CreateLocalUser( HSteamPipe* phSteamPipe, EAccountType eAccountType ) = 0;

	// removes an allocated user
	virtual void ReleaseUser( HSteamPipe hSteamPipe, HSteamUser hUser ) = 0;

	// retrieves the ISteamUser interface associated with the handle
	virtual ISteamUser* GetISteamUser( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;

	// retrieves the ISteamGameServer interface associated with the handle
	virtual ISteamGameServer* GetISteamGameServer( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;

	// set the local IP and Port to bind to
	// this must be set before CreateLocalUser()
	virtual void SetLocalIPBinding( uint32 unIP, uint16 usPort ) = 0;

	// returns the ISteamFriends interface
	virtual ISteamFriends* GetISteamFriends( HSteamUser hSteamUser, HSteamPipe hSteamPipe, const char* pchVersion ) = 0;

	// returns the ISteamUtils interface
	virtual ISteamUtils* GetISteamUtils( HSteamPipe hSteamPipe, const char* pchVersion ) = 0;
};

class ISteamUtils007
{
public:
	// return the number of seconds since the user
	virtual uint32 GetSecondsSinceAppActive( ) = 0;
	virtual uint32 GetSecondsSinceComputerActive( ) = 0;

	// the universe this client is connecting to
	virtual EUniverse GetConnectedUniverse( ) = 0;

	// Steam server time - in PST, number of seconds since January 1, 1970 (i.e unix time)
	virtual uint32 GetServerRealTime( ) = 0;

	// returns the 2 digit ISO 3166-1-alpha-2 format country code this client is running in (as looked up via an IP-to-location database)
	// e.g "US" or "UK".
	virtual const char* GetIPCountry( ) = 0;

	// returns true if the image exists, and valid sizes were filled out
	virtual bool GetImageSize( int iImage, uint32* pnWidth, uint32* pnHeight ) = 0;

	// returns true if the image exists, and the buffer was successfully filled out
	// results are returned in RGBA format
	// the destination buffer size should be 4 * height * width * sizeof(char)
	virtual bool GetImageRGBA( int iImage, uint8* pubDest, int nDestBufferSize ) = 0;

	// returns the IP of the reporting server for valve - currently only used in Source engine games
	virtual bool GetCSERIPPort( uint32* unIP, uint16* usPort ) = 0;

	// return the amount of battery power left in the current system in % [0..100], 255 for being on AC power
	virtual uint8 GetCurrentBatteryPower( ) = 0;

	// returns the appID of the current process
	virtual uint32 GetAppID( ) = 0;

	// Sets the position where the overlay instance for the currently calling game should show notifications.
	// This position is per-game and if this function is called from outside of a game context it will do nothing.
	virtual void SetOverlayNotificationPosition( ENotificationPosition eNotificationPosition ) = 0;

	// API asynchronous call results
	// can be used directly, but more commonly used via the callback dispatch API (see steam_api.h)
	virtual bool IsAPICallCompleted( SteamAPICall_t hSteamAPICall, bool* pbFailed ) = 0;
	virtual ESteamAPICallFailure GetAPICallFailureReason( SteamAPICall_t hSteamAPICall ) = 0;
	virtual bool GetAPICallResult( SteamAPICall_t hSteamAPICall, void* pCallback, int cubCallback, int iCallbackExpected, bool* pbFailed ) = 0;

	// this needs to be called every frame to process matchmaking results
	// redundant if you're already calling SteamAPI_RunCallbacks()
	virtual void RunFrame( ) = 0;

	// returns the number of IPC calls made since the last time this function was called
	// Used for perf debugging so you can understand how many IPC calls your game makes per frame
	// Every IPC call is at minimum a thread context switch if not a process one so you want to rate
	// control how often you do them.
	virtual uint32 GetIPCCallCount( ) = 0;

	// API warning handling
	// 'int' is the severity; 0 for msg, 1 for warning
	// 'const char *' is the text of the message
	// callbacks will occur directly after the API function is called that generated the warning or message
	virtual void SetWarningMessageHook( SteamAPIWarningMessageHook_t pFunction ) = 0;

	// Returns true if the overlay is running & the user can access it. The overlay process could take a few seconds to
	// start & hook the game process, so this function will initially return false while the overlay is loading.
	virtual bool IsOverlayEnabled( ) = 0;

	// Normally this call is unneeded if your game has a constantly running frame loop that calls the
	// D3D Present API, or OGL SwapBuffers API every frame.
	//
	// However, if you have a game that only refreshes the screen on an event driven basis then that can break
	// the overlay, as it uses your Present/SwapBuffers calls to drive it's internal frame loop and it may also
	// need to Present() to the screen any time an even needing a notification happens or when the overlay is
	// brought up over the game by a user.  You can use this API to ask the overlay if it currently need a present
	// in that case, and then you can check for this periodically (roughly 33hz is desirable) and make sure you
	// refresh the screen with Present or SwapBuffers to allow the overlay to do it's work.
	virtual bool BOverlayNeedsPresent( ) = 0;

	// Asynchronous call to check if an executable file has been signed using the public key set on the signing tab
	// of the partner site, for example to refuse to load modified executable files.
	// The result is returned in CheckFileSignature_t.
	//   k_ECheckFileSignatureNoSignaturesFoundForThisApp - This app has not been configured on the signing tab of the partner site to enable this function.
	//   k_ECheckFileSignatureNoSignaturesFoundForThisFile - This file is not listed on the signing tab for the partner site.
	//   k_ECheckFileSignatureFileNotFound - The file does not exist on disk.
	//   k_ECheckFileSignatureInvalidSignature - The file exists, and the signing tab has been set for this file, but the file is either not signed or the signature does not match.
	//   k_ECheckFileSignatureValidSignature - The file is signed and the signature is valid.
	virtual SteamAPICall_t CheckFileSignature( const char* szFileName ) = 0;
};

class ISteamFriends015
{
public:
	virtual const char* GetPersonaName( ) = 0;
	virtual SteamAPICall_t SetPersonaName( const char* pchPersonaName ) = 0;
	virtual EPersonaState GetPersonaState( ) = 0;
	virtual int GetFriendCount( int iFriendFlags ) = 0;
	STEAMWORKS_STRUCT_RETURN_2( CSteamID, GetFriendByIndex, int, iFriend, int, iFriendFlags ) /*virtual CSteamID GetFriendByIndex( int iFriend, int iFriendFlags ) = 0;*/
	virtual EFriendRelationship GetFriendRelationship( CSteamID steamIDFriend ) = 0;
	virtual EPersonaState GetFriendPersonaState( CSteamID steamIDFriend ) = 0;
	virtual const char* GetFriendPersonaName( CSteamID steamIDFriend ) = 0;
	virtual bool GetFriendGamePlayed( CSteamID steamID, FriendGameInfo_t* pGamePlayInfo ) = 0;
	virtual const char* GetFriendPersonaNameHistory( CSteamID steamIDFriend, int iPersonaName ) = 0;
	virtual int GetFriendSteamLevel( CSteamID steamIDFriend ) = 0;
	virtual const char* GetPlayerNickname( CSteamID steamIDPlayer ) = 0;
	virtual int16 GetFriendsGroupCount( ) = 0;
	virtual int16 GetFriendsGroupIDByIndex( int32 ) = 0;
	virtual const char* GetFriendsGroupName( int16 ) = 0;
	virtual int GetFriendsGroupMembersCount( int16 ) = 0;
	virtual int GetFriendsGroupMembersList( int16, CSteamID*, int32 ) = 0;
	virtual bool HasFriend( CSteamID steamIDFriend, int iFriendFlags ) = 0;
	virtual int GetClanCount( ) = 0;
	STEAMWORKS_STRUCT_RETURN_1( CSteamID, GetClanByIndex, int, iClan ) /*virtual CSteamID GetClanByIndex( int iClan ) = 0;*/
	virtual const char* GetClanName( CSteamID steamIDClan ) = 0;
	virtual const char* GetClanTag( CSteamID steamIDClan ) = 0;
	virtual bool GetClanActivityCounts( CSteamID steamID, int* pnOnline, int* pnInGame, int* pnChatting ) = 0;
	virtual SteamAPICall_t DownloadClanActivityCounts( CSteamID groupIDs[ ], int nIds ) = 0;
	virtual int GetFriendCountFromSource( CSteamID steamIDSource ) = 0;
	STEAMWORKS_STRUCT_RETURN_2( CSteamID, GetFriendFromSourceByIndex, CSteamID, steamIDSource, int, iFriend ) /*virtual CSteamID GetFriendFromSourceByIndex( CSteamID steamIDSource, int iFriend ) = 0;*/
	virtual bool IsUserInSource( CSteamID steamIDUser, CSteamID steamIDSource ) = 0;
	virtual void SetInGameVoiceSpeaking( CSteamID steamIDUser, bool bSpeaking ) = 0;
	virtual void ActivateGameOverlay( const char* pchDialog ) = 0;
	virtual void ActivateGameOverlayToUser( const char* pchDialog, CSteamID steamID ) = 0;
	virtual void ActivateGameOverlayToWebPage( const char* pchURL ) = 0;
	virtual void ActivateGameOverlayToStore( uint32 nAppID, EOverlayToStoreFlag eFlag ) = 0;
	virtual void SetPlayedWith( CSteamID steamIDUserPlayedWith ) = 0;
	virtual void ActivateGameOverlayInviteDialog( CSteamID steamIDLobby ) = 0;
	virtual int GetSmallFriendAvatar( CSteamID steamIDFriend ) = 0;
	virtual int GetMediumFriendAvatar( CSteamID steamIDFriend ) = 0;
	virtual int GetLargeFriendAvatar( CSteamID steamIDFriend ) = 0;
	virtual bool RequestUserInformation( CSteamID steamIDUser, bool bRequireNameOnly ) = 0;
	virtual SteamAPICall_t RequestClanOfficerList( CSteamID steamIDClan ) = 0;
	STEAMWORKS_STRUCT_RETURN_1( CSteamID, GetClanOwner, CSteamID, steamIDClan ) /*virtual CSteamID GetClanOwner( CSteamID steamIDClan ) = 0;*/
	virtual int GetClanOfficerCount( CSteamID steamIDClan ) = 0;
	STEAMWORKS_STRUCT_RETURN_2( CSteamID, GetClanOfficerByIndex, CSteamID, steamIDClan, int, iOfficer ) /*virtual CSteamID GetClanOfficerByIndex( CSteamID steamIDClan, int iOfficer ) = 0;*/
	virtual EUserRestriction GetUserRestrictions( ) = 0;
	virtual bool SetRichPresence( const char* pchKey, const char* pchValue ) = 0;
	virtual void ClearRichPresence( ) = 0;
	virtual const char* GetFriendRichPresence( CSteamID steamIDFriend, const char* pchKey ) = 0;
	virtual int GetFriendRichPresenceKeyCount( CSteamID steamIDFriend ) = 0;
	virtual const char* GetFriendRichPresenceKeyByIndex( CSteamID steamIDFriend, int iKey ) = 0;
	virtual void RequestFriendRichPresence( CSteamID steamIDFriend ) = 0;
	virtual bool InviteUserToGame( CSteamID steamIDFriend, const char* pchConnectString ) = 0;
	virtual int GetCoplayFriendCount( ) = 0;
	STEAMWORKS_STRUCT_RETURN_1( CSteamID, GetCoplayFriend, int, iCoplayFriend ) /*virtual CSteamID GetCoplayFriend( int iCoplayFriend ) = 0;*/
	virtual int GetFriendCoplayTime( CSteamID steamIDFriend ) = 0;
	virtual uint32 GetFriendCoplayGame( CSteamID steamIDFriend ) = 0;
	virtual SteamAPICall_t JoinClanChatRoom( CSteamID steamIDClan ) = 0;
	virtual bool LeaveClanChatRoom( CSteamID steamIDClan ) = 0;
	virtual int GetClanChatMemberCount( CSteamID steamIDClan ) = 0;
	STEAMWORKS_STRUCT_RETURN_2( CSteamID, GetChatMemberByIndex, CSteamID, steamIDClan, int, iUser ) /*virtual CSteamID GetChatMemberByIndex( CSteamID steamIDClan, int iUser ) = 0;*/
	virtual bool SendClanChatMessage( CSteamID steamIDClanChat, const char* pchText ) = 0;
	virtual int GetClanChatMessage( CSteamID steamIDClanChat, int iMessage, void* prgchText, int cchTextMax, EChatEntryType* peChatEntryType, CSteamID* pSteamIDChatter ) = 0;
	virtual bool IsClanChatAdmin( CSteamID steamIDClanChat, CSteamID steamIDUser ) = 0;
	virtual bool IsClanChatWindowOpenInSteam( CSteamID steamIDClanChat ) = 0;
	virtual bool OpenClanChatWindowInSteam( CSteamID steamIDClanChat ) = 0;
	virtual bool CloseClanChatWindowInSteam( CSteamID steamIDClanChat ) = 0;
	virtual bool SetListenForFriendsMessages( bool bInterceptEnabled ) = 0;
	virtual bool ReplyToFriendMessage( CSteamID steamIDFriend, const char* pchMsgToSend ) = 0;
	virtual int GetFriendMessage( CSteamID steamIDFriend, int iMessageID, void* pvData, int cubData, EChatEntryType* peChatEntryType ) = 0;
	virtual SteamAPICall_t GetFollowerCount( CSteamID steamID ) = 0;
	virtual SteamAPICall_t IsFollowing( CSteamID steamID ) = 0;
	virtual SteamAPICall_t EnumerateFollowingList( uint32 unStartIndex ) = 0;
};