#include "CritHacks.h"
#include "../../SDK/Math/MD5.h"
#include "../Menu/Input/Input.h"
#include "../../../Globals.h"
#include "../../Hooks/HookList.h"
#pragma warning ( disable : 4018 )

bool CCrithack::ShouldForceCrit( )
{
	if ( gLocalWeapon->CritTokenBucket( ) < CalculateCost( gLocalWeapon ) )
		return false;

	if ( gLocalWeapon->IsRapidFireCrits() )
		if ( gLocalWeapon->CritChecks( ) < ( gLocalWeapon->CritSeedRequests( ) + 1 ) * 10 )
			return false;

	if ( G->CritHack->Force || ( ( GetAsyncKeyState( Config->Misc->CritHack->CritKey ) & 0x8000 )
		&& Input->InFocus( ) && !gSurface->IsCursorVisible( ) ) )
	{
		if ( Data->CritBan )
		{
			if ( gLocalWeapon->Slot( ) != 2 )
				return false;
		}

		return true;
	}

	return false;
}

void CCrithack::Force( CUserCmd* pCmd )
{
	static auto tf_weapon_criticals = gCvar->FindVar( "tf_weapon_criticals" );
	if ( tf_weapon_criticals && tf_weapon_criticals->GetInt( ) == 0 )
		return;

	if ( !gPlayerResource )
		return;

	Resync( );

	if ( !gLocalPlayer || !gLocalWeapon || !Config->Misc->CritHack->Enabled )
		return;

	Fill( pCmd );

	if ( G->Aimbot.SavedWeaponID == TF_WEAPON_MINIGUN && pCmd->buttons & IN_ATTACK )
		pCmd->buttons &= ~( IN_ATTACK2 ); // heavy fix

	// isattacking is true for 3 ticks at least - for melee
	if ( !CanForce( ) )
	{
		G->CritHack->Force = false; // make sure this doesnt get set to true on 
		// weapons that cant crit and queue up a crit 
		return;
	}

	if ( gLocalPlayer->IsCritBoosted( ) )
		return;

	static int SafeIdx = 0, CritIdx = 0, CheckIdx = 0;
	auto& Safe = Data->Safe[ gLocalWeapon->GetIndex( ) ];
	auto& Crit = Data->Crit[ gLocalWeapon->GetIndex( ) ];

	bool ShouldDo = ShouldForceCrit();

	if ( gLocalWeapon->IsRapidFireCrits( ) && ( gLocalWeapon->LastRapidfireCritCheckTime( ) + 1.f > gGlobals->interval_per_tick * gLocalPlayer->TickBase( ) ) )
		return;

	if ( Crit.size( ) > 2 && !ShouldDo )
	{
		CritIdx = CritIdx % Crit.size( );
		CanCrit( Crit[ CritIdx ].CmdNum );
		CritIdx++;
	}

	if ( ( TF2->IsAttacking( pCmd ) || gLocalWeapon->Slot( ) == SLOT_MELEE ) )
	{
		if ( !ShouldDo )
		{
			if ( Safe.size( ) <= 2 )
				return;

			SafeIdx = SafeIdx % Safe.size( );

			pCmd->command_number = Safe[ SafeIdx ].CmdNum;
			WishRandomSeed = Safe[ SafeIdx ].Seed;

			if ( gLocalWeapon->Slot( ) == SLOT_MELEE && TF2->IsAttacking( pCmd ) )
				Safe.erase( Safe.begin( ) + SafeIdx );

			SafeIdx++;
		}
		else
		{
			if ( Crit.size( ) <= 2 )
				return;

			CritIdx = CritIdx % Crit.size( );

			pCmd->command_number = Crit[ CritIdx ].CmdNum;
			WishRandomSeed = Crit[ CritIdx ].Seed;

			if ( gLocalWeapon->Slot( ) == 2 && TF2->IsAttacking( pCmd ) )
				Crit.erase( Crit.begin( ) + CritIdx );

			CritIdx++;

			if ( ( pCmd->buttons & IN_ATTACK ) && gLocalWeapon->Slot( ) == SLOT_MELEE )
			{
				HasFiredMelee = true;
			}
		}
	}

	G->CritHack->Force = false;
}

int CCrithack::DecryptEncryptSeed( CBaseCombatWeapon* Weapon, const int seed )
{

	if ( Weapon == nullptr )
		return 0;

	int extra = Weapon->GetIndex( ) << 8 | gEngineClient->GetLocalPlayer( );

	if ( Weapon->Slot( ) == 2 )
		extra <<= 8;

	return extra ^ seed;
}

bool CCrithack::IsPureCritCommand( const int command_number, const int range, const bool lower_than )
{
	if ( !gLocalPlayer || !gLocalWeapon )
		return false;

	const auto random_seed = MD5_PseudoRandom( command_number ) & 0x7FFFFFFF;

	TF2->RandomSeed( DecryptEncryptSeed( gLocalWeapon, random_seed ) );

	return lower_than ? TF2->RandomInt( 0, 9999 ) < range : TF2->RandomInt( 0, 9999 ) > range;
}

void CCrithack::ClearCommandsIfNeeded( CUserCmd* pCmd )
{
	static int OldLocalPlayer = gEngineClient->GetLocalPlayer( );
	if ( OldLocalPlayer != gEngineClient->GetLocalPlayer( ) )
	{
		Data->Safe.clear( );
		Data->Crit.clear( );
		OldLocalPlayer = gEngineClient->GetLocalPlayer( );
	}

	for ( size_t i = 0; i < Data->Crit[ gLocalWeapon->GetIndex( ) ].size( ); i++ )
		if ( Data->Crit[ gLocalWeapon->GetIndex( ) ][ i ].CmdNum < pCmd->command_number )
			Data->Crit[ gLocalWeapon->GetIndex( ) ].erase( Data->Crit[ gLocalWeapon->GetIndex( ) ].begin( ) + i );

	for ( size_t i = 0; i < Data->Safe[ gLocalWeapon->GetIndex( ) ].size( ); i++ )
		if ( Data->Safe[ gLocalWeapon->GetIndex( ) ][ i ].CmdNum < pCmd->command_number )
			Data->Safe[ gLocalWeapon->GetIndex( ) ].erase( Data->Safe[ gLocalWeapon->GetIndex( ) ].begin( ) + i );
}

void CCrithack::Fill( CUserCmd* pCmd )
{
	if ( !gLocalPlayer || !Config->Misc->CritHack->Enabled || !gEngineClient->IsInGame( ) || !CanForce( ) )
		return;

	ClearCommandsIfNeeded( pCmd );

	int SeedsToSearch = static_cast< int >( std::ceil( gMath->ReMapValClamped(
		static_cast< float >( Config->Misc->CritHack->Optimization ),
		0.f, 100.f,
		1028.f, 128.f ) ) );

	// lets keep searching for seeds, even tho we shouldnt pershlaps
	for ( int i = 0; i < SeedsToSearch; i++ )
	{
		bool Skip = false;
		int idx = i * i;

		for ( auto& cmd : Data->Crit[ gLocalWeapon->GetIndex( ) ] )
		{
			if ( cmd.CmdNum == idx )
			{
				Skip = true;
				break;
			}
		}

		if ( Skip )
			continue;

		for ( auto& cmd : Data->Safe[ gLocalWeapon->GetIndex( ) ] )
		{
			if ( cmd.CmdNum == idx )
			{
				Skip = true;
				break;
			}
		}

		if ( Skip )
			continue;

		if ( IsPureCritCommand( pCmd->command_number + idx, 1, true ) &&
			Data->Crit[ gLocalWeapon->GetIndex( ) ].size( ) <= 25 )
		{

			CCritCommand Command {};
			Command.Seed = MD5_PseudoRandom( pCmd->command_number + idx ) & 0x7FFFFFFF;
			Command.CmdNum = pCmd->command_number + idx;

			Data->Crit[ gLocalWeapon->GetIndex( ) ].emplace_back( Command );

			if ( Config->Misc->CritHack->Optimization > 55 || Data->Crit[ gLocalWeapon->GetIndex( ) ].size( ) > 12 )
				break;
		}
		else if ( IsPureCritCommand( pCmd->command_number + idx, 9998, false ) &&
			Data->Safe[ gLocalWeapon->GetIndex( ) ].size( ) <= 48 )
		{
			CCritCommand Command {};
			Command.Seed = MD5_PseudoRandom( pCmd->command_number + idx ) & 0x7FFFFFFF;
			Command.CmdNum = pCmd->command_number + idx;

			Data->Safe[ gLocalWeapon->GetIndex( ) ].emplace_back( Command );

			if ( Config->Misc->CritHack->Optimization > 55 )
				break;
		}
	}
}

float CCrithack::DamageTillCrit( )
{
	float dealt = Data->Damage->RangedDamage;
	int required = 0;
	for ( ; required < 4200; required++ )
	{
		const auto racist = Data->Damage->CritDamage * 0.333333334f;
		const auto Pizza = racist + ( dealt + required ) - Data->Damage->CritDamage;
		if ( racist <= 0.f || Pizza <= 0.f )
			return 0;

		float pizza = racist / Pizza;
		if ( pizza < Data->CorrectCritChance )
			break;
	}

	return required;
}

void CCrithack::Reset( )
{
	Data->Crit = {};
	Data->Safe = {};
	Data->CorrectCritChance = {};
	Data->UnFixedChance = {};
	Data->CritBan = false;
	Data->Damage = new CDamageData( );
}

float CCrithack::BucketCap( )
{
	static auto var = gCvar->FindVar( "tf_weapon_criticals_bucket_cap" );
	return var->GetFloat( );
}

void CCrithack::OnEvent( IGameEvent* Event )
{
	// gathers info

	if ( !gLocalPlayer || !gLocalWeapon )
		return;

	const auto attacked = gEntityList->GetClientEntity( gEngineClient->GetPlayerForUserID( Event->GetInt( "userid" ) ) );
	const auto attacker = gEntityList->GetClientEntity( gEngineClient->GetPlayerForUserID( Event->GetInt( "attacker" ) ) );
	if ( !attacked || !attacker )
		return;

	const auto crit = ( Event->GetBool( "crit" ) || Event->GetBool( "minicrit" ) );
	auto damage = static_cast< int >( Event->GetInt( "damageamount" ) );

	if ( attacker == gLocalPlayer && attacked != gLocalPlayer )
	{
		auto& Status = CritHack->PlayersStatus[ attacked->GetIndex( ) ];
		auto HealthDelta = Status.Health - Event->GetInt( "health" );
		Status.Health = Event->GetInt( "health" );
		Status.Updated = true;

		if ( damage > HealthDelta && !Event->GetInt( "health" ) )
			damage = HealthDelta;

		if ( damage <= 0 )
		{
			// its better to account for MORE crit damage
			// to account for LESS critdamage (eg. Damage below 0, correcting to 1 
			// Player health : 1, Damage : 153), clamp that to max health,
			// and the estimate becomes real :alien:
			// TODO: figure out a better way of reclamping the damage to correct values
			// on player death
			damage = std::clamp( Event->GetInt( "damageamount" ), 0, attacked->As<CBasePlayer>( )->GetMaxHealth( ) );
			if ( G->Authentication->IsDebugAllowed )
				gCvar->ConsoleColorPrintf( Color( 125, 125, 255 ),
					"Damage below 0, correcting to %i \n Player health: %i, Damage: %i\n", damage,
					attacked->As<CBasePlayer>( )->Health( ), Event->GetInt( "damageamount" ) );
		}

		if ( gLocalWeapon->Slot( ) == 2 )
		{
			Data->Damage->MeleeDamage += damage;
		}
		else
		{
			if ( crit )
			{
				if ( gLocalPlayer->IsCritBoosted( ) )
					Data->Damage->BoostedDamage += damage;
				else
				{
					Data->Damage->CritDamage += damage;
				}
			}
		}
	}
}

void CCrithack::CanCrit( int cmd )
{
	Data->CanCrit = false;

	auto Weapon = gLocalWeapon;
	if ( !Weapon )
		return;

	const auto Start = 0xA54/*m_flCritTokenBucket*/;
	const auto End = 0xB60/*m_flLastRapidFireCritCheckTime*/ + sizeof( float );
	const auto Size = End - Start;
	static auto BackupMemory = malloc( Size );
	memcpy( BackupMemory, static_cast< void* >( Weapon + Start ), Size );

	float NewObservedCritChance = 0.f;
	const auto OldSeed = *gRandomSeed;

	Weapon->RandomSeed( )++;
	Weapon->LastRapidfireCritCheckTime( ) = 0.f;

	*gRandomSeed = MD5_PseudoRandom( cmd ) & 0x7FFFFFFF;

	Data->Forcing = true;
	{
		auto WeaponMode = Weapon->WeaponMode( );
		Weapon->WeaponMode( ) = 0;
		using fn = void( __thiscall* )( void* );
		reinterpret_cast<fn>(CalcIsAttackCritical::OriginalFunction)( Weapon );
		NewObservedCritChance = Weapon->ObservedCritChance( );
		Weapon->WeaponMode( ) = WeaponMode;
	}
	Data->Forcing = false;
	
	Data->CanCrit = Weapon->CritShot( );

	*gRandomSeed = OldSeed;
	memcpy( static_cast< void* >( Weapon + Start ), BackupMemory, Size );
	gLocalWeapon->ObservedCritChance( ) = NewObservedCritChance;
}

void CCrithack::HandleCanFireRandomCriticalShot( float Chance )
{
	auto Weapon = gLocalWeapon;
	if ( !Weapon )
		return;

	Weapon->ObservedCritChance( ) = 0.f;
	Data->CorrectCritChance = 0;
	Data->CritBan = false;

	const float CritDamage = Data->Damage->CritDamage;
	const float DamageOverall = Data->Damage->RangedDamage;

	if ( Weapon->Slot( ) == 2 )
		return;

	const float Damage = CritDamage / 3.f;
	Weapon->ObservedCritChance( ) = Damage / ( Damage + 
		( ( DamageOverall > 0 ? DamageOverall : 1 ) -CritDamage ) );

	Data->CorrectCritChance = Chance + 0.1f;
	Data->CritBan = Weapon->ObservedCritChance( ) >= Data->CorrectCritChance;
}

bool CCrithack::IsAttackCriticalHandler( )
{
	if ( !gPrediction->FirstTimePredicted || !gLocalPlayer || !gLocalWeapon )
		return false;

	// time to fix heavy
	if ( G->Aimbot.SavedWeaponID == TF_WEAPON_FLAMETHROWER || G->Aimbot.SavedWeaponID == TF_WEAPON_MINIGUN )
	{
		static auto AmmoCount = gLocalPlayer->GetAmmoCount( gLocalWeapon->PrimaryAmmoType( ) );
		if ( AmmoCount != gLocalPlayer->GetAmmoCount( gLocalWeapon->PrimaryAmmoType( ) ) )
		{
			AmmoCount = gLocalPlayer->GetAmmoCount( gLocalWeapon->PrimaryAmmoType( ) );
			return false;
		}
	}

	if ( WishRandomSeed != 0 )
	{
		*gRandomSeed = WishRandomSeed;
		WishRandomSeed = 0;
	}

	return true;
}

/*float CCrithack::GetWeaponMaxMult( CBaseCombatWeapon* Weapon )
{
	if ( !Weapon )
		return 3.f;

	switch ( Weapon->ItemDefinitionIndex( ) )
	{
		case Scout_m_ForceANature:
		case Scout_m_FestiveForceANature:
		{
			return 2.f;
		}
		default: 
		{
			switch ( Weapon->WeaponID( ) )
			{
				case TF_WEAPON_REVOLVER:
				case TF_WEAPON_ROCKETLAUNCHER:
				case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
				{
					return 2.f;
				}
				case TF_WEAPON_GRENADELAUNCHER:
				case TF_WEAPON_PIPEBOMBLAUNCHER:
				{
					return 2.5f;
				}
				default: return 3.f;
			}
		}
	}
}*/

float CCrithack::CalculateCost(CBaseCombatWeapon* Weapon)
{
	if (!gLocalPlayer || !gLocalWeapon)
		return 0.f;

	int SeedRequests = *reinterpret_cast<int*>(gLocalWeapon + 0xA5C);
	int CritChecks = *reinterpret_cast<int*>(gLocalWeapon + 0xA58);
	float mult = 1.f;
	if (SeedRequests > 0 && CritChecks > 0)
		mult = gLocalWeapon->Slot() == 2 ? ( HasFiredMelee ? 0.5f : 1.0f ) : gMath->ReMapValClamped(static_cast<float>(SeedRequests) / static_cast<float>(CritChecks), 0.1f, 1.f, 1.f, 3.f);

	Data->Multiplier = mult;
	Data->CritCost = std::clamp(
		Data->Damage->CorrectDamage * mult * ( Weapon->Slot( ) == SLOT_MELEE ? ( HasFiredMelee ? 1.0f : 0.5f ) : /*GetWeaponMaxMult( gLocalWeapon )*/ 3.f ),
		G->Aimbot.SavedWeaponID == TF_WEAPON_MINIGUN ? 1000.f : 0.f, 1000.f + 1000.f * mult );
	return Data->CritCost;
}

int CCrithack::PotentialCrits()
{
	if (!gLocalPlayer || !gLocalWeapon)
		return 0;

	auto Cost = CalculateCost(gLocalWeapon);
	auto Bucket = *reinterpret_cast<float*>(gLocalWeapon + 0xA54);

	int Crits = 0;
	for (; Crits < (gLocalWeapon->Slot() == SLOT_MELEE ? 100 : 20); Crits++)
	{
		if (Bucket >= Cost)
			Bucket -= Cost;
		else
			break;
	}
	return Crits;
}

bool CCrithack::CanForce( )
{
	if ( !gLocalPlayer || !gLocalWeapon || TF2->IsNonAimable( gLocalWeapon ) )
		return false;

	if ( gLocalPlayer->Class( ) == CLASS_SNIPER && gLocalWeapon->Slot( ) == 0 )
		return false;

	if ( gLocalPlayer->Class( ) == CLASS_SPY && ( gLocalWeapon->Slot( ) == 2 || gLocalWeapon->Slot( ) == 1 ) )
		return 0;

	if ( G->Aimbot.SavedWeaponID == TF_WEAPON_MEDIGUN )
		return false;

	switch ( gLocalWeapon->ItemDefinitionIndex( ) )
	{
	case Spy_m_FestiveAmbassador:
	case Spy_m_TheAmbassador:
	case Scout_s_MadMilk:
	case Scout_s_MutatedMilk:
	case Engi_p_PDA:
	case Engi_p_ConstructionPDA:
	case Engi_p_DestructionPDA:
	case Engi_p_ConstructionPDAR:
	case Heavy_s_Sandvich:
	case Heavy_s_SecondBanana:
	case Heavy_s_TheDalokohsBar:
	case Heavy_s_FestiveSandvich:
	case Heavy_s_Fishcake:
	case Heavy_s_RoboSandvich:
	case Engi_m_FestiveFrontierJustice:
	case Engi_m_TheFrontierJustice:
	case Soldier_m_RocketJumper:
	case Soldier_t_TheHalfZatoichi:
	case Demoman_s_StickyJumper:
	case Demoman_t_TheEyelander:
	case Demoman_t_TheClaidheamhMor:
	case Demoman_t_ThePersianPersuader:
	case Demoman_t_UllapoolCaber:
	case Pyro_m_ThePhlogistinator:
	case Pyro_m_DragonsFury:
	case Pyro_s_ThermalThruster:
	case Pyro_s_TheManmelter:
	case Pyro_s_GasPasser:
	case Pyro_t_TheAxtinguisher:
	case Soldier_t_TheMarketGardener:
	case Spy_m_TheDiamondback:
	case Scout_s_TheFlyingGuillotine:
	case Scout_s_TheFlyingGuillotineG:
		return false;
		break;
	}

	return true;
}

void CCrithack::Resync( )
{
	if ( !gLocalPlayer || !gPlayerResource )
		return;

	float ProperDmg = gPlayerResource->GetDamage( gLocalPlayer->GetIndex( ) ) -
		CritHack->Data->Damage->CritDamage - CritHack->Data->Damage->BoostedDamage - CritHack->Data->Damage->MeleeDamage;

	CritHack->Data->Damage->RangedDamage = ProperDmg;
}

void CCrithack::UpdatePlayers( )
{
	for ( int i = 1; i < gEntityList->GetHighestEntityIndex( ); i++ )
	{
		auto Ent = gEntityList->GetClientEntity( i )->As<CBasePlayer>( );
		if ( !Ent || !Ent->IsPlayer( ) )
			continue;

		if ( gPlayerResource->GetHealth( i ) )
		{
			auto& Status = PlayersStatus[ i ];
			if ( !Status.Updated && !( Status.Health != gPlayerResource->GetHealth( i ) || Status.Class != gPlayerResource->GetClass( i ) ) )
			{
				Status.Health = gPlayerResource->GetHealth( i );
				Status.Class = gPlayerResource->GetClass( i );
			}

			Status.Updated = false;
		}
	}
}