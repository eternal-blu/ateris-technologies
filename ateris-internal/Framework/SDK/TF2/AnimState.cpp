#include "AnimState.h"
#include "../../../Globals.h"

void CTFPlayerAnimState::ResetGestureSlot( int iGestureSlot )
{
	static auto FN = Memory->FindSignature< void( __thiscall* )( void*, int ) >( "client.dll", "55 8B EC 56 8B 75 ? 57 56 8B F9 E8 ? ? ? ? 84 C0 74 ? C1 E6" );
	FN( this, iGestureSlot );
}

void CTFPlayerAnimState::AddVCDSequenceToGestureSlot( int iGestureSlot, int iGestureSequence, float flCycle, bool bAutoKill )
{
	//static auto FN = reinterpret_cast< void( __thiscall* )( void*, int, int, float, bool ) >( G->FakeTaunt->AddVCDSequenceToGestureSlot );
	static auto FN = Memory->FindSignature<void( __thiscall* )( void*, int, int, float, bool )>( "client.dll", "55 8B EC 51 56 8B F1 8B 46 ? 89 45 ? 85 C0 0F 84 ? ? ? ? 8B 46 ? 53 8B 5D ? 57 8B FB 03 FF 83 7C F8 ? ? 0F 84 ? ? ? ? 53 E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? 8B" );
	FN( this, iGestureSlot, iGestureSequence, flCycle, bAutoKill );
}

void CTFPlayerAnimState::RunGestureSlotAnimEventsToCompletion( GestureSlot_t* pGesture )
{
	static auto FN = Memory->FindSignature<void( __thiscall* )( void*, GestureSlot_t* )>( "client.dll", "55 8B EC 83 EC ? 56 8B 71 ? 89 75 ? 85 F6 0F 84 ? ? ? ? 80 BE ? ? ? ? ? 0F 85 ? ? ? ? 83 BE ? ? ? ? ? 75 ? 8B CE E8" );
	FN( this, pGesture );
}

void CTFPlayerAnimState::UpdateGestureLayer( CStudioHdr* pStudioHdr, GestureSlot_t* pGesture )
{
	static auto FN = Memory->FindSignature<void( __thiscall* )( void*, CStudioHdr *, GestureSlot_t* )>( "client.dll", "55 8B EC 8B 55 ? 53 8B D9 85 D2 0F 84 ? ? ? ? 57 8B 7D ? 85 FF 0F 84 ? ? ? ? 8B 4B ? 85 C9 0F 84" );
	FN( this, pStudioHdr, pGesture );
}

void CTFPlayerAnimState::AddToGestureSlot( int iGestureSlot, int iGestureActivity, bool bAutoKill )
{
	static auto FN = Memory->FindSignature<void( __thiscall* )( void*, int, int, bool )>( "client.dll", "55 8B EC 51 56 8B F1 8B 46 ? 89 45 ? 85 C0 0F 84 ? ? ? ? 8B 46 ? 53 8B 5D ? 57 8B FB 03 FF 83 7C F8 ? ? 0F 84 ? ? ? ? 53 E8 ? ? ? ? 84 C0 0F 84 ? ? ? ? FF 75 ? 8B 4D ? E8 ? ? ? ? 89 45" );
	FN( this, iGestureSlot, iGestureActivity, bAutoKill );
}

void CTFPlayerAnimState::ComputeSequences( CStudioHdr* pStudioHdr )
{
	static auto FN = Memory->FindSignature<void( __thiscall* )( void*, CStudioHdr* )>( "client.dll", "55 8B EC 56 8B F1 E8 ? ? ? ? 8B CE E8 ? ? ? ? FF 75 ? 8B CE E8 ? ? ? ? 5E 5D C2" );
	FN( this, pStudioHdr );
}

void CTFPlayerAnimState::ComputeGestureSequence( CStudioHdr* pStudioHdr )
{
	static auto FN = Memory->FindSignature<void( __thiscall* )( void*, CStudioHdr* )>( "client.dll", "55 8B EC 53 56 57 33 DB 8B F9 33 F6 8D 64 24" );
	FN( this, pStudioHdr );
}