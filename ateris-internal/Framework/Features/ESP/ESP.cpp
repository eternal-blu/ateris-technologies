#include "ESP.h"
#include "../../../Globals.h"

void CESP::DrawEntities( )
{
	if ( !gLocalPlayer || gEngineVGui->IsGameUIVisible( ) || !Config->ESP->Global->Enabled )
		return;

	DrawWorld( );
	DrawBuildings( );
	DrawPlayers( );
}

void CESP::DrawPlayers( )
{
	if ( !Config->ESP->Players->Enabled )
		return;

	for ( const auto& Player : gPlayers )
	{
		if ( !Player || ( Player->IsDormant( ) && ( gGlobals->curtime - G->FarEsp[ Player ] > 3.f || !gPlayerResource->IsAlive( Player->GetIndex( ) ) ) ) || !Player->IsAlive( ) )
		{
			continue;
		}

		if ( !gInput->CAM_IsThirdPerson( ) && Player == gLocalPlayer )
		{
			continue;
		}

		if ( Player != gLocalPlayer && !Player->IsSteamFriend( ) )
		{
			if ( !Config->ESP->Players->Team && !Player->IsEnemy( ) )
			{
				continue;
			}

			if ( !Config->ESP->Players->Enemy && Player->IsEnemy( ) )
			{
				continue;
			}
		}
		else
		{
			if ( Player->IsSteamFriend( ) && !Config->ESP->Players->Friends )
			{
				continue;
			}

			if ( Player == gLocalPlayer && !Config->ESP->Players->Local )
			{
				continue;
			}
		}

		Color ESP = Player->GetESPColor( Config->ESP->Global->ColorScheme );
		if ( Player->IsLocalPlayer( ) )
		{
			ESP = Config->Colors->ESPPlayerLocal;
		}

		if ( Player->IsSteamFriend( ) )
		{
			ESP = Config->Colors->ESPPlayerFriend;
		}

		if ( Config->Visuals->PlayerArrows ) {
			if ( !Player->IsEntityOnScreen( ) )
				PlayerArrow( Player, ESP );
		}

		Rect_t BBox;
		Vector Transformed[ 8 ];

		if ( !TF2->ComputeBoundingBox( Player, Transformed, BBox ) )
		{
			continue;
		}

		const Rect_t& BoundingBox = const_cast< Rect_t& >( BBox ); //In Mad's professional coder words: "jod bless performance"
		Color String = Player->IsDormant( ) ? Color( 255, 255, 255, 180 ) : Color( 255, 255, 255, 255 );

		PlayerInfo_t info;
		gEngineClient->GetPlayerInfo( Player->GetIndex( ), &info );

		if ( Player->IsSteamFriend( ) )
		{
			Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 25, FONT_ESP, Center_X, { 0, 200, 255, 255 }, true, "Friend" );	
		}
		else
		{
			if ( gPriorities[ info.friendsID ].Priority >= 10 )
			{
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 25, FONT_ESP, Center_X, 
					{ 255, 50, 50, 255 }, true, gPriorities[ info.friendsID ].CustomFlag );
				break;
			}
			else
			{
				switch ( gPriorities[ info.friendsID ].Priority )
				{
				case 0: break;
				case 1:
					Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 25, FONT_ESP, Center_X, { 255, 128, 0, 255 }, true, "Suspicious" );
					break;
				case 2:
					Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 25, FONT_ESP, Center_X, { 255, 50, 50, 255 }, true, "Cheater" );
					break;
				case 3:
					Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 25, FONT_ESP, Center_X, { 0, 200, 255, 255 }, true, "Friend" );
					break;
				case 4:
					Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 25, FONT_ESP, Center_X, { 150, 100, 255, 255 }, true, "Important" );
					break;
				default: break;
				}
			}
		}

		if ( Config->ESP->Players->EnabledBox )
		{
			switch ( Config->ESP->Players->Box )
			{
				case 0:
				{
					Draw->Rectangle( BoundingBox.x, BoundingBox.y, BoundingBox.w, BoundingBox.h, ESP );
					break;
				}

				case 1:
				{
					Draw3DBox( Transformed, ESP );
					break;
				}
				default: break;
			}
		}

		if ( Config->ESP->Players->Name )
		{
			std::wstring Name = TF2->Utf8ToWide( info.name );
			if ( Config->Visuals->StreamerMode )
			{
				if ( Player->IsLocalPlayer( ) && Config->Visuals->HideLocal )
				{
					Name = L"Local";
				}

				if ( Player->IsSteamFriend( ) && Config->Visuals->HideFriends )
				{
					Name = L"Friend";
				}
			}
			Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 13, FONT_ESP, Center_X, String, true, Name.c_str( ) );

			if ( !info.fakeplayer && Config->ESP->Players->Avatar && BoundingBox.h > 32 && !Player->IsDormant( ) )
			{
				int Avatar = gSteamFriends->GetSmallFriendAvatar( Player->GetSteamID( ) );
				unsigned int width, height;
				if ( gSteamUtils->GetImageSize( Avatar, &width, &height ) )
				{
					// the destination buffer size should be 4 * height * width * sizeof(char)
					unsigned char Image[ 4 * 32 * 32 ];
					if ( gSteamUtils->GetImageRGBA( Avatar, Image, 4 * 32 * 32 ) )
					{
						if ( !PlayerAvatars[ info.friendsID ] )
						{
							PlayerAvatars[ info.friendsID ] = Draw->CreateTexture( Image, 32, 32, true );
						}
						else
						{
							Draw->Rectangle( BoundingBox.x + BoundingBox.w + 2, BoundingBox.y - 1, 34, 34, { 0, 0, 0, 255 }, false );
							Draw->TextureRectangle( BoundingBox.x + BoundingBox.w + 3, BoundingBox.y, 32, 32, PlayerAvatars[ info.friendsID ], { 255, 255, 255, 255 } );
						}
					}
				}
			}
		}

		if ( Config->ESP->Players->Weapon )
		{
			Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y + BoundingBox.h, FONT_SMALL, Center_X, String, true, TF2->Utf8ToWide( Player->GetWeaponName( ) ).c_str( ) );
		}

		if ( Config->ESP->Players->HealthBar )
		{
			if ( PlayerHealth[ Player->GetIndex( ) ] < 0 || PlayerHealth[ Player->GetIndex( ) ] > Player->GetMaxHealth( ) )
				PlayerHealth[ Player->GetIndex( ) ] = Player->Health( );

			float& AnimatedHealth = PlayerHealth[ Player->GetIndex( ) ];

			if ( AnimatedHealth > Player->Health( ) )
			{
				if ( Player->GetMaxHealth( ) < 451 )
					AnimatedHealth -= ( Player->GetMaxHealth( ) * 1.5f ) * gGlobals->frametime;
				else
					AnimatedHealth -= ( Player->GetMaxHealth( ) * 1.5f ) * gGlobals->frametime * Player->GetMaxHealth( ) / 300.f;
			}
			else
				AnimatedHealth = Player->Health( );

			auto HealthRatio = gMath->GetFraction( ( Config->ESP->Players->PHealthBar->Animated ? AnimatedHealth : Player->Health( ) ), Player->GetMaxHealth( ), 0.0f );
			auto BarHeight = static_cast< float >( BoundingBox.h ) * HealthRatio;
			auto StartY = ( BoundingBox.y + ( BoundingBox.h - BarHeight ) ) + ( Player->Health( ) < Player->GetMaxHealth( ) ? 1 : 0 );
			auto HealthColor = Config->ESP->Players->PHealthBar->CustomColor ? ESP : TF2->GetHealthColor( Player->Health( ), Player->GetMaxHealth( ) );

			Draw->Rectangle( BoundingBox.x - 6, BoundingBox.y - 1, 4, BoundingBox.h + 2, { 0, 0, 0, 180 }, false, true );
			Draw->Rectangle( BoundingBox.x - 5, StartY, 2, BarHeight, HealthColor, false, true );
			Draw->Rectangle( BoundingBox.x - 6, BoundingBox.y - 1, 4, BoundingBox.h + 2, { 0, 0, 0, 180 }, false );
			if ( Player->Health( ) < Player->GetMaxHealth( ) )
			{
				Draw->String( BoundingBox.x - 7, StartY - 1, FONT_SMALL, Reverse_X, { 255, 255, 255, 255 }, true, L"%i", Player->Health( ) );
			}

			if ( Config->ESP->Players->PHealthBar->Lines )
			{
				float HealthBarY = 0.f;
				for ( int i = 1; i <= Config->ESP->Players->PHealthBar->LinesCount; i++ )
				{
					if ( i == 1 )
						HealthBarY = 1.f / ( Config->ESP->Players->PHealthBar->LinesCount + 1 );

					if ( BoundingBox.y + BoundingBox.h * HealthBarY > StartY )
					{
						Draw->Line( BoundingBox.x - 6, BoundingBox.y + BoundingBox.h * HealthBarY, BoundingBox.x - 3, BoundingBox.y + BoundingBox.h * HealthBarY, { 0, 0, 0, 220 }, false );
					}
					HealthBarY += 1.f / ( Config->ESP->Players->PHealthBar->LinesCount + 1 );
				}
			}
		}

		if ( Config->ESP->Players->UberBar && Player->Class( ) == CLASS_MEDIC )
		{
			const auto& Medigun = Player->GetWeaponFromSlot( SLOT_SECONDARY );

			if ( !Medigun )
				continue;

			float flUber = Medigun->ChargeLevel( ) * ( Medigun->ItemDefinitionIndex( ) == Medic_s_TheVaccinator
														 ? 400.0f
														 : 100.0f );

			float flMaxUber = ( Medigun->ItemDefinitionIndex( ) == Medic_s_TheVaccinator ? 400.0f : 100.0f );

			if ( flUber > flMaxUber )
			{
				flUber = flMaxUber;
			}

			static constexpr int RECT_WIDTH = 2;
			auto HealthRatio = gMath->GetFraction( flUber, flMaxUber, 0.0f );
			auto BarHeight = static_cast< float >( BoundingBox.h ) * HealthRatio;
			auto StartY = ( BoundingBox.y + ( BoundingBox.h - BarHeight ) ) + ( flUber < flMaxUber ? 1 : 0 );

			float ratio = flUber / flMaxUber;
			Draw->Rectangle( BoundingBox.x + BoundingBox.w + 2, BoundingBox.y - 1, RECT_WIDTH + 2, BoundingBox.h + 2, { 0, 0, 0, 180 }, false, true );
			Draw->Rectangle( BoundingBox.x + BoundingBox.w + 3, StartY, 2, BarHeight, { 255, 110, 255, 255 }, false, true );
			Draw->Rectangle( BoundingBox.x + BoundingBox.w + 2, BoundingBox.y - 1, RECT_WIDTH + 2, BoundingBox.h + 2, { 0, 0, 0, 180 }, false );

			if ( flMaxUber > 100.f )
			{
				Draw->Line( BoundingBox.x + BoundingBox.w + 2, BoundingBox.y + BoundingBox.h / 2, BoundingBox.x + BoundingBox.w + 5, BoundingBox.y + BoundingBox.h / 2, { 0, 0, 0, 220 }, false );//middle
				Draw->Line( BoundingBox.x + BoundingBox.w + 2, BoundingBox.y + BoundingBox.h / 4, BoundingBox.x + BoundingBox.w + 5, BoundingBox.y + BoundingBox.h / 4, { 0, 0, 0, 220 }, false );//top
				Draw->Line( BoundingBox.x + BoundingBox.w + 2, BoundingBox.y + BoundingBox.h * 0.75, BoundingBox.x + BoundingBox.w + 5, BoundingBox.y + BoundingBox.h * 0.75, { 0, 0, 0, 220 }, false );//bottom
			}
		}

		if ( Config->ESP->Players->Flags )
		{
			int y = BoundingBox.y - 2;
			PlayerInfo_t PlayerInfo;
			gEngineClient->GetPlayerInfo( Player->GetIndex( ), &PlayerInfo );
			int TextOffset = ( !PlayerInfo.fakeplayer && Config->ESP->Players->Avatar && BoundingBox.h > 32 && !Player->IsDormant( ) ) ? 36 : 0;

			if ( Player->IsDormant( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 220, 240, 170 ), true, "D" );
				TextOffset += 8;
			}

			if ( Player->SimulationTime( ) < Player->OldSimulationTime( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 25, 25 ), true, "WARPING" );
				TextOffset += 8;
			}

			if ( Player->IsOverhealed( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 150, 255, 150 ), true, "+%i HP", Player->Health( ) - Player->GetMaxHealth( ) );
				TextOffset += 8;
			}

			if ( Player->IsCloaked( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 200, 200, 200 ), true, "CLOAK" );
				TextOffset += 8;
			}

			if ( Player->IsInvulnerable( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 110, 255 ), true, "UBER" );
				TextOffset += 8;
			}

			if ( Player->IsCritBoosted( ) || Player->IsInCondition( TF_COND_RUNE_IMBALANCE ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 150, 255 ), true, "CRITS" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_BLASTJUMPING ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 150, 255 ), true, "BLAST" );
				TextOffset += 8;
			}

			if ( Player->IsStunned( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 150, 150, 150 ), true, "STUN" );
				TextOffset += 8;
			}

			if ( Player->IsSlowed( ) && Player->Class( ) == CLASS_HEAVY )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 150, 150, 150 ), true, "REV" );
				TextOffset += 8;
			}

			if ( Player->IsBonked( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 144, 110 ), true, "BONK" );
				TextOffset += 8;
			}

			if ( Player->IsZoomed( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 50, 200, 255 ), true, "ZOOM" );
				TextOffset += 8;
			}

			if ( Player->IsTaunting( ) || ( G->FakeTaunt->FakeTauntTime > gEngineClient->Time( ) && Player == gLocalPlayer ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 200, 100, 255 ), true, "TAUNT" );
				TextOffset += 8;
			}

			if ( Player->IsMarkedForDeath( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 200, 255, 50 ), true, "MARKED" );
				TextOffset += 8;
			}

			if ( Player->IsInBattalionsRange( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 128, 128, 255 ), true, "BATTALIONS" );
				TextOffset += 8;
			}

			if ( Player->IsInConcherorRange( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 255, 150 ), true, "CONCHEROR" );
				TextOffset += 8;
			}

			if ( Player->IsInBuffBannerRange( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 150, 255 ), true, "BUFF BANNER" );
				TextOffset += 8;
			}

			if ( Player->IsMiniCritBoosted( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 150, 255 ), true, "MINICRIT" );
				TextOffset += 8;
			}

			if ( Player->IsCoveredInMilk( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 255, 255 ), true, "MILK" );
				TextOffset += 8;
			}

			if ( Player->IsBleeding( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 50, 50 ), true, "BLEED" );
				TextOffset += 8;
			}

			if ( Player->IsOnFire( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 150, 50 ), true, "FIRE" );
				TextOffset += 8;
			}

			if ( Player->IsBulletCharged( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 110, 255 ), true, "BULLET CHARGE" );
				TextOffset += 8;
			}
			else if ( Player->IsBulletResistant( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 160, 255 ), true, "BULLET RES" );
				TextOffset += 8;
			}

			if ( Player->IsExplosiveCharged( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 110, 255 ), true, "BLAST CHARGE" );
				TextOffset += 8;
			}
			else if ( Player->IsExplosiveResistant( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 160, 255 ), true, "BLAST RES" );
				TextOffset += 8;
			}

			if ( Player->IsFireCharged( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 110, 255 ), true, "FIRE CHARGE" );
				TextOffset += 8;
			}
			else if ( Player->IsFireResistant( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 160, 255 ), true, "FIRE RES" );
				TextOffset += 8;
			}

			//mannpower conditions
			if ( Player->IsInCondition( TF_COND_RUNE_STRENGTH ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 80, 80 ), true, "STRENGTH" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_HASTE ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 255, 0 ), true, "HASTE" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_REGEN ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 150, 255, 150 ), true, "REGENERATION" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_RESIST ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 128, 128, 255 ), true, "RESISTANCE" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_VAMPIRE ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 200, 50, 50 ), true, "VAMPIRE" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_REFLECT ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 128, 0 ), true, "REFLECT" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_PRECISION ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 255, 0 ), true, "PRECISION" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_AGILITY ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 0, 190, 255 ), true, "AGILITY" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_KNOCKOUT ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 150, 150, 150 ), true, "KNOCK-OUT" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_KING ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 200, 50 ), true, "KING" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_KING_BUFFED ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 200, 50 ), true, "KING BUFFED" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_PLAGUE ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 150, 200, 50 ), true, "PLAGUE" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_PLAGUE ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, Color( 255, 50, 50 ), true, "PLAGUED" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_RUNE_SUPERNOVA ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, gGlobals->tickcount % 63 < 32 ? Color( 255, 255, 255 ) : Color( 128, 128, 128 ), true, "SUPERNOVA (FAG)" );
				TextOffset += 8;
			}

			if ( Player->IsInCondition( TF_COND_POWERUPMODE_DOMINANT ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3 + ( Player->Class( ) == CLASS_MEDIC ? 4 : 0 ), y + TextOffset, FONT_SMALL, 0, gGlobals->tickcount % 63 < 32 ? Color( 255, 255, 255 ) : Color( 128, 128, 128 ), true, "DOMINANT (FAG)" );
				TextOffset += 8;
			}
		}
	}
}

void CESP::DrawBuildings( )
{
	if ( !Config->ESP->Buildings->Enabled )
		return;

	for ( const auto& Building : gBuildings )
	{
		if ( !Building || Building->IsDormant( ) )
		{
			continue;
		}

		const auto& Builder = Building->GetBuilder( );
		if ( !Builder )
		{
			continue;
		}

		if ( Builder != gLocalPlayer && !Builder->IsSteamFriend( ) )
		{
			if ( !Config->ESP->Buildings->Team && !Building->IsEnemy( ) )
			{
				continue;
			}

			if ( !Config->ESP->Buildings->Enemy && Building->IsEnemy( ) )
			{
				continue;
			}
		}
		else
		{
			if ( Builder->IsSteamFriend( ) && !Config->ESP->Buildings->Friends )
			{
				continue;
			}

			if ( Builder == gLocalPlayer && !Config->ESP->Buildings->Local )
			{
				continue;
			}
		}

		Rect_t BBox;
		Vector Transformed[ 8 ];

		if ( !TF2->ComputeBoundingBox( Building, Transformed, BBox ) )
		{
			continue;
		}

		const Rect_t& BoundingBox = const_cast< Rect_t& >( BBox ); //In Mad's professional coder words: "jod bless performance"
		Color String = { 255, 255, 255, 255 };
		Color ESP = Building->GetESPColor( Config->ESP->Global->ColorScheme );
		if ( Builder->IsLocalPlayer( ) )
		{
			ESP = Config->Colors->ESPBuildingLocal;
		}

		if ( Builder->IsSteamFriend( ) )
		{
			ESP = Config->Colors->ESPBuildingFriend;
		}

		if ( Config->ESP->Buildings->EnabledBox )
		{
			switch ( Config->ESP->Buildings->Box )
			{
				case 0:
				{
					Draw->Rectangle( BoundingBox.x, BoundingBox.y, BoundingBox.w, BoundingBox.h, ESP );
					break;
				}

				case 1:
				{
					Draw3DBox( Transformed, ESP );
					break;
				}
				default: break;
			}
		}

		if ( Config->ESP->Buildings->Name )
		{
			Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 13, FONT_ESP, Center_X, String, true, Building->GetBuildingName( ) );
		}

		if ( Config->ESP->Buildings->HealthBar )
		{
			if ( PlayerHealth[ Building->GetIndex( ) ] < 0 || PlayerHealth[ Building->GetIndex( ) ] > Building->MaxHealth( ) )
				PlayerHealth[ Building->GetIndex( ) ] = Building->Health( );

			float& AnimatedHealth = PlayerHealth[ Building->GetIndex( ) ];

			if ( AnimatedHealth > Building->Health( ) )
			{
				if ( Building->MaxHealth( ) < 451 )
					AnimatedHealth -= ( Building->MaxHealth( ) * 1.5f ) * gGlobals->frametime;
				else
					AnimatedHealth -= ( Building->MaxHealth( ) * 1.5f ) * gGlobals->frametime * Building->MaxHealth( ) / 300.f;
			}
			else
				AnimatedHealth = Building->Health( );

			auto HealthRatio = gMath->GetFraction( ( Config->ESP->Buildings->PHealthBar->Animated ? AnimatedHealth : Building->Health( ) ), Building->MaxHealth( ), 0.0f );
			auto BarHeight = static_cast< float >( BoundingBox.h ) * HealthRatio;
			auto StartY = ( BoundingBox.y + ( BoundingBox.h - BarHeight ) ) + ( Building->Health( ) < Building->MaxHealth( ) ? 1 : 0 );
			auto HealthColor = Config->ESP->Buildings->PHealthBar->CustomColor ? ESP : TF2->GetHealthColor( Building->Health( ), Building->MaxHealth( ) );

			Draw->Rectangle( BoundingBox.x - 6, BoundingBox.y - 1, 4, BoundingBox.h + 2, { 0, 0, 0, 180 }, false, true );
			Draw->Rectangle( BoundingBox.x - 5, StartY, 2, BarHeight, HealthColor, false, true );
			if ( Building->Health( ) < Building->MaxHealth( ) )
			{
				Draw->String( BoundingBox.x - 6, StartY - 3, FONT_SMALL, Reverse_X, { 255, 255, 255, 255 }, true, L"%i", Building->Health( ) );
			}

			if ( Config->ESP->Buildings->PHealthBar->Lines )
			{
				float HealthBarY = 0.f;
				for ( int i = 1; i <= Config->ESP->Buildings->PHealthBar->LinesCount; i++ )
				{
					if ( i == 1 )
						HealthBarY = 1.f / ( Config->ESP->Buildings->PHealthBar->LinesCount + 1 );

					if ( BoundingBox.y + BoundingBox.h * HealthBarY > StartY )
					{
						Draw->Line( BoundingBox.x - 6, BoundingBox.y + BoundingBox.h * HealthBarY, BoundingBox.x - 3, BoundingBox.y + BoundingBox.h * HealthBarY, { 0, 0, 0, 220 }, false );
					}
					HealthBarY += 1.f / ( Config->ESP->Buildings->PHealthBar->LinesCount + 1 );
				}
			}
		}

		if ( Config->ESP->Buildings->Flags )
		{
			int TextOffset = 0;
			if ( Building->IsTeleporterAndRecharging( ) )
			{
				float RechargeTime = Building->RechargeTime( ) - gGlobals->curtime + 0.05f;

				if ( RechargeTime > 0 )
				{
					Draw->String( BoundingBox.x + BoundingBox.w + 3, BoundingBox.y - 2, FONT_SMALL, 0, { 255, 255, 255, 255 }, true, "RECHARGES IN: %.1f", RechargeTime );
					TextOffset += 8;
				}
			}

			if ( Building->UpgradeLevel( ) )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3, BoundingBox.y + TextOffset - 2, FONT_SMALL, 0, { 255, 255, 255, 255 }, true, "LEVEL: %i", Building->UpgradeLevel( ) );
			}
		}
	}
}

void CESP::DrawWorld( )
{
	if ( !Config->ESP->World->Enabled )
		return;

	if ( Config->ESP->World->Healthpacks )
	{
		for ( const auto& Health : gHealthpacks )
		{
			if ( !Health || Health->IsDormant( ) )
			{
				continue;
			}

			Rect_t BBox;
			Vector Transformed[ 8 ];

			if ( !TF2->ComputeBoundingBox( Health, Transformed, BBox ) )
			{
				continue;
			}

			const Rect_t& BoundingBox = const_cast< Rect_t& >( BBox ); //In Mad's professional coder words: "jod bless performance"
			Color String = { 255, 255, 255, 255 };
			Color ESP = Health->GetESPColor( Config->ESP->Global->ColorScheme );

			if ( Config->ESP->World->EnabledBox )
			{
				switch ( Config->ESP->World->Box )
				{
					case 0:
					{
						Draw->Rectangle( BoundingBox.x, BoundingBox.y, BoundingBox.w, BoundingBox.h, ESP );
						break;
					}

					case 1:
					{
						Draw3DBox( Transformed, ESP );
						break;
					}
					default: break;
				}
			}

			if ( Config->ESP->World->Name )
			{
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 13, FONT_ESP, Center_X, String, true, "Health" );
			}

			if ( Config->ESP->World->Distance )
			{
				float distance = gLocalPlayer->Origin( ).DistTo( Health->Origin( ) ) * 0.01905f;
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y + BoundingBox.h, FONT_SMALL, Center_X, String, true, "%iM", ( int )distance );
			}
		}
	}

	if ( Config->ESP->World->Ammopacks )
	{
		for ( const auto& Ammo : gAmmopacks )
		{
			if ( !Ammo || Ammo->IsDormant( ) )
			{
				continue;
			}

			Rect_t BBox;
			Vector Transformed[ 8 ];

			if ( !TF2->ComputeBoundingBox( Ammo, Transformed, BBox ) )
			{
				continue;
			}

			const Rect_t& BoundingBox = const_cast< Rect_t& >( BBox ); //In Mad's professional coder words: "jod bless performance"
			Color String = { 255, 255, 255, 255 };
			Color ESP = Ammo->GetESPColor( Config->ESP->Global->ColorScheme );

			if ( Config->ESP->World->EnabledBox )
			{
				switch ( Config->ESP->World->Box )
				{
					case 0:
					{
						Draw->Rectangle( BoundingBox.x, BoundingBox.y, BoundingBox.w, BoundingBox.h, ESP );
						break;
					}

					case 1:
					{
						Draw3DBox( Transformed, ESP );
						break;
					}
					default: break;
				}
			}

			if ( Config->ESP->World->Name )
			{
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 13, FONT_ESP, Center_X, String, true, "Ammo" );
			}

			if ( Config->ESP->World->Distance )
			{
				float distance = gLocalPlayer->Origin( ).DistTo( Ammo->Origin( ) ) * 0.01905f;
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y + BoundingBox.h, FONT_SMALL, Center_X, String, true, "%iM", ( int )distance );
			}
		}
	}

	if ( Config->ESP->World->Intels )
	{
		for ( const auto& Intel : gIntels )
		{
			if ( !Intel || Intel->IsDormant( ) )
			{
				continue;
			}

			if ( Intel->FlagStatus( ) == TF_FLAGINFO_STOLEN )
			{
				continue;
			}

			Rect_t BBox;
			Vector Transformed[ 8 ];

			if ( !TF2->ComputeBoundingBox( Intel, Transformed, BBox ) )
			{
				continue;
			}

			const Rect_t& BoundingBox = const_cast< Rect_t& >( BBox ); //In Mad's professional coder words: "jod bless performance"
			Color String = { 255, 255, 255, 255 };
			Color ESP = Intel->GetESPColor( Config->ESP->Global->ColorScheme );

			if ( Config->ESP->World->EnabledBox )
			{
				switch ( Config->ESP->World->Box )
				{
					case 0:
					{
						Draw->Rectangle( BoundingBox.x, BoundingBox.y, BoundingBox.w, BoundingBox.h, ESP );
						break;
					}

					case 1:
					{
						Draw3DBox( Transformed, ESP );
						break;
					}
					default: break;
				}
			}

			if ( Config->ESP->World->Name )
			{
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 13, FONT_ESP, Center_X, String, true, "Intel" );
			}

			if ( Config->ESP->World->Distance )
			{
				float distance = gLocalPlayer->Origin( ).DistTo( Intel->Origin( ) ) * 0.01905f;
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y + BoundingBox.h, FONT_SMALL, Center_X, String, true, "%iM", ( int )distance );
			}

			int y = BoundingBox.y - 2;
			int TextOffset = 0;

			//zphr said i should remove it so ye
			/*Draw->String( BoundingBox.x + BoundingBox.w + 3, y + TextOffset, FONT_SMALL, 0, Color( 255, 255, 255 ), true, Intel->GetIntelStatus( ) );
			TextOffset += 8;*/

			if ( Intel->FlagStatus( ) == TF_FLAGINFO_DROPPED )
			{
				Draw->String( BoundingBox.x + BoundingBox.w + 3, y + TextOffset, FONT_SMALL, 0, Color( 255, 255, 255 ), true, "RETURNS IN: %.1f", Intel->GetIntelReturnTime( ) );
				TextOffset += 8;
			}
		}
	}

	if ( Config->ESP->World->Bombs )
	{
		for ( const auto& Bomb : gBombs )
		{
			if ( !Bomb || Bomb->IsDormant( ) )
			{
				continue;
			}

			Rect_t BBox;
			Vector Transformed[ 8 ];

			if ( !TF2->ComputeBoundingBox( Bomb, Transformed, BBox ) )
			{
				continue;
			}

			const Rect_t& BoundingBox = const_cast< Rect_t& >( BBox ); //In Mad's professional coder words: "jod bless performance"
			Color String = { 255, 255, 255, 255 };
			Color ESP = Bomb->GetESPColor( Config->ESP->Global->ColorScheme );

			if ( Config->ESP->World->EnabledBox )
			{
				switch ( Config->ESP->World->Box )
				{
					case 0:
					{
						Draw->Rectangle( BoundingBox.x, BoundingBox.y, BoundingBox.w, BoundingBox.h, ESP );
						break;
					}

					case 1:
					{
						Draw3DBox( Transformed, ESP );
						break;
					}
					default: break;
				}
			}

			if ( Config->ESP->World->Name )
			{
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 13, FONT_ESP, Center_X, String, true, "Bomb" );
			}

			if ( Config->ESP->World->Distance )
			{
				float distance = gLocalPlayer->Origin( ).DistTo( Bomb->Origin( ) ) * 0.01905f;
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y + BoundingBox.h, FONT_SMALL, Center_X, String, true, "%iM", ( int )distance );
			}
		}
	}

	if ( Config->ESP->World->MvMCash )
	{
		for ( const auto& Cash : gMvMCash )
		{
			if ( !Cash || Cash->IsDormant( ) )
			{
				continue;
			}

			Rect_t BBox;
			Vector Transformed[ 8 ];

			if ( !TF2->ComputeBoundingBox( Cash, Transformed, BBox ) )
			{
				continue;
			}

			const Rect_t& BoundingBox = const_cast< Rect_t& >( BBox ); //In Mad's professional coder words: "jod bless performance"
			Color String = { 255, 255, 255, 255 };
			Color ESP = Config->Colors->ESPWorldMoney;

			if ( Config->ESP->World->EnabledBox )
			{
				switch ( Config->ESP->World->Box )
				{
					case 0:
					{
						Draw->Rectangle( BoundingBox.x, BoundingBox.y, BoundingBox.w, BoundingBox.h, ESP );
						break;
					}

					case 1:
					{
						Draw3DBox( Transformed, ESP );
						break;
					}
					default: break;
				}
			}

			if ( Config->ESP->World->Name )
			{
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 13, FONT_ESP, Center_X, String, true, "Money" );
			}

			if ( Config->ESP->World->Distance )
			{
				float distance = gLocalPlayer->Origin( ).DistTo( Cash->Origin( ) ) * 0.01905f;
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y + BoundingBox.h, FONT_SMALL, Center_X, String, true, "%iM", ( int )distance );
			}
		}
	}

	if ( Config->ESP->World->MPPowerups )
	{
		for ( const auto& Powerup : gPowerups )
		{
			if ( !Powerup || Powerup->IsDormant( ) )
			{
				continue;
			}

			Rect_t BBox;
			Vector Transformed[ 8 ];

			if ( !TF2->ComputeBoundingBox( Powerup, Transformed, BBox ) )
			{
				continue;
			}

			const Rect_t& BoundingBox = const_cast< Rect_t& >( BBox ); //In Mad's professional coder words: "jod bless performance"
			Color String = { 255, 255, 255, 255 };
			Color ESP = Config->Colors->ESPWorldPowerup;

			if ( Config->ESP->World->EnabledBox )
			{
				switch ( Config->ESP->World->Box )
				{
					case 0:
					{
						Draw->Rectangle( BoundingBox.x, BoundingBox.y, BoundingBox.w, BoundingBox.h, ESP );
						break;
					}

					case 1:
					{
						Draw3DBox( Transformed, ESP );
						break;
					}
					default: break;
				}
			}

			if ( Config->ESP->World->Name )
			{
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 13, FONT_ESP, Center_X, String, true, Powerup->GetPowerupName( FNV1A::Hash( gModelInfo->GetModelName( Powerup->GetModel( ) ) ) ) );
			}

			if ( Config->ESP->World->Distance )
			{
				float distance = gLocalPlayer->Origin( ).DistTo( Powerup->Origin( ) ) * 0.01905f;
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y + BoundingBox.h, FONT_SMALL, Center_X, String, true, "%iM", ( int )distance );
			}
		}
	}

	if ( Config->ESP->World->MvMTanks || Config->ESP->World->MvMSentryBusters )
	{
		for ( const auto& Entity : gMvMEntities )
		{
			if ( !Entity || Entity->IsDormant( ) )
			{
				continue;
			}

			Rect_t BBox;
			Vector Transformed[ 8 ];

			if ( !TF2->ComputeBoundingBox( Entity, Transformed, BBox ) )
			{
				continue;
			}

			if ( !Config->ESP->World->Team && !Entity->IsEnemy( ) )
			{
				continue;
			}

			if ( !Config->ESP->World->Enemy && Entity->IsEnemy( ) )
			{
				continue;
			}

			if ( !Config->ESP->World->MvMSentryBusters && Entity->IsSentryBuster( ) )
			{
				continue;
			}

			if ( !Config->ESP->World->MvMTanks && Entity->IsTank( ) )
			{
				continue;
			}

			const Rect_t& BoundingBox = const_cast< Rect_t& >( BBox ); //In Mad's professional coder words: "jod bless performance"
			Color String = { 255, 255, 255, 255 };
			Color ESP = Entity->GetESPColor( Config->ESP->Global->ColorScheme );

			if ( Config->ESP->World->EnabledBox )
			{
				switch ( Config->ESP->World->Box )
				{
					case 0:
					{
						Draw->Rectangle( BoundingBox.x, BoundingBox.y, BoundingBox.w, BoundingBox.h, ESP );
						break;
					}

					case 1:
					{
						Draw3DBox( Transformed, ESP );
						break;
					}
					default: break;
				}
			}

			if ( Config->ESP->World->Name )
			{
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 13, FONT_ESP, Center_X, String, true, Entity->IsTank( ) ? "Tank" : "Sentry Buster" );
			}

			if ( Config->ESP->World->Distance )
			{
				float distance = gLocalPlayer->Origin( ).DistTo( Entity->Origin( ) ) * 0.01905f;
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y + BoundingBox.h, FONT_SMALL, Center_X, String, true, "%iM", ( int )distance );
			}
		}
	}

	if ( Config->ESP->World->Projectiles )
	{
		for ( const auto& Projectile : gProjectiles )
		{
			if ( !Projectile || Projectile->IsDormant( ) )
			{
				continue;
			}

			Rect_t BBox;
			Vector Transformed[ 8 ];

			if ( !TF2->ComputeBoundingBox( Projectile, Transformed, BBox ) )
			{
				continue;
			}

			CBasePlayer* Thrower = Projectile->GetThrower( );
			if ( !Thrower )
			{
				Thrower = Projectile->GetOwnerEntity( )->As<CBasePlayer>( );
			}

			if ( !Thrower )
			{
				continue;
			}

			if ( Thrower != gLocalPlayer && !Thrower->IsSteamFriend( ) )
			{
				if ( !Config->ESP->World->Team && !Projectile->IsEnemy( ) )
				{
					continue;
				}

				if ( !Config->ESP->World->Enemy && Projectile->IsEnemy( ) )
				{
					continue;
				}
			}
			else
			{
				if ( Thrower->IsSteamFriend( ) && !Config->ESP->World->Friends )
				{
					continue;
				}

				if ( Thrower == gLocalPlayer && !Config->ESP->World->Local )
				{
					continue;
				}
			}

			const Rect_t& BoundingBox = const_cast< Rect_t& >( BBox ); //In Mad's professional coder words: "jod bless performance"
			Color String = { 255, 255, 255, 255 };
			Color ESP = Projectile->GetESPColor( Config->ESP->Global->ColorScheme );
			if ( Thrower->IsLocalPlayer( ) )
			{
				ESP = Config->Colors->ESPWorldLocal;
			}

			if ( Thrower->IsSteamFriend( ) )
			{
				ESP = Config->Colors->ESPWorldFriend;
			}

			if ( Config->ESP->World->EnabledBox )
			{
				switch ( Config->ESP->World->Box )
				{
					case 0:
					{
						Draw->Rectangle( BoundingBox.x, BoundingBox.y, BoundingBox.w, BoundingBox.h, ESP );
						break;
					}

					case 1:
					{
						Draw3DBox( Transformed, ESP );
						break;
					}
					default: break;
				}
			}

			if ( Config->ESP->World->Name )
			{
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y - 13, FONT_ESP, Center_X, String, true, Projectile->GetProjectileName( ) );
			}

			if ( Config->ESP->World->Distance )
			{
				float distance = gLocalPlayer->Origin( ).DistTo( Projectile->Origin( ) ) * 0.01905f;
				Draw->String( BoundingBox.x + BoundingBox.w * 0.5f, BoundingBox.y + BoundingBox.h, FONT_SMALL, Center_X, String, true, "%iM", ( int )distance );
			}
		}
	}
}

void CESP::Draw3DBox( const Vector* points, Color& Clr )
{
	Vector vStart, vEnd;

	for ( int i = 0; i < 3; i++ )
	{
		if ( TF2->WorldToScreen( points[ i ], vStart ) )
		{
			if ( TF2->WorldToScreen( points[ i + 1 ], vEnd ) )
			{
				Draw->Line( vStart.x, vStart.y, vEnd.x, vEnd.y, Clr );
			}
		}
	}

	if ( TF2->WorldToScreen( points[ 0 ], vStart ) )
	{
		if ( TF2->WorldToScreen( points[ 3 ], vEnd ) )
		{
			Draw->Line( vStart.x, vStart.y, vEnd.x, vEnd.y, Clr );
		}
	}

	for ( int i = 4; i < 7; i++ )
	{
		if ( TF2->WorldToScreen( points[ i ], vStart ) )
		{
			if ( TF2->WorldToScreen( points[ i + 1 ], vEnd ) )
			{
				Draw->Line( vStart.x, vStart.y, vEnd.x, vEnd.y, Clr );
			}
		}
	}

	if ( TF2->WorldToScreen( points[ 4 ], vStart ) )
	{
		if ( TF2->WorldToScreen( points[ 7 ], vEnd ) )
		{
			Draw->Line( vStart.x, vStart.y, vEnd.x, vEnd.y, Clr );
		}
	}

	if ( TF2->WorldToScreen( points[ 0 ], vStart ) )
	{
		if ( TF2->WorldToScreen( points[ 6 ], vEnd ) )
		{
			Draw->Line( vStart.x, vStart.y, vEnd.x, vEnd.y, Clr );
		}
	}

	if ( TF2->WorldToScreen( points[ 1 ], vStart ) )
	{
		if ( TF2->WorldToScreen( points[ 5 ], vEnd ) )
		{
			Draw->Line( vStart.x, vStart.y, vEnd.x, vEnd.y, Clr );
		}
	}

	if ( TF2->WorldToScreen( points[ 2 ], vStart ) )
	{
		if ( TF2->WorldToScreen( points[ 4 ], vEnd ) )
		{
			Draw->Line( vStart.x, vStart.y, vEnd.x, vEnd.y, Clr );
		}
	}

	if ( TF2->WorldToScreen( points[ 3 ], vStart ) )
	{
		if ( TF2->WorldToScreen( points[ 7 ], vEnd ) )
		{
			Draw->Line( vStart.x, vStart.y, vEnd.x, vEnd.y, Clr );
		}
	}
}

void CESP::PlayerArrow( CBasePlayer* Player, Color& Clr ) {
	if ( !Player || Player->IsDormant( ) )
		return;

	auto get_clockwise_angle = [ & ] ( const Vector& view_angles, const Vector& aim_angle ) -> float {
		Vector angle, aim;
		gMath->AngleVectors( view_angles, &angle );
		gMath->AngleVectors( aim_angle, &aim );

		return -atan2f( angle.x * aim.y - angle.y * aim.x, angle.x * aim.x + angle.y * aim.y );
	};

	const Vector angle_to = gMath->CalcAngle( gLocalPlayer->Origin( ), Player->Origin( ) );
	const float degrees = get_clockwise_angle( gEngineClient->GetViewAngles( ), angle_to );

	const float x1 = ( TF2->ScreenWidth * Config->Visuals->ArrowsScreenDistance * ( Config->Visuals->ArrowsScreenShape ? ( ( float )TF2->ScreenWidth / ( float )TF2->ScreenHeight ) : 1.0f ) + 5.0f ) * cos( degrees - PI / 2 );
	const float y1 = ( TF2->ScreenWidth * Config->Visuals->ArrowsScreenDistance + 5.0f ) * sin( degrees - PI / 2 );
	const float x2 = ( TF2->ScreenWidth * Config->Visuals->ArrowsScreenDistance * ( Config->Visuals->ArrowsScreenShape ? ( ( float )TF2->ScreenWidth / ( float )TF2->ScreenHeight ) : 1.0f ) + 15.0f ) * cos( degrees - PI / 2 );
	const float y2 = ( TF2->ScreenWidth * Config->Visuals->ArrowsScreenDistance + 15.0f ) * sin( degrees - PI / 2 );

	const float arrow_angle = DEG2RAD( 90.f );
	const float arrow_length = 25.f;

	const Vector2D line{ x2 - x1, y2 - y1 };
	const float length = line.Length( );

	const float f_point_on_line = arrow_length / ( atanf( arrow_angle ) * length );
	const Vector2D point_on_line = Vector2D( x2, y2 ) + ( line * f_point_on_line * -1.0f );
	const Vector2D normal_Vector{ -line.y, line.x };
	const Vector2D normal = Vector2D( arrow_length, arrow_length ) / ( length * 2 );

	const Vector2D rotation = normal * normal_Vector;
	const Vector2D left = point_on_line + rotation;
	const Vector2D right = point_on_line - rotation;

	const float center_x = TF2->ScreenWidth * 0.5f;
	const float center_y = TF2->ScreenHeight * 0.5f;

	//outer
	//Draw->Line( center_x + left.x, center_y + left.y, center_x + right.x, center_y + right.y, Clr, false );
	Draw->Line( center_x + left.x, center_y + left.y, center_x + x2, center_y + y2, Clr, false );
	Draw->Line( center_x + right.x, center_y + right.y, center_x + x2, center_y + y2, Clr, false );

	//inner
	Draw->Line( center_x + left.x, center_y + left.y, center_x + ( point_on_line.x + ( x2 - point_on_line.x ) * 0.45f ),
				  center_y + ( point_on_line.y + ( y2 - point_on_line.y ) * 0.45f ), Clr, false );

	Draw->Line( center_x + right.x, center_y + right.y, center_x + ( point_on_line.x + ( x2 - point_on_line.x ) * 0.45f ),
				  center_y + ( point_on_line.y + ( y2 - point_on_line.y ) * 0.45f ), Clr, false );

	/*Draw->Line( center_x + x2, center_y + y2, center_x + ( point_on_line.x + ( x2 - point_on_line.x ) * 0.41f ),
				  center_y + ( point_on_line.y + ( y2 - point_on_line.y ) * 0.41f ), Clr, false );*/

	//bottom triangle
	/*Vertex_t triangle[ 3 ];
	triangle[ 0 ].Init( { center_x + left.x, center_y + left.y } );
	triangle[ 1 ].Init( { center_x + right.x, center_y + right.y } );
	triangle[ 2 ].Init( { center_x + ( point_on_line.x + ( x2 - point_on_line.x ) * 0.41f ), center_y + ( point_on_line.y + ( y2 - point_on_line.y ) * 0.41f ) } );
	Draw->TexturedPolygon( 3, triangle, { Clr.r, Clr.g, Clr.b, 60 } );*/

	//left triangle
	Vertex_t left_triangle[ 3 ];
	left_triangle[ 0 ].Init( { center_x + x2, center_y + y2 } );
	left_triangle[ 1 ].Init( { center_x + left.x, center_y + left.y } );
	left_triangle[ 2 ].Init( { center_x + ( point_on_line.x + ( x2 - point_on_line.x ) * 0.45f ), center_y + ( point_on_line.y + ( y2 - point_on_line.y ) * 0.45f ) } );
	Draw->TexturedPolygon( 3, left_triangle, { Clr.r, Clr.g, Clr.b, 60 } );

	//right triangle
	Vertex_t right_triangle[ 3 ];
	right_triangle[ 0 ].Init( { center_x + x2, center_y + y2 } );
	right_triangle[ 1 ].Init( { center_x + right.x, center_y + right.y } );
	right_triangle[ 2 ].Init( { center_x + ( point_on_line.x + ( x2 - point_on_line.x ) * 0.45f ), center_y + ( point_on_line.y + ( y2 - point_on_line.y ) * 0.45f ) } );
	Draw->TexturedPolygon( 3, right_triangle, { Clr.r, Clr.g, Clr.b, 60 } );
}