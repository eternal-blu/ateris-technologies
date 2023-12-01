#include "Surface.h"

void ISurface::StartDrawing( )
{
	static auto FN = Memory->FindSignature<void( __thiscall* )( void* )>( "vguimatsurface.dll", "55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14" );
	return FN( this );
}

void ISurface::FinishDrawing( )
{
	static auto FN = Memory->FindSignature<void( __thiscall* )( void* )>( "vguimatsurface.dll", "55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A 00" );
	return FN( this );
}