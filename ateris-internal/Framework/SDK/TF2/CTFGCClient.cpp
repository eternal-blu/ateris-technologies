#include "CTFGCClient.h"
#include "..\..\Utils\Memory\Memory.h"

void CTFPartyClient::LoadSavedCasualCriteria( )
{
	static auto Sig = Memory->FindSignature<void( __thiscall* )( void* )>(
		"client.dll", "83 79 ? ? C6 81 ? ? ? ? ? 74 ? 80 79 ? ? 74 ? C6 81 ? ? ? ? ? FF 35" );

	if ( !Sig ) return;

	Sig( this );
}

bool CTFPartyClient::InQueueForMatchGroup( EMatchGroup Group )
{
	static auto Sig = Memory->FindSignature<bool( __thiscall* )( void*, EMatchGroup )>(
		"client.dll", "55 8B EC 56 8B 75 ? 57 56 8B F9 E8 ? ? ? ? 83 C4 ? 84 C0 74 ? 83 FE ? 74 ? 8B 57 ? 33 C0 85 D2 7E ? 8B 4F ? 39 31 74 ? 40 83 C1 ? 3B C2 7C ? 83 C8 ? 83 F8 ? 5F" );

	if ( !Sig )
		return false;

	return Sig( this, Group );
}

void CTFPartyClient::RequestQueueForMatch( EMatchGroup Group )
{
	static auto Sig = Memory->FindSignature<void( __thiscall* )( void*, EMatchGroup )>(
		"client.dll", "55 8B EC 83 EC ? 56 8B 75 ? 57 56 8B F9 E8 ? ? ? ? 83 C4 ? 84 C0" );

	if ( !Sig ) return;

	Sig( this, Group );
}

bool CTFPartyClient::QueueForCasual( )
{
	if ( !InQueueForMatchGroup( EMatchGroup::k_nMatchGroup_Casual_12v12 ) )
	{
		//LoadSavedCasualCriteria( );
		RequestQueueForMatch( EMatchGroup::k_nMatchGroup_Casual_12v12 );
		return true;
	}

	return false;
}

CTFParty* CTFGCClientSystem::GetParty( )
{
	static auto Sig = Memory->FindSignature<void( __thiscall* )( void* )>(
		"client.dll", "8B 89 ? ? ? ? 85 C9 74 ? 68 ? ? ? ? E8 ? ? ? ? 85 C0 74 ? 8B 48 ? 85 C9 74 ? 8B 40 ? 8B 44 88" );

	return nullptr;
}