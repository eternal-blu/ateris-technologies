#include "Visuals.h"
#include "../../../Globals.h"
#include "../../Utils/DrawTools/DrawTools.h"
#include "../Menu/Config.h"
#include "../CritHacks/CritHacks.h"
#include "../Menu/Input/Input.h"
#include "../Models/Models.h"
#include "../Outlines/Outlines.h"
#include "../LagCompensation/LagComp.h"
#include "../Menu/Menu.h"
#include "../NoSpread/NoSpread.h"
#include "../Projectile/ProjectileSim/ProjectileSim.h"
#include "../Anti-Aimbot/Anti-Aimbot.h"
#include "../Auto/AutoMedic/AutoMedic.h"
#include "../Projectile/Movement/Movement.h"
#include "../Misc/Misc.h"
#include "../Projectile/Projectile.h"

void CVisuals::Doubletap()
{
	if (!gLocalPlayer || !gLocalWeapon || !Config->Misc->DT->Enabled || !Config->Misc->DT->Indicator->Enabled || gEngineVGui->IsGameUIVisible( ) )
		return;

	int w = 120;
	int h = 6;

	if ( !DTIndicatorX || !DTIndicatorY )
	{
		DTIndicatorX = ( TF2->ScreenWidth / 2 ) - 120 / 2;
		DTIndicatorY = ( TF2->ScreenHeight / 2 ) + 50;
	}

	if ( Menu->IsOpen && !Menu->SliderIntInfo->Var && !Menu->SliderFloatInfo->Var && !Menu->IsComboboxOpen && !Menu->IsMultiComboboxOpen && !Menu->IsColorPickerOpen )
	{
		gSurface->SurfaceGetCursorPos( MouseX, MouseY );

		static POINT delta;
		static bool drag = false;
		static bool move = false;
		bool held = Input->IsHeld( VK_LBUTTON );
		bool pressed = Input->IsPressed( VK_LBUTTON );

		auto mousex = MouseX;
		auto mousey = MouseY;

		if ( ( mousex > DTIndicatorX && mousex < DTIndicatorX + w && mousey > DTIndicatorY && mousey < DTIndicatorY + 40 ) )
		{
			if ( pressed ) {
				gSurface->PlaySound( "ui/buttonclick.wav" );
			}
			else if ( held ) {
				drag = true;

				if ( !move )
				{
					delta.x = mousex - DTIndicatorX;
					delta.y = mousey - DTIndicatorY;
					move = true;
				}
			}
		}

		if ( drag )
		{
			DTIndicatorX = std::clamp( mousex - ( int )delta.x, 0, TF2->ScreenWidth - w );
			DTIndicatorY = std::clamp( mousey - ( int )delta.y, 0, TF2->ScreenHeight - 40 );
		}

		if ( !held )
		{
			drag = false;
			move = false;
		}
	}

	int ToSend = MAX_COMMANDS - ( gLocalWeapon->IsForceANature( ) ? 2 : 0 );
	int Charged = G->Doubletap.Charged < 3 && gLocalWeapon->IsForceANature( ) ? 0 : G->Doubletap.Charged;
	float ratio = std::clamp( ( float )Charged / ToSend, 0.0f, 1.0f );
	static float Interp = 0.00f; Interp = Draw->EaseIn( Interp, ratio, 0.90f ); Interp = std::clamp( Interp, 0.f, 1.f );
	Color BarColor = Color( 255, 255, 255 );

	int TextOffset = 0;
	if ( G->Doubletap.Charged < ToSend && !G->Doubletap.Recharge )
	{
		BarColor = Color( 255, 25, 25 );
		Draw->String( DTIndicatorX + w * 0.5f, DTIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 25, 25 ), L"DT Impossible" );
		TextOffset += 12;
		Draw->String( DTIndicatorX + w * 0.5f, DTIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 255, 255 ), L"Cost: %i > Charged: %i", ToSend, G->Doubletap.Charged );
		TextOffset += 16;
	}
	else if ( G->Doubletap.Recharge )
	{
		BarColor = Color( 255, 128, 0 );
		Draw->String( DTIndicatorX + w * 0.5f, DTIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 128, 0 ), L"Charging: %i/22", G->Doubletap.Charged );
		TextOffset += 16;
	}
	else if ( G->Doubletap.Wait && !G->Doubletap.Recharge )
	{
		BarColor = Color( 255, 128, 0 );
		Draw->String( DTIndicatorX + w * 0.5f, DTIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 128, 0 ), L"Wait: %.1f", TICKS_TO_TIME(G->Doubletap.Wait) );
		TextOffset += 16;
	}
	else
	{
		BarColor = Color( 15, 210, 70 );
		Draw->String( DTIndicatorX + w * 0.5f, DTIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 15, 210, 70 ), L"DT Ready" );
		TextOffset += 16;
	}

	Color DTColor1 = Config->Misc->DT->Indicator->CustomFade ? Config->Colors->DTFade1 : Config->Colors->MenuFade1;
	Color DTColor2 = Config->Misc->DT->Indicator->CustomFade ? Config->Colors->DTFade2 : Config->Colors->MenuFade2;

	Draw->Rectangle( DTIndicatorX + 1, DTIndicatorY + TextOffset, w - 2, h, { 39, 35, 37, 200 }, false, true );
	//Draw->FadeRectangle( x + 1, y + 1, Interp * ( w - 2 ), h - 2, { 80, 188, 188, 255 }, { 40, 102, 162, 255 }, true );
	Draw->FadeRectangle( DTIndicatorX + 1, DTIndicatorY + TextOffset + 1, Interp * ( w - 2 ), h - 2, DTColor1, Menu->Accent1Left, Menu->Accent1Right, true );
	Draw->FadeRectangle( DTIndicatorX + 1, DTIndicatorY + TextOffset + 1, Interp * ( w - 2 ), h - 2, DTColor2, Menu->Accent2Left, Menu->Accent2Right, true );

	Vertex_t LeftTriangle[ 3 ];
	LeftTriangle[ 0 ].Init( { ( float )DTIndicatorX - h, ( float )DTIndicatorY + TextOffset } );
	LeftTriangle[ 1 ].Init( { ( float )DTIndicatorX + 1, ( float )DTIndicatorY + TextOffset } );
	LeftTriangle[ 2 ].Init( { ( float )DTIndicatorX + 1, ( float )DTIndicatorY + TextOffset + h } );

	Vertex_t RightTriangle[ 3 ];
	RightTriangle[ 0 ].Init( { ( float )DTIndicatorX + w - 1, ( float )DTIndicatorY + TextOffset } );
	RightTriangle[ 1 ].Init( { ( float )DTIndicatorX + w + h, ( float )DTIndicatorY + TextOffset + h } );
	RightTriangle[ 2 ].Init( { ( float )DTIndicatorX + w - 1, ( float )DTIndicatorY + TextOffset + h } );

	Draw->TexturedPolygon( 3, LeftTriangle, { 39, 35, 37, 200 } );
	Draw->TexturedPolygon( 3, RightTriangle, { 39, 35, 37, 200 } );

	LeftTriangle[ 0 ].Init( { ( float )DTIndicatorX - h + 2, ( float )DTIndicatorY + TextOffset + 1 } );
	LeftTriangle[ 1 ].Init( { ( float )DTIndicatorX + 1, ( float )DTIndicatorY + TextOffset + 1 } );
	LeftTriangle[ 2 ].Init( { ( float )DTIndicatorX + 1, ( float )DTIndicatorY + TextOffset + h - 1 } );

	RightTriangle[ 0 ].Init( { ( float )DTIndicatorX + w - 1, ( float )DTIndicatorY + TextOffset + 1 } );
	RightTriangle[ 1 ].Init( { ( float )DTIndicatorX + w + h - 2, ( float )DTIndicatorY + TextOffset + h - 1 } );
	RightTriangle[ 2 ].Init( { ( float )DTIndicatorX + w - 1, ( float )DTIndicatorY + TextOffset + h - 1 } );

	/*Draw->TexturedPolygon( 3, LeftTriangle, { 39, 35, 37, 200 } );
	Draw->TexturedPolygon( 3, RightTriangle, { 39, 35, 37, 200 } );

	Draw->TexturedPolygon( 3, LeftTriangle, { 0, 0, 0, 255 } );
	Draw->TexturedPolygon( 3, RightTriangle, { 0, 0, 0, 255 } );*/

	if ( Interp > 0.01f )
	{
		Draw->TexturedPolygon( 3, LeftTriangle, { DTColor1.r, DTColor1.g, DTColor1.b, Menu->Accent1Left } );
		Draw->TexturedPolygon( 3, LeftTriangle, { DTColor2.r, DTColor2.g, DTColor2.b, Menu->Accent2Left } );
	}

	if ( Charged > 20 )
	{
		Draw->TexturedPolygon( 3, RightTriangle, { DTColor1.r, DTColor1.g, DTColor1.b, Menu->Accent1Right } );
		Draw->TexturedPolygon( 3, RightTriangle, { DTColor2.r, DTColor2.g, DTColor2.b, Menu->Accent2Right } );
	}
}

void CVisuals::DrawLockedTarget( )
{
	if ( !gLocalPlayer || !gLocalWeapon || !Config->AutoMedic->TargetLock || gEngineVGui->IsGameUIVisible( ) )
		return;

	if ( AutoMedic->LockedTarget <= 0 || gLocalWeapon->ItemDefinitionIndex() != Medic_s_TheVaccinator )
		return;

	int HealingTargetIdx = -2;
	if ( gLocalWeapon->HealingTarget( ) )
		HealingTargetIdx = gLocalWeapon->HealingTarget( )->GetIndex( );

	PlayerInfo_t info;
	if (gEngineClient->GetPlayerInfo( AutoMedic->LockedTarget, &info ))
	{
		int x = TF2->ScreenWidth * 0.5f, y = TF2->ScreenHeight * 0.60f + 23;

		Draw->String( x, y, FONT_INDICATOR, Center_X,
			AutoMedic->LockedTarget == HealingTargetIdx ?
			Color( 15, 210, 70 ) : Color( 255, 25, 25 ), true, L"Locked Target: %s",
			TF2->Utf8ToWide( info.name ).c_str( ) );
	}
}

void CVisuals::Crithack( )
{
	if ( !gLocalPlayer || !gLocalWeapon || !Config->Misc->CritHack->Enabled || !Config->Misc->CritHack->Indicator->Enabled || gEngineVGui->IsGameUIVisible( ) )
	{
		return;
	}

	if ( !CritHack->CanForce( ) )
	{
		return;
	}

	if ( !CritIndicatorX || !CritIndicatorY )
	{
		CritIndicatorX = ( TF2->ScreenWidth / 2 ) - 100 / 2;
		CritIndicatorY = TF2->ScreenHeight * 0.60f + 23;
	}

	if ( Menu->IsOpen && !Menu->SliderIntInfo->Var && !Menu->SliderFloatInfo->Var && !Menu->IsComboboxOpen && !Menu->IsMultiComboboxOpen && !Menu->IsColorPickerOpen )
	{
		gSurface->SurfaceGetCursorPos( MouseX, MouseY );

		static POINT delta;
		static bool drag = false;
		static bool move = false;
		bool held = Input->IsHeld( VK_LBUTTON );
		bool pressed = Input->IsPressed( VK_LBUTTON );

		auto mousex = MouseX;
		auto mousey = MouseY;

		if ( ( mousex > CritIndicatorX && mousex < CritIndicatorX + 100 && mousey > CritIndicatorY && mousey < CritIndicatorY + 40 ) )
		{
			if ( pressed ) {
				gSurface->PlaySound( "ui/buttonclick.wav" );
			}
			else if ( held ) {
				drag = true;

				if ( !move )
				{
					delta.x = mousex - CritIndicatorX;
					delta.y = mousey - CritIndicatorY;
					move = true;
				}
			}
		}

		if ( drag )
		{
			CritIndicatorX = std::clamp( mousex - ( int )delta.x, 0, TF2->ScreenWidth - 100 );
			CritIndicatorY = std::clamp( mousey - ( int )delta.y, 0, TF2->ScreenHeight - 40 );
		}

		if ( !held )
		{
			drag = false;
			move = false;
		}
	}

	float Cost = std::clamp( CritHack->CalculateCost( gLocalWeapon ), 0.f, 1000.f );
	int CritsPossible = 0;
	if ( Cost > 0.f )
		CritsPossible = std::clamp( floor( CritHack->BucketCap( ) / Cost ), 0.f, 1000.f );

	int Crits = CritHack->PotentialCrits( );

	//rest calculation
	const float rest = gLocalWeapon->CritTokenBucket( ) - ( CritHack->PotentialCrits( ) * Cost );
	const float restratio = std::clamp( rest / Cost, 0.0f, 1.0f );

	static auto tf_weapon_criticals = gCvar->FindVar( "tf_weapon_criticals" );
	if ( !tf_weapon_criticals || tf_weapon_criticals->GetInt() == 0 )
	{
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY, FONT_INDICATOR, Center_X, Color( 255, 255, 25 ), L"Server Disabled Crits" );
		return;
	}

	int TextOffset = 0;
	if ( gLocalPlayer->IsCritBoosted( ) )
	{
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY, FONT_INDICATOR, Center_X, gGlobals->tickcount % 63 < 32 ? Color( 0, 168, 244 ) : Color( 0, 84, 122 ), L"Crit Boosted" );
		return;
	}

	if ( gLocalWeapon->CritTime( ) > ( gLocalPlayer->TickBase( ) * gGlobals->interval_per_tick ) )
	{
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, gGlobals->tickcount % 63 < 32 ? Color( 0, 168, 244 ) : Color( 0, 84, 122 ), L"Streaming Crits" );
		TextOffset += 14;
		float ratio = ( gLocalWeapon->CritTime( ) - ( gLocalPlayer->TickBase( ) * gGlobals->interval_per_tick ) ) / 2.f;
		Draw->Rectangle( CritIndicatorX + 100 / 2 - 24, CritIndicatorY + TextOffset - 1, 48, 5, { 0, 0, 0, 200 }, false, true );
		Draw->Rectangle( CritIndicatorX + 100 / 2 - 23, CritIndicatorY + TextOffset, 46.f * ratio, 3, Color( 0, 168, 244 ), false, true );
		TextOffset += 5;
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 255, 255 ), L"%i/%i Potential Crits", CritHack->PotentialCrits( ), CritsPossible );
		TextOffset += 12;
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 255, 255 ), L"Crit Damage: %i", ( int )CritHack->Data->Damage->CritDamage );
		return;
	}

	if ( gLocalWeapon->Slot( ) != 2 && CritHack->DamageTillCrit( ) > 0.f && CritHack->Data->CritBan )
	{
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 25, 25 ), L"Crit Throttled" );
		TextOffset += 12;
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 25, 25 ), L"Deal %i Damage", static_cast< int >( ceil( CritHack->DamageTillCrit( ) ) ) );
		return;
	}

	if ( gLocalWeapon->LastRapidfireCritCheckTime( ) + 1.f > ( gLocalPlayer->TickBase( ) * gGlobals->interval_per_tick ) )
	{
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, gGlobals->tickcount % 63 < 32 ? Color( 255, 128, 0 ) : Color( 128, 64, 0 ), L"Wait: %.2fs", ( gLocalWeapon->LastRapidfireCritCheckTime( ) + 1.f ) - ( gLocalPlayer->TickBase( ) * gGlobals->interval_per_tick ) );
		TextOffset += 14;
		float ratio = ( gLocalWeapon->LastRapidfireCritCheckTime( ) + 1.f ) - ( gLocalPlayer->TickBase( ) * gGlobals->interval_per_tick );
		Draw->Rectangle( CritIndicatorX + 100 / 2 - 24, CritIndicatorY + TextOffset - 1, 48, 5, { 0, 0, 0, 200 }, false, true );
		Draw->Rectangle( CritIndicatorX + 100 / 2 - 23, CritIndicatorY + TextOffset, 46.f * ratio, 3, gGlobals->tickcount % 63 < 32 ? Color( 255, 128, 0 ) : Color( 128, 64, 0 ), false, true );
		TextOffset += 5;
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 255, 255 ), L"%i/%i Potential Crits", CritHack->PotentialCrits( ), CritsPossible );
		if ( CritHack->PotentialCrits( ) < CritsPossible && gLocalWeapon->Slot( ) != SLOT_MELEE )
		{
			TextOffset += 14;
			Draw->Rectangle( CritIndicatorX + 100 / 2 - 24, CritIndicatorY + TextOffset - 1, 48, 5, { 0, 0, 0, 200 }, false, true );
			Draw->Rectangle( CritIndicatorX + 100 / 2 - 23, CritIndicatorY + TextOffset, 46.f * restratio, 3, Config->Colors->MenuAccent, false, true );
			TextOffset += 5;
		}
		return;
	}

	if ( Cost > gLocalWeapon->CritTokenBucket( ) )
	{
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 25, 25 ), L"Crit Impossible" );
		TextOffset += 12;
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 255, 255 ), L"Cost: %i > Tokens: %i", ( int )Cost, ( int )gLocalWeapon->CritTokenBucket( ) );
		if ( CritHack->PotentialCrits( ) < CritsPossible && gLocalWeapon->Slot( ) != SLOT_MELEE )
		{
			TextOffset += 14;
			Draw->Rectangle( CritIndicatorX + 100 / 2 - 24, CritIndicatorY + TextOffset - 1, 48, 5, { 0, 0, 0, 200 }, false, true );
			Draw->Rectangle( CritIndicatorX + 100 / 2 - 23, CritIndicatorY + TextOffset, 46.f * restratio, 3, Config->Colors->MenuAccent, false, true );
			TextOffset += 5;
		}
		else
		{
			TextOffset += 12;
		}
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 255, 255 ), L"%i/%i Potential Crits", CritHack->PotentialCrits( ), CritsPossible );
		return;
	}

	if ( gLocalWeapon->IsRapidFireCrits( ) && gLocalWeapon->CritChecks( ) < ( gLocalWeapon->CritSeedRequests( ) + 1 ) * 10 )
	{
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 25, 25 ), L"Crit Throttled" );
		TextOffset += 12;
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 25, 25 ), L"Unbalanced Crit Ratio" );
		TextOffset += 12;

		float Checks = static_cast< float >( gLocalWeapon->CritChecks( ) );
		float Required = ( gLocalWeapon->CritSeedRequests( ) + 1 ) * 10.f;
		if ( Checks > 0 )
		{
			float Procentage = ( Checks / Required ) * 100.f;
			Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 255, 255 ), L"%.0f/100", Procentage );
			TextOffset += 12;
		}
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 255, 255 ), L"%i/%i Potential Crits", CritHack->PotentialCrits( ), CritsPossible );
		return;
	}

	if ( CritHack->Data->Safe[ gLocalWeapon->GetIndex( ) ].size( ) < 2 || CritHack->Data->Crit[ gLocalWeapon->GetIndex( ) ].size( ) < 1 )
	{
		Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 255, 0 ),
			L"Calculating... Crit: %i | Safe: %i", CritHack->Data->Crit[ gLocalWeapon->GetIndex( ) ].size(),
			CritHack->Data->Safe[ gLocalWeapon->GetIndex( ) ].size( ) );
		return;
	}

	Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 15, 210, 70 ), L"Crit Ready" );
	TextOffset += 12;
	Draw->String( CritIndicatorX + 100 / 2, CritIndicatorY + TextOffset, FONT_INDICATOR, Center_X, Color( 255, 255, 255 ), L"%i/%i Potential Crits", CritHack->PotentialCrits( ), CritsPossible );
	if ( CritHack->PotentialCrits( ) < CritsPossible && gLocalWeapon->Slot( ) != SLOT_MELEE )
	{
		TextOffset += 14;
		Draw->Rectangle( CritIndicatorX + 100 / 2 - 24, CritIndicatorY + TextOffset - 1, 48, 5, { 0, 0, 0, 200 }, false, true );
		Draw->Rectangle( CritIndicatorX + 100 / 2 - 23, CritIndicatorY + TextOffset, 46.f * restratio, 3, Config->Colors->MenuAccent, false, true );
		TextOffset += 5;
	}
}

void CVisuals::FakeLatency( )
{
	if ( !Config->Aimbot->Hitscan->FakeLatency || !gLocalPlayer || !gLocalWeapon || !gClientState->m_NetChannel || !gEngineClient->IsInGame( ) )
	{
		return;
	}

	int w = 180;
	int h = 6;

	if ( !LatencyIndicatorX || !LatencyIndicatorY )
	{
		LatencyIndicatorX = 30;
		LatencyIndicatorY = ( TF2->ScreenHeight / 2 ) + 100;
	}

	if ( Menu->IsOpen && !Menu->SliderIntInfo->Var && !Menu->SliderFloatInfo->Var && !Menu->IsComboboxOpen && !Menu->IsMultiComboboxOpen && !Menu->IsColorPickerOpen )
	{
		gSurface->SurfaceGetCursorPos( MouseX, MouseY );

		static POINT delta;
		static bool drag = false;
		static bool move = false;
		bool held = Input->IsHeld( VK_LBUTTON );
		bool pressed = Input->IsPressed( VK_LBUTTON );

		auto mousex = MouseX;
		auto mousey = MouseY;

		if ( ( mousex > LatencyIndicatorX && mousex < LatencyIndicatorX + w && mousey > LatencyIndicatorY && mousey < LatencyIndicatorY + 40 ) )
		{
			if ( pressed ) {
				gSurface->PlaySound( "ui/buttonclick.wav" );
			}
			else if ( held ) {
				drag = true;

				if ( !move )
				{
					delta.x = mousex - LatencyIndicatorX;
					delta.y = mousey - LatencyIndicatorY;
					move = true;
				}
			}
		}

		if ( drag )
		{
			LatencyIndicatorX = std::clamp( mousex - ( int )delta.x, 0, TF2->ScreenWidth - w );
			LatencyIndicatorY = std::clamp( mousey - ( int )delta.y, 0, TF2->ScreenHeight - 40 );
		}

		if ( !held )
		{
			drag = false;
			move = false;
		}
	}

	float ratio = ( gClientState->m_NetChannel->GetLatency( FLOW_INCOMING ) + 
		gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ) ) / 1.0f;

	int LatencyAdded = ( int )( ( gClientState->m_NetChannel->GetLatency( FLOW_INCOMING ) + 
		gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ) ) * 1000.f );

	Draw->String( LatencyIndicatorX, LatencyIndicatorY, FONT_INDICATOR, 0, { 255, 255, 255, 255 },
		"Romanian Deportation Protocol: %i", LatencyAdded );
	Draw->Rectangle( LatencyIndicatorX, LatencyIndicatorY + 12, w + 2, 5, { 0, 0, 0, 200 }, false, true );
	Draw->Rectangle( LatencyIndicatorX + 1, LatencyIndicatorY + 13, w * ratio, 3, Config->Colors->MenuAccent, false, true );
}

// isn't there a better way to detect if a key was pressed and not held down?
bool ThirdpersonKeyIsDown = false;
bool ThirdpersonKeyWasDown = false;

void CVisuals::ThirdPerson( CViewSetup* View )
{
	if ( gLocalPlayer && gLocalPlayer->IsAlive( ) && View )
	{	
		const auto& GameRules = gGameRules->Get( );
		if ( !GameRules )
		{
			return;
		}

		const auto& GameRulesProxy = GameRules->GetProxy( );
		if ( !GameRulesProxy )
		{
			return;
		}

		ThirdpersonKeyWasDown = ThirdpersonKeyIsDown;
		ThirdpersonKeyIsDown = (GetAsyncKeyState(Config->Visuals->ThirdPersonKey) & 0x8000 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( );
		if ( Input->InFocus( ) && ThirdpersonKeyIsDown && ThirdpersonKeyIsDown != ThirdpersonKeyWasDown && !gLocalPlayer->IsTaunting( ) && G->FakeTaunt->FakeTauntTime < gEngineClient->Time( ) && !gSurface->IsCursorVisible( ) )
		{
			InThirdPerson = !InThirdPerson;
		}

		if ( gLocalPlayer->IsTaunting( ) || gLocalPlayer->ForceTauntCam( ) 
			 || ( GameRules && GameRulesProxy && GameRulesProxy->RoundState( ) == GR_STATE_TEAM_WIN && GameRulesProxy->WinningTeam( ) != gLocalPlayer->TeamNum( ) ) )
		{
			gLocalPlayer->ForceTauntCam( ) = 0;
			InThirdPerson = false;
			return;
		}

		if ( G->FakeTaunt->FakeTauntTime > gEngineClient->Time( ) )
		{
			InThirdPerson = true;
		}
		else if ( G->FakeTaunt->FakeTauntTime + 0.1f > gEngineClient->Time( ) )
		{
			InThirdPerson = false;
		}

		if ( InThirdPerson )
		{
			gInput->CAM_ToThirdPerson( );
		}
		else
		{
			gInput->CAM_ToFirstPerson( );
			return;
		}

		gLocalPlayer->UpdateVisibility( );
		//gLocalWeapon->UpdateVisibility( );
		//gLocalPlayer->UpdateWearables( );

		if ( InThirdPerson )
		{
			/*if ( G->FakeTaunt->FakeTauntTime < gEngineClient->Time( ) )
			{
				if ( Config->Misc->AntiAim->Enabled )
				{
					gPrediction->SetLocalViewAngles( G->AntiAim->RealAngles );

					const auto& AnimState = gLocalPlayer->AnimState( );
					if ( AnimState )
					{
						AnimState->m_flCurrentFeetYaw = G->AntiAim->RealAngles.y;
						AnimState->m_flGoalFeetYaw = G->AntiAim->RealAngles.y;
					}
				}
				else
				{
					gPrediction->SetLocalViewAngles( G->AnimationFix->CurrentAngles );

					const auto& AnimState = gLocalPlayer->AnimState( );
					if ( AnimState )
					{
						AnimState->m_flCurrentFeetYaw = G->AnimationFix->CurrentAngles.y;
						AnimState->m_flGoalFeetYaw = G->AnimationFix->CurrentAngles.y;
					}
				}
			}*/

			if ( (GetAsyncKeyState( Config->Visuals->ThirdPersonSidewaysCycleKey ) & 0x1 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
			{
				Config->Visuals->ThirdPersonSideways = -Config->Visuals->ThirdPersonSideways;
			}

			Vector Forward, Right, Up;
			gMath->AngleVectors( gEngineClient->GetViewAngles( ), &Forward, &Right, &Up );

			int DistanceForward = Config->Visuals->ThirdPersonOffset ? Config->Visuals->ThirdPersonForward : 150;
			int DistanceSideways = Config->Visuals->ThirdPersonOffset ? Config->Visuals->ThirdPersonSideways : 0;
			int DistanceUpward = Config->Visuals->ThirdPersonOffset ? Config->Visuals->ThirdPersonUpward : 0;
			Vector Offset = ( Forward * DistanceForward ) - ( Right * DistanceSideways ) - ( Up * DistanceUpward );
			Vector Delta = View->origin - Offset;

			Ray_t Ray; CTraceFilterWorldAndPropsOnly Filter; trace_t Trace;
			Ray.Init( View->origin, Delta, { -12.f, -12.f, -12.f }, { 12.f, 12.f, 12.f } );
			gEngineTrace->TraceRay( Ray, MASK_SOLID, &Filter, &Trace );

			View->origin -= Offset * Trace.Fraction;
		}
	}
}

void CVisuals::HitMarker( )
{
	/*if ( G->HitMarkers.empty( ) )
		return;

	for ( auto& HitMarker : G->HitMarkers )
	{
		if ( !HitMarker || !HitMarker->Entity || !HitMarker->Entity->GetClientClass( ) )
			continue;

		if ( HitMarker->DrawTime - gEngineClient->Time( ) > 0.f )
		{
			if ( HitMarker->Alpha > 5.f )
			{
				HitMarker->Alpha -= 255 * gGlobals->frametime;
			}
			else
			{
				HitMarker->Alpha = 0.f;
			}

			int alpha = ( int )HitMarker->Alpha;
			Color Col = HitMarker->Col;

			if ( Config->Visuals->HitMarker2D )
			{
				int CenterX = TF2->ScreenWidth / 2;
				int CenterY = TF2->ScreenHeight / 2;

				Draw->Line( CenterX - 10, CenterY - 10, CenterX - 5, CenterY - 5, { Col.r, Col.g, Col.b, alpha }, { 0, 0, 0, alpha } );
				Draw->Line( CenterX + 6, CenterY - 6, CenterX + 11, CenterY - 11, { Col.r, Col.g, Col.b, alpha }, { 0, 0, 0, alpha } );

				Draw->Line( CenterX - 10, CenterY + 10, CenterX - 5, CenterY + 5, { Col.r, Col.g, Col.b, alpha }, { 0, 0, 0, alpha } );
				Draw->Line( CenterX + 6, CenterY + 6, CenterX + 11, CenterY + 11, { Col.r, Col.g, Col.b, alpha }, { 0, 0, 0, alpha } );
			}

			if ( Config->Visuals->HitMarker3D )
			{
				if ( !HitMarker->EndPos.IsZero( ) )
				{
					Vector Screen;
					if ( TF2->WorldToScreen( HitMarker->EndPos, Screen ) )
					{
						Draw->Line( Screen.x - 10, Screen.y - 10, Screen.x - 5, Screen.y - 5, { Col.r, Col.g, Col.b, alpha }, { 0, 0, 0, alpha } );
						Draw->Line( Screen.x + 6, Screen.y - 6, Screen.x + 11, Screen.y - 11, { Col.r, Col.g, Col.b, alpha }, { 0, 0, 0, alpha } );

						Draw->Line( Screen.x - 10, Screen.y + 10, Screen.x - 5, Screen.y + 5, { Col.r, Col.g, Col.b, alpha }, { 0, 0, 0, alpha } );
						Draw->Line( Screen.x + 6, Screen.y + 6, Screen.x + 11, Screen.y + 11, { Col.r, Col.g, Col.b, alpha }, { 0, 0, 0, alpha } );

						//Draw->String( Screen.x, Screen.y + 15, FONT_MENU, 0, { 255, 255, 255, 255 }, true, "%.2f", HitMarker->DrawTime - gGlobals->curtime );
					}
				}
			}

			if ( HitMarker->Entity->IsPlayer( ) && Config->Visuals->HitboxHitMarker )
			{
				const model_t* model = HitMarker->Entity->GetModel( );
				if ( !model )
				{
					return;
				}

				const studiohdr_t* hdr = gModelInfo->GetStudioModel( model );
				if ( !hdr )
				{
					return;
				}

				const mstudiohitboxset_t* set = hdr->GetHitboxSet( HitMarker->Entity->As<CBasePlayer>( )->HitboxSet( ) );
				if ( !set )
				{
					return;
				}

				const mstudiobbox_t* bbox = set->hitbox( HitMarker->Hitbox );
				if ( !bbox )
				{
					return;
				}

				Matrix3x4 rotMatrix;
				gMath->AngleMatrix( bbox->angle, rotMatrix );

				Matrix3x4 Bones[ 128 ];
				if ( HitMarker->Entity->SetupBones( Bones, 128, 0x100, gGlobals->curtime ) )
				{
					Matrix3x4 matrix;
					gMath->ConcatTransforms( Bones[ bbox->bone ], rotMatrix, matrix );

					auto bbox_angle = Vector( );
					gMath->MatrixAngles( matrix, bbox_angle );

					auto matrix_origin = Vector( );
					gMath->GetMatrixOrigin( matrix, matrix_origin );

					Color BoxColor = HitMarker->Entity->GetColor( Config->Models->Global->ColorScheme );
					if ( HitMarker->ShowHitbox )
					{
						gDebugOverlay->AddBoxOverlay2( matrix_origin, bbox->bbmin, bbox->bbmax, bbox_angle, { BoxColor.r, BoxColor.g, BoxColor.b, 10 }, BoxColor, 1.5f );
						HitMarker->ShowHitbox = false;
					}
				}
			}
		}
	}*/

	if ( !Config->Visuals->HitMarker2D || gEngineVGui->IsGameUIVisible( ) )
		return;

	float TimeDelta = G->HitMarker->DrawTime - gEngineClient->Time( );
	if ( TimeDelta > 0.f && G->HitMarker->Damage > 0 )
	{
		G->HitMarker->Alpha = gMath->ReMapValClamped( TimeDelta, 0.f, 1.5f, 0.f, 255.f );

		int alpha = ( int )G->HitMarker->Alpha;
		Color Col = G->HitMarker->Col;

		int CenterX = TF2->ScreenWidth / 2;
		int CenterY = TF2->ScreenHeight / 2;
		if ( !G->HitMarker->Position.IsZero( ) )
		{
			Vector Screen;
			if ( !TF2->WorldToScreen( G->HitMarker->Position, Screen ) )
				return;

			CenterX = Screen.x;
			CenterY = Screen.y;
		}

		Draw->Line( CenterX - 10, CenterY - 10, CenterX - 5, CenterY - 5, { Col.r, Col.g, Col.b, alpha }, true, true );
		Draw->Line( CenterX + 6, CenterY - 6, CenterX + 11, CenterY - 11, { Col.r, Col.g, Col.b, alpha }, true, true );
		Draw->Line( CenterX - 10, CenterY + 10, CenterX - 5, CenterY + 5, { Col.r, Col.g, Col.b, alpha }, true, true );
		Draw->Line( CenterX + 6, CenterY + 6, CenterX + 11, CenterY + 11, { Col.r, Col.g, Col.b, alpha }, true, true );

		int wide = 0, tall = 0;
		Draw->GetTextSize( FONT_ESP, wide, tall, "-%i", ( G->HitMarker->Damage ) );

		Draw->String( CenterX - ( wide / 2 ), CenterY + 15, FONT_ESP, 0,
			{ G->HitMarker->Col.r, G->HitMarker->Col.g, G->HitMarker->Col.b, alpha }, 
			true, true, "-%i", ( G->HitMarker->Damage ) );
	}
}

void CVisuals::HitMarkerEvent( IGameEvent* Event )
{
	if ( !gLocalPlayer || !gLocalWeapon )
		return;

	const auto attacked = gEntityList->GetClientEntity( gEngineClient->GetPlayerForUserID( Event->GetInt( "userid" ) ) );
	const auto attacker = gEntityList->GetClientEntity( gEngineClient->GetPlayerForUserID( Event->GetInt( "attacker" ) ) );
	if ( !attacked || !attacker || attacker != gLocalPlayer || attacked == gLocalPlayer )
		return;

	const auto crit = Event->GetBool( "crit" );
	auto damage = static_cast< float >( Event->GetInt( "damageamount" ) );

	G->HitMarker->DrawTime = gEngineClient->Time( ) + 1.5f;
	G->HitMarker->Alpha = 255.f;
	G->HitMarker->Damage = damage;
	G->HitMarker->Position = G->Aimbot.AimPoint;

	G->HitMarker->Col = { 255, 255, 255 };
	if ( crit )
	{
		G->HitMarker->Col = { 255, 50, 50 };
		G->HitMarker->Killed = ( damage >= ( attacked->IsPlayer( ) ? attacked->As<CBasePlayer>( )->Health( ) :
			attacked->As<CBaseObject>( )->Health( ) ) || Event->GetInt( "health" ) <= 0 );
	}

	if ( Config->Visuals->HitscanHitboxes && TF2->IsHitscanWeapon( gLocalWeapon ) )
	{
		Color Hitboxes = Config->Visuals->HitboxesCustomColor ? Config->Colors->HitboxColor : attacked->As<CBasePlayer>( )->GetESPColor( Config->ESP->Global->ColorScheme );
		TF2->DrawHitboxes( attacked->As<CBasePlayer>( ),
						   { Hitboxes.r, Hitboxes.g, Hitboxes.b, ( unsigned char )( Config->Visuals->HitboxesAlpha * 255.f ) },
						   Hitboxes, Config->Visuals->HitboxesTime );
	}
	else if ( Config->Visuals->ProjectileBoundingBox && TF2->IsProjectileWeapon( gLocalWeapon ) && damage > 0 )
	{
		Color BoundingBox = Config->Visuals->BoundingBoxCustomColor ? Config->Colors->BoundingBoxColor : attacked->As<CBasePlayer>( )->GetESPColor( Config->ESP->Global->ColorScheme );
		gDebugOverlay->AddBoxOverlay2( attacked->As<CBasePlayer>( )->Origin( ), attacked->As<CBasePlayer>( )->Mins( ), attacked->As<CBasePlayer>( )->Maxs( ),
									   { 0, 0, 0 }, { BoundingBox.r, BoundingBox.g, BoundingBox.b, ( unsigned char )( Config->Visuals->BoundingBoxAlpha * 255.f ) },
									   BoundingBox, Config->Visuals->BoundingBoxTime );
	}
}

void CVisuals::ModulateTexture( const Color& clr, const char* texture )
{
	for ( MaterialHandle_t h = gMaterialSystem->FirstMaterial( ); h != gMaterialSystem->InvalidMaterial( ); h = gMaterialSystem->NextMaterial( h ) )
	{
		if ( IMaterial* pMaterial = gMaterialSystem->GetMaterial( h ) )
		{
			if ( pMaterial->IsErrorMaterial( ) )
				continue;

			std::string_view group( pMaterial->GetTextureGroupName( ) );

			if ( group.find( texture ) < group.npos )
			{
				pMaterial->ColorModulate( clr.r / 255.f, clr.g / 255.f, clr.b / 255.f );
				//pMaterial->AlphaModulate( clr.a / 255.f );
			}
		}
	}
}

void CVisuals::UpdateWorldModulation( )
{
	if ( !Config->Visuals->WorldModulation->Enabled || Config->Visuals->WorldModulation->Mode == 1 )
	{
		ModulateTexture( { 255, 255, 255, 255 }, "World textures" );
		ModulateTexture( { 255, 255, 255, 255 }, "SkyBox textures" );
		return;
	}

	if ( !Config->Visuals->WorldModulation->World )
	{
		ModulateTexture( { 255, 255, 255, 255 }, "World textures" );
	}

	if ( !Config->Visuals->WorldModulation->Skybox )
	{
		ModulateTexture( { 255, 255, 255, 255 }, "SkyBox textures" );
	}

	/*Color ModulationColor = Config->Visuals->WorldModulation->Mode == 2 ? Config->Colors->WorldColor : ( Config->Visuals->WorldModulation->Mode == 1 ? Color( 255, 255, 255 ) : */

	Color ModulationColor;
	switch ( Config->Visuals->WorldModulation->Mode )
	{
		case 0:
		{
			ModulationColor = Color( Config->Visuals->WorldModulation->Amount * 255.f,
									 Config->Visuals->WorldModulation->Amount * 255.f,
									 Config->Visuals->WorldModulation->Amount * 255.f );
			break;
		}

		case 2:
		{
			ModulationColor = Config->Colors->WorldColor;
			break;
		}

		default:
		{
			ModulationColor = Color( 255, 255, 255 );
			break;
		}
	}

	if ( Config->Visuals->WorldModulation->World )
	{
		ModulateTexture( ModulationColor, "World textures" );
	}

	if ( Config->Visuals->WorldModulation->Skybox )
	{
		ModulateTexture( ModulationColor, "SkyBox textures" );
	}
}

void CVisuals::Spectators( )
{
	int TextOffset = 0;
	for ( const auto& Player : gPlayers )
	{
		if ( !Player || Player->IsAlive( ) || Player->IsDormant( ) || Player == gLocalPlayer )
		{
			continue;
		}

		const auto& Observed = gEntityList->GetClientEntityFromHandle( Player->ObserverTarget( ) );

		if ( Observed && gLocalPlayer && gLocalPlayer->IsAlive( ) && Observed == gLocalPlayer )
		{
			Color String = Color( 255, 255, 255 );

			switch ( Player->ObserverMode( ) )
			{
				case OBS_MODE_FIRSTPERSON:
				{
					String = Color( 255, 50, 50 );
					break;
				}
				default: break;
			}

			if ( Player->IsSteamFriend( ) )
			{
				String = Color( 150, 255, 150 );
			}

			PlayerInfo_t pi;
			if ( gEngineClient->GetPlayerInfo( Player->GetIndex( ), &pi ) && ( Player->ObserverMode( ) == OBS_MODE_FIRSTPERSON || Player->ObserverMode( ) == OBS_MODE_THIRDPERSON ) )
			{
				Draw->String( TF2->ScreenWidth - 5, 20 + TextOffset, FONT_ESP, Reverse_X, String, TF2->Utf8ToWide( pi.name ).c_str( ) );
				TextOffset += 12;
			}
		}
	}
}

void CVisuals::Watermark( )
{
	if ( gEngineVGui->IsGameUIVisible( ) || !gEngineClient->IsInGame( ) || !gClientState->m_NetChannel || G->Authentication->UserName.empty( ) )
		return;

	/*std::wstringstream text;

	int ping = 0;
	if ( gEngineClient->IsInGame( ) && gClientState->m_NetChannel )
	{
		ping = int( gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ) * 1000.f );
	}
	else
	{
		ping = 0;
	}

	static int fps = 0;
	static float time = gEngineClient->Time( );
	float elapsed = gEngineClient->Time( ) - time;
	if ( elapsed > 1.f )
	{
		fps = int( 1.f / gGlobals->frametime );
		time = gEngineClient->Time( );
	}

	std::string name = "None";
	if ( gClientState->m_NetChannel && gClientState->m_NetChannel->GetName( ) )
	{
		name = gClientState->m_NetChannel->GetName( );
	}
	else
	{
		name = "None";
	}

	std::string levelname = "None";
	if ( gEngineClient->IsConnected( ) )
	{
		const char* map = gEngineClient->GetLevelName( );
		levelname = map;
		levelname.erase( levelname.begin( ), levelname.begin( ) + 5 );
		levelname.erase( levelname.end( ) - 4, levelname.end( ) );
	}
	else
	{
		levelname = "None";
	}

	text << L"| debug | delay: " << ping << "ms | " << fps << "FPS | channel name: " << name.c_str( ) << " | level name: " << levelname.c_str( );

	int wide = 0, tall = 0;
	Draw->GetTextSize( FONT_MENU, wide, tall, text.str( ).c_str( ) );

	int x = TF2->ScreenWidth - wide - 70;
	int y = 10;
	int w = wide + 5;
	int h = 18;

	Draw->Rectangle( x - 2 - 19, y - 2, w + 1 + 19, h, Config->Colors->MenuBackground, false, true );
	Draw->Rectangle( x - 2 - 19 - 1, y - 2 - 1, w + 1 + 19 + 2, h + 2, Config->Colors->MenuBorders, false );
	Draw->Rectangle( x - 2 - 19 - 2, y - 2 - 2, w + 1 + 19 + 4, h + 4, Config->Colors->MenuBackground, false );

	Draw->TextureRectangle( x - 19, y - 2, 18, 18, Menu->MenuWatermark->GetTextureID( ), { 255, 255, 255, 255 } );
	Draw->String( x, y + 1, FONT_MENU, 0, { 255, 255, 255, 255 }, true, text.str( ).c_str( ) );*/

	std::stringstream Watermark;

	int Latency = ( int )( gClientState->m_NetChannel->GetLatency( FLOW_OUTGOING ) * 1000.f );

	/*static int FramesPerSecond = 0;
	static float LastUpdate = gEngineClient->Time( );
	float Elapsed = gEngineClient->Time( ) - LastUpdate;
	if ( Elapsed > 1.f )
	{
		FramesPerSecond = int( 1.f / gGlobals->frametime );
		LastUpdate = gEngineClient->Time( );
	}*/

	Watermark << G->Authentication->UserName << " | " << G->Authentication->BuildName << " | " << Latency << "ms | " << gClientState->m_NetChannel->GetName( );
	Draw->Rectangle( 7, 7, 41, 39, { 0, 0, 0, 255 }, false, true );
	Draw->TextureRectangle( 12, 10, 28, 28, Menu->AterisLogo28px, { 255, 255, 255, 255 } );

	int wide = 0, tall = 0;
	Draw->GetTextSize( FONT_WATERMARK, wide, tall, Watermark.str( ).c_str( ) );

	Draw->FadeRectangle( 39 + 7, 7, 2, 39, Config->Colors->MenuFade1, Menu->Accent1Left, Menu->Accent1Right, false );
	Draw->FadeRectangle( 39 + 7, 7, 2, 39, Config->Colors->MenuFade2, Menu->Accent2Left, Menu->Accent2Right, false );

	Draw->FadeRectangle( 39 + 7 + 2, 7, wide + 20, 39, { 0, 0, 0, 255 }, 255, 5, true );
	Draw->FadeRectangle( 39 + 7 + 2, 7, wide + 20, 39, { 0, 0, 0, 255 }, 255, 5, true );
	//Draw->FadeRectangle( 39 + 7 + 2, 7, wide * 2, 39, { 0, 0, 0, 255 }, 255, 0, true );

	Draw->String( 40 + 10 + 7, 20 + 7, FONT_WATERMARK, Center_Y, { 255, 255, 255, 255 }, Watermark.str( ).c_str( ) );
}

void CVisuals::ScopeVisuals( )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !Config->Visuals->RemoveScope || gEngineVGui->IsGameUIVisible( ) )
		return;

	if ( !gLocalPlayer->IsZoomed( ) )
		return;

	Draw->Line( TF2->ScreenWidth / 2, TF2->ScreenHeight / 2 - 200, TF2->ScreenWidth / 2, TF2->ScreenHeight / 2 + 200, { 0, 0, 0, 255 }, false );
	Draw->Line( TF2->ScreenWidth / 2 - 200, TF2->ScreenHeight / 2, TF2->ScreenWidth / 2 + 200, TF2->ScreenHeight / 2, { 0, 0, 0, 255 }, false );
}

void CVisuals::DrawNoSpread( )
{
	if ( !gLocalPlayer || !gLocalWeapon || !Config->Misc->NoSpread->Enabled || !Config->Misc->NoSpread->Indicator || gEngineVGui->IsGameUIVisible( ) )
		return;

	/*float time = NoSpread->ServerTime;
	int hours = 0;
	int minutes = 0;
	int seconds = 0;

	if ( time > 3600 ) {
		hours = time / 3600;
		time -= hours * 3600;
	}

	if ( time > 60 ) {
		minutes = time / 60;
		time -= minutes * 60;
	}

	seconds = time;
	std::string first = "%i hours, %i minutes, ";
	std::string middle = seconds < 10 ? "0" : "";
	std::string last = "%i seconds";
	std::string timestr = first + middle + last;
	Draw->String( 5, 512, FONT_INDICATOR, 0, { 255, 255, 255, 255 }, true, timestr.c_str( ), hours, minutes, seconds );

	if ( NoSpread->MantissaStep < 16.f )
	{
		Draw->String( 5, 500, FONT_INDICATOR, 0, { 255, 255, 255, 255 }, true, "NoSpread: Impossible" );
		return;
	}

	std::string text = "";

	if ( gLocalWeapon->IsSpreadWeapon( ) )
	{
		switch ( NoSpread->SyncState )
		{
			case CNoSpread::SyncState_t::NOT_SYNCED:
			{
				text = "Not synced";
				break;
			}

			case CNoSpread::SyncState_t::CORRECTING:
			{
				text = "Correcting";
				break;
			}

			case CNoSpread::SyncState_t::SYNCED:
			{
				text = "Synced";
				break;
			}

			case CNoSpread::SyncState_t::DEAD_SYNC:
			{
				text = "Dead Sync";
				break;
			}

			default: break;
		}
	}
	else
	{
		text = "Disabled";
	}

	Draw->String( 5, 500, FONT_INDICATOR, 0, { 255, 255, 255, 255 }, true, text.c_str( ) );
	Draw->String( 5, 524, FONT_INDICATOR, 0, { 255, 255, 255, 255 }, true, "Mantissa step: %.0f", NoSpread->MantissaStep );*/

	if ( !NoSpread->RandomFloatVals.empty( ) && NoSpread->CurrentWeaponSpread )
	{
		Vector Screen = {};
		if ( !G->Aimbot.AimPoint.IsZero( ) )
		{
			if ( TF2->WorldToScreen( G->Aimbot.AimPoint, Screen ) )
			{
				for ( const auto& [x, y] : NoSpread->RandomFloatVals )
				{
					Draw->FilledCircle( Screen.x + 50 * x, Screen.y + 50 * y, 4, 16, { 255, 255, 255, 80 } );
				}
			}
		}
		else
		{
			for ( const auto& [x, y] : NoSpread->RandomFloatVals )
			{
				Draw->FilledCircle( ( TF2->ScreenWidth / 2.f ) + 50 * x, ( TF2->ScreenHeight / 2.f ) + 50 * y, 4, 16, { 255, 255, 255, 80 } );
			}
		}
	}
}

void CVisuals::DrawInfo( )
{
	if ( !gLocalPlayer || !gLocalWeapon || gEngineVGui->IsGameUIVisible( ) )
		return;

	if ( Misc->TimeSinceLag > gEngineClient->Time( ) )
	{
		//todo: draw some shit to indicate client lag
	}

	int nTextOffset = 0;
	if ( Config->Misc->FakeLag->Enabled )
	{
		if ( G->FakeLag->Active && !G->FakeLag->FakeDucking )
		{
			int Col = gMath->ReMapValClamped( gClientState->chokedcommands, 0, 22, 48, 180 );
			Draw->String( 10, TF2->ScreenHeight * 0.5f + TF2->ScreenHeight * 0.15f, FONT_INDICATOR2, 0, Config->Colors->MenuAccent, false, "FL" );
			nTextOffset += 28;

			if ( G->FakeLag->BreakingLagComp && gLocalPlayer->Velocity( ).Length2D( ) > 10.f )
			{
				bool BreakingLagComp = false;
				if ( !gClientState->chokedcommands )
				{
					static Vector LastOrigin = gLocalPlayer->Origin( );
					if ( gLocalPlayer->Origin( ) != LastOrigin )
					{
						BreakingLagComp = ( gLocalPlayer->Origin( ) ).Length2D( ) > 64;
						LastOrigin = gLocalPlayer->Origin( );
					}
				}

				Draw->String( 10, TF2->ScreenHeight * 0.5f + TF2->ScreenHeight * 0.15f + nTextOffset, FONT_INDICATOR2, 0, Config->Colors->MenuAccent, false, "LC" );
				nTextOffset += 28;
			}
		}
	}

	if ( Config->Misc->FakeDuck->Enabled )
	{
		if ( G->FakeLag->FakeDucking && !G->FakeLag->Active )
		{
			Draw->String( 10, TF2->ScreenHeight * 0.5f + TF2->ScreenHeight * 0.15f, FONT_INDICATOR2, 0, Config->Colors->MenuAccent, false, "FD" );
			nTextOffset += 28;
		}
	}

	if ( Config->Misc->NoSpread->Enabled && Config->Misc->NoSpread->Indicator && gLocalWeapon->IsSpreadWeapon( ) )
	{
		Draw->String( 10, TF2->ScreenHeight * 0.5f + TF2->ScreenHeight * 0.15f + nTextOffset, FONT_INDICATOR2, 0,
					  NoSpread->SyncState == CNoSpread::SyncState_t::SYNCED ? Config->Colors->MenuAccent : Color( 128, 128, 128, 255 ), false, "NS" );
		nTextOffset += 28;
	}

	if ( Config->Misc->AntiAim->Enabled && AntiAim->IsToggled )
	{
		bool Left = false;
		bool Right = false;

		Draw->String( 10, TF2->ScreenHeight * 0.5f + TF2->ScreenHeight * 0.15f + nTextOffset, FONT_INDICATOR2, 0, Config->Colors->MenuAccent, false, "AA" );
		nTextOffset += 28;

		float UsedYaw = G->AntiAim->RealAngles.y - gEngineClient->GetViewAngles( ).y;

		if ( UsedYaw == 0.0f || UsedYaw == 180.0f || UsedYaw == -180.0f )
		{
			Left = false;
			Right = false;
		}

		if ( UsedYaw < 0.0f && UsedYaw > -180.0f )
		{
			Left = false;
			Right = true;
		}

		if ( UsedYaw > 0.0f && UsedYaw < 180.0f )
		{
			Left = true;
			Right = false;
		}

		if ( UsedYaw > -360.f && UsedYaw < -180.f )
		{
			Left = true;
			Right = false;
		}

		if ( UsedYaw < 360.f && UsedYaw > 180.f )
		{
			Left = false;
			Right = true;
		}

		if ( G->AntiAim->RealAngles.x < 0.f )
		{
			if ( Left )
			{
				Left = false;
			}
			else
			{
				Left = true;
			}

			if ( Right )
			{
				Right = false;
			}
			else
			{
				Right = true;
			}
		}

		Vertex_t LeftTriangle[ 3 ];
		LeftTriangle[ 0 ].Init( { TF2->ScreenWidth * 0.5f - 50 - 16, TF2->ScreenHeight * 0.5f } );
		LeftTriangle[ 1 ].Init( { TF2->ScreenWidth * 0.5f - 50, TF2->ScreenHeight * 0.5f + 8 } );
		LeftTriangle[ 2 ].Init( { TF2->ScreenWidth * 0.5f - 50, TF2->ScreenHeight * 0.5f - 8 } );
		Draw->TexturedPolygon( 3, LeftTriangle, Left ? Color( 50, 120, 235, 80 ) : Color( 255, 255, 255, 80 ) );

		Vertex_t RightTriangle[ 3 ];
		RightTriangle[ 0 ].Init( { TF2->ScreenWidth * 0.5f + 50 + 16, TF2->ScreenHeight * 0.5f } );
		RightTriangle[ 1 ].Init( { TF2->ScreenWidth * 0.5f + 50, TF2->ScreenHeight * 0.5f + 8 } );
		RightTriangle[ 2 ].Init( { TF2->ScreenWidth * 0.5f + 50, TF2->ScreenHeight * 0.5f - 8 } );
		Draw->TexturedPolygon( 3, RightTriangle, Right ? Color( 50, 120, 235, 80 ) : Color( 255, 255, 255, 80 ) );
	}
}

void CVisuals::Viewmodel( )
{
	if ( G->ConVars->WeaponSway )
	{
		if ( Config->Visuals->ViewmodelSway )
		{
			G->ConVars->WeaponSway->SetValue( Config->Visuals->ViewmodelSwayAmount );
		}
		else
		{
			if ( G->ConVars->WeaponSway->GetFloat( ) )
			{
				G->ConVars->WeaponSway->SetValue( 0.0f );
			}
		}
	}

	static bool flip = false;
	if ( G->ConVars->FlipViewmodels )
	{
		if ( Config->Visuals->ViewmodelFlip )
		{
			if ( !G->ConVars->FlipViewmodels->GetInt( ) )
			{
				G->ConVars->FlipViewmodels->SetValue( 1 );
				flip = true;
			}
		}
		else
		{
			if ( flip || G->ConVars->FlipViewmodels->GetInt( ) )
			{
				G->ConVars->FlipViewmodels->SetValue( 0 );
				flip = false;
			}
		}
	}

	static bool bSet = false;
	if ( G->ConVars->ViewmodelFOV )
	{
		if ( !bSet )
		{
			Config->Visuals->ViewmodelFOV = G->ConVars->ViewmodelFOV->GetInt( );
			bSet = true;
		}

		G->ConVars->ViewmodelFOV->SetValue( Config->Visuals->ViewmodelFOV );
	}
}

void CVisuals::ProjectileCam( )
{
	if ( !gLocalPlayer || !gLocalWeapon || !TF2->IsProjectileWeapon( gLocalWeapon ) || !Config->Visuals->ProjectileLine || !Config->Visuals->ProjectileCamera || G->ProjectileCamera->ProjectileCameraPos.IsZero( ) || gEngineVGui->IsGameUIVisible( ) )
		return;

	Draw->Rectangle( ProjectileCamX - 1, ProjectileCamY - 21,
					 Config->Visuals->ProjectileCameraWidth + 2,
					 Config->Visuals->ProjectileCameraHeight + 22,
					 Config->Colors->MenuBorders, false );

	Draw->Rectangle( ProjectileCamX - 2, ProjectileCamY - 22,
					 Config->Visuals->ProjectileCameraWidth + 4,
					 Config->Visuals->ProjectileCameraHeight + 24,
					 Config->Colors->MenuBackground, false );

	Draw->Rectangle( ProjectileCamX, ProjectileCamY - 20,
					 Config->Visuals->ProjectileCameraWidth,
					 20,
					 Config->Colors->MenuBackground, false, true );

	Draw->Line( ProjectileCamX - 1, ProjectileCamY,
				ProjectileCamX + Config->Visuals->ProjectileCameraWidth + 1, ProjectileCamY, Config->Colors->MenuBorders, false );

	Draw->String( ProjectileCamX + Config->Visuals->ProjectileCameraWidth / 2, ProjectileCamY - 11, FONT_MENU,
				  Center_X | Center_Y, Config->Colors->MenuAccent, "Projectile Camera" );

	if ( Menu->IsOpen && !Menu->SliderIntInfo->Var && !Menu->SliderFloatInfo->Var && !Menu->IsComboboxOpen && !Menu->IsMultiComboboxOpen && !Menu->IsColorPickerOpen )
	{
		gSurface->SurfaceGetCursorPos( MouseX, MouseY );

		static POINT delta;
		static bool drag = false;
		static bool move = false;
		bool held = Input->IsHeld( VK_LBUTTON );
		bool pressed = Input->IsPressed( VK_LBUTTON );

		auto mousex = MouseX;
		auto mousey = MouseY;

		if ( ( mousex > ProjectileCamX - 2 && mousex < ProjectileCamX + Config->Visuals->ProjectileCameraWidth + 2 && mousey > ProjectileCamY - 22 && mousey < ProjectileCamY ) )
		{
			if ( pressed ) {
				gSurface->PlaySound( "ui/buttonclick.wav" );
			}
			else if ( held ) {
				drag = true;

				if ( !move )
				{
					delta.x = mousex - ProjectileCamX;
					delta.y = mousey - ProjectileCamY;
					move = true;
				}
			}
		}

		if ( drag )
		{
			ProjectileCamX = std::clamp( mousex - ( int )delta.x, 2, TF2->ScreenWidth - Config->Visuals->ProjectileCameraWidth - 2 );
			ProjectileCamY = std::clamp( mousey - ( int )delta.y, 22, TF2->ScreenHeight - Config->Visuals->ProjectileCameraHeight - 2 );
		}

		if ( !held )
		{
			drag = false;
			move = false;
		}
	}
}

void CVisuals::PredictionLine( )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !gLocalWeapon || gEngineVGui->IsGameUIVisible( ) )
		return;

	static std::vector<Vector> PrePosVec {}, PostPosVec {};
	if ( MovementSim->ClearCache )
	{
		if ( !PrePosVec.empty( ) )
			PrePosVec.clear( );

		if ( !PostPosVec.empty( ) )
			PostPosVec.clear( );

		PrePosVec = MovementSim->OldPositions;
		PostPosVec = MovementSim->PredictedPositions;

		MovementSim->ClearCache = false;
	}

	float TimeDelta = MovementSim->ShotTime - gEngineClient->Time( );
	if ( TimeDelta > 0.f && !PrePosVec.empty( ) && !PostPosVec.empty( ) )
	{
		Color PaypalMonster( 255, 255, 255, 169 );
		Vector Past {}, Future {};
		for ( size_t i = 0; i < PostPosVec.size( ); i++ )
		{
			if ( TF2->WorldToScreen( PrePosVec[ i ], Past ) )
			{
				if ( TF2->WorldToScreen( PostPosVec[ i ], Future ) )
				{
					Draw->Line( Past.x, Past.y, Future.x, Future.y, PaypalMonster, true, true );
				}
			}
		}
	}
}

void CVisuals::ProjectileLine2( )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !gLocalWeapon || !Config->Visuals->ProjectileLine || gEngineVGui->IsGameUIVisible( ) )
		return;

	if ( OldPrediction.empty( ) || NewPrediction.empty( ) )
		return;

	gSurface->DrawSetColor( 0, 0, 0, 255 );
	for ( size_t i = 0; i < NewPrediction.size( ); i++ )
	{
		Vector a, b;
		if ( TF2->WorldToScreen( OldPrediction[ i ], a ) && TF2->WorldToScreen( NewPrediction[ i ], b ) )
		{
			gSurface->DrawLine( a.x, a.y + 1, b.x, b.y + 1 );
			gSurface->DrawLine( a.x + 1, a.y, b.x + 1, b.y );
			gSurface->DrawLine( a.x, a.y - 1, b.x, b.y - 1 );
			gSurface->DrawLine( a.x - 1, a.y, b.x - 1, b.y );
		}
	}

	gSurface->DrawSetColor( Color( Config->Colors->ProjectileLineColor.r, Config->Colors->ProjectileLineColor.g, Config->Colors->ProjectileLineColor.b, ( int )( Config->Visuals->ProjectileLineAlpha * 255.f ) ) );
	for ( size_t i = 0; i < NewPrediction.size( ); i++ )
	{
		Vector a, b;
		if ( TF2->WorldToScreen( OldPrediction[ i ], a ) && TF2->WorldToScreen( NewPrediction[ i ], b ) )
		{
			gSurface->DrawLine( a.x, a.y, b.x, b.y );
		}
	}
}

void CVisuals::ProjectileLine( )
{
	OldPrediction.clear( );
	NewPrediction.clear( );

	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !gLocalWeapon || !Config->Visuals->ProjectileLine || gEngineVGui->IsGameUIVisible( ) )
		return;

	ProjectileInfo info = { };

	if ( !ProjectileSim->GetInfo( gEngineClient->GetViewAngles( ), info ) )
		return;

	if ( Config->Aimbot->Projectile->MaxStickyCharge > 0 && gLocalWeapon->Slot( ) == SLOT_SECONDARY && gLocalPlayer->Class( ) == CLASS_DEMOMAN ) {
		//modify speed based on custom charge max
		float get_max_charge = Config->Aimbot->Projectile->MaxStickyCharge / 27.0f;
		if ( gLocalWeapon->ItemDefinitionIndex( ) == Demoman_s_TheQuickiebombLauncher )
			get_max_charge = Config->Aimbot->Projectile->MaxStickyCharge / 70.0f;

		float diff = 15;
		if ( gLocalWeapon->ItemDefinitionIndex( ) == Demoman_s_TheQuickiebombLauncher )
			diff = 5;

		float new_speed = gMath->ReMapValClamped( get_max_charge - TICKS_TO_TIME( diff ), 0.0f, TF2->AttribHookFloat( 4.0f, "stickybomb_charge_rate", gLocalWeapon, 0x0 ), 900.0f, 2400.0f );
		info.Speed = new_speed;
	}

	if ( !ProjectileSim->Init( info ) )
		return;

	static bool set = false;
	if ( ( GetAsyncKeyState( Config->Aimbot->Projectile->ResetPositionKey ) & 0x1 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
	{
		G->ProjectileCamera->SavedAimPoint = Vector( );
	}

	for ( int n = 0; n < TIME_TO_TICKS( 10.0f ); n++ ) {
		auto old_position = ProjectileSim->GetOrigin( );
		ProjectileSim->Simulate( );
		auto new_position = ProjectileSim->GetOrigin( );

		//Draw->RenderLine( old_position, new_position, Color( 235, 235, 235, 175 ), false );
		OldPrediction.push_back( old_position );
		NewPrediction.push_back( new_position );

		Ray_t ray = { }; CTraceFilterHitscan filter; trace_t trace = { };
		ray.Init( old_position, new_position, Vector( -4.0f, -4.0f, -4.0f ), Vector( 4.0f, 4.0f, 4.0f ) );
		filter.Skip = gLocalPlayer;
		gEngineTrace->TraceRay( ray, MASK_SOLID, &filter, &trace );

		if ( trace.DidHit( ) ) {
			if ( G->ProjectileCamera->SavedAimPoint.IsZero( ) )
			{
				G->ProjectileCamera->ProjectileCameraPos = new_position;
			}
			else
			{
				G->ProjectileCamera->ProjectileCameraPos = G->ProjectileCamera->SavedAimPoint;
			}

			if ( gLocalWeapon->Slot( ) == SLOT_SECONDARY && ( GetAsyncKeyState( Config->Aimbot->Projectile->ChangePositionKey ) & 0x1 ) && Input->InFocus( ) && !gSurface->IsCursorVisible( ) )
			{
				G->ProjectileCamera->SavedAimPoint = new_position;

				Ray_t ray = { }; CTraceFilterHitscan filter; trace_t trace = { };
				ray.Init( G->ProjectileCamera->SavedAimPoint, G->ProjectileCamera->SavedAimPoint + Vector( 0.0f, 0.0f, -15.f ), Vector( -4.0f, -4.0f, -4.0f ), Vector( 4.0f, 4.0f, 4.0f ) );
				gEngineTrace->TraceRay( ray, MASK_SOLID, &filter, &trace );
				if ( trace.DidHit( ) ) {
					G->ProjectileCamera->SavedAimPoint += Vector( 0.0f, 0.0f, 20.f );
				}

				ray.Init( G->ProjectileCamera->SavedAimPoint, G->ProjectileCamera->SavedAimPoint + Vector( 0.0f, 0.0f, 15.f ), Vector( -4.0f, -4.0f, -4.0f ), Vector( 4.0f, 4.0f, 4.0f ) );
				gEngineTrace->TraceRay( ray, MASK_SOLID, &filter, &trace );
				if ( trace.DidHit( ) ) {
					G->ProjectileCamera->SavedAimPoint += Vector( 0, 0, -20.f );
				}

				ray.Init( G->ProjectileCamera->SavedAimPoint, G->ProjectileCamera->SavedAimPoint + Vector( 0.0f, -50.f, 0.0f ), Vector( -4.0f, -4.0f, -4.0f ), Vector( 4.0f, 4.0f, 4.0f ) );
				gEngineTrace->TraceRay( ray, MASK_SOLID, &filter, &trace );
				if ( trace.DidHit( ) ) {
					G->ProjectileCamera->SavedAimPoint += Vector( 0, 55.f, 0 );
				}

				ray.Init( G->ProjectileCamera->SavedAimPoint, G->ProjectileCamera->SavedAimPoint + Vector( 0.0f, 50.f, 0.0f ), Vector( -4.0f, -4.0f, -4.0f ), Vector( 4.0f, 4.0f, 4.0f ) );
				gEngineTrace->TraceRay( ray, MASK_SOLID, &filter, &trace );
				if ( trace.DidHit( ) ) {
					G->ProjectileCamera->SavedAimPoint += Vector( 0, -55.f, 0 );
				}

				ray.Init( G->ProjectileCamera->SavedAimPoint, G->ProjectileCamera->SavedAimPoint + Vector( -50.f, 0.0f, 0.0f ), Vector( -4.0f, -4.0f, -4.0f ), Vector( 4.0f, 4.0f, 4.0f ) );
				gEngineTrace->TraceRay( ray, MASK_SOLID, &filter, &trace );
				if ( trace.DidHit( ) ) {
					G->ProjectileCamera->SavedAimPoint += Vector( 55.f, 0, 0 );
				}

				ray.Init( G->ProjectileCamera->SavedAimPoint, G->ProjectileCamera->SavedAimPoint + Vector( 50.f, 0.0f, 0.0f ), Vector( -4.0f, -4.0f, -4.0f ), Vector( 4.0f, 4.0f, 4.0f ) );
				gEngineTrace->TraceRay( ray, MASK_SOLID, &filter, &trace );
				if ( trace.DidHit( ) ) {
					G->ProjectileCamera->SavedAimPoint += Vector( -55.f, 0, 0 );
				}
			}

			auto angles = Vector( );
			gMath->VectorAngles( trace.Plane.Normal, angles );

			Draw->RenderBox( new_position, angles, Vector( -1.0f, -4.5f, -4.5f ), Vector( 1.0f, 4.5f, 4.5f ), Color( Config->Colors->ProjectileLineColor.r, Config->Colors->ProjectileLineColor.g, Config->Colors->ProjectileLineColor.b, ( int )( Config->Visuals->ProjectileLineAlpha * 255.f ) ), false, false );
			Draw->RenderWireframeBox( new_position, angles, Vector( -1.0f, -4.5f, -4.5f ), Vector( 1.0f, 4.5f, 4.5f ), Color( 0, 0, 0, 255 ), false );

			break;
		}
	}

	if ( !G->ProjectileCamera->SavedAimPoint.IsZero( ) )
	{
		//Draw->DrawRadius( G->ProjectileCamera->SavedAimPoint, 50.f, 50, 255, 50 );
		/*static auto RenderSphere = reinterpret_cast< void( __cdecl* )( const Vector&, float, int, int, Color, IMaterial* ) >( G->RenderSphereAddress );
		RenderSphere( G->ProjectileCamera->SavedAimPoint, 50.f, 50, 50, { 50, 255, 50, 50 }, Models->FlatMaterial );*/
		/*Vector Edge, LastEdge;
		float Radius = 50.f;
		LastEdge = Vector( G->ProjectileCamera->SavedAimPoint.x, Radius + G->ProjectileCamera->SavedAimPoint.y, G->ProjectileCamera->SavedAimPoint.z );

		for ( float angle = 0.0f; angle <= 360.0f; angle += 11.25f )
		{
			Edge.x = Radius * cos( DEG2RAD( angle ) ) + G->ProjectileCamera->SavedAimPoint.x;
			Edge.y = Radius * sin( DEG2RAD( angle ) ) + G->ProjectileCamera->SavedAimPoint.y;
			Edge.z = G->ProjectileCamera->SavedAimPoint.z;

			RenderLine( Edge, LastEdge, { 50, 255, 50, 255 }, false );
			RenderTriangle( Edge, LastEdge, G->ProjectileCamera->SavedAimPoint, { 50, 255, 50, 50 }, false );

			LastEdge = Edge;
		}*/

		float flPoint = ( float )PI * 2.0f / 32.f;
		float flRadius = 50.f;

		for ( float flAngle = 0; flAngle < ( PI * 2.0f ); flAngle += flPoint )
		{
			Vector vStart( flRadius * cosf( flAngle ) + G->ProjectileCamera->SavedAimPoint.x,
						   flRadius * sinf( flAngle ) + G->ProjectileCamera->SavedAimPoint.y,
						   G->ProjectileCamera->SavedAimPoint.z );
			Vector vEnd( flRadius * cosf( flAngle + flPoint ) + G->ProjectileCamera->SavedAimPoint.x,
						 flRadius * sinf( flAngle + flPoint ) + G->ProjectileCamera->SavedAimPoint.y,
						 G->ProjectileCamera->SavedAimPoint.z );
			Draw->RenderLine( vStart, vEnd, { 50, 255, 50, 255 }, false );
			Draw->RenderTriangle( vEnd, vStart, G->ProjectileCamera->SavedAimPoint, { 50, 255, 50, 50 }, false );
			Draw->RenderTriangle( vStart, vEnd, G->ProjectileCamera->SavedAimPoint, { 50, 255, 50, 50 }, false );
		}
	}
}

void CVisuals::DrawBacktrack( void* ECX, void* EDX, const DrawModelState_t& State, const ModelRenderInfo_t& Info, std::function< void( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4* ) > f )
{
	auto Entity = gEntityList->GetClientEntity( Info.entity_index )->As<CBaseEntity>( );
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !Entity || !Entity->As<CBasePlayer>( )->IsAlive( ) || Entity == gLocalPlayer || !Entity->IsPlayer( ) || Entity->IsDormant( ) )
		return;

	if ( !Config->Visuals->DrawBackTrack )
		return;

	if ( !Config->Visuals->BackTrackModel && !Config->Visuals->BackTrackOutline )
		return;

	if ( LagComp->Records[ Entity->GetIndex( ) ].empty( ) )
		return;

	if ( Outlines->IsRendering( ) && Config->Visuals->BackTrackOutline && Config->Outlines->Global->Enabled && !G->ProjectileCamera->IsDrawingProjectileCamera )
	{
		if ( Config->Visuals->LastTickOnly )
		{
			CLagComp::CLagCompRecord Record = LagComp->GetLastTick( Entity->GetIndex( ) );

			if ( Record.Boner && Record.Model && Record.SetupBoner )
				f( ECX, EDX, State, Info, Record.Boner );
		}
		else
		{
			auto ValidRecords = LagComp->GetValidTicks( Entity->GetIndex( ) );
			if ( !ValidRecords.empty( ) )
			{
				for ( size_t i = 0; i < ValidRecords.size( ); i++ )
				{
					auto& Record = ValidRecords[ i ];

					if ( i == 0 )
						continue;

					if ( Record.Velocity.Length( ) < 1.f )
						continue;

					if ( Record.Boner && Record.Model && Record.SetupBoner )
						f( ECX, EDX, State, Info, Record.Boner );
				}
			}
		}
	}

	if ( Models->IsRendering( ) && Config->Models->Global->Enabled )
	{
		if ( Models->FlatMaterial )
			gModelRender->ForcedMaterialOverride( Models->FlatMaterial );

		Color Backtrack = Entity->GetModelColor( Config->Models->Global->ColorScheme );
		if ( Entity->As<CBasePlayer>( )->IsSteamFriend( ) )
		{
			Backtrack = Config->Colors->ModelsPlayerFriend;
		}

		gRenderView->SetColorModulation( Backtrack );

		if ( Config->Visuals->BackTrackModel )
		{
			gRenderView->SetBlend( Config->Visuals->BackTrackAlpha );
			gMaterialSystem->GetRenderContext( )->DepthRange( 0.0f, 0.2f );
		}
		else
		{
			gRenderView->SetBlend( 0.f );
			gMaterialSystem->GetRenderContext( )->DepthRange( 0.0f, 1.0f );
		}

		if ( Config->Visuals->LastTickOnly )
		{
			CLagComp::CLagCompRecord Record = LagComp->GetLastTick( Entity->GetIndex( ) );

			if ( Record.Boner && Record.Model && Record.SetupBoner )
				f( ECX, EDX, State, Info, Record.Boner );
		}
		else
		{
			auto ValidRecords = LagComp->GetValidTicks( Entity->GetIndex( ) );
			if ( !ValidRecords.empty( ) )
			{
				for ( size_t i = 0; i < ValidRecords.size( ); i++ )
				{
					auto& Record = ValidRecords[ i ];

					if ( i == 0 )
						continue;

					if ( Record.Velocity.Length( ) < 1.f )
						continue;

					gDebugOverlay->AddTextOverlay( Record.Origin, gGlobals->absoluteframetime, "%i", i );
					//gRenderView->SetBlend( gMath->ReMapValClamped( i, 0, LagComp->Records[ Entity->GetIndex( ) ].size( ), Config->Visuals->BackTrackAlpha, 0.f) );

					if ( Record.Boner && Record.Model && Record.SetupBoner )
						f( ECX, EDX, State, Info, Record.Boner );
				}
			}
		}
	}
}

void CVisuals::DrawViewmodel( void* ECX, void* EDX, const DrawModelState_t& State, const ModelRenderInfo_t& Info, Matrix3x4* Matrix, std::function< void( void*, void*, const DrawModelState_t&, const ModelRenderInfo_t&, Matrix3x4* ) > f )
{
	auto Entity = gEntityList->GetClientEntity( Info.entity_index )->As<CBaseEntity>( );
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || gEngineVGui->IsGameUIVisible( ) || G->ProjectileCamera->IsDrawingProjectileCamera )
		return;

	if ( Entity && !Entity->IsDormant( ) && Entity->GetClientClass( )->ClassID == EClassID::CTFViewModel && Config->Visuals->HandsModels )
	{
		IMaterial* material = nullptr;

		switch ( Config->Visuals->HandsModel ) {
			case 1: {
				material = Models->FlatMaterial;
				break;
			}
			case 2: {
				material = Models->ShadedMaterial;
				break;
			}
			case 3: {
				material = Models->FresnelMaterial;
				break;
			}
			default: break;
		}

		if ( material )
		{
			gRenderView->SetColorModulation( Config->Colors->HandsColor );
			gRenderView->SetBlend( Config->Visuals->HandsAlpha );
		}
		else
		{
			gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
			gRenderView->SetBlend( 1.0f );
		}

		if ( !Config->Visuals->DisableHands )
		{
			gModelRender->ForcedMaterialOverride( material );
			f( ECX, EDX, State, Info, Matrix );

			if ( !Config->Visuals->HandsOverlay )
			{
				gRenderView->SetBlend( 1.0f );
				gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
				gModelRender->ForcedMaterialOverride( nullptr );
				return;
			}

			IMaterial* overlay_material = nullptr;
			switch ( Config->Visuals->HandsOverlayModel )
			{
				case 0:
				{
					if ( IMaterialVar* $envmaptint = Models->OverlayMaterial->FindVar( "$envmaptint", nullptr, false ) )
						$envmaptint->SetVecValue( ( float )Config->Colors->HandsOverlayColor.r / 255.f,
												  ( float )Config->Colors->HandsOverlayColor.g / 255.f,
												  ( float )Config->Colors->HandsOverlayColor.b / 255.f );

					overlay_material = Models->OverlayMaterial;
					break;
				}
				case 1:
				{
					if ( IMaterialVar* $sheenmaptint = Models->KSOverlayMaterial->FindVar( "$sheenmaptint", nullptr, false ) )
						$sheenmaptint->SetVecValue( ( float )Config->Colors->HandsOverlayColor.r / 255.f,
													( float )Config->Colors->HandsOverlayColor.g / 255.f,
													( float )Config->Colors->HandsOverlayColor.b / 255.f );

					overlay_material = Models->KSOverlayMaterial;
					break;
				}
				case 2:
				{
					if ( IMaterialVar* $envmaptint = Models->EsoOverlayMaterial->FindVar( "$envmaptint", nullptr, false ) )
						$envmaptint->SetVecValue( ( float )Config->Colors->HandsOverlayColor.r / 255.f,
												  ( float )Config->Colors->HandsOverlayColor.g / 255.f,
												  ( float )Config->Colors->HandsOverlayColor.b / 255.f );

					overlay_material = Models->EsoOverlayMaterial;
					break;
				}
				case 3:
				{
					gRenderView->SetColorModulation( Config->Colors->HandsOverlayColor );
					overlay_material = Models->FlatOverlayMaterial;
					break;
				}
			}

			overlay_material->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, Config->Visuals->HandsOverlayWireframe );
			overlay_material->SetMaterialVarFlag( MATERIAL_VAR_ADDITIVE, true );

			gRenderView->SetBlend( Config->Visuals->HandsOverlayAlpha );
			gModelRender->ForcedMaterialOverride( overlay_material );
			f( ECX, EDX, State, Info, Matrix );
		}

		gRenderView->SetBlend( 1.0f );
		gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
		gModelRender->ForcedMaterialOverride( nullptr );
	}

	if ( !Entity && Info.pModel && Config->Visuals->HandsModels )
	{
		std::string_view szModelName( gModelInfo->GetModelName( Info.pModel ) );

		if ( szModelName.find( ( "weapon" ) ) != std::string_view::npos
			 && szModelName.find( ( "arrow" ) ) == std::string_view::npos
			 && szModelName.find( ( "w_syringe" ) ) == std::string_view::npos
			 && szModelName.find( ( "nail" ) ) == std::string_view::npos
			 && szModelName.find( ( "shell" ) ) == std::string_view::npos
			 && szModelName.find( ( "parachute" ) ) == std::string_view::npos
			 && szModelName.find( ( "buffbanner" ) ) == std::string_view::npos
			 && szModelName.find( ( "shogun_warbanner" ) ) == std::string_view::npos
			 && szModelName.find( ( "targe" ) ) == std::string_view::npos //same as world model, can't filter
			 && szModelName.find( ( "shield" ) ) == std::string_view::npos //same as world model, can't filter
			 && szModelName.find( ( "repair_claw" ) ) == std::string_view::npos )
		{
			IMaterial* material = nullptr;

			switch ( Config->Visuals->HandsModel ) {
				case 1: {
					material = Models->FlatMaterial;
					break;
				}
				case 2: {
					material = Models->ShadedMaterial;
					break;
				}
				case 3: {
					material = Models->FresnelMaterial;
					break;
				}
				default: break;
			}

			if ( material )
			{
				gRenderView->SetColorModulation( Config->Colors->HandsColor );
				gRenderView->SetBlend( Config->Visuals->HandsAlpha );
			}
			else
			{
				gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
				gRenderView->SetBlend( 1.0f );
			}

			if ( !Config->Visuals->DisableWeapon )
			{
				gModelRender->ForcedMaterialOverride( material );
				f( ECX, EDX, State, Info, Matrix );

				if ( !Config->Visuals->HandsOverlay )
				{
					gRenderView->SetBlend( 1.0f );
					gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
					gModelRender->ForcedMaterialOverride( nullptr );
					return;
				}

				IMaterial* overlay_material = nullptr;
				switch ( Config->Visuals->HandsOverlayModel )
				{
					case 0:
					{
						if ( IMaterialVar* $envmaptint = Models->OverlayMaterial->FindVar( "$envmaptint", nullptr, false ) )
							$envmaptint->SetVecValue( ( float )Config->Colors->HandsOverlayColor.r / 255.f,
													  ( float )Config->Colors->HandsOverlayColor.g / 255.f,
													  ( float )Config->Colors->HandsOverlayColor.b / 255.f );

						overlay_material = Models->OverlayMaterial;
						break;
					}
					case 1:
					{
						if ( IMaterialVar* $sheenmaptint = Models->KSOverlayMaterial->FindVar( "$sheenmaptint", nullptr, false ) )
							$sheenmaptint->SetVecValue( ( float )Config->Colors->HandsOverlayColor.r / 255.f,
														( float )Config->Colors->HandsOverlayColor.g / 255.f,
														( float )Config->Colors->HandsOverlayColor.b / 255.f );

						overlay_material = Models->KSOverlayMaterial;
						break;
					}
					case 2:
					{
						if ( IMaterialVar* $envmaptint = Models->EsoOverlayMaterial->FindVar( "$envmaptint", nullptr, false ) )
							$envmaptint->SetVecValue( ( float )Config->Colors->HandsOverlayColor.r / 255.f,
													  ( float )Config->Colors->HandsOverlayColor.g / 255.f,
													  ( float )Config->Colors->HandsOverlayColor.b / 255.f );

						overlay_material = Models->EsoOverlayMaterial;
						break;
					}
					case 3:
					{
						gRenderView->SetColorModulation( Config->Colors->HandsOverlayColor );
						overlay_material = Models->FlatOverlayMaterial;
						break;
					}
				}

				overlay_material->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, Config->Visuals->HandsOverlayWireframe );
				overlay_material->SetMaterialVarFlag( MATERIAL_VAR_ADDITIVE, true );

				gRenderView->SetBlend( Config->Visuals->HandsOverlayAlpha );
				gModelRender->ForcedMaterialOverride( overlay_material );
				f( ECX, EDX, State, Info, Matrix );
			}
		}

		gRenderView->SetBlend( 1.0f );
		gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
		gModelRender->ForcedMaterialOverride( nullptr );
	}
}