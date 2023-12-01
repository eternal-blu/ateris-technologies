#pragma once

enum EMatchGroup
{
    k_nMatchGroup_Invalid = -1,
    k_nMatchGroup_First = 0,

    k_nMatchGroup_MvM_Practice = 0,
    k_nMatchGroup_MvM_MannUp,

    k_nMatchGroup_Ladder_6v6,
    k_nMatchGroup_Ladder_9v9,
    k_nMatchGroup_Ladder_12v12,

    k_nMatchGroup_Casual_6v6,
    k_nMatchGroup_Casual_9v9,
    k_nMatchGroup_Casual_12v12,

    k_nMatchGroup_Count,
};

class CTFParty
{
public:
};

//game coordinator
class CTFGCClientSystem
{
public:
	CTFParty* GetParty( );
};

class CTFPartyClient
{
public:
	void LoadSavedCasualCriteria( );
    bool InQueueForMatchGroup( EMatchGroup Group );
    void RequestQueueForMatch( EMatchGroup Group );
    bool QueueForCasual( );
};

struct LevelInfo
{
    unsigned short Medal;
    unsigned short pad_0;
    unsigned long Level;
    unsigned char pad_1[ 8 ];
    int cutlstring10;
};