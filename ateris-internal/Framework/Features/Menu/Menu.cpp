#include "Menu.h"
#include "../Visuals/Visuals.h"
#include "../Misc/Misc.h"
#include "Textures.h"

void CMenu::Run( )
{
	IsTyping = false;

	if ( Input->IsPressed( VK_INSERT ) && !Input->IsHeld( VK_INSERT ) && Input->InFocus( ) )
	{
		gSurface->SetCursorAlwaysVisible( IsOpen = !IsOpen );
	}

	Accent( );
	if ( !IsOpen )
	{
		return;
	}

	WindowSize.h = 429;
	if ( CurrentMenuTab == TAB_AIM )
	{
		WindowSize.h += 205;
	}

	if ( CurrentMenuTab == TAB_VISUALS )
	{
		WindowSize.h += 130;
		if ( CurrentVisualsSubtab == SUBTAB_MODELS )
		{
			if ( Config->Models->Players->TwoModels )
			{
				WindowSize.h += 77;
			}

			if ( Config->Models->World->TwoModels )
			{
				WindowSize.h += 58;
			}
		}
	}

	if ( CurrentMenuTab == TAB_MISC )
	{
		WindowSize.h += 174;
	}

	if ( CurrentMenuTab == TAB_HVH )
	{
		WindowSize.h += 20;
		if ( Config->Misc->AntiAim->DrawFake && Config->Misc->AntiAim->FakeMode == 3 && Config->Misc->AntiAim->RealMode == 3 )
		{
			WindowSize.h += 20;
			if ( Config->Misc->AntiAim->FakeModel )
			{
				WindowSize.h += 24;
			}
		}
	}

	if ( CurrentMenuTab == TAB_PLAYERLIST )
	{
		WindowSize.h += 5;
		if ( LowestGroupPos > WindowSize.y + WindowSize.h - 15 )
		{
			if ( gPlayers.size( ) > 21 )
			{
				int Addition = gPlayers.size( ) - 21;
				WindowSize.h += ( Addition * 20 );
			}
		}
	}

	if ( !SliderFloatInfo->Var && !SliderIntInfo->Var )
	{
		Input->Drag( WindowSize.x, WindowSize.y, WindowSize.w, 22, 0 );
	}

	RenderWindow( "Ateris.Technologies", WindowSize, Config->Colors->MenuBackground, Config->Colors->MenuBorders, { 0, 100, 200, 255 } );

	LastTabPos = { WindowSize.x + 1, WindowSize.y + 22, 88, 25 };
	//LastSubtabPos = { WindowSize.x, WindowSize.y + 15 + 20, ( int )( WindowSize.w * 0.25f ), 15 };
	LastGroupPos = { WindowSize.x + 93, WindowSize.y + 26, 203, 0, };


	if ( MenuTab( "Aimbot", CurrentMenuTab == TAB_AIM ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
		CurrentMenuTab = TAB_AIM;

	if ( MenuTab( "Visuals", CurrentMenuTab == TAB_VISUALS ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
		CurrentMenuTab = TAB_VISUALS;

	if ( CurrentMenuTab == TAB_VISUALS )
	{
		Draw->Line( WindowSize.x + 90, WindowSize.y + 20 + 15 + 3, WindowSize.x + WindowSize.w, WindowSize.y + 20 + 15 + 3, Config->Colors->MenuBorders, false );
		//Draw->FadeRectangle( WindowSize.x + 90, WindowSize.y + 20 + 15 + 3, WindowSize.w - 90, 1, Config->Colors->m_ui_accent, { 0, 100, 200, 255 }, true );
		LastSubtabPos = { WindowSize.x + 90 + 2, WindowSize.y + 20 + 2, 81, 15 };

		if ( MenuSubtab( "ESP", CurrentVisualsSubtab == SUBTAB_ESP ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
			CurrentVisualsSubtab = SUBTAB_ESP;

		if ( MenuSubtab( "Models", CurrentVisualsSubtab == SUBTAB_MODELS ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
			CurrentVisualsSubtab = SUBTAB_MODELS;

		if ( MenuSubtab( "Model Colors", CurrentVisualsSubtab == SUBTAB_MODELS2 ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
			CurrentVisualsSubtab = SUBTAB_MODELS2;

		if ( MenuSubtab( "Outlines", CurrentVisualsSubtab == SUBTAB_OUTLINES ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
			CurrentVisualsSubtab = SUBTAB_OUTLINES;

		if ( MenuSubtab( "Misc", CurrentVisualsSubtab == SUBTAB_MISC_ ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
			CurrentVisualsSubtab = SUBTAB_MISC_;
	}

	if ( MenuTab( "Misc", CurrentMenuTab == TAB_MISC ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
		CurrentMenuTab = TAB_MISC;

	if ( MenuTab( "HvH", CurrentMenuTab == TAB_HVH ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
		CurrentMenuTab = TAB_HVH;

	if ( MenuTab( "Auto", CurrentMenuTab == TAB_AUTO ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
		CurrentMenuTab = TAB_AUTO;

	if ( MenuTab( "Player List", CurrentMenuTab == TAB_PLAYERLIST ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
		CurrentMenuTab = TAB_PLAYERLIST;

	if ( MenuTab( "Skin Changer", CurrentMenuTab == TAB_SKINCHANGER ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
		CurrentMenuTab = TAB_SKINCHANGER;

	if ( G->Authentication->IsDebugAllowed )
	{
		if ( MenuTab( "Debug", CurrentMenuTab == TAB_DEBUG ) && !IsMultiComboboxOpen && !IsComboboxOpen && !IsColorPickerOpen )
			CurrentMenuTab = TAB_DEBUG;
	}

	if ( !ConfigList.empty( ) )
	{
		ConfigList.clear( );
	}

	for ( const auto& entry : std::filesystem::directory_iterator( ConfigPath ) )
	{
		if ( std::string( std::filesystem::path( entry ).filename( ).string( ) ).find( ( ".json" ) ) == std::string_view::npos )
			continue;

		std::string s = entry.path( ).filename( ).string( );
		s.erase( s.end( ) - 5, s.end( ) );

		ConfigList.push_back( s );
	}

	std::vector<const char*> list = { };
	for ( const auto& config : ConfigList )
	{
		list.push_back( config.c_str( ) );
	}

	/*if ( !DeviceList.empty( ) )
	{
		DeviceList.clear( );
	}

	for ( size_t i = 0; i < waveOutGetNumDevs( ); i++ )
	{
		tagWAVEOUTCAPSA info;
		waveOutGetDevCapsA( i, &info, sizeof( struct tagWAVEOUTCAPSA ) );

		std::string DeviceName = info.szPname;
		gCvar->ConsoleColorPrintf( { 0, 240, 140, 255 }, "Found audio device [%i] - %s\n", i, DeviceName.c_str( ) );

		DeviceList.push_back( DeviceName );
	}

	std::vector<const char*> device_list = { };
	for ( const auto& device : DeviceList )
	{
		device_list.push_back( device.c_str( ) );
	}*/

	//LastTabPos = { WindowSize.x + 1, WindowSize.y + 21, 88, 25 };
	PushGroupX( WindowSize.x - 3 );
	PushGroupY( WindowSize.y + WindowSize.h - 76 );

	if ( TextBox( "", output, 87 ) )
	{
		if ( !std::filesystem::exists( ConfigPath + "\\" + output ) )
		{
			ConfigList.push_back( output.c_str( ) );
			SaveConfig( output.c_str( ) );
		}
		output = { };
	}

	MenuSpace( -4 );
	ComboBox( "", ConfigSelected, list, 87 );

	static bool SaveProcess = false;
	if ( !SaveProcess )
	{
		PushGroupX( WindowSize.x - 3 );
		MenuSpace( -4 );
		Button( "Save", [ & ] ( )
		{
			SaveProcess = true;
		}, 87 );
	}
	else
	{
		PushGroupX( WindowSize.x - 3 );
		MenuSpace( -4 );
		Button( "Sure?", [ & ] ( )
		{
			if ( list[ ConfigSelected ] )
			{
				SaveConfig( list[ ConfigSelected ] );
			}
			SaveProcess = false;
		}, 87 );

		if ( Input->IsPressed( VK_ESCAPE ) )
		{
			SaveProcess = false;
		}
	}

	PushGroupX( WindowSize.x - 3 );
	MenuSpace( -4 );

	static bool LoadProcess = false;
	if ( !LoadProcess )
	{
		Button( "Load", [ & ] ( )
		{
			if ( list[ ConfigSelected ] )
			{
				LoadProcess = true;
			}
		}, 87 );
	}
	else
	{
		Button( "Sure?", [ & ] ( )
		{
			if ( list[ ConfigSelected ] )
			{
				LoadConfig( list[ ConfigSelected ] );
			}
			LoadProcess = false;
		}, 87 );

		if ( Input->IsPressed( VK_ESCAPE ) )
		{
			LoadProcess = false;
		}
	}

	static bool DeleteProcess = false;
	if ( !DeleteProcess )
	{
		PushGroupX( WindowSize.x - 3 );
		MenuSpace( -4 );
		Button( "Delete", [ & ] ( )
		{
			DeleteProcess = true;
		}, 87 );
	}
	else
	{
		PushGroupX( WindowSize.x - 3 );
		MenuSpace( -4 );
		Button( "Sure?", [ & ] ( )
		{
			if ( list[ ConfigSelected ] )
			{
				if ( std::filesystem::exists( ConfigPath + "\\" + list[ ConfigSelected ] + ".json" ) )
					std::filesystem::remove( ConfigPath + "\\" + list[ ConfigSelected ] + ".json" );
			}
			DeleteProcess = false;
		}, 87 );

		if ( Input->IsPressed( VK_ESCAPE ) )
		{
			DeleteProcess = false;
		}
	}

	switch ( CurrentMenuTab )
	{
		case TAB_AIM:
		{
			PopGroupX( false );
			PopGroupY( true );
			MenuGroupStart( );
			{
				CheckBoxKey( "Enabled", Config->Aimbot->Global->Enabled, Config->Aimbot->Global->AimKey );
				CheckBox( "Auto Secondary Switch", Config->Aimbot->Global->AutoSecondarySwitch );
				SliderInt( "FoV", Config->Aimbot->Global->FOV, 0, 180, 193 );
				MultiComboBox( "Targets", {
					{"Players", &Config->Aimbot->Targetting->TargetPlayers},
					{"Buildings", &Config->Aimbot->Targetting->TargetBuildings},
					{"Stickies",&Config->Aimbot->Targetting->TargetStickies},
					{"Bonked", &Config->Aimbot->Targetting->TargetBonk},
					{"Ubered", &Config->Aimbot->Targetting->TargetUber},
					{"Vaccinator'd", &Config->Aimbot->Targetting->TargetVacc},
					{"Friends", &Config->Aimbot->Targetting->TargetFriends},
					{"Lethal Bombs", &Config->Aimbot->Targetting->TargetLethalBombs}
							   } );
			}
			MenuGroup( "General" );

			MenuGroupStart( );
			{
				CheckBox( "Tap Fire", Config->Aimbot->Hitscan->TapFire );
				CheckBox( "Wait for Charge", Config->Aimbot->Hitscan->WaitForCharge );
				CheckBox( "Wait for Headshot", Config->Aimbot->Hitscan->WaitForHeadshot );
				CheckBox( "Prevent Bait", Config->Aimbot->Hitscan->PreventBait );
				ComboBox( "Preference", Config->Aimbot->Hitscan->Preference, { "Head", "Body", "Auto" } );
				MultiComboBox( "Hitboxes", { { "Head", &Config->Aimbot->Hitscan->TargetHead },
								{ "Chest", &Config->Aimbot->Hitscan->TargetChest },
								{ "Upper Arms", &Config->Aimbot->Hitscan->TargetArms },
								{ "Stomach", &Config->Aimbot->Hitscan->TargetStomach },
								{ "Pelvis", &Config->Aimbot->Hitscan->TargetPelvis } } );
				SliderInt( "Point Scale (Heavy)", Config->Aimbot->Hitscan->HeavyPointScale, 10, 80, 193, 1, "%i%%" );
				SliderInt( "Point Scale", Config->Aimbot->Hitscan->PointScale, 10, 100, 193, 1, "%i%%" );
				CheckBox( "Legitbob", Config->Aimbot->Hitscan->LegitBob->Enabled );
				SliderFloat( "FOV", Config->Aimbot->Hitscan->LegitBob->FOV, 0.f, 1.2f, 193, 0.1f, "%.1fdeg" );
				SliderFloat( "Dead Zone", Config->Aimbot->Hitscan->LegitBob->DeadZone, 0.f, std::max( Config->Aimbot->Hitscan->LegitBob->FOV - 1.f, 0.1f ), 193, 0.5f, "%.1fdeg" );
				SliderFloat( "Strength", Config->Aimbot->Hitscan->LegitBob->Strenght, 0.f, 100.f, 193, 1.f, "%.1f%%" );
				CheckBox( "Draw Hitboxes", Config->Visuals->HitscanHitboxes );
				CheckBoxColor( "Custom Color", Config->Visuals->HitboxesCustomColor, Config->Colors->HitboxColor );
				SliderFloat( "Hitboxes Alpha", Config->Visuals->HitboxesAlpha, 0.0f, 1.0f, 193 );
				SliderFloat( "Draw Time", Config->Visuals->HitboxesTime, 0.1f, 10.0f, 193 );
			}
			MenuGroup( "Hitscan" );

			MenuGroupStart( );
			{
				CheckBox( "Lag Comp", Config->Aimbot->Hitscan->LagCompMethod );
				CheckBox( "Draw LagComp", Config->Visuals->DrawBackTrack );
				CheckBox( "Only Last Tick", Config->Visuals->LastTickOnly );
				MultiComboBox( "Draw Types", {
				{ "Model", &Config->Visuals->BackTrackModel },
				{ "Outline", &Config->Visuals->BackTrackOutline } } );
				if ( Config->Visuals->BackTrackModel )
				{
					SliderFloat( "BT Alpha", Config->Visuals->BackTrackAlpha, 0.0f, 1.0f, 193 );
				}
				CheckBoxKey( "Fake Latency", Config->Aimbot->Hitscan->FakeLatency, Config->Aimbot->Hitscan->FakeLatencyKey );
				CheckBox( "Dynamic Latency", Config->Aimbot->Hitscan->DynamicLatency );
				if ( !Config->Aimbot->Hitscan->DynamicLatency )
				{
					SliderInt( "Latency Amount", Config->Aimbot->Hitscan->FakeLatencyAmount, 100, 800, 193, 10 );
				}
				CheckBox( "Ping Reducer", Config->Aimbot->Hitscan->ReduceScoreboardPing );
				SliderInt( "Desired Ping", Config->Aimbot->Hitscan->ScoreboardPingGoal, 5, 100, 193, 5 );
			}
			MenuGroup( "Lag Compensation" );

			PopGroupX( true );
			PopGroupY( true );

			MenuGroupStart( );
			{
				MultiComboBox( "Flags", { {"Predict Strafing", &Config->Aimbot->Projectile->PredictStrafing },
							   { "Predict Jumping", &Config->Aimbot->Projectile->PredictJumping },
							   { "Wait For Kill (Sniper)", &Config->Aimbot->Projectile->WaitForHeadshot },
							   { "Delay Shot", &Config->Aimbot->Projectile->DelayShot } } );
				SliderFloat( "Max Projectile Travel", Config->Aimbot->Projectile->MaxProjectileTime, 2.f, 10.f, 193 );
				SliderFloat( "Max Player Travel", Config->Aimbot->Projectile->MaxTime, 2.f, 10.f, 193 );
				SliderInt( "Hit Chance", Config->Aimbot->Projectile->HitChance, 0, 100, 193 );
				SliderInt( "Max Sticky Charge", Config->Aimbot->Projectile->MaxStickyCharge, 0, 100, 193 );
				CheckBox( "Draw BBox", Config->Visuals->ProjectileBoundingBox );
				CheckBoxColor( "Custom Color", Config->Visuals->BoundingBoxCustomColor, Config->Colors->BoundingBoxColor );
				SliderFloat( "Alpha", Config->Visuals->BoundingBoxAlpha, 0.0f, 1.0f, 193 );
				SliderFloat( "Time", Config->Visuals->BoundingBoxTime, 0.1f, 10.0f, 193 );
				CheckBox( "Splash Bot", Config->Aimbot->Projectile->SplashBot );
				SliderInt( "Splash Radius", Config->Aimbot->Projectile->SplashRadius, 25, 120, 193 );
				SliderInt( "Sphere Points", Config->Aimbot->Projectile->SplashPoints, 5, 50, 193 );
				KeyBind( "Change Airstrike Position", Config->Aimbot->Projectile->ChangePositionKey );
				KeyBind( "Reset Airstrike Position", Config->Aimbot->Projectile->ResetPositionKey );
				KeyBind( "Gaza Bombing", Config->Aimbot->Projectile->GazaBombingKey );
			}
			MenuGroup( "Projectile" );

			MenuGroupStart( );
			{
				static int Mode = 0;
				ComboBox( "Mode", Mode, { "Classes", "Special" } );
				if ( Mode == 0 )
				{
					SliderInt( "Scout Priority", Config->Aimbot->Priorities->Scout, 0, 10, 94 );
					PushGroupX( LastGroupPos.x + 94 + 5 );
					PushGroupY( LastGroupPos.y - 24 );
					SliderInt( "Soldier Priority", Config->Aimbot->Priorities->Soldier, 0, 10, 94 );
					PushGroupX( LastGroupPos.x - 94 - 5 );
					SliderInt( "Pyro Priority", Config->Aimbot->Priorities->Pyro, 0, 10, 94 );
					PushGroupX( LastGroupPos.x + 94 + 5 );
					PushGroupY( LastGroupPos.y - 24 );
					SliderInt( "Demoman Priority", Config->Aimbot->Priorities->Demoman, 0, 10, 94 );
					PushGroupX( LastGroupPos.x - 94 - 5 );
					SliderInt( "Heavy Priority", Config->Aimbot->Priorities->Heavy, 0, 10, 94 );
					PushGroupX( LastGroupPos.x + 94 + 5 );
					PushGroupY( LastGroupPos.y - 24 );
					SliderInt( "Engineer Priority", Config->Aimbot->Priorities->Engineer, 0, 10, 94 );
					PushGroupX( LastGroupPos.x - 94 - 5 );
					SliderInt( "Medic Priority", Config->Aimbot->Priorities->Medic, 0, 10, 94 );
					PushGroupX( LastGroupPos.x + 94 + 5 );
					PushGroupY( LastGroupPos.y - 24 );
					SliderInt( "Sniper Priority", Config->Aimbot->Priorities->Sniper, 0, 10, 94 );
					PushGroupX( LastGroupPos.x - 94 - 5 );
					SliderInt( "Spy Priority", Config->Aimbot->Priorities->Spy, 0, 10, 193 );
				}
				else
				{
					SliderInt( "Buildings Priority", Config->Aimbot->Priorities->OtherBuildings, 0, 10, 94 );
					PushGroupX( LastGroupPos.x + 94 + 5 );
					PushGroupY( LastGroupPos.y - 24 );
					SliderInt( "Cheater Priority", Config->Aimbot->Priorities->Cheaters, 0, 10, 94 );
					PushGroupX( LastGroupPos.x - 94 - 5 );
					SliderInt( "Friend Priority", Config->Aimbot->Priorities->Friends, 0, 10, 94 );
					PushGroupX( LastGroupPos.x + 94 + 5 );
					PushGroupY( LastGroupPos.y - 24 );
					SliderInt( "Cloaked Priority", Config->Aimbot->Priorities->Cloaked, 0, 10, 94 );
					PushGroupX( LastGroupPos.x - 94 - 5 );
					SliderInt( "Zoomed Priority", Config->Aimbot->Priorities->Scoped, 0, 10, 94 );
					PushGroupX( LastGroupPos.x + 94 + 5 );
					PushGroupY( LastGroupPos.y - 24 );
					SliderInt( "Revved Priority", Config->Aimbot->Priorities->Revved, 0, 10, 94 );
					PushGroupX( LastGroupPos.x - 94 - 5 );
					SliderInt( "Sentry Priority", Config->Aimbot->Priorities->Sentry, 0, 10, 94 );
					PushGroupX( LastGroupPos.x + 94 + 5 );
					PushGroupY( LastGroupPos.y - 24 );
					SliderInt( "Stickies Priority", Config->Aimbot->Priorities->Stickies, 0, 10, 94 );
					PushGroupX( LastGroupPos.x - 94 - 5 );
					SliderInt( "Other Priority", Config->Aimbot->Priorities->Other, 0, 10, 193 );
				}

			}
			MenuGroup( "Priorities" );

			MenuGroupStart( );
			{
				CheckBox( "Enabled", Config->Aimbot->Melee->Enabled );
				CheckBox( "Require Aim Key", Config->Aimbot->Melee->RequireAimKey );
				CheckBox( "Predict Warp", Config->Aimbot->Melee->WarpPrediction );
			}
			MenuGroup( "Melee" );

			break;
		}
		case TAB_VISUALS:
		{
			switch ( CurrentVisualsSubtab )
			{
				case SUBTAB_ESP:
				{
					PopGroupX( false );
					PushGroupY( WindowSize.y + 24 + 18 );
					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->ESP->Global->Enabled );
						ComboBox( "Color Scheme", Config->ESP->Global->ColorScheme, { "Enemy/Team", "Red/Blu" } ); //0 - Enemy/Team, 1 - Red/Blu
					}
					MenuGroup( "Global" );

					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->ESP->Buildings->Enabled );
						if ( Config->ESP->Buildings->Enabled )
						{
							MultiComboBox( "Entities", { { "Enemies", &Config->ESP->Buildings->Enemy },
										   { "Teammates", &Config->ESP->Buildings->Team },
										   { "Friends", &Config->ESP->Buildings->Friends },
										   { "Local", &Config->ESP->Buildings->Local } } );

							MultiComboBox( "Render", { { "Name", &Config->ESP->Buildings->Name },
										   { "Box", &Config->ESP->Buildings->EnabledBox },
										   { "Flags", &Config->ESP->Buildings->Flags },
										   { "Health Bar", &Config->ESP->Buildings->HealthBar } } );

							if ( Config->ESP->Buildings->EnabledBox )
							{
								ComboBox( "Box Style", Config->ESP->Buildings->Box, { "2D", "3D" } );
							}

							if ( Config->ESP->Buildings->HealthBar )
							{
								MultiComboBox( "Health Bar Flags", { { "Lines", &Config->ESP->Buildings->PHealthBar->Lines },
											   { "Animated", &Config->ESP->Buildings->PHealthBar->Animated },
											   { "Custom Color", &Config->ESP->Buildings->PHealthBar->CustomColor } } );

								if ( Config->ESP->Buildings->PHealthBar->Lines )
								{
									SliderInt( "Health Bar Lines", Config->ESP->Buildings->PHealthBar->LinesCount, 1, 9, 193 );
								}
							}

							if ( Config->ESP->Global->ColorScheme )
							{
								ColorPicker( "Red Buildings", Config->Colors->ESPBuildingRed );
								ColorPicker( "Blu Buildings", Config->Colors->ESPBuildingBlu );
							}
							else
							{
								ColorPicker( "Enemy Buildings", Config->Colors->ESPBuildingEnemy );
								ColorPicker( "Team Buildings", Config->Colors->ESPBuildingTeam );
							}
							ColorPicker( "Friend Buildings", Config->Colors->ESPBuildingFriend );
							ColorPicker( "Local Building", Config->Colors->ESPBuildingLocal );
						}
					}
					MenuGroup( "Buildings" );

					PopGroupX( true );
					PushGroupY( WindowSize.y + 24 + 18 );
					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->ESP->Players->Enabled );
						if ( Config->ESP->Players->Enabled )
						{
							MultiComboBox( "Entities", { { "Enemies", &Config->ESP->Players->Enemy },
										   { "Teammates", &Config->ESP->Players->Team },
										   { "Friends", &Config->ESP->Players->Friends },
										   { "Local", &Config->ESP->Players->Local } } );

							MultiComboBox( "Render", { { "Name", &Config->ESP->Players->Name },
										   { "Box", &Config->ESP->Players->EnabledBox },
										   { "Flags", &Config->ESP->Players->Flags },
										   { "Weapon", &Config->ESP->Players->Weapon },
										   { "Health Bar", &Config->ESP->Players->HealthBar },
										   { "Uber Bar", &Config->ESP->Players->UberBar },
										   { "Out of Bounds", &Config->Visuals->PlayerArrows },
										   { "Avatar", &Config->ESP->Players->Avatar } } );

							if ( Config->ESP->Players->EnabledBox )
							{
								ComboBox( "Box Style", Config->ESP->Players->Box, { "2D", "3D" } );
							}

							if ( Config->ESP->Players->HealthBar )
							{
								MultiComboBox( "Health Bar Flags", { { "Lines", &Config->ESP->Players->PHealthBar->Lines },
											   { "Animated", &Config->ESP->Players->PHealthBar->Animated },
											   { "Custom Color", &Config->ESP->Players->PHealthBar->CustomColor } } );

								if ( Config->ESP->Players->PHealthBar->Lines )
								{
									SliderInt( "Health Bar Lines", Config->ESP->Players->PHealthBar->LinesCount, 1, 9, 193 );
								}
							}

							if ( Config->Visuals->PlayerArrows )
							{
								SliderFloat( "Screen Distance", Config->Visuals->ArrowsScreenDistance, 0.f, 0.25f, 193 );
								ComboBox( "Screen Shape", Config->Visuals->ArrowsScreenShape, { "Circle", "Ellipsis" } );
							}

							if ( Config->ESP->Global->ColorScheme )
							{
								ColorPicker( "Red Players", Config->Colors->ESPPlayerRed );
								ColorPicker( "Blu Players", Config->Colors->ESPPlayerBlu );
							}
							else
							{
								ColorPicker( "Enemies", Config->Colors->ESPPlayerEnemy );
								ColorPicker( "Teammates", Config->Colors->ESPPlayerTeam );
							}
							ColorPicker( "Friends", Config->Colors->ESPPlayerFriend );
							ColorPicker( "Local", Config->Colors->ESPPlayerLocal );
						}
					}
					MenuGroup( "Players" );

					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->ESP->World->Enabled );
						if ( Config->ESP->World->Enabled )
						{
							MultiComboBox( "Entities", { { "Healthpacks", &Config->ESP->World->Healthpacks },
										   { "Ammopacks", &Config->ESP->World->Ammopacks },
										   { "Intel", &Config->ESP->World->Intels },
										   { "Bombs", &Config->ESP->World->Bombs },
										   { "Projectiles", &Config->ESP->World->Projectiles },
										   { "MvM Money", &Config->ESP->World->MvMCash },
										   { "MvM Tanks", &Config->ESP->World->MvMTanks },
										   { "MvM Sentry Busters", &Config->ESP->World->MvMSentryBusters },
										   { "Powerups", &Config->ESP->World->MPPowerups },
										   { "Enemies", &Config->ESP->World->Enemy },
										   { "Teammates", &Config->ESP->World->Team },
										   { "Friends", &Config->ESP->World->Friends },
										   { "Local", &Config->ESP->World->Local } } );

							MultiComboBox( "Render", { { "Name", &Config->ESP->World->Name },
										   { "Box", &Config->ESP->World->EnabledBox },
										   { "Distance", &Config->ESP->World->Distance } } );

							if ( Config->ESP->World->EnabledBox )
							{
								ComboBox( "Box Style", Config->ESP->World->Box, { "2D", "3D" } );
							}

							ColorPicker( "Healthpacks", Config->Colors->ESPWorldHealth );
							ColorPicker( "Ammopacks", Config->Colors->ESPWorldAmmo );
							ColorPicker( "Bombs", Config->Colors->ESPWorldBomb );
							ColorPicker( "MvM Money", Config->Colors->ESPWorldMoney );
							ColorPicker( "Powerups", Config->Colors->ESPWorldPowerup );
							if ( Config->ESP->Global->ColorScheme )
							{
								ColorPicker( "Red Projectiles", Config->Colors->ESPWorldRed );
								ColorPicker( "Blu Projectiles", Config->Colors->ESPWorldBlu );
							}
							else
							{
								ColorPicker( "Enemy Projectiles", Config->Colors->ESPWorldEnemy );
								ColorPicker( "Team Projectiles", Config->Colors->ESPWorldTeam );
							}
							ColorPicker( "Friend Projectiles", Config->Colors->ESPWorldFriend );
							ColorPicker( "Local Projectiles", Config->Colors->ESPWorldLocal );
						}
					}
					MenuGroup( "World" );
					break;
				}

				case SUBTAB_MODELS:
				{
					PopGroupX( false );
					PushGroupY( WindowSize.y + 24 + 18 );
					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->Models->Global->Enabled );
						ComboBox( "Render Order", Config->Models->Global->RenderingOrder, { "Models First", "Outlines First" } ); //0 - Enemy/Team, 1 - Red/Blu

					}
					MenuGroup( "Global" );

					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->Models->Buildings->Enabled );
						if ( Config->Models->Buildings->Enabled )
						{
							MultiComboBox( "Entities", { { "Enemies", &Config->Models->Buildings->Enemy },
										   { "Teammates", &Config->Models->Buildings->Team },
										   { "Friends", &Config->Models->Buildings->Friends },
										   { "Local", &Config->Models->Buildings->Local } } );

							CheckBox( "Two Models", Config->Models->Buildings->TwoModels );

							if ( Config->Models->Buildings->TwoModels )
							{
								ComboBox( "Vis Material", Config->Models->Buildings->Unoccluded->Model, { "None", "Flat", "Shaded", "Fresnel" } );
								SliderFloat( "Vis Alpha", Config->Models->Buildings->Unoccluded->Alpha, 0.0f, 1.0f, 193 );

								CheckBox( "Vis Overlay", Config->Models->Buildings->Unoccluded->Overlay );
								if ( Config->Models->Buildings->Unoccluded->Overlay )
								{
									ComboBox( "Overlay Mat", Config->Models->Buildings->Unoccluded->OverlayModel, { "Contour", "Sheen", "Glow", "Flat" } );
									SliderFloat( "Vis Alpha", Config->Models->Buildings->Unoccluded->OverlayAlpha, 0.0f, 1.0f, 193 );
									CheckBox( "Wireframe", Config->Models->Buildings->Unoccluded->OverlayWireframe );
								}
								MenuSpace( 10 );
								ComboBox( "Invis Material", Config->Models->Buildings->Occluded->Model, { "None", "Flat", "Shaded", "Fresnel" } );
								SliderFloat( "Invis Alpha", Config->Models->Buildings->Occluded->Alpha, 0.0f, 1.0f, 193 );

								CheckBox( "Invis Overlay", Config->Models->Buildings->Occluded->Overlay );
								if ( Config->Models->Buildings->Occluded->Overlay )
								{
									ComboBox( "Overlay Mat", Config->Models->Buildings->Occluded->OverlayModel, { "Contour", "Sheen", "Glow", "Flat" } );
									SliderFloat( "Invis Alpha", Config->Models->Buildings->Occluded->OverlayAlpha, 0.0f, 1.0f, 193 );
									CheckBox( "Wireframe", Config->Models->Buildings->Occluded->OverlayWireframe );
								}
							}
							else
							{
								MultiComboBox( "Flags", { { "IgnoreZ", &Config->Models->Buildings->IgnoreZ },
											   { "Overlay", &Config->Models->Buildings->Overlay } } );

								ComboBox( "Material", Config->Models->Buildings->Model, { "None", "Flat", "Shaded", "Fresnel" } );
								SliderFloat( "Alpha", Config->Models->Buildings->Alpha, 0.0f, 1.0f, 193 );

								if ( Config->Models->Buildings->Overlay )
								{
									ComboBox( "Overlay Mat", Config->Models->Buildings->OverlayModel, { "Contour", "Sheen", "Glow", "Flat" } );
									SliderFloat( "Alpha", Config->Models->Buildings->OverlayAlpha, 0.0f, 1.0f, 193 );
									CheckBox( "Wireframe", Config->Models->Buildings->OverlayWireframe );
								}
							}
						}
					}
					MenuGroup( "Buildings" );

					PopGroupX( true );
					PushGroupY( WindowSize.y + 24 + 18 );
					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->Models->Players->Enabled );
						if ( Config->Models->Players->Enabled )
						{
							MultiComboBox( "Entities", { { "Enemies", &Config->Models->Players->Enemy },
										   { "Teammates", &Config->Models->Players->Team },
										   { "Friends", &Config->Models->Players->Friends },
										   { "Local", &Config->Models->Players->Local } } );

							CheckBox( "Two Models", Config->Models->Players->TwoModels );

							if ( Config->Models->Players->TwoModels )
							{
								ComboBox( "Vis Material", Config->Models->Players->Unoccluded->Model, { "None", "Flat", "Shaded", "Fresnel" } );
								SliderFloat( "Vis Alpha", Config->Models->Players->Unoccluded->Alpha, 0.0f, 1.0f, 193 );

								CheckBox( "Vis Overlay", Config->Models->Players->Unoccluded->Overlay );
								if ( Config->Models->Players->Unoccluded->Overlay )
								{
									ComboBox( "Overlay Type", Config->Models->Players->Unoccluded->OverlayType, { "Custom", "High KD Only" } );
									if ( !Config->Models->Players->Unoccluded->OverlayType )
									{
										ComboBox( "Overlay Mat", Config->Models->Players->Unoccluded->OverlayModel, { "Contour", "Sheen", "Glow", "Flat" } );
										SliderFloat( "Vis Alpha", Config->Models->Players->Unoccluded->OverlayAlpha, 0.0f, 1.0f, 193 );
										CheckBox( "Wireframe", Config->Models->Players->Unoccluded->OverlayWireframe );
									}
								}
								MenuSpace( 10 );
								ComboBox( "Invis Material", Config->Models->Players->Occluded->Model, { "None", "Flat", "Shaded", "Fresnel" } );
								SliderFloat( "Invis Alpha", Config->Models->Players->Occluded->Alpha, 0.0f, 1.0f, 193 );

								CheckBox( "Invis Overlay", Config->Models->Players->Occluded->Overlay );
								if ( Config->Models->Players->Occluded->Overlay )
								{
									ComboBox( "Overlay Type", Config->Models->Players->Occluded->OverlayType, { "Custom", "High KD Only" } );
									if ( !Config->Models->Players->Occluded->OverlayType )
									{
										ComboBox( "Overlay Mat", Config->Models->Players->Occluded->OverlayModel, { "Contour", "Sheen", "Glow", "Flat" } );
										SliderFloat( "Invis Alpha", Config->Models->Players->Occluded->OverlayAlpha, 0.0f, 1.0f, 193 );
										CheckBox( "Wireframe", Config->Models->Players->Occluded->OverlayWireframe );
									}
								}
							}
							else
							{
								MultiComboBox( "Flags", { { "IgnoreZ", &Config->Models->Players->IgnoreZ },
											   { "Overlay", &Config->Models->Players->Overlay } } );

								ComboBox( "Material", Config->Models->Players->Model, { "None", "Flat", "Shaded", "Fresnel" } );
								SliderFloat( "Alpha", Config->Models->Players->Alpha, 0.0f, 1.0f, 193 );

								if ( Config->Models->Players->Overlay )
								{
									ComboBox( "Overlay Type", Config->Models->Players->OverlayType, { "Custom", "High KD Only" } );
									if ( !Config->Models->Players->OverlayType )
									{
										ComboBox( "Overlay Mat", Config->Models->Players->OverlayModel, { "Contour", "Sheen", "Glow", "Flat" } );
										SliderFloat( "Alpha", Config->Models->Players->OverlayAlpha, 0.0f, 1.0f, 193 );
										CheckBox( "Wireframe", Config->Models->Players->OverlayWireframe );
									}
								}
							}
						}
					}
					MenuGroup( "Players" );

					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->Models->World->Enabled );
						if ( Config->Models->World->Enabled )
						{
							MultiComboBox( "Entities", { { "Healthpacks", &Config->Models->World->Healthpacks },
										   { "Ammopacks", &Config->Models->World->Ammopacks },
										   { "Intel", &Config->Models->World->Intels },
										   { "Bombs", &Config->Models->World->Bombs },
										   { "Projectiles", &Config->Models->World->Projectiles },
										   { "MvM Money", &Config->Models->World->MvMCash },
										   { "MvM Tanks", &Config->Models->World->MvMTanks },
										   { "MvM Sentry Busters", &Config->Models->World->MvMSentryBusters },
										   { "Powerups", &Config->Models->World->MPPowerups },
										   { "Enemies", &Config->Models->World->Enemy },
										   { "Teammates", &Config->Models->World->Team },
										   { "Friends", &Config->Models->World->Friends },
										   { "Local", &Config->Models->World->Local } } );

							CheckBox( "Two Models", Config->Models->World->TwoModels );

							if ( Config->Models->World->TwoModels )
							{
								ComboBox( "Vis Material", Config->Models->World->Unoccluded->Model, { "None", "Flat", "Shaded", "Fresnel" } );
								SliderFloat( "Vis Alpha", Config->Models->World->Unoccluded->Alpha, 0.0f, 1.0f, 193 );

								CheckBox( "Vis Overlay", Config->Models->World->Unoccluded->Overlay );
								if ( Config->Models->World->Unoccluded->Overlay )
								{
									ComboBox( "Overlay Mat", Config->Models->World->Unoccluded->OverlayModel, { "Contour", "Sheen", "Glow", "Flat" } );
									SliderFloat( "Vis Alpha", Config->Models->World->Unoccluded->OverlayAlpha, 0.0f, 1.0f, 193 );
									CheckBox( "Wireframe", Config->Models->World->Unoccluded->OverlayWireframe );
								}
								MenuSpace( 10 );
								ComboBox( "Invis Material", Config->Models->World->Occluded->Model, { "None", "Flat", "Shaded", "Fresnel" } );
								SliderFloat( "Invis Alpha", Config->Models->World->Occluded->Alpha, 0.0f, 1.0f, 193 );

								CheckBox( "Invis Overlay", Config->Models->World->Occluded->Overlay );
								if ( Config->Models->World->Occluded->Overlay )
								{
									ComboBox( "Overlay Mat", Config->Models->World->Occluded->OverlayModel, { "Contour", "Sheen", "Glow", "Flat" } );
									SliderFloat( "Invis Alpha", Config->Models->World->Occluded->OverlayAlpha, 0.0f, 1.0f, 193 );
									CheckBox( "Wireframe", Config->Models->World->Occluded->OverlayWireframe );
								}
							}
							else
							{
								MultiComboBox( "Flags", { { "IgnoreZ", &Config->Models->World->IgnoreZ },
											   { "Overlay", &Config->Models->World->Overlay } } );

								ComboBox( "Material", Config->Models->World->Model, { "None", "Flat", "Shaded", "Fresnel" } );
								SliderFloat( "Alpha", Config->Models->World->Alpha, 0.0f, 1.0f, 193 );

								if ( Config->Models->World->Overlay )
								{
									ComboBox( "Overlay Mat", Config->Models->World->OverlayModel, { "Contour", "Sheen", "Glow", "Flat" } );
									SliderFloat( "Alpha", Config->Models->World->OverlayAlpha, 0.0f, 1.0f, 193 );
									CheckBox( "Wireframe", Config->Models->World->OverlayWireframe );
								}
							}
						}
					}
					MenuGroup( "World" );
					break;
				}

				case SUBTAB_MODELS2:
				{
					PopGroupX( false );
					PushGroupY( WindowSize.y + 24 + 18 );
					MenuGroupStart( );
					{
						ComboBox( "Color Scheme", Config->Models->Global->ColorScheme, { "Enemy/Team", "Red/Blu" } ); //0 - Enemy/Team, 1 - Red/Blu
					}
					MenuGroup( "Global" );

					MenuGroupStart( );
					{
						if ( Config->Models->Buildings->TwoModels )
						{
							if ( Config->Models->Global->ColorScheme )
							{
								ColorPicker( "Red Buildings Visible", Config->Colors->ModelsBuildingRedUnoccluded );
								ColorPicker( "Red Buildings Invisible", Config->Colors->ModelsBuildingRedOccluded );
								ColorPicker( "Blu Buildings Visible", Config->Colors->ModelsBuildingBluUnoccluded );
								ColorPicker( "Blu Buildings Invisible", Config->Colors->ModelsBuildingBluOccluded );
							}
							else
							{
								ColorPicker( "Enemies Visible", Config->Colors->ModelsBuildingEnemyUnoccluded );
								ColorPicker( "Enemies Invisible", Config->Colors->ModelsBuildingEnemyOccluded );
								ColorPicker( "Teammates Visible", Config->Colors->ModelsBuildingTeamUnoccluded );
								ColorPicker( "Teammates Invisible", Config->Colors->ModelsBuildingTeamOccluded );
							}

							ColorPicker( "Friends Visible", Config->Colors->ModelsBuildingFriendUnoccluded );
							ColorPicker( "Friends Invisible", Config->Colors->ModelsBuildingFriendOccluded );
							ColorPicker( "Local Visible", Config->Colors->ModelsBuildingLocalUnoccluded );
							ColorPicker( "Local Invisible", Config->Colors->ModelsBuildingLocalOccluded );
						}
						else
						{
							if ( Config->Models->Global->ColorScheme )
							{
								ColorPicker( "Red Buildings", Config->Colors->ModelsBuildingRed );
								ColorPicker( "Blu Buildings", Config->Colors->ModelsBuildingBlu );
							}
							else
							{
								ColorPicker( "Enemies", Config->Colors->ModelsBuildingEnemy );
								ColorPicker( "Teammates", Config->Colors->ModelsBuildingTeam );
							}
							ColorPicker( "Friends", Config->Colors->ModelsBuildingFriend );
							ColorPicker( "Local", Config->Colors->ModelsBuildingLocal );
						}
					}
					MenuGroup( "Buildings" );

					PopGroupX( true );
					PushGroupY( WindowSize.y + 24 + 18 );
					MenuGroupStart( );
					{
						if ( Config->Models->Players->TwoModels )
						{
							if ( Config->Models->Global->ColorScheme )
							{
								ColorPicker( "Red Players Visible", Config->Colors->ModelsPlayerRedUnoccluded );
								ColorPicker( "Red Players Invisible", Config->Colors->ModelsPlayerRedOccluded );
								ColorPicker( "Blu Players Visible", Config->Colors->ModelsPlayerBluUnoccluded );
								ColorPicker( "Blu Players Invisible", Config->Colors->ModelsPlayerBluOccluded );
							}
							else
							{
								ColorPicker( "Enemies Visible", Config->Colors->ModelsPlayerEnemyUnoccluded );
								ColorPicker( "Enemies Invisible", Config->Colors->ModelsPlayerEnemyOccluded );
								ColorPicker( "Teammates Visible", Config->Colors->ModelsPlayerTeamUnoccluded );
								ColorPicker( "Teammates Invisible", Config->Colors->ModelsPlayerTeamOccluded );
							}
							ColorPicker( "Friends Visible", Config->Colors->ModelsPlayerFriendUnoccluded );
							ColorPicker( "Friends Invisible", Config->Colors->ModelsPlayerFriendOccluded );
							ColorPicker( "Local Visible", Config->Colors->ModelsPlayerLocalUnoccluded );
							ColorPicker( "Local Invisible", Config->Colors->ModelsPlayerLocalOccluded );
							ColorPicker( "Ubered Visible", Config->Colors->ModelsPlayerUberUnoccluded );
							ColorPicker( "Ubered Invisible", Config->Colors->ModelsPlayerUberOccluded );
						}
						else
						{
							if ( Config->Models->Global->ColorScheme )
							{
								ColorPicker( "Red Players", Config->Colors->ModelsPlayerRed );
								ColorPicker( "Blu Players", Config->Colors->ModelsPlayerBlu );
							}
							else
							{
								ColorPicker( "Enemies", Config->Colors->ModelsPlayerEnemy );
								ColorPicker( "Teammates", Config->Colors->ModelsPlayerTeam );
							}
							ColorPicker( "Friends", Config->Colors->ModelsPlayerFriend );
							ColorPicker( "Local", Config->Colors->ModelsPlayerLocal );
							ColorPicker( "Ubered", Config->Colors->ModelsPlayerUber );
						}
					}
					MenuGroup( "Players" );

					MenuGroupStart( );
					{
						if ( Config->Models->World->TwoModels )
						{
							ColorPicker( "Healthpack Visible", Config->Colors->ModelsWorldHealthUnoccluded );
							ColorPicker( "Healthpack Invisible", Config->Colors->ModelsWorldHealthOccluded );
							ColorPicker( "Ammopack Visible", Config->Colors->ModelsWorldAmmoUnoccluded );
							ColorPicker( "Ammopack Invisible", Config->Colors->ModelsWorldAmmoOccluded );
							ColorPicker( "Bomb Visible", Config->Colors->ModelsWorldBombUnoccluded );
							ColorPicker( "Bomb Invisible", Config->Colors->ModelsWorldBombOccluded );
							ColorPicker( "MvM Money Visible", Config->Colors->ModelsWorldMoneyUnoccluded );
							ColorPicker( "MvM Money Invisible", Config->Colors->ModelsWorldMoneyOccluded );
							ColorPicker( "Powerups Visible", Config->Colors->ModelsWorldPowerupUnoccluded );
							ColorPicker( "Powerups Invisible", Config->Colors->ModelsWorldPowerupOccluded );

							if ( Config->Models->Global->ColorScheme )
							{
								ColorPicker( "Red Projectiles Visible", Config->Colors->ModelsWorldRedUnoccluded );
								ColorPicker( "Red Projectiles Invisible", Config->Colors->ModelsWorldRedOccluded );
								ColorPicker( "Blu Projectiles Visible", Config->Colors->ModelsWorldBluUnoccluded );
								ColorPicker( "Blu Projectiles Invisible", Config->Colors->ModelsWorldBluOccluded );
							}
							else
							{
								ColorPicker( "Enemies Visible", Config->Colors->ModelsWorldEnemyUnoccluded );
								ColorPicker( "Enemies Invisible", Config->Colors->ModelsWorldEnemyOccluded );
								ColorPicker( "Teammates Visible", Config->Colors->ModelsWorldTeamUnoccluded );
								ColorPicker( "Teammates Invisible", Config->Colors->ModelsWorldTeamOccluded );
							}

							ColorPicker( "Friends Visible", Config->Colors->ModelsWorldFriendUnoccluded );
							ColorPicker( "Friends Invisible", Config->Colors->ModelsWorldFriendOccluded );
							ColorPicker( "Local Visible", Config->Colors->ModelsWorldLocalUnoccluded );
							ColorPicker( "Local Invisible", Config->Colors->ModelsWorldLocalOccluded );
						}
						else
						{
							ColorPicker( "Healthpack", Config->Colors->ModelsWorldHealth );
							ColorPicker( "Ammopack", Config->Colors->ModelsWorldAmmo );
							ColorPicker( "Bomb", Config->Colors->ModelsWorldBomb );
							ColorPicker( "MvM Money", Config->Colors->ModelsWorldMoney );
							ColorPicker( "Powerups", Config->Colors->ModelsWorldPowerup );
							if ( Config->Models->Global->ColorScheme )
							{
								ColorPicker( "Red Projectiles", Config->Colors->ModelsWorldRed );
								ColorPicker( "Blu Projectiles", Config->Colors->ModelsWorldBlu );
							}
							else
							{
								ColorPicker( "Enemies", Config->Colors->ModelsWorldEnemy );
								ColorPicker( "Teammates", Config->Colors->ModelsWorldTeam );
							}
							ColorPicker( "Friends", Config->Colors->ModelsWorldFriend );
							ColorPicker( "Local", Config->Colors->ModelsWorldLocal );
						}
					}
					MenuGroup( "World" );
					break;
				}

				case SUBTAB_OUTLINES:
				{
					PopGroupX( false );
					PushGroupY( WindowSize.y + 24 + 18 );
					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->Outlines->Global->Enabled );
						ComboBox( "Color Scheme", Config->Outlines->Global->ColorScheme, { "Enemy/Team", "Red/Blu" } ); //0 - Enemy/Team, 1 - Red/Blu
						MultiComboBox( "Outline Types", { { "Blur", &Config->Outlines->Global->BlurOutline },
									   { "Stencil", &Config->Outlines->Global->StencilOutline } } );
						if ( Config->Outlines->Global->BlurOutline )
						{
							SliderInt( "Blur Scale", Config->Outlines->Global->BlurScale, 1, 10, 193 );
						}

						if ( Config->Outlines->Global->StencilOutline )
						{
							CheckBox( "Cheap Stencil", Config->Outlines->Global->CheapStencil );
							SliderInt( "Stencil Scale", Config->Outlines->Global->StencilScale, 1, 5, 193 );
						}
					}
					MenuGroup( "Global" );

					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->Outlines->Buildings->Enabled );
						if ( Config->Outlines->Buildings->Enabled )
						{
							MultiComboBox( "Entities", { { "Enemies", &Config->Outlines->Buildings->Enemy },
										   { "Teammates", &Config->Outlines->Buildings->Team },
										   { "Friends", &Config->Outlines->Buildings->Friends },
										   { "Local", &Config->Outlines->Buildings->Local } } );
							SliderFloat( "Alpha", Config->Outlines->Buildings->Alpha, 0.0f, 1.0f, 193 );

							if ( Config->Outlines->Global->ColorScheme )
							{
								ColorPicker( "Red Buildings", Config->Colors->OutlinesBuildingRed );
								ColorPicker( "Blu Buildings", Config->Colors->OutlinesBuildingBlu );
							}
							else
							{
								ColorPicker( "Enemies", Config->Colors->OutlinesBuildingEnemy );
								ColorPicker( "Teammates", Config->Colors->OutlinesBuildingTeam );
							}
							ColorPicker( "Friends", Config->Colors->OutlinesBuildingFriend );
							ColorPicker( "Local", Config->Colors->OutlinesBuildingLocal );
						}
					}
					MenuGroup( "Buildings" );

					PopGroupX( true );
					PushGroupY( WindowSize.y + 24 + 18 );
					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->Outlines->Players->Enabled );
						if ( Config->Outlines->Players->Enabled )
						{
							MultiComboBox( "Entities", { { "Enemy", &Config->Outlines->Players->Enemy },
										   { "Team", &Config->Outlines->Players->Team },
										   { "Friends", &Config->Outlines->Players->Friends },
										   { "Local", &Config->Outlines->Players->Local } } );
							SliderFloat( "Alpha", Config->Outlines->Players->Alpha, 0.0f, 1.0f, 193 );

							if ( Config->Outlines->Global->ColorScheme )
							{
								ColorPicker( "Red Players", Config->Colors->OutlinesPlayerRed );
								ColorPicker( "Blu Players", Config->Colors->OutlinesPlayerBlu );
							}
							else
							{
								ColorPicker( "Enemies", Config->Colors->OutlinesPlayerEnemy );
								ColorPicker( "Teammates", Config->Colors->OutlinesPlayerTeam );
							}
							ColorPicker( "Friends", Config->Colors->OutlinesPlayerFriend );
							ColorPicker( "Local", Config->Colors->OutlinesPlayerLocal );
							ColorPicker( "Ubered", Config->Colors->OutlinesPlayerUber );
						}
					}
					MenuGroup( "Players" );

					MenuGroupStart( );
					{
						CheckBox( "Enabled", Config->Outlines->World->Enabled );
						if ( Config->Outlines->World->Enabled )
						{
							MultiComboBox( "Entities", { { "Healthpacks", &Config->Outlines->World->Healthpacks },
										   { "Ammopacks", &Config->Outlines->World->Ammopacks },
										   { "Intel", &Config->Outlines->World->Intels },
										   { "Bombs", &Config->Outlines->World->Bombs },
										   { "Projectiles", &Config->Outlines->World->Projectiles },
										   { "MvM Money", &Config->Outlines->World->MvMCash },
										   { "MvM Tanks", &Config->Outlines->World->MvMTanks },
										   { "MvM Sentry Busters", &Config->Outlines->World->MvMSentryBusters },
										   { "Powerups", &Config->Outlines->World->MPPowerups },
										   { "Enemy", &Config->Outlines->World->Enemy },
										   { "Team", &Config->Outlines->World->Team },
										   { "Friends", &Config->Outlines->World->Friends },
										   { "Local", &Config->Outlines->World->Local } } );
							SliderFloat( "Alpha", Config->Outlines->World->Alpha, 0.0f, 1.0f, 193 );

							ColorPicker( "Healthpack", Config->Colors->OutlinesWorldHealth );
							ColorPicker( "Ammopack", Config->Colors->OutlinesWorldAmmo );
							ColorPicker( "Bombs", Config->Colors->OutlinesWorldBomb );
							ColorPicker( "MvM Money", Config->Colors->OutlinesWorldMoney );
							ColorPicker( "Powerups", Config->Colors->OutlinesWorldPowerup );
							if ( Config->Outlines->Global->ColorScheme )
							{
								ColorPicker( "Red Projectiles", Config->Colors->OutlinesWorldRed );
								ColorPicker( "Blu Projectiles", Config->Colors->OutlinesWorldBlu );
							}
							else
							{
								ColorPicker( "Enemies", Config->Colors->OutlinesWorldEnemy );
								ColorPicker( "Teammates", Config->Colors->OutlinesWorldTeam );
							}
							ColorPicker( "Friends", Config->Colors->OutlinesWorldFriend );
							ColorPicker( "Local", Config->Colors->OutlinesWorldLocal );
						}
					}
					MenuGroup( "World" );
					break;
				}

				case SUBTAB_MISC_:
				{
					PopGroupX( false );
					PushGroupY( WindowSize.y + 24 + 18 );
					MenuGroupStart( );
					{
						SliderInt( "FOV", Config->Visuals->FOV, 30, 120, 193, 1, "%ideg" );
						SliderFloat( "Aspect Ratio", Config->Visuals->AspectRatio, 0.0f, 2.0f, 193 );
						SliderFloat( "Bloom Scale", Config->Visuals->BloomScale, 0.0f, 20.0f, 193 );
						if ( Config->Visuals->RemoveScope )
						{
							SliderInt( "Scoped FOV", Config->Visuals->ScopedFOV, 30, 120, 193, 1, "%ideg" );
						}
						MultiComboBox( "Removals", {
							{"Shake", &Config->Visuals->RemoveShake},
							{"Fade", &Config->Visuals->RemoveFade},
							{"Post Process", &Config->Visuals->RemovePostProcessing},
							{"Scope", &Config->Visuals->RemoveScope},
							{"Recoil", &Config->Visuals->RemoveViewPunch},
							{ "Equip Region", &Config->Visuals->UnlockEquipRegion }
									   } );
						ComboBox( "Bullet Tracers", Config->Visuals->BulletTracers, { "None", "Machina", "C.A.P.P.E.R", "Short Circuit", "Merasmus Beam 1", "Merasmus Beam 2", "Big Nasty" } );

						CheckBox( "Hit Marker", Config->Visuals->HitMarker2D );
						/*MultiComboBox( "Hit Marker", {
							{"Center", &Config->Visuals->HitMarker2D},
							{"World", &Config->Visuals->HitMarker3D},
							{"Show Hitbox", &Config->Visuals->HitboxHitMarker},
									   } );*/

						CheckBoxKey( "Third Person", Config->Visuals->ThirdPersonOffset, Config->Visuals->ThirdPersonKey );
						KeyBind( "Sideways Cycle", Config->Visuals->ThirdPersonSidewaysCycleKey );
						SliderInt( "Offset Forward", Config->Visuals->ThirdPersonForward, -300, 300, 193 );
						SliderInt( "Offset Sideways", Config->Visuals->ThirdPersonSideways, -300, 300, 193 );
						SliderInt( "Offset Upward", Config->Visuals->ThirdPersonUpward, -300, 300, 193 );
						CheckBox( "Streamer Mode", Config->Visuals->StreamerMode );
						MultiComboBox( "Hide Names", { { "Local", &Config->Visuals->HideLocal },
									   { "Friends", &Config->Visuals->HideFriends } } );
						CheckBox( "Medal Changer", Config->Visuals->MedalChanger );
						SliderInt( "Medal Tier", Config->Visuals->MedalTier, 1, 8, 193 );
						SliderInt( "Medal Level", Config->Visuals->MedalLevel, 1, 150, 193 );
						CheckBox( "Optimized Text", Config->Visuals->OptimizedFonts );
					}
					MenuGroup( "General" );

					MenuGroupStart( );
					{
						if ( CheckBox( "Enabled", Config->Visuals->WorldModulation->Enabled ) )
						{
							Visuals->UpdateWorldModulation( );
						}

						ComboBox( "Properties", Config->Visuals->WorldModulation->Mode, { "Night Mode", "Fullbright", "Custom Color" } );
						MultiComboBox( "Textures", { { "World", &Config->Visuals->WorldModulation->World },
									   { "Skybox", &Config->Visuals->WorldModulation->Skybox } } );
						if ( Config->Visuals->WorldModulation->Enabled )
						{
							if ( !Config->Visuals->WorldModulation->Mode )
							{
								if ( SliderFloat( "Darkness", Config->Visuals->WorldModulation->Amount, 0.0f, 1.0f, 193 ) )
								{
									Visuals->UpdateWorldModulation( );
								}
								SliderFloat( "Alpha", Config->Visuals->WorldModulation->Alpha, 0.0f, 1.0f, 193 );
							}

							if ( Config->Visuals->WorldModulation->Mode == 2 )
							{
								ColorPicker( "World Color", Config->Colors->WorldColor );
								SliderFloat( "Alpha", Config->Visuals->WorldModulation->Alpha, 0.0f, 1.0f, 193 );
							}
						}
						TextBox( "Custom Skybox", Config->Visuals->SkyboxName, 120 );
					}
					MenuGroup( "World Properties" );

					PopGroupX( true );
					PushGroupY( WindowSize.y + 24 + 18 );

					/*MenuGroupStart( );
					{
						CheckBox( "Draw LagComp", Config->Visuals->DrawBackTrack );
						CheckBox( "Only Last Tick", Config->Visuals->LastTickOnly );
						if ( Config->Visuals->DrawBackTrack )
						{
							MultiComboBox( "Draw Types", {
							{ "Model", &Config->Visuals->BackTrackModel },
							{ "Outline", &Config->Visuals->BackTrackOutline } } );
							if ( Config->Visuals->BackTrackModel )
							{
								SliderFloat( "Alpha", Config->Visuals->BackTrackAlpha, 0.0f, 1.0f, 193 );
							}
						}
					}
					MenuGroup( "LagComp" );*/

					MenuGroupStart( );
					{
						CheckBox( "Viewmodel Sway", Config->Visuals->ViewmodelSway );
						SliderFloat( "Sway Amount", Config->Visuals->ViewmodelSwayAmount, 0.01f, 1.f, 193 );
						CheckBox( "Viewmodel Flip", Config->Visuals->ViewmodelFlip );
						SliderInt( "Viewmodel FOV", Config->Visuals->ViewmodelFOV, 30, 170, 193 );
						CheckBox( "Viewmodel Pos", Config->Visuals->ViewmodelPos );

						SliderInt( "Viewmodel X", Config->Visuals->ViewmodelX, -30, 30, 193 );
						SliderInt( "Viewmodel Y", Config->Visuals->ViewmodelY, -30, 30, 193 );
						SliderInt( "Viewmodel Z", Config->Visuals->ViewmodelZ, -30, 30, 193 );
						SliderInt( "Viewmodel Roll", Config->Visuals->ViewmodelRoll, -180, 180, 193 );

						CheckBoxColor( "Hands Model", Config->Visuals->HandsModels, Config->Colors->HandsColor );
						ComboBox( "Material", Config->Visuals->HandsModel, { "None", "Flat", "Shaded", "Fresnel" } );
						MultiComboBox( "Don't Draw", { { "Hands", &Config->Visuals->DisableHands },
									   { "Weapon", &Config->Visuals->DisableWeapon } } );
						SliderFloat( "Alpha", Config->Visuals->HandsAlpha, 0.0f, 1.0f, 193 );
						CheckBoxColor( "Hands Overlay", Config->Visuals->HandsOverlay, Config->Colors->HandsOverlayColor );
						ComboBox( "Overlay Material", Config->Visuals->HandsOverlayModel, { "Contour", "Sheen", "Glow", "Flat" } );
						CheckBox( "Overlay Wireframe", Config->Visuals->HandsOverlayWireframe );
						SliderFloat( "Overlay Alpha", Config->Visuals->HandsOverlayAlpha, 0.0f, 1.0f, 193 );
					}
					MenuGroup( "Viewmodel" );

					MenuGroupStart( );
					{
						ColorPicker( "Menu Outline", Config->Colors->MenuOutline );
						ColorPicker( "Menu Background", Config->Colors->MenuBackground );
						//ColorPicker( "Menu Foreground", Config->Colors->MenuForeground );
						ColorPicker( "Menu Borders", Config->Colors->MenuBorders );
						ColorPicker( "Menu Accent 1", Config->Colors->MenuFade1 );
						ColorPicker( "Menu Accent 2", Config->Colors->MenuFade2 );
						ColorPicker( "Menu Shine", Config->Colors->MenuShine );
					}
					MenuGroup( "Menu" );

					break;
				}
				default: break;
			}
			break;
		}
		case TAB_MISC:
		{
			PopGroupX( false );
			PopGroupY( true );
			MenuGroupStart( );
			{
				MultiComboBox( "Movement", { { "Auto-Jump", &Config->Misc->BHop },
							   { "Auto-Strafe", &Config->Misc->AutoStrafer },
							   { "Fast Stop", &Config->Misc->FastStop },
							   { "Avoid Push", &Config->Misc->AvoidTeamPush },
							   { "Fast Backup", &Config->Misc->FastBackup },
							   { "Edge Jump", &Config->Misc->EdgeJump } } );
				if ( Config->Misc->AutoStrafer )
				{
					SliderInt( "Turn Speed", Config->Misc->StraferScale, 0, 65, 193, 1, "%i%%" );
				}
				if ( Config->Misc->EdgeJump )
				{
					KeyBind( "Edge Jump Key", Config->Misc->EdgeJumpKey );
				}
				CheckBoxKey( "High Jump", Config->Misc->HighJump, Config->Misc->HighJumpKey );
				CheckBoxKey( "Auto Retreat", Config->Misc->AutoRetreat, Config->Misc->AutoRetreatKey );
				CheckBox( "Anti-Backstab", Config->Misc->AntiBackstab );
				CheckBox( "Anti-AutoBalance", Config->Misc->AntiAutoBalance );
				CheckBox( "Fake Taunt", Config->Visuals->FakeTaunt->Enabled );
			}
			MenuGroup( "Main" );

			//PopGroupX(false);
			//PopGroupY(false);
			MenuGroupStart( );
			{
				CheckBoxKey( "Enabled", Config->Misc->DT->Enabled, Config->Misc->DT->DTKey );
				if ( !Config->Misc->DT->ShootWood )
				{
					ComboBox( "Delay Mode", Config->Misc->DT->DelayMode, { "Fast", "Safe" } );
					SliderInt( "Delay Ticks", Config->Misc->DT->DelayTicks, 1, 5, 193, 1, "%iticks" );
				}
				KeyBind( "Recharge Key", Config->Misc->DT->RechargeKey );
				KeyBind( "Warp Key", Config->Misc->DT->WarpKey );
				ComboBox( "Warp Mode", Config->Misc->DT->WarpMode, {
					"Tick",
					"Dynamic",
					"Pulse",
					"Fluctuate"
						  } );

				if ( Config->Misc->DT->WarpMode == 0 )
					SliderInt( "Warp Ticks", Config->Misc->DT->WarpTicks, 1, 22, 150, 1, "%iticks" );

				CheckBox( "Nitro Mode (-WOOD-)", Config->Misc->DT->ShootWood );
				CheckBox( "Anti-Warp", Config->Misc->DT->AntiWarp );
				CheckBox( "Indicator", Config->Misc->DT->Indicator->Enabled );
				CheckBox( "Custom Colors", Config->Misc->DT->Indicator->CustomFade );
				ColorPicker( "Indicator Accent 1", Config->Colors->DTFade1 );
				ColorPicker( "Indicator Accent 2", Config->Colors->DTFade2 );

				MultiComboBox( "Activation", {
					{ "In Air", &Config->Misc->DT->ActiveInAir },
					{ "Manual Fire", &Config->Misc->DT->ActiveOnlyAimbot },
					{ "Lethal Only", &Config->Misc->DT->ActivateOnlyLethal } } );
			}
			MenuGroup( "DoubleTap" );

			MenuGroupStart( );
			{
				CheckBox( "Enabled", Config->Visuals->RagdollEffects );
				MultiComboBox( "Entities", { { "Enemy", &Config->Visuals->RagdollEnemy },
							   { "Teammates", &Config->Visuals->RagdollTeam },
							   { "Killed by Local", &Config->Visuals->RagdollLocal }, } );

				MultiComboBox( "Particle Effects", { { "Burning", &Config->Visuals->RagdollBurning },
							   { "Electrocuted", &Config->Visuals->RagdollElectrocuted },
							   { "Dissolving", &Config->Visuals->RagdollDissolving },
							   { "Become Ash", &Config->Visuals->RagdollBecomeAsh } } );

				ComboBox( "Model Effect", Config->Visuals->RagdollBaseType, { "None", "Gold", "Ice" } );
			}
			MenuGroup( "Ragdolls" );

			PopGroupX( true );
			PopGroupY( true );

			MenuGroupStart( );
			{
				CheckBoxKey( "Enabled", Config->Misc->CritHack->Enabled, Config->Misc->CritHack->CritKey );
				CheckBox( "Skip Random", Config->Misc->CritHack->SkipRandom );
				MultiComboBox( "Force On",
							   {
								   { "Melee", &Config->Misc->CritHack->ForceOnMelee },
								   { "Cheater", &Config->Misc->CritHack->ForceOnCheater },
								   { "Friends", &Config->Misc->CritHack->ForceOnFriends },
								   { "Important", &Config->Misc->CritHack->ForceOnStreamers },
								   { "Suspicious", &Config->Misc->CritHack->ForceOnSuspicious }
							   } );
				CheckBox( "Indicator", Config->Misc->CritHack->Indicator->Enabled );
				SliderInt( "Optimization", Config->Misc->CritHack->Optimization, 0, 100, 130, 1, "%i%%" );
			}
			MenuGroup( "CritHack" );

			MenuGroupStart( );
			{
				CheckBox( "Enabled", Config->Misc->VoiceSpam );
				ComboBox( "Mode", Config->Misc->VoiceSpamMode, { "KillSay", "Music" } );
				//TextBox( "Device", SelectedDevice, 120 );
				Button( "Refresh Devices", [ & ] ( ) { Misc->InitializeSound( ); }, 159 );
				TextBox( "Sound", SelectedSound, 159 );
				if ( Config->Misc->VoiceSpamMode )
				{
					Button( "Play Sound", [ & ] ( ) { Misc->VoiceSay( ); }, 159 );
				}
			}
			MenuGroup( "Voice Spam" );

			MenuGroupStart( );
			{
				CheckBox( "Enabled", Config->Misc->NoSpread->Enabled );
				CheckBox( "Indicator", Config->Misc->NoSpread->Indicator );
			}
			MenuGroup( "NoSpread" );

			MenuGroupStart( );
			{
				CheckBoxColor( "Projectile Line", Config->Visuals->ProjectileLine, Config->Colors->ProjectileLineColor );
				SliderFloat( "Line Alpha", Config->Visuals->ProjectileLineAlpha, 0.0f, 1.0f, 193 );
				if ( Config->Visuals->ProjectileLine )
				{
					CheckBox( "Projectile Camera", Config->Visuals->ProjectileCamera );
					if ( Config->Visuals->ProjectileCamera )
					{
						SliderInt( "Camera FOV", Config->Visuals->ProjectileCameraFOV, 30, 120, 193 );
						SliderInt( "Camera Width", Config->Visuals->ProjectileCameraWidth, 200, 400, 193 );
						SliderInt( "Camera Height", Config->Visuals->ProjectileCameraHeight, 200, 400, 193 );
					}
				}
			}
			MenuGroup( "Projectile Visualization" );

			MenuGroupStart( );
			{
				CheckBox( "Enabled", Config->Misc->Matchmaking->RegionSelector->Enabled );

				if ( Config->Misc->Matchmaking->RegionSelector->Enabled )
				{
					MultiComboBox( "Regions", { { "EU East", &Config->Misc->Matchmaking->RegionSelector->EUEast },
								   { "EU West", &Config->Misc->Matchmaking->RegionSelector->EUWest },
								   { "Asia", &Config->Misc->Matchmaking->RegionSelector->Asia },
								   { "Middle East", &Config->Misc->Matchmaking->RegionSelector->MiddleEast },
								   { "Hong Kong", &Config->Misc->Matchmaking->RegionSelector->HongKong },
								   { "Japan", &Config->Misc->Matchmaking->RegionSelector->Japan },
								   { "India", &Config->Misc->Matchmaking->RegionSelector->India },
								   { "Australia", &Config->Misc->Matchmaking->RegionSelector->Australia },
								   { "Africa", &Config->Misc->Matchmaking->RegionSelector->Africa },
								   { "Chile", &Config->Misc->Matchmaking->RegionSelector->Chile },
								   { "Peru", &Config->Misc->Matchmaking->RegionSelector->Peru },
								   { "Brazil", &Config->Misc->Matchmaking->RegionSelector->Brazil },
								   { "US East", &Config->Misc->Matchmaking->RegionSelector->USEast },
								   { "US North West", &Config->Misc->Matchmaking->RegionSelector->USNorthWest },
								   { "US South West", &Config->Misc->Matchmaking->RegionSelector->USSouthWest }, } );
				}
				CheckBox( "Auto Accept", Config->Misc->Matchmaking->AutoAccept );
				CheckBox( "Prevent Auto Join", Config->Misc->Matchmaking->PreventAutoJoin );
			}
			MenuGroup( "Matchmaking" );

			MenuGroupStart( );
			{
				CheckBox( "Enabled", Config->Visuals->Beams->Enabled );
				ComboBox( "Color Scheme", Config->Visuals->Beams->ColorScheme, { "Enemy/Team", "Red/Blu" } );
				MultiComboBox( "Entities", { { "Enemies", &Config->Visuals->Beams->Enemy },
							   { "Teammates", &Config->Visuals->Beams->Team },
							   { "Friends", &Config->Visuals->Beams->Friends },
							   { "Local", &Config->Visuals->Beams->Local } } );

				if ( Config->Visuals->Beams->ColorScheme )
				{
					ColorPicker( "Red", Config->Colors->BeamsRed );
					ColorPicker( "Blu", Config->Colors->BeamsBlu );
				}
				else
				{
					ColorPicker( "Enemies", Config->Colors->BeamsEnemy );
					ColorPicker( "Teammates", Config->Colors->BeamsTeam );
				}
				ColorPicker( "Friends", Config->Colors->BeamsFriend );
				ColorPicker( "Local", Config->Colors->BeamsLocal );
			}
			MenuGroup( "Beams" );

			break;
		}
		case TAB_HVH:
		{
			PopGroupX( false );
			PopGroupY( true );
			MenuGroupStart( );
			{
				CheckBoxKey( "Enabled", Config->Misc->FakeLag->Enabled, Config->Misc->FakeLag->FakeLagKey );
				CheckBox( "Draw Lag", Config->Misc->FakeLag->DrawLag );
				MultiComboBox( "Draw Types", {
					{ "Model", &Config->Misc->FakeLag->LagModel },
					{ "Outline", &Config->Misc->FakeLag->LagOutline } } );

				if ( Config->Misc->FakeLag->LagModel )
				{
					SliderFloat( "Alpha", Config->Misc->FakeLag->ModelAlpha, 0.0f, 1.0f, 193 );
					ColorPicker( "Model Color", Config->Colors->LagModel );
				}

				if ( Config->Misc->FakeLag->LagOutline )
				{
					ColorPicker( "Model Color", Config->Colors->LagGlow );
				}

				SliderInt( "Lag Ticks", Config->Misc->FakeLag->Value, 0, 23, 193 );
				CheckBox( "Break LC", Config->Misc->FakeLag->BreakLagComp );

				MultiComboBox( "Conditions", { { "Moving", &Config->Misc->FakeLag->BreakMoving },
							   { "In Air", &Config->Misc->FakeLag->BreakInAir } } );
				CheckBox( "Unchoke On Attack", Config->Misc->FakeLag->UnchokeOnAttack );
			}
			MenuGroup( "Fake Lag" );

			MenuGroupStart( );
			{
				CheckBoxKey( "Enabled", Config->Misc->FakeDuck->Enabled, Config->Misc->FakeDuck->FakeDuckKey );
			}
			MenuGroup( "Fake Duck" );

			PopGroupX( true );
			PopGroupY( true );

			MenuGroupStart( );
			{
				CheckBoxKey( "Enabled", Config->Misc->AntiAim->Enabled, Config->Misc->AntiAim->ToggleKey );
				CheckBox( "Rage AA", Config->Misc->AntiAim->RageActive );

				CheckBox( "Fake Model", Config->Misc->AntiAim->DrawFake );
				MultiComboBox( "Draw Types", {
					{ "Model", &Config->Misc->AntiAim->FakeModel },
					{ "Outline", &Config->Misc->AntiAim->FakeOutline } } );

				if ( Config->Misc->AntiAim->FakeModel )
				{
					SliderFloat( "Alpha", Config->Misc->AntiAim->FakeAlpha, 0.0f, 1.0f, 193 );
					ColorPicker( "Model Color", Config->Colors->FakeModel );
				}

				if ( Config->Misc->AntiAim->FakeOutline )
				{
					ColorPicker( "Outline Color", Config->Colors->FakeGlow );
				}

				KeyBind( "Cycle Key", Config->Misc->AntiAim->CycleKey );
				if ( !Config->Misc->AntiAim->RageActive )
				{
					SliderInt( "Legit Yaw", Config->Misc->AntiAim->LegitYaw, -180, 180, 193 );
				}
				else
				{
					ComboBox( "Fake Mode", Config->Misc->AntiAim->FakeMode, { "Static", "Jitter", "Spin", "Rotate" } );
					if ( Config->Misc->AntiAim->FakeMode != 2 )
					{
						SliderInt( "Fake Yaw", Config->Misc->AntiAim->FakeYaw, -180, 180, 193 );
					}
					switch ( Config->Misc->AntiAim->FakeMode )
					{
						case 1:
						{
							SliderInt( "Jitter Delta", Config->Misc->AntiAim->FakeDelta, 0, 180, 193 );
							break;
						}

						case 2:
						{
							SliderInt( "Spin Speed", Config->Misc->AntiAim->FakeSpin, -45, 45, 193 );
							break;
						}

						case 3:
						{
							SliderInt( "Rotate Speed", Config->Misc->AntiAim->FakeRotate, 1, 60, 193 );
							SliderInt( "Angle 1", Config->Misc->AntiAim->FakeAngle1, -180, 180, 193 );
							SliderInt( "Angle 2", Config->Misc->AntiAim->FakeAngle2, -180, 180, 193 );
							break;
						}
					}

					MenuSpace( 10 );

					ComboBox( "Real Mode", Config->Misc->AntiAim->RealMode, { "Static", "Jitter", "Spin", "Rotate" } );
					if ( Config->Misc->AntiAim->RealMode != 2 )
					{
						SliderInt( "Real Yaw", Config->Misc->AntiAim->RealYaw, -180, 180, 193 );
					}
					switch ( Config->Misc->AntiAim->RealMode )
					{
						case 1:
						{
							SliderInt( "Jitter Delta", Config->Misc->AntiAim->RealDelta, 0, 180, 193 );
							break;
						}

						case 2:
						{
							SliderInt( "Spin Speed", Config->Misc->AntiAim->RealSpin, -45, 45, 193 );
							break;
						}

						case 3:
						{
							SliderInt( "Rotate Speed", Config->Misc->AntiAim->RealRotate, 1, 60, 193 );
							SliderInt( "Angle 1", Config->Misc->AntiAim->RealAngle1, -180, 180, 193 );
							SliderInt( "Angle 2", Config->Misc->AntiAim->RealAngle2, -180, 180, 193 );
							break;
						}
					}

					MenuSpace( 10 );

					ComboBox( "Pitch Mode", Config->Misc->AntiAim->Pitch, { "None", "Automatic", "Manual" } );
					if ( Config->Misc->AntiAim->Pitch == 2 )
					{
						ComboBox( "Manual Pitch", Config->Misc->AntiAim->RealPitch, { "Up", "Down", "Jitter" } );
						CheckBox( "Fake Pitch", Config->Misc->AntiAim->FakePitch );
					}
				}
			}
			MenuGroup( "Anti-Aim" );
			break;
		}
		case TAB_AUTO:
		{
			PopGroupX( false );
			PopGroupY( true );
			MenuGroupStart( );
			{
				CheckBox( "Enabled", Config->AutoVacc->Enabled );
				CheckBox( "Log Pops", Config->AutoVacc->LogReasons );
				//MenuSpace(10);
				MultiComboBox( "Pop Conditions", {
					{ "Explosive", &Config->AutoVacc->AutoPopExplosive },
					{ "Heavy [HP+]", &Config->AutoVacc->AutoPopHigherHealthHeavies },
					{ "Heavy", &Config->AutoVacc->AutoPopHeavies },
					{ "Shotgun", &Config->AutoVacc->AutoPopShotgun },
					{ "Headshot", &Config->AutoVacc->AutoPopHeadshot },
					{ "Crossbow Bolt", &Config->AutoVacc->AutoPopCrossbow }
							   } );
				CheckBox( "Ignore Friend Heavies", Config->AutoVacc->IgnoreFriendHeavies );

				if ( Config->AutoVacc->AutoPopHigherHealthHeavies )
				{
					SliderFloat( "Minimum Health Difference", Config->AutoVacc->AutoPopMinHealth, 1.f, 100.f, 193, 1.f, "%.0f%%" );
				}

				if ( Config->AutoVacc->AutoPopHigherHealthHeavies || Config->AutoVacc->AutoPopHeavies )
					SliderFloat( "Heavy Distance", Config->AutoVacc->HeavyDistance, 50.f, 600.f, 193, 1.f, "%.0funits" );

			}
			MenuGroup( "Vaccinator" );

			MenuGroupStart( );
			{
				CheckBox( "Enabled", Config->OtherAuto->Enabled );
				CheckBox( "Auto Detonation", Config->OtherAuto->AutoDetonation );
				ComboBox( "Mode", Config->OtherAuto->AutoDetonationMode, { "Best Damage", "Best Range" } );
				MultiComboBox( "Targets", {
					{"Players", &Config->OtherAuto->TargetPlayers},
					{"Buildings", &Config->OtherAuto->TargetBuildings},
					{"Stickies",&Config->OtherAuto->TargetStickies},
					{"Bonked", &Config->OtherAuto->TargetBonk},
					{"Ubered", &Config->OtherAuto->TargetUber},
					{"Vaccinator'd", &Config->OtherAuto->TargetVacc},
					{"Friends", &Config->OtherAuto->TargetFriends}
							   } );

				CheckBox( "Auto Airblast", Config->OtherAuto->AutoAirblast );
				CheckBox( "Predict Path", Config->OtherAuto->AutoAirblastPath );
				MultiComboBox( "Blast Targets", {
					{"Explosive", &Config->OtherAuto->BlastExplosive},
					{"Arrows", &Config->OtherAuto->BlastArrows},
					{"Stickies",&Config->OtherAuto->BlastStickies},
							   } );

			}
			MenuGroup( "Other" );

			PopGroupX( true );
			PopGroupY( true );
			MenuGroupStart( );
			{
				CheckBox( "Enabled", Config->AutoMedic->Enabled );
				CheckBoxKey( "Lock Target", Config->AutoMedic->TargetLock, Config->AutoMedic->LockTarget );
				CheckBox( "Render", Config->AutoMedic->Render );
			}
			MenuGroup( "Medic" );
			break;
		}
		case TAB_PLAYERLIST:
		{
			PopGroupX( false );
			PopGroupY( true );
			MenuGroupStart( );
			{
				for ( int i = 1; i < 64; i++ )
				{
					PlayerInfo_t info;
					if ( gEngineClient->GetPlayerInfo( i, &info ) )
					{
						if ( info.fakeplayer || info.friendsID <= 0 )
							continue;

						RenderPlayer( i, info );
					}
				}

			}
			MenuGroup( "General" );

			PopGroupX( true );
			PopGroupY( true );

			PlayerInfo_t OpenInfo;
			MenuGroupStart( );
			{
				PlayerInfo_t info;

				for ( int i = 1; i < 64; i++ )
				{
					if ( gEngineClient->GetPlayerInfo( i, &info ) )
					{
						if ( info.fakeplayer )
						{
							continue;
						}

						if ( gPrioritiesOpen[ info.friendsID ] )
						{
							OpenInfo = info;
							ComboBox( "Priority", gPriorities[ info.friendsID ].Priority,
									  {
										  "Default",
										  "Suspicious",
										  "Cheater",
										  "Friend",
										  "Important"
									  } );

							if ( Button( "Save", 193 ) )
								SavePlayer( info.friendsID, gPriorities[ info.friendsID ] );

							CheckBox( "Resolver", gPriorities[ info.friendsID ].Resolver );
							CheckBox( "Kill Say", gPriorities[ info.friendsID ].VoiceSay );

							if ( auto Player = ( CBasePlayer* )gEntityList->GetClientEntity( i ) )
							{
								if ( gLocalPlayer != Player && gLocalPlayer && gLocalPlayer->TeamNum( ) == Player->TeamNum( ) )
								{
									if ( Button( "Kick", 193 ) )
									{
										char buffer[ 128 ];
										sprintf_s( buffer, "callvote kick %i", info.userID );
										gEngineClient->ClientCmd_Unrestricted( buffer );
									}
								}
							}
						}
					}
				}
			}

			const char* name = "No Player Selected";
			if ( strlen( OpenInfo.name ) > 2 && OpenInfo.userID > 0 && gPrioritiesOpen[ OpenInfo.friendsID ] )
				name = OpenInfo.name;

			MenuGroup( name );
			break;
		}
		case TAB_SKINCHANGER:
		{
			RenderSkinChanger( );
			break;
		}
		case TAB_DEBUG:
		{
			if ( G->Authentication->IsDebugAllowed )
			{
				PopGroupX( false );
				PopGroupY( true );
				MenuGroupStart( );
				{
					CheckBox( "Enable Info", Config->Debug->EnableInfo );
					SliderFloat( "Decay Rate", Config->Debug->DecayRate, 0.f, 1.f, 193, 0.1f );
					SliderFloat( "Clamp", Config->Debug->ClampCheck, 0.1f, 14.f, 193, 0.1f );
					SliderInt( "Samples", Config->Debug->Samples, 2, 64, 193, 2 );
				}
				MenuGroup( "General" );
			}
			break;
		}
		default: break;
	}

	if ( IsComboboxOpen )
	{
		if ( UpdateCombo( ) )
		{
			Visuals->UpdateWorldModulation( );
		}
	}

	if ( IsMultiComboboxOpen )
	{
		if ( UpdateMultiCombo( ) )
		{
			Visuals->UpdateWorldModulation( );
		}
	}

	if ( IsColorPickerOpen )
	{
		if ( UpdateColor( ) )
		{
			Visuals->UpdateWorldModulation( );
		}
	}

}

void CMenu::RenderPlayer( int idx, const PlayerInfo_t& info )
{
	Color clr = Color( 255, 255, 255 );
	switch ( gPriorities[ info.friendsID ].Priority )
	{
		case 0: break;
		case 1: clr = Color( 230, 135, 10 ); break;
		case 2: clr = Color( 255, 50, 50 ); break;
		case 3: clr = Color( 0, 200, 255 ); break;
		case 4: clr = Color( 50, 100, 255 ); break;
	}

	if ( ToggleableText( TF2->Utf8ToWide( info.name ).c_str( ), gPrioritiesOpen[ info.friendsID ], clr ) )
	{
		if ( gPrioritiesOpen[ info.friendsID ] )
		{
			for ( auto const& [key, val] : gPrioritiesOpen )
			{
				if ( key == info.friendsID )
					continue;

				gPrioritiesOpen[ key ] = false;
			}
		}
	}
}

void CMenu::SavePlayer( int FriendsID, const PriorityPlayer_t& Player )
{
	Json::Value Root;
	std::ifstream Input( "AterisPlayer.list" );
	if ( Input.is_open( ) )
	{
		Input >> Root;
		Input.close( );
	}

	if ( !Root[ std::to_string( FriendsID ) ][ "priority" ].isNull( ) )
	{
		if ( Root[ std::to_string( FriendsID ) ][ "priority" ].asInt( ) == Player.Priority )
			return;

		if ( Root[ std::to_string( FriendsID ) ][ "resolver" ].asBool( ) == Player.Resolver )
			return;

		if ( Root[ std::to_string( FriendsID ) ][ "voicesay" ].asBool( ) == Player.VoiceSay )
			return;
	}

	Root[ std::to_string( FriendsID ) ][ "priority" ] = Player.Priority;
	Root[ std::to_string( FriendsID ) ][ "resolver" ] = Player.Resolver;
	Root[ std::to_string( FriendsID ) ][ "voicesay" ] = Player.VoiceSay;
	std::ofstream output( "AterisPlayer.list" );
	if ( output.is_open( ) )
	{
		output << Root;
		output.close( );
	}
}

void CMenu::Init( )
{
	//MenuBackground = Draw->CreateTexture( Textures::MenuBackground, 501, 650 );
	ComboArrow = Draw->CreateTexture( Textures::ComboArrow, 13, 13 );
	AterisLogo18px = Draw->CreateTexture( Textures::AterisLogo18x18px, 18, 18 );
	AterisLogo28px = Draw->CreateTexture( Textures::AterisLogo28x28px, 28, 28 );

	ConfigPath = std::filesystem::current_path( ).string( ) + ( "\\ateris" );
	if ( !std::filesystem::exists( ConfigPath ) )
	{
		std::filesystem::create_directory( ConfigPath );
	}

	if ( std::filesystem::exists( ConfigPath ) && !std::filesystem::exists( ConfigPath + "\\" + "Default.json" ) )
	{
		SaveConfig( "Default" );
	}

	SoundFolder = ConfigPath + "\\VoiceSpam";
	if ( !std::filesystem::exists( SoundFolder ) )
	{
		std::filesystem::create_directory( SoundFolder );
	}

	//Misc->InitializeSound( );
}

void CMenu::Reset( )
{
	if ( IsOpen )
	{
		gSurface->SetCursorAlwaysVisible( false );
		IsOpen = false;
	}

	Draw->DeleteTexturesFromMemory( );
}

void CMenu::RenderSkinChanger( )
{
	PopGroupX( false );
	PopGroupY( true );
	MenuGroupStart( );
	{
		if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !gLocalWeapon )
		{
			Text( "Respawn or join a game", Color( 255, 255, 255 ) );
			return;
		}

		auto& SkinChangerEntry = Config->SkinChanger->Skins[ gLocalWeapon->GetIndex( ) ];

		CheckBox( "Enable", Config->SkinChanger->Enabled );
		LastGroupPos.y = LastGroupPos.y + 12;
		Text( gLocalWeapon->GetWeaponName( ), Color( 255, 255, 255 ) );
		CheckBox( "Australium", Config->SkinChanger->Skins[ gLocalWeapon->ItemDefinitionIndex( ) ].Aussie );
		CheckBox( "Weapon Swap", SkinChangerEntry.SwapModel );
		if ( SkinChangerEntry.SwapModel )
		{
			ComboBox( "Weapon", SkinChangerEntry.Model,
					  {
						  "Sniper Rifle",
						  "Shooting Star",
						  "Machina",
						  "Rocket Launcher",
						  "Grenade Launcher",
						  "Minigun",
						  "Direct Hit",
						  "Original",
						  "Cow Mangler"
					  } );
		}
	}
	MenuGroup( "Main" );
}