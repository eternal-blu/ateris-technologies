#include "Entities.h"
#include "../../../Globals.h"

void CEntityCache::Fill( )
{
	gLocalPlayer = gEntityList->GetClientEntity( gEngineClient->GetLocalPlayer( ) )->As<CBasePlayer>( );
	if ( gLocalPlayer )
		gLocalWeapon = gLocalPlayer->ActiveWeapon( );

	for ( int i = 1; i < gEntityList->GetHighestEntityIndex( ); i++ )
	{
		const auto& Entity = gEntityList->GetClientEntity( i )->As<CBaseEntity>( );
		if ( !Entity )
		{
			continue;
		}

		auto ClassID = Entity->GetClientClass( )->ClassID;
		switch ( ClassID )
		{
			case EClassID::CTFPlayer:
			{
				if ( Entity->As<CBasePlayer>( )->IsGhost( ) )
					continue;

				if ( Entity->TeamNum( ) == TEAM_RED || Entity->TeamNum( ) == TEAM_BLU )
				{
					if ( Entity->IsSentryBuster( ) )
					{
						gMvMEntities.push_back( Entity );
					}
					else
					{
						gPlayers.push_back( Entity->As<CBasePlayer>( ) );
					}
					gEntities.push_back( Entity );
				}
				break;
			}

			case EClassID::CObjectSentrygun:
			case EClassID::CObjectDispenser:
			case EClassID::CObjectTeleporter:
			{
				gBuildings.push_back( Entity->As<CBaseObject>( ) );
				gEntities.push_back( Entity );
				break;
			}

			case EClassID::CBaseAnimating:
			{
				const auto& name = FNV1A::Hash( gModelInfo->GetModelName( Entity->GetModel( ) ) );
				if ( Entity->IsHealth( name ) )
				{
					gHealthpacks.push_back( Entity );
					gEntities.push_back( Entity );
				}

				if ( Entity->IsAmmo( name ) )
				{
					gAmmopacks.push_back( Entity );
					gEntities.push_back( Entity );
				}

				if ( Entity->IsPowerup( name ) )
				{
					gPowerups.push_back( Entity );
				}
				break;
			}

			case EClassID::CTFAmmoPack:
			{
				gAmmopacks.push_back( Entity );
				gEntities.push_back( Entity );
				break;
			}

			case EClassID::CCaptureFlag:
			{
				gIntels.push_back( Entity );
				//gEntities.push_back( Entity );
				break;
			}

			case EClassID::CTFGenericBomb:
			case EClassID::CTFPumpkinBomb:
			{
				gBombs.push_back( Entity );
				gEntities.push_back( Entity );
				break;
			}

			case EClassID::CSniperDot:
			{
				if ( const auto& Owner = Entity->GetOwnerEntity( ) )
				{
					gSniperDots[ Owner->GetIndex( ) ] = Entity;
				}
				break;
			}

			case EClassID::CTFProjectile_Rocket:
			case EClassID::CTFGrenadePipebombProjectile:
			case EClassID::CTFProjectile_Jar:
			case EClassID::CTFProjectile_JarGas:
			case EClassID::CTFProjectile_JarMilk:
			case EClassID::CTFProjectile_Arrow:
			case EClassID::CTFProjectile_Flare:
			case EClassID::CTFProjectile_Cleaver:
			case EClassID::CTFProjectile_HealingBolt:
			case EClassID::CTFProjectile_EnergyRing:
			case EClassID::CTFProjectile_EnergyBall:
			case EClassID::CTFProjectile_ThrowableBreadMonster:
			case EClassID::CTFStunBall:
			case EClassID::CTFBall_Ornament: {
				gProjectiles.push_back( Entity );
				gEntities.push_back( Entity );
				break;
			}
			case EClassID::CTFPlayerResource:
			{
				gPlayerResource = Entity->As<CTFPlayerResource>( );
			}

			case EClassID::CCurrencyPack:
			{
				if ( Entity->Origin( ).IsZero( ) )
					continue;

				gMvMCash.push_back( Entity );
				break;
			}
			case EClassID::CTFTankBoss:
			{
				gMvMEntities.push_back( Entity );
				gEntities.push_back( Entity );
				break;
			}
			default: break;
		}
	}
}

void CEntityCache::Clear( )
{
	gPlayerResource = nullptr;
	gPlayers.clear( );
	gBuildings.clear( );
	gHealthpacks.clear( );
	gAmmopacks.clear( );
	gIntels.clear( );
	gBombs.clear( );
	gSniperDots.clear( );
	gProjectiles.clear( );
	gMvMCash.clear( );
	gMvMEntities.clear( );
	gPowerups.clear( );
	gEntities.clear( );
}

CBasePlayer* CBaseCombatWeapon::HealingTarget()
{
	return (CBasePlayer*)gEntityList->GetClientEntityFromHandle(hHealingTarget());
}

int CBasePlayer::GetMaxHealth()
{
	return Memory->GetVFunc<int(__thiscall*)(CBasePlayer*)>(this, 107)(this);
}

bool CBasePlayer::PhysicsRunTick( int Method )
{
	typedef bool( __thiscall* fn )( void*, int );
	static auto Func = Memory->FindSignature<fn>( "client.dll", "55 8B EC 53 8B D9 56 57 8B 83 ? ? ? ? C1" );
	return Func( this, Method );
}

void CBasePlayer::SetNextThink( float Time, const char* context )
{
	typedef void( __thiscall* fn )( void*, float, const char* );
	static auto Func = Memory->FindSignature<fn>( "client.dll", "55 8B EC F3 0F 10 45 ? 0F 2E 05 ? ? ? ? 53" );
	return Func( this, Time, context );
}

int CBasePlayer::GetNextThink( const char* context )
{
	typedef int( __thiscall* fn )( void*, const char* );
	static auto Func = Memory->FindSignature<fn>( "client.dll", "55 8B EC 8B 45 ? 56 8B F1 85 C0 75 ? 8B 86" );
	return Func( this, context );
}

void CBasePlayer::PreThink( )
{
	return Memory->GetVFunc<void( __thiscall* )( CBasePlayer* )>( this, 260 )( this );
}

void CBasePlayer::PostThink( )
{
	return Memory->GetVFunc<void( __thiscall* )( CBasePlayer* )>( this, 261 )( this );
}

void CBasePlayer::Think( )
{
	return Memory->GetVFunc<void( __thiscall* )( CBasePlayer* )>( this, 174 )( this );
}

bool CBaseCombatWeapon::CanFireCritical(bool HeadShot)
{
	bool Result = false;
	if (gLocalPlayer)
	{
		// lets promise ze compiler we wont change this bitch
		const int OldFov = gLocalPlayer->FOV();
		gLocalPlayer->FOV() = 70;
		using fn = bool(__thiscall*)(CBaseCombatWeapon*, bool, CBasePlayer*);
		Result = (*(fn**)this)[425](this, HeadShot, nullptr);
		gLocalPlayer->FOV() = OldFov;
	}
	return Result;
}

bool CBaseCombatWeapon::CanHeadshot( )
{
	return ( DamageType( ) & DMG_USE_HITLOCATIONS ) && CanFireCritical( true );
}

bool CBaseCombatWeapon::InReload() {
	static DWORD dwNextPrimaryAttack = NetVars->GetNetVar("CBaseCombatWeapon", "m_flNextPrimaryAttack");
	bool m_bInReload = *reinterpret_cast<bool*>(this + (dwNextPrimaryAttack + 0xC));
	int m_iReloadMode = *reinterpret_cast<int*>(this + 0xB28);
	return (m_bInReload || m_iReloadMode != 0);
}

bool CBaseCombatWeapon::CanFire()
{
	if ( !gLocalPlayer )
		return false;

	if (WeaponID() == TF_WEAPON_FLAME_BALL)
		return (*reinterpret_cast<float*>(this + 0x1B40) >= 100.f);

	const float ServerTime = TICKS_TO_TIME( gLocalPlayer->TickBase( ) );
	return NextPrimaryAttack() <= ServerTime && gLocalPlayer->NextAttack() <= ServerTime;
}

bool CBaseCombatWeapon::CanFireSecondary( )
{
	if ( !gLocalPlayer )
		return false;

	const float ServerTime = gLocalPlayer->TickBase( ) * gGlobals->interval_per_tick;
	return NextSecondaryAttack( ) <= ServerTime;
}

float CBaseCombatWeapon::SmackTime()
{
	static auto dwOffset = NetVars->GetNetVar("CTFWeaponBase", "m_nInspectStage") + 0x1C;
	return *reinterpret_cast<float*>(this + dwOffset);
}

void CBaseCombatWeapon::InvokeAttackHandler()
{
	static auto Siggy = Memory->FindSignature<void(__thiscall*)(void*)>("client.dll", "53 57 6A ? 68 ? ? ? ? 68 ? ? ? ? 6A ? 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B D8 83 C4 ? 85 DB 0F 84");
	if (!Siggy) return;
	Siggy(this);
}

bool CBaseCombatWeapon::IsCriticalHelper()
{
	return Memory->GetVFunc<bool(__thiscall*)(CBaseCombatWeapon*)>(this, 396)(this);
}

bool CBaseEntity::IsEnemy()
{
	return gLocalPlayer->TeamNum() != TeamNum();
}

Vector CBasePlayer::GetHitboxPos(int Hitbox)
{
	if (auto hdr = gModelInfo->GetStudioModel(GetModel()))
	{
		Matrix3x4 Matrix[128];
		SetupBones(Matrix, 128, 0x100, gGlobals->curtime);

		if (Matrix)
		{
			if (auto set = hdr->GetHitboxSet(HitboxSet())) 
			{
				if (const auto& box = set->hitbox(Hitbox)) 
				{
					Vector pos = (box->bbmin + box->bbmax) * 0.5f;
					Vector out = {};
					gMath->VectorTransform(pos, Matrix[box->bone], out);
					return out;
				}
			}
		}
	}

	return {};
}

Vector CBasePlayer::GetHitboxPos(int Hitbox, Matrix3x4* Matrix, model_t* model)
{
	if (auto hdr = gModelInfo->GetStudioModel(model))
	{
		if (Matrix)
		{
			if (auto set = hdr->GetHitboxSet(HitboxSet()))
			{
				if (const auto& box = set->hitbox(Hitbox))
				{
					Vector pos = (box->bbmin + box->bbmax) * 0.5f;
					Vector out = {};
					gMath->VectorTransform(pos, Matrix[box->bone], out);
					return out;
				}
			}
		}
	}

	return {};
}

CBaseCombatWeapon* CBasePlayer::ActiveWeapon()
{
	auto Handle = hActiveWeapon( );
	if ( !Handle.IsValid( ) )
		return nullptr;

	return gEntityList->GetClientEntityFromHandle(Handle)->As< CBaseCombatWeapon>();
}

float CBasePlayer::MaxSpeed( bool bIgnoreSpecialAbility )
{
	static auto Siggy = Memory->FindSignature<void*>( "client.dll", "55 8B EC 83 EC 14 83 3D ? ? ? ? ? 56 8B F1 75 09 D9 EE 5E 8B E5 5D C2 04 00" );
	return reinterpret_cast< float( __thiscall* )( CBasePlayer*, bool ) >( Siggy )( this, bIgnoreSpecialAbility );
}

CBaseCombatWeapon* CBasePlayer::GetWeaponFromSlot( int slot )
{
	static DWORD MyWeapons = NetVars->GetNetVar( "CBaseCombatCharacter", "m_hMyWeapons" );
	int Weapon = *reinterpret_cast< int* >( this + ( MyWeapons + ( slot * 0x4 ) ) );
	return gEntityList->GetClientEntityFromHandle( Weapon )->As< CBaseCombatWeapon>( );
}

void CBaseCombatWeapon::GetSpreadAngles( Vector& out )
{
	return Memory->FindSignature<void( __thiscall* )( void*, Vector& )>( "client.dll", "55 8B EC 83 EC 18 56 57 6A 00 68 ? ? ? ? 68 ? ? ? ? 6A 00 8B F9 E8 ? ? ? ? 50 E8 ? ? ? ? 8B F0 83 C4 14 85 F6 74 10 8B 06 8B CE 8B 80 ? ? ? ? FF D0 84 C0 75 02" )( this, out );
}

float CBaseCombatWeapon::GetWeaponSpread( )
{
	static auto fn = reinterpret_cast< float( __thiscall* )( decltype( this ) ) >( Memory->FindSignature<DWORD>( "client.dll", "55 8B EC 83 EC 08 56 8B F1 57 6A 01 6A 00 8B 96 ? ? ? ? 8B 86 ? ? ? ? C1 E2 06 56 68 ? ? ? ? 51" ) );
	return fn( this );
}

bool CBaseCombatWeapon::IsSpreadWeapon( )
{
	switch ( WeaponID( ) )
	{
		case TF_WEAPON_MINIGUN:
		case TF_WEAPON_REVOLVER:
		case TF_WEAPON_SMG: { return true; }
		default: { return false; }
	}

	return false;
}

bool CBaseCombatWeapon::IsAttacking( CUserCmd* Cmd )
{
	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !this )
		return false;

	if ( ItemDefinitionIndex( ) == Soldier_m_TheBeggarsBazooka )
	{
		static bool loading = false;

		if ( this->Clip1( ) > 0 )
			loading = true;

		if ( !( Cmd->buttons & IN_ATTACK ) && loading ) {
			loading = false;
			return true;
		}
	}
	else
	{
		if ( WeaponID( ) == TF_WEAPON_COMPOUND_BOW || ( ( gLocalPlayer->Class( ) == CLASS_DEMOMAN && Slot( ) == 1 ) || WeaponID( ) == TF_WEAPON_STICKY_BALL_LAUNCHER ) )
		{
			static bool charging = false;

			if ( ChargeBeginTime( ) > 0.0f )
				charging = true;

			if ( !( Cmd->buttons & IN_ATTACK ) && charging ) {
				charging = false;
				return true;
			}
		}
	}

	if ( Slot( ) == 2 && gLocalPlayer->Class( ) != CLASS_SPY )
		return fabs( SmackTime( ) - gGlobals->curtime ) < gGlobals->interval_per_tick * 2.0f;

	bool shooting = Cmd->buttons & IN_ATTACK && CanFire( );
	return shooting;
}

const char* CBasePlayer::GetWeaponName( )
{
	if ( !ActiveWeapon( ) ) return "";

	switch ( ActiveWeapon( )->ItemDefinitionIndex( ) )
	{
		case Scout_m_ForceANature:
		case Scout_m_FestiveForceANature: { return "FORCE-A-NATURE"; }
		case Scout_m_FestiveScattergun: { return "SCATTERGUN"; }
		case Scout_m_BackcountryBlaster: { return "BACK SCATTER"; }
		case Scout_s_MutatedMilk: { return "MAD MILK"; }
		case Scout_s_TheWinger: { return "WINGER"; }
		case Scout_s_FestiveBonk:
		case Scout_s_BonkAtomicPunch: { return "BONK ATOMIC PUNCH"; }
		case Scout_s_PrettyBoysPocketPistol: { return "POCKET PISTOL"; }
		case Scout_s_CritaCola: { return "CRIT A COLA"; }
		case Scout_t_FestiveBat: { return "BAT"; }
		case Scout_t_FestiveHolyMackerel: { return "HOLY MACKEREL"; }
		case Scout_t_TheAtomizer: { return "ATOMIZER"; }
		case Scout_t_TheCandyCane: { return "CANDY CANE"; }
		case Scout_t_TheFanOWar: { return "FAN O WAR"; }
		case Scout_t_SunonaStick: { return "SUN ON A STICK"; }
		case Scout_t_TheBostonBasher: { return "BOSTON BASHER"; }
		case Scout_t_ThreeRuneBlade: { return "THREE RUNE BLADE"; }
		case Scout_t_TheFreedomStaff: { return "FREEDOM STAFF"; }
		case Scout_t_TheBatOuttaHell: { return "BAT OUTTA HELL"; }
		case Scout_s_Lugermorph:
		case Scout_s_VintageLugermorph: { return "LUGERMORPH"; }
		case Scout_s_TheCAPPER: { return "C.A.P.P.E.R"; }
		case Scout_t_UnarmedCombat: { return "UNARMED COMBAT"; }
		case Scout_t_Batsaber: { return "BATSABER"; }
		case Scout_t_TheHamShank: { return "HAM SHANK"; }
		case Scout_t_TheNecroSmasher: { return "NEGRO SMASHER"; }
		case Scout_t_TheConscientiousObjector: { return "OBJECTOR"; }
		case Scout_t_TheCrossingGuard: { return "CROSSING GUARD"; }
		case Scout_t_TheMemoryMaker: { return "MEMORY MAKER"; }

		case Soldier_m_FestiveRocketLauncher: { return "ROCKET LAUNCHER"; }
		case Soldier_m_RocketJumper: { return "ROCKET JUMPER"; }
		case Soldier_m_TheAirStrike: { return "AIR STRIKE"; }
		case Soldier_m_TheLibertyLauncher: { return "LIBERTY LAUNCHER"; }
		case Soldier_m_TheOriginal: { return "ORIGINAL"; }
		case Soldier_m_FestiveBlackBox:
		case Soldier_m_TheBlackBox: { return "BLACK BOX"; }
		case Soldier_m_TheBeggarsBazooka: { return "BEGGARS BAZOOKA"; }
		case Soldier_s_FestiveShotgun: { return "SHOTGUN"; }
		case Soldier_s_FestiveBuffBanner: { return "BUFF BANNER"; }
		case Soldier_s_TheConcheror: { return "CONCHEROR"; }
		case Soldier_s_TheBattalionsBackup: { return "BATTALIONS BACKUP"; }
		case Soldier_s_PanicAttack: { return "PANIC ATTACK"; }
		case Soldier_t_TheMarketGardener: { return "MARKET GARDENER"; }
		case Soldier_t_TheDisciplinaryAction: { return "DISCIPLINARY ACTION"; }
		case Soldier_t_TheEqualizer: { return "EQUALIZER"; }
		case Soldier_t_ThePainTrain: { return "PAIN TRAIN"; }
		case Soldier_t_TheHalfZatoichi: { return "HALF ZATOICHI"; }

		case Pyro_m_FestiveFlameThrower: { return "FLAME THROWER"; }
		case Pyro_m_ThePhlogistinator: { return "PHLOGISTINATOR"; }
		case Pyro_m_FestiveBackburner:
		case Pyro_m_TheBackburner: { return "BACKBURNER"; }
		case Pyro_m_TheRainblower: { return "RAINBLOWER"; }
		case Pyro_m_TheDegreaser: { return "DEGREASER"; }
		case Pyro_m_NostromoNapalmer: { return "NOSTROMO NAPALMER"; }
		case Pyro_s_FestiveFlareGun: { return "FLARE GUN"; }
		case Pyro_s_TheScorchShot: { return "SCORCH SHOT"; }
		case Pyro_s_TheDetonator: { return "DETONATOR"; }
		case Pyro_s_TheReserveShooter: { return "RESERVE SHOOTER"; }
		case Pyro_t_TheFestiveAxtinguisher:
		case Pyro_t_TheAxtinguisher: { return "AXTINGUISHER"; }
		case Pyro_t_Homewrecker: { return "HOMEWRECKER"; }
		case Pyro_t_ThePowerjack: { return "POWERJACK"; }
		case Pyro_t_TheBackScratcher: { return "BACK SCRATCHER"; }
		case Pyro_t_TheThirdDegree: { return "THIRD DEGREE"; }
		case Pyro_t_ThePostalPummeler: { return "POSTAL PUMMELER"; }
		case Pyro_t_PrinnyMachete: { return "PRINNY MACHETE"; }
		case Pyro_t_SharpenedVolcanoFragment: { return "VOLCANO FRAGMENT"; }
		case Pyro_t_TheMaul: { return "MAUL"; }
		case Pyro_t_TheLollichop: { return "LOLLICHOP"; }

		case Demoman_m_FestiveGrenadeLauncher: { return "GRENADE LAUNCHER"; }
		case Demoman_m_TheIronBomber: { return "IRON BOMBER"; }
		case Demoman_m_TheLochnLoad: { return "LOCH N LOAD"; }
		case Demoman_s_FestiveStickybombLauncher: { return "STICKYBOMB LAUNCHER"; }
		case Demoman_s_StickyJumper: { return "STICKY JUMPER"; }
		case Demoman_s_TheQuickiebombLauncher: { return "QUICKIEBOMB LAUNCHER"; }
		case Demoman_s_TheScottishResistance: { return "SCOTTISH RESISTANCE"; }
		case Demoman_t_HorselessHeadlessHorsemannsHeadtaker: { return "HORSEMANNS HEADTAKER"; }
		case Demoman_t_TheScottishHandshake: { return "SCOTTISH HANDSHAKE"; }
		case Demoman_t_FestiveEyelander:
		case Demoman_t_TheEyelander: { return "EYELANDER"; }
		case Demoman_t_TheScotsmansSkullcutter: { return "SCOTSMANS SKULLCUTTER"; }
		case Demoman_t_ThePersianPersuader: { return "PERSIAN PERSUADER"; }
		case Demoman_t_NessiesNineIron: { return "NESSIES NINE IRON"; }
		case Demoman_t_TheClaidheamhMor: { return "CLAIDHEAMH MOR"; }

		case Heavy_m_IronCurtain: { return "IRON CURTAIN"; }
		case Heavy_m_FestiveMinigun: { return "MINIGUN"; }
		case Heavy_m_Tomislav: { return "TOMISLAV"; }
		case Heavy_m_TheBrassBeast: { return "BRASS BEAST"; }
		case Heavy_m_Natascha: { return "NATASCHA"; }
		case Heavy_m_TheHuoLongHeaterG:
		case Heavy_m_TheHuoLongHeater: { return "HUO-LONG HEATER"; }
		case Heavy_s_TheFamilyBusiness: { return "FAMILY BUSINESS"; }
		case Heavy_s_FestiveSandvich:
		case Heavy_s_RoboSandvich:
		case Heavy_s_Sandvich: { return "SANDVICH"; }
		case Heavy_s_Fishcake: { return "FISHCAKE"; }
		case Heavy_s_SecondBanana: { return "BANANA"; }
		case Heavy_s_TheDalokohsBar: { return "CHOCOLATE"; }
		case Heavy_s_TheBuffaloSteakSandvich: { return "STEAK"; }
		case Heavy_t_FistsofSteel: { return "FISTS OF STEEL"; }
		case Heavy_t_TheHolidayPunch: { return "HOLIDAY PUNCH"; }
		case Heavy_t_WarriorsSpirit: { return "WARRIORS SPIRIT"; }
		case Heavy_t_TheEvictionNotice: { return "EVICTION NOTICE"; }
		case Heavy_t_TheKillingGlovesofBoxing: { return "KILLING GLOVES OF BOXING"; }
		case Heavy_t_ApocoFists: { return "APOCO-FISTS"; }
		case Heavy_t_FestiveGlovesofRunningUrgently:
		case Heavy_t_GlovesofRunningUrgently: { return "GLOVES OF RUNNING URGENTLY"; }
		case Heavy_t_TheBreadBite: { return "BREAD BITE"; }

		case Engi_m_FestiveFrontierJustice: { return "FRONTIER JUSTICE"; }
		case Engi_m_TheWidowmaker: { return "WIDOWMAKER"; }
		case Engi_s_TheGigarCounter:
		case Engi_s_FestiveWrangler: { return "WRANGLER"; }
		case Engi_s_TheShortCircuit: { return "SHORT CIRCUIT"; }
		case Engi_t_FestiveWrench: { return "WRENCH"; }
		case Engi_t_GoldenWrench: { return "GOLDEN WRENCH"; }
		case Engi_t_TheGunslinger: { return "GUNSLINGER"; }
		case Engi_t_TheJag: { return "JAG"; }
		case Engi_t_TheEurekaEffect: { return "EUREKA EFFECT"; }
		case Engi_t_TheSouthernHospitality: { return "SOUTHERN HOSPITALITY"; }

		case Medic_m_FestiveCrusadersCrossbow: { return "CRUSADERS CROSSBOW"; }
		case Medic_m_TheOverdose: { return "OVERDOSE"; }
		case Medic_m_TheBlutsauger: { return "BLUTSAUGER"; }
		case Medic_s_FestiveMediGun: { return "MEDIGUN"; }
		case Medic_s_TheQuickFix: { return "QUICK FIX"; }
		case Medic_s_TheKritzkrieg: { return "KRITZKRIEG"; }
		case Medic_s_TheVaccinator: { return "VACCINATOR"; }
		case Medic_t_FestiveBonesaw: { return "BONESAW"; }
		case Medic_t_FestiveUbersaw:
		case Medic_t_TheUbersaw: { return "UBERSAW"; }
		case Medic_t_TheVitaSaw: { return "VITASAW"; }
		case Medic_t_TheSolemnVow: { return "SOLEMN VOW"; }
		case Medic_t_Amputator: { return "AMPUTATOR"; }

		case Sniper_m_FestiveSniperRifle: { return "SNIPER RIFLE"; }
		case Sniper_m_FestiveHuntsman:
		case Sniper_m_TheHuntsman: { return "HUNTSMAN"; }
		case Sniper_m_TheMachina: { return "MACHINA"; }
		case Sniper_m_TheAWPerHand: { return "AWPER HAND"; }
		case Sniper_m_TheHitmansHeatmaker: { return "HITMANS HEATMAKER"; }
		case Sniper_m_TheSydneySleeper: { return "SYDNEY SLEEPER"; }
		case Sniper_m_ShootingStar: { return "SHOOTING STAR"; }
		case Sniper_s_FestiveJarate: { return "JARATE"; }
		case Sniper_s_TheSelfAwareBeautyMark: { return "JARATE"; }
		case Sniper_s_FestiveSMG: { return "SMG"; }
		case Sniper_t_TheBushwacka: { return "BUSHWACKA"; }
		case Sniper_t_KukriR:
		case Sniper_t_Kukri: { return "KUKRI"; }
		case Sniper_t_TheShahanshah: { return "SHAHANSHAH"; }
		case Sniper_t_TheTribalmansShiv: { return "TRIBALMANS SHIV"; }

		case Spy_m_FestiveRevolver: { return "REVOLVER"; }
		case Spy_m_FestiveAmbassador:
		case Spy_m_TheAmbassador: { return "AMBASSADOR"; }
		case Spy_m_BigKill: { return "BIG KILL"; }
		case Spy_m_TheDiamondback: { return "DIAMONDBACK"; }
		case Spy_m_TheEnforcer: { return "ENFORCER"; }
		case Spy_m_LEtranger: { return "LETRANGER"; }
		case Spy_s_Sapper:
		case Spy_s_SapperR:
		case Spy_s_FestiveSapper: { return "SAPPER"; }
		case Spy_s_TheRedTapeRecorder:
		case Spy_s_TheRedTapeRecorderG: { return "RED TAPE RECORDER"; }
		case Spy_s_TheApSapG: { return "AP-SAP"; }
		case Spy_s_TheSnackAttack: { return "SNACK ATTACK"; }
		case Spy_t_FestiveKnife: { return "KNIFE"; }
		case Spy_t_ConniversKunai: { return "KUNAI"; }
		case Spy_t_YourEternalReward: { return "YOUR ETERNAL REWARD"; }
		case Spy_t_TheBigEarner: { return "BIG EARNER"; }
		case Spy_t_TheSpycicle: { return "SPYCICLE"; }
		case Spy_t_TheSharpDresser: { return "SHARP DRESSER"; }
		case Spy_t_TheWangaPrick: { return "WANGA PRICK"; }
		case Spy_t_TheBlackRose: { return "BLACK ROSE"; }

		case Heavy_m_Deflector_mvm: { return "DEFLECTOR"; }
		case Misc_t_FryingPan: { return "FRYING PAN"; }
		case Misc_t_GoldFryingPan: { return "GOLDEN FRYING PAN"; }
		case Misc_t_Saxxy: { return "SAXXY"; }

		default:
		{
			switch ( ActiveWeapon( )->WeaponID( ) )
			{
				//scout
				case TF_WEAPON_SCATTERGUN: { return "SCATTERGUN"; }
				case TF_WEAPON_HANDGUN_SCOUT_PRIMARY: { return "SHORTSTOP"; }
				case TF_WEAPON_HANDGUN_SCOUT_SECONDARY: { return "PISTOL"; }
				case TF_WEAPON_SODA_POPPER: { return "SODA POPPER"; }
				case TF_WEAPON_PEP_BRAWLER_BLASTER: { return "BABY FACES BLASTER"; }
				case TF_WEAPON_PISTOL_SCOUT: { return "PISTOL"; }
				case TF_WEAPON_JAR_MILK: { return "MAD MILK"; }
				case TF_WEAPON_CLEAVER: { return "CLEAVER"; }
				case TF_WEAPON_BAT: { return "BAT"; }
				case TF_WEAPON_BAT_WOOD: { return "SANDMAN"; }
				case TF_WEAPON_BAT_FISH: { return "HOLY MACKEREL"; }
				case TF_WEAPON_BAT_GIFTWRAP: { return "WRAP ASSASSIN"; }

				//soldier
				case TF_WEAPON_ROCKETLAUNCHER: { return "ROCKET LAUNCHER"; }
				case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT: { return "DIRECT HIT"; }
				case TF_WEAPON_PARTICLE_CANNON: { return "COW MANGLER 5000"; }
				case TF_WEAPON_SHOTGUN_SOLDIER: { return "SHOTGUN"; }
				case TF_WEAPON_BUFF_ITEM: { return "BUFF BANNER"; }
				case TF_WEAPON_RAYGUN: { return "RIGHTEOUS BISON"; }
				case TF_WEAPON_SHOVEL: { return "SHOVEL"; }

				//pyro
				case TF_WEAPON_FLAMETHROWER: { return "FLAME THROWER"; }
				case TF_WEAPON_FLAME_BALL: { return "DRAGONS FURY"; }
				case TF_WEAPON_SHOTGUN_PYRO: { return "SHOTGUN"; }
				case TF_WEAPON_FLAREGUN: { return "FLAREGUN"; }
				case TF_WEAPON_FLAREGUN_REVENGE: { return "MANMELTER"; }
				case TF_WEAPON_JAR_GAS: { return "GAS PASSER"; }
				case TF_WEAPON_ROCKETPACK: { return "THERMAL THRUSTER"; }
				case TF_WEAPON_FIREAXE: { return "FIRE AXE"; }
				case TF_WEAPON_BREAKABLE_SIGN: { return "NEON ANNIHILATOR"; }
				case TF_WEAPON_SLAP: { return "HOT HAND"; }

				//demoman
				case TF_WEAPON_GRENADELAUNCHER: { return "GRENADE LAUNCHER"; }
				case TF_WEAPON_PIPEBOMBLAUNCHER: { return "STICKYBOMB LAUNCHER"; }
				case TF_WEAPON_CANNON: { return "LOOSE CANNON"; }
				case TF_WEAPON_BOTTLE: { return "BOTTLE"; }
				case TF_WEAPON_SWORD: { return "SWORD"; }
				case TF_WEAPON_STICKBOMB: { return "ULLAPOOL CABER"; }

				//heavy
				case TF_WEAPON_MINIGUN: { return "MINIGUN"; }
				case TF_WEAPON_SHOTGUN_HWG: { return "SHOTGUN"; }
				case TF_WEAPON_LUNCHBOX: { return "LUNCHBOX"; }
				case TF_WEAPON_FISTS: { return "FISTS"; }

				//engineer
				case TF_WEAPON_SHOTGUN_PRIMARY: { return "SHOTGUN"; }
				case TF_WEAPON_SHOTGUN_BUILDING_RESCUE: { return "RESCUE RANGER"; }
				case TF_WEAPON_SENTRY_REVENGE: { return "FRONTIER JUSTICE"; }
				case TF_WEAPON_DRG_POMSON: { return "POMSON 6000"; }
				case TF_WEAPON_PISTOL: { return "PISTOL"; }
				case TF_WEAPON_LASER_POINTER: { return "WRANGLER"; }
				case TF_WEAPON_MECHANICAL_ARM: { return "MECHANICAL ARM"; }
				case TF_WEAPON_WRENCH: { return "WRENCH"; }
				case TF_WEAPON_PDA_ENGINEER_DESTROY: { return "DESTRUCTION PDA"; }
				case TF_WEAPON_PDA_ENGINEER_BUILD: { return "CONSTRUCTION PDA"; }
				case TF_WEAPON_BUILDER: { return "TOOLBOX"; }

				//medic
				case TF_WEAPON_SYRINGEGUN_MEDIC: { return "SYRINGE GUN"; }
				case TF_WEAPON_CROSSBOW: { return "CROSSBOW"; }
				case TF_WEAPON_MEDIGUN: { return "MEDIGUN"; }
				case TF_WEAPON_BONESAW: { return "BONESAW"; }

				//sniper
				case TF_WEAPON_SNIPERRIFLE: { return "SNIPER RIFLE"; }
				case TF_WEAPON_COMPOUND_BOW: { return "COMPOUND BOW"; }
				case TF_WEAPON_SNIPERRIFLE_DECAP: { return "BAZAAR BARGAIN"; }
				case TF_WEAPON_SNIPERRIFLE_CLASSIC: { return "CLASSIC"; }
				case TF_WEAPON_SMG: { return "SMG"; }
				case TF_WEAPON_CHARGED_SMG: { return "CLEANERS CARBINE"; }
				case TF_WEAPON_JAR: { return "JARATE"; }
				case TF_WEAPON_CLUB: { return "CLUB"; }

				//spy
				case TF_WEAPON_REVOLVER: { return "REVOLVER"; }
				case TF_WEAPON_PDA_SPY_BUILD: { return "SAPPER"; }
				case TF_WEAPON_KNIFE: { return "KNIFE"; }
				case TF_WEAPON_PDA_SPY: { return "DISGUISE KIT"; }
				case TF_WEAPON_INVIS: { return "INVIS WATCH"; }

				case TF_WEAPON_GRAPPLINGHOOK: { return "GRAPPLING HOOK"; }

				default: break;
			}
		}
	}

	return "";
}

const char* CBaseCombatWeapon::GetWeaponName( )
{
	if ( !this ) return "";

	switch ( this->WeaponID( ) )
	{
		//scout
	case TF_WEAPON_SCATTERGUN:
	{
		return "SCATTERGUN";
	}
	case TF_WEAPON_HANDGUN_SCOUT_PRIMARY:
	{
		return "SHORTSTOP";
	}
	case TF_WEAPON_HANDGUN_SCOUT_SECONDARY:
	{
		return "PISTOL";
	}
	case TF_WEAPON_SODA_POPPER:
	{
		return "SODA POPPER";
	}
	case TF_WEAPON_PEP_BRAWLER_BLASTER:
	{
		return "BABY FACES BLASTER";
	}
	case TF_WEAPON_PISTOL_SCOUT:
	{
		return "PISTOL";
	}
	case TF_WEAPON_JAR_MILK:
	{
		return "MAD MILK";
	}
	case TF_WEAPON_CLEAVER:
	{
		return "CLEAVER";
	}
	case TF_WEAPON_BAT:
	{
		return "BAT";
	}
	case TF_WEAPON_BAT_WOOD:
	{
		return "SANDMAN";
	}
	case TF_WEAPON_BAT_FISH:
	{
		return "HOLY MACKEREL";
	}
	case TF_WEAPON_BAT_GIFTWRAP:
	{
		return "WRAP ASSASSIN";
	}

	//soldier
	case TF_WEAPON_ROCKETLAUNCHER:
	{
		return "ROCKET LAUNCHER";
	}
	case TF_WEAPON_ROCKETLAUNCHER_DIRECTHIT:
	{
		return "DIRECT HIT";
	}
	case TF_WEAPON_PARTICLE_CANNON:
	{
		return "COW MANGLER 5000";
	}
	case TF_WEAPON_SHOTGUN_SOLDIER:
	{
		return "SHOTGUN";
	}
	case TF_WEAPON_BUFF_ITEM:
	{
		return "BUFF BANNER";
	}
	case TF_WEAPON_RAYGUN:
	{
		return "RIGHTEOUS BISON";
	}
	case TF_WEAPON_SHOVEL:
	{
		return "SHOVEL";
	}

	//pyro
	case TF_WEAPON_FLAMETHROWER:
	{
		return "FLAME THROWER";
	}
	case TF_WEAPON_FLAME_BALL:
	{
		return "DRAGONS FURY";
	}
	case TF_WEAPON_SHOTGUN_PYRO:
	{
		return "SHOTGUN";
	}
	case TF_WEAPON_FLAREGUN:
	{
		return "FLAREGUN";
	}
	case TF_WEAPON_FLAREGUN_REVENGE:
	{
		return "MANMELTER";
	}
	case TF_WEAPON_JAR_GAS:
	{
		return "GAS PASSER";
	}
	case TF_WEAPON_ROCKETPACK:
	{
		return "THERMAL THRUSTER";
	}
	case TF_WEAPON_FIREAXE:
	{
		return "FIRE AXE";
	}
	case TF_WEAPON_BREAKABLE_SIGN:
	{
		return "NEON ANNIHILATOR";
	}
	case TF_WEAPON_SLAP:
	{
		return "HOT HAND";
	}

	//demoman
	case TF_WEAPON_GRENADELAUNCHER:
	{
		return "GRENADE LAUNCHER";
	}
	case TF_WEAPON_PIPEBOMBLAUNCHER:
	{
		return "STICKYBOMB LAUNCHER";
	}
	case TF_WEAPON_CANNON:
	{
		return "LOOSE CANNON";
	}
	case TF_WEAPON_BOTTLE:
	{
		return "BOTTLE";
	}
	case TF_WEAPON_SWORD:
	{
		return "SWORD";
	}
	case TF_WEAPON_STICKBOMB:
	{
		return "ULLAPOOL CABER";
	}

	//heavy
	case TF_WEAPON_MINIGUN:
	{
		return "MINIGUN";
	}
	case TF_WEAPON_SHOTGUN_HWG:
	{
		return "SHOTGUN";
	}
	case TF_WEAPON_LUNCHBOX:
	{
		return "LUNCHBOX";
	}
	case TF_WEAPON_FISTS:
	{
		return "FISTS";
	}

	//engineer
	case TF_WEAPON_SHOTGUN_PRIMARY:
	{
		return "SHOTGUN";
	}
	case TF_WEAPON_SHOTGUN_BUILDING_RESCUE:
	{
		return "RESCUE RANGER";
	}
	case TF_WEAPON_SENTRY_REVENGE:
	{
		return "FRONTIER JUSTICE";
	}
	case TF_WEAPON_DRG_POMSON:
	{
		return "POMSON 6000";
	}
	case TF_WEAPON_PISTOL:
	{
		return "PISTOL";
	}
	case TF_WEAPON_LASER_POINTER:
	{
		return "WRANGLER";
	}
	case TF_WEAPON_MECHANICAL_ARM:
	{
		return "MECHANICAL ARM";
	}
	case TF_WEAPON_WRENCH:
	{
		return "WRENCH";
	}
	case TF_WEAPON_PDA_ENGINEER_DESTROY:
	{
		return "DESTRUCTION PDA";
	}
	case TF_WEAPON_PDA_ENGINEER_BUILD:
	{
		return "CONSTRUCTION PDA";
	}
	case TF_WEAPON_BUILDER:
	{
		return "TOOLBOX";
	}

	//medic
	case TF_WEAPON_SYRINGEGUN_MEDIC:
	{
		return "SYRINGE GUN";
	}
	case TF_WEAPON_CROSSBOW:
	{
		return "CROSSBOW";
	}
	case TF_WEAPON_MEDIGUN:
	{
		return "MEDIGUN";
	}
	case TF_WEAPON_BONESAW:
	{
		return "BONESAW";
	}

	//sniper
	case TF_WEAPON_SNIPERRIFLE:
	{
		return "SNIPER RIFLE";
	}
	case TF_WEAPON_COMPOUND_BOW:
	{
		return "COMPOUND BOW";
	}
	case TF_WEAPON_SNIPERRIFLE_DECAP:
	{
		return "BAZAAR BARGAIN";
	}
	case TF_WEAPON_SNIPERRIFLE_CLASSIC:
	{
		return "CLASSIC";
	}
	case TF_WEAPON_SMG:
	{
		return "SMG";
	}
	case TF_WEAPON_CHARGED_SMG:
	{
		return "CLEANERS CARBINE";
	}
	case TF_WEAPON_JAR:
	{
		return "JARATE";
	}
	case TF_WEAPON_CLUB:
	{
		return "CLUB";
	}

	//spy
	case TF_WEAPON_REVOLVER:
	{
		return "REVOLVER";
	}
	case TF_WEAPON_PDA_SPY_BUILD:
	{
		return "SAPPER";
	}
	case TF_WEAPON_KNIFE:
	{
		return "KNIFE";
	}
	case TF_WEAPON_PDA_SPY:
	{
		return "DISGUISE KIT";
	}
	case TF_WEAPON_INVIS:
	{
		return "INVIS WATCH";
	}

	case TF_WEAPON_GRAPPLINGHOOK:
	{
		return "GRAPPLING HOOK";
	}

	default: break;
	}

	return "";
}

Color CBaseEntity::GetColor( int colorscheme )
{
	if ( colorscheme )
	{
		if ( IsPlayer( ) || IsBuilding( ) || IsProjectile( ) )
		{
			if ( TeamNum( ) == TEAM_RED )
			{
				return { 255, 50, 50 };
			}
			else if ( TeamNum( ) == TEAM_BLU )
			{
				return { 0, 200, 255 };
			}
			else
			{
				return { 255, 255, 255 };
			}
		}
	}
	else
	{
		if ( IsPlayer( ) || IsBuilding( ) || IsProjectile( ) )
		{
			if ( TeamNum( ) == gLocalPlayer->TeamNum( ) )
			{
				return { 0, 255, 100 };
			}
			else
			{
				return { 255, 100, 50 };
			}
		}
	}
	return { 255, 255, 255 };
}

Color CBaseEntity::GetOccludedColor( int colorscheme )
{
	if ( colorscheme )
	{
		if ( IsPlayer( ) || IsBuilding( ) || IsProjectile( ) )
		{
			if ( TeamNum( ) == TEAM_RED )
			{
				return { 128, 25, 25 };
			}
			else if ( TeamNum( ) == TEAM_BLU )
			{
				return { 0, 100, 128 };
			}
			else
			{
				return { 128, 128, 128 };
			}
		}
	}
	else
	{
		if ( IsPlayer( ) || IsBuilding( ) || IsProjectile( ) )
		{
			if ( TeamNum( ) == gLocalPlayer->TeamNum( ) )
			{
				return { 0, 128, 50 };
			}
			else
			{
				return { 128, 50, 50 };
			}
		}
	}
	return { 128, 128, 128 };
}

void CBasePlayer::UpdateClientSideAnimation( )
{
	static auto FN = reinterpret_cast< void( __thiscall* )( void* ) >( G->AnimationFix->UpdateClientSideAnimation );
	FN( this );
}

bool CBaseEntity::IsHealth( const FNV1A_t ModelHash )
{
	switch ( ModelHash )
	{
		case FNV1A::HashConst( TF_MEDKIT_SMALL_BDAY ):
		case FNV1A::HashConst( TF_MEDKIT_MEDIUM_BDAY ):
		case FNV1A::HashConst( TF_MEDKIT_LARGE_BDAY ):
		case FNV1A::HashConst( TF_MEDKIT_SMALL_HALLOWEEN ):
		case FNV1A::HashConst( TF_MEDKIT_MEDIUM_HALLOWEEN ):
		case FNV1A::HashConst( TF_MEDKIT_LARGE_HALLOWEEN ):
		case FNV1A::HashConst( LUNCHBOX_DROP_MODEL ):
		case FNV1A::HashConst( LUNCHBOX_STEAK_DROP_MODEL ):
		case FNV1A::HashConst( LUNCHBOX_ROBOT_DROP_MODEL ):
		case FNV1A::HashConst( LUNCHBOX_FESTIVE_DROP_MODEL ):
		case FNV1A::HashConst( TF_MEDKIT_SMALL ):
		case FNV1A::HashConst( TF_MEDKIT_MEDIUM ):
		case FNV1A::HashConst( TF_MEDKIT_LARGE ): { return true; }
		default: { return false; }
	}
}

bool CBaseEntity::IsAmmo( const FNV1A_t ModelHash )
{
	switch ( ModelHash )
	{
		case FNV1A::HashConst( TF_AMMOPACK_SMALL_BDAY ):
		case FNV1A::HashConst( TF_AMMOPACK_MEDIUM_BDAY ):
		case FNV1A::HashConst( TF_AMMOPACK_LARGE_BDAY ):
		case FNV1A::HashConst( TF_AMMOPACK_SMALL ):
		case FNV1A::HashConst( TF_AMMOPACK_MEDIUM ):
		case FNV1A::HashConst( TF_AMMOPACK_LARGE ): { return true; }
		default: { return false; }
	}
}

bool CBaseEntity::IsPowerup( const FNV1A_t ModelHash )
{
	switch ( ModelHash )
	{
		case FNV1A::HashConst( TF_POWERUP_AGILITY ):
		case FNV1A::HashConst( TF_POWERUP_CRIT ):
		case FNV1A::HashConst( TF_POWERUP_DEFENSE ):
		case FNV1A::HashConst( TF_POWERUP_HASTE ):
		case FNV1A::HashConst( TF_POWERUP_KING ):
		case FNV1A::HashConst( TF_POWERUP_KNOCKOUT ):
		case FNV1A::HashConst( TF_POWERUP_PLAGUE ):
		case FNV1A::HashConst( TF_POWERUP_PRECISION ):
		case FNV1A::HashConst( TF_POWERUP_REFLECT ):
		case FNV1A::HashConst( TF_POWERUP_REGEN ):
		case FNV1A::HashConst( TF_POWERUP_RESISTANCE ):
		case FNV1A::HashConst( TF_POWERUP_STRENGTH ):
		case FNV1A::HashConst( TF_POWERUP_SUPERNOVA ):
		case FNV1A::HashConst( TF_POWERUP_VAMPIRE ): { return true; }
		default: { return false; }
	}

	return false;
}

bool CBaseEntity::IsSentryBuster( )
{
	switch ( FNV1A::Hash( gModelInfo->GetModelName( this->GetModel( ) ) ) )
	{
		case FNV1A::HashConst( TF_BOT_SENTRY_BUSTER ): { return true; }
		default: { return false; }
	}

	return false;
}

const wchar_t* CBaseEntity::GetPowerupName( const FNV1A_t ModelHash )
{
	switch ( ModelHash )
	{
		case FNV1A::HashConst( TF_POWERUP_AGILITY ): { return L"Agility"; }
		case FNV1A::HashConst( TF_POWERUP_CRIT ): { return L"Crits"; }
		case FNV1A::HashConst( TF_POWERUP_DEFENSE ): { return L"Defense"; }
		case FNV1A::HashConst( TF_POWERUP_HASTE ): { return L"Haste"; }
		case FNV1A::HashConst( TF_POWERUP_KING ): { return L"King"; }
		case FNV1A::HashConst( TF_POWERUP_KNOCKOUT ): { return L"Knockout"; }
		case FNV1A::HashConst( TF_POWERUP_PLAGUE ): { return L"Plague"; }
		case FNV1A::HashConst( TF_POWERUP_PRECISION ): { return L"Precision"; }
		case FNV1A::HashConst( TF_POWERUP_REFLECT ): { return L"Reflect"; }
		case FNV1A::HashConst( TF_POWERUP_REGEN ): { return L"Regeneration"; }
		case FNV1A::HashConst( TF_POWERUP_RESISTANCE ): { return L"Resistance"; }
		case FNV1A::HashConst( TF_POWERUP_STRENGTH ): { return L"Strength"; }
		case FNV1A::HashConst( TF_POWERUP_SUPERNOVA ): { return L"Supernova"; }
		case FNV1A::HashConst( TF_POWERUP_VAMPIRE ): { return L"Vampire"; }
		default: { return L"Unknown"; }
	}
	return L"Unknown";
}

const wchar_t* CBaseEntity::GetProjectileName( )
{
	switch ( GetClientClass( )->ClassID ) {
		case EClassID::CTFProjectile_Rocket:
		case EClassID::CTFProjectile_SentryRocket:
			return L"Rocket";

		case EClassID::CTFGrenadePipebombProjectile:
			if ( PipebombType( ) == TYPE_STICKY )
				return L"Sticky";
			else
				return L"Pipebomb";

		case EClassID::CTFProjectile_Jar:
		case EClassID::CTFProjectile_ThrowableRepel:
			return L"Jarate";

		case EClassID::CTFProjectile_JarGas:
			return L"Gas Passer";

		case EClassID::CTFProjectile_JarMilk:
		case EClassID::CTFProjectile_ThrowableBreadMonster:
			return L"Mad Milk";

		case EClassID::CTFProjectile_Arrow:
			return L"Arrow";

		case EClassID::CTFProjectile_Flare:
			return L"Flare";

		case EClassID::CTFProjectile_Cleaver:
			return L"Cleaver";

		case EClassID::CTFProjectile_HealingBolt:
			return L"Healing Bolt";

		case EClassID::CTFProjectile_EnergyRing:
			return L"Energy Ring";

		case EClassID::CTFProjectile_EnergyBall:
			return L"Energy Ball";

		case EClassID::CTFStunBall:
			return L"Baseball";

		case EClassID::CTFBall_Ornament:
			return L"Ornament Ball";

		default: return L"";
	}
}

const wchar_t* CBaseEntity::GetIntelStatus( )
{
	if ( !this || !this->IsIntel( ) )
	{
		return L"";
	}

	switch ( this->FlagStatus( ) )
	{
		case TF_FLAGINFO_HOME: return L"AT BASE";
		case TF_FLAGINFO_STOLEN: return L"PICKED UP";
		case TF_FLAGINFO_DROPPED: return L"DROPPED";
		default: return L"";
	}

	return L"";
}

float CBaseEntity::GetIntelReturnTime( )
{
	if ( !this || !this->IsIntel( ) )
		return 0.0f;

	return this->ResetTime( ) - gGlobals->curtime + 0.2f;
}

CBasePlayer* CBaseObject::GetBuilder( )
{
	return gEntityList->GetClientEntityFromHandle( this->Builder( ) )->As<CBasePlayer>( );
}

WeaponData_t CBaseCombatWeapon::GetWeaponData( int m_iWeaponMode )
{
	static auto FN = Memory->FindSignature<CTFWeaponInfo* ( __cdecl* )( int )>( "client.dll", "55 8B EC 66 8B ? ? 66 3B 05 ? ? ? ? 73" );
	return FN( WeaponID( ) )->WeaponData[ m_iWeaponMode ];
}

int CBasePlayer::GetAmmoCount(int iAmmoIndex)
{
	static auto FN = Memory->FindSignature<int(__thiscall*)(CBaseEntity*, int)>("client.dll", "55 8B EC 56 8B 75 08 57 8B F9 83 FE FF 75 08 5F 33 C0 5E 5D C2 04 00");
	return FN(this, iAmmoIndex);
}

CTFWeaponInfo* CBaseCombatWeapon::GetWeaponInfo( )
{
	static auto FN = Memory->FindSignature<CTFWeaponInfo* ( __cdecl* )( int )>( "client.dll", "55 8B EC 66 8B ? ? 66 3B 05 ? ? ? ? 73" );
	if ( !this )
	{
		return nullptr;
	}
	return FN( WeaponID( ) );
}

CBaseEntity* CBaseEntity::GetOwnerEntity( )
{
	return gEntityList->GetClientEntityFromHandle( this->OwnerEntity( ) )->As<CBaseEntity>( );
}

CBasePlayer* CBaseEntity::GetThrower( )
{
	return gEntityList->GetClientEntityFromHandle( this->Thrower( ) )->As<CBasePlayer>( );
}

CBaseEntity* CBaseEntity::MoveChild( )
{
	return gEntityList->GetClientEntity( *reinterpret_cast< int* >( reinterpret_cast< DWORD >( this ) + 0x1B0 ) & 0xFFF )->As<CBaseEntity>( );
}

CBaseEntity* CBaseEntity::NextMovePeer( )
{
	return gEntityList->GetClientEntity( *reinterpret_cast< int* >( reinterpret_cast< DWORD >( this ) + 0x1B4 ) & 0xFFF )->As<CBaseEntity>( );
}

CBaseEntity* CBaseEntity::MoveParent( )
{
	return gEntityList->GetClientEntity( *reinterpret_cast< int* >( reinterpret_cast< DWORD >( this ) + 0x1B8 ) & 0xFFF )->As<CBaseEntity>( );
}

int CBasePlayer::LookupSequence( CStudioHdr* hdr, const char* label )
{
	//static auto FN = reinterpret_cast< int( __cdecl* )( CStudioHdr*, const char* ) >( G->FakeTaunt->LookupSequence );
	static auto FN = Memory->FindSignature<int( __cdecl* )( CStudioHdr*, const char* )>( "client.dll", "55 8B EC 56 8B 75 ? 85 F6 75 ? 33 C0 5E 5D C3 8B CE E8 ? ? ? ? 84 C0 74 ? 53 57 8B" );
	return FN( hdr, label );
}

CSteamID CBasePlayer::GetSteamID( )
{
	PlayerInfo_t pi = { };
	if ( gEngineClient->GetPlayerInfo( this->GetIndex( ), &pi ) )
	{
		CSteamID steamID{ pi.friendsID, 1, k_EUniversePublic, k_EAccountTypeIndividual };
		return steamID;
	}
	return CSteamID( );
}

bool CBaseEntity::IsEntityOnScreen( )
{
	if ( gLocalPlayer->WorldSpaceCenter( ).DistTo( this->WorldSpaceCenter( ) ) > 200.f ) {
		Vector screen = {};
		if ( TF2->WorldToScreen( this->WorldSpaceCenter( ), screen ) ) {
			if ( screen.x < -250 || screen.x > TF2->ScreenWidth + 250
				 || screen.y < -250 || screen.y > TF2->ScreenHeight + 250 )
				return false;
		}
		else return false;
	}

	return true;
}

bool CBasePlayer::IsLocalPlayer( )
{
	return this == gLocalPlayer;
}

bool CBaseCombatWeapon::IsTauntable( )
{
	return this->ItemDefinitionIndex( ) != Scout_s_BonkAtomicPunch && this->ItemDefinitionIndex( ) != Scout_s_FestiveBonk
		&& this->ItemDefinitionIndex( ) != Scout_s_CritaCola && this->WeaponID( ) != TF_WEAPON_JAR 
		&& this->WeaponID( ) != TF_WEAPON_PDA_ENGINEER_BUILD && this->WeaponID( ) != TF_WEAPON_PDA_ENGINEER_DESTROY
		&& this->WeaponID( ) != TF_WEAPON_PDA_SPY_BUILD;
}

const char* CBasePlayer::SequenceLabel( )
{
	if ( this && this->ActiveWeapon( ) )
	{
		switch ( this->ActiveWeapon( )->Slot( ) )
		{
			case SLOT_PRIMARY:
			{
				switch ( this->Class( ) )
				{
					case CLASS_DEMOMAN:
					{
						return "taunt02";
					}

					default:
					{
						switch ( this->ActiveWeapon( )->ItemDefinitionIndex( ) )
						{
							case Scout_m_ForceANature:
							case Scout_m_FestiveForceANature:
							case Scout_m_TheSodaPopper:
							{
								return "taunt01_alt";
							}
							case Soldier_m_TheDirectHit:
							case Soldier_m_TheBeggarsBazooka:
							case Engi_m_FestiveFrontierJustice:
							case Engi_m_TheFrontierJustice:
							{
								return "taunt07";
							}
							case Soldier_m_TheCowMangler5000:
							{
								return "taunt08";
							}
							case Pyro_m_TheRainblower:
							{
								return "taunt_bubbles";
							}
							case Sniper_m_TheHuntsman:
							case Sniper_m_TheFortifiedCompound:
							{
								return "taunt04";
							}
							default: return "taunt01";
						}
						break;
					}
				}
			}

			case SLOT_SECONDARY:
			{
				switch ( this->Class( ) )
				{
					case CLASS_DEMOMAN:
					{
						switch ( this->ActiveWeapon( )->ItemDefinitionIndex( ) )
						{
							case Demoman_s_TheScottishResistance:
							{
								return "taunt08";
							}
							default: return "taunt01";
						}
					}
					default:
					{
						switch ( this->ActiveWeapon( )->ItemDefinitionIndex( ) )
						{
							case Soldier_s_SoldiersShotgun:
							case Soldier_s_ShotgunR:
							case Soldier_s_FestiveShotgun:
							case Soldier_s_TheReserveShooter:
							case Soldier_s_PanicAttack:
							case Heavy_s_Sandvich:
							case Heavy_s_SecondBanana:
							case Heavy_s_Fishcake:
							case Heavy_s_RoboSandvich:
							{
								return "taunt04";
							}
							case Pyro_s_TheScorchShot:
							{
								return "taunt_scorch_shot";
							}
							case Pyro_s_ThermalThruster:
							{
								return "taunt_gaspasser";
							}
							case Engi_s_TheShortCircuit:
							{
								return "taunt03";
							}
							case Medic_s_TheKritzkrieg:
							{
								return "taunt06";
							}
							default: return "taunt02";
						}
					}
				}
				break;
			}

			case SLOT_MELEE:
			{
				switch ( this->ActiveWeapon( )->ItemDefinitionIndex( ) )
				{
					case Scout_t_TheAtomizer:
					case Scout_t_TheSandman:
					case Soldier_t_TheEscapePlan:
					case Soldier_t_TheEqualizer:
					{
						return "taunt05";
					}
					case Pyro_t_HotHand:
					{
						return "taunt02";
					}
					case Demoman_t_UllapoolCaber:
					{
						return "taunt04";
					}
					case Demoman_t_TheClaidheamhMor:
					case Demoman_t_TheEyelander:
					case Demoman_t_FestiveEyelander:
					case Demoman_t_ThePersianPersuader:
					{
						return "taunt09";
					}
					case Heavy_t_FistsofSteel:
					case Heavy_t_GlovesofRunningUrgently:
					case Heavy_t_GlovesofRunningUrgentlyMvM:
					case Heavy_t_FestiveGlovesofRunningUrgently:
					case Heavy_t_TheKillingGlovesofBoxing:
					case Heavy_t_WarriorsSpirit:
					case Heavy_t_TheEvictionNotice:
					{
						return "taunt05";
					}
					case Medic_t_TheUbersaw:
					case Medic_t_FestiveUbersaw:
					{
						return "taunt08";
					}
					default: return "taunt03";
				}
			}

			default:
			{
				if ( this->ActiveWeapon( )->WeaponID( ) == TF_WEAPON_PDA_SPY )
				{
					return "taunt05";
				}

				return "";
			}
		}
	}

	return "";
}

Color CBasePlayer::PlayerESPColor( int ColorScheme )
{
	if ( ColorScheme )
	{
		if ( TeamNum( ) == TEAM_RED )
		{
			return Config->Colors->ESPPlayerRed;
		}
		else if ( TeamNum( ) == TEAM_BLU )
		{
			return Config->Colors->ESPPlayerBlu;
		}
		else
		{
			return { 255, 255, 255 };
		}
	}
	else
	{
		if ( TeamNum( ) == gLocalPlayer->TeamNum( ) )
		{
			return Config->Colors->ESPPlayerTeam;
		}
		else
		{
			return Config->Colors->ESPPlayerEnemy;
		}
	}

	return { 255, 255, 255 };
}

Color CBasePlayer::PlayerModelColor( int ColorScheme )
{
	if ( IsLocalPlayer( ) )
	{
		return Config->Colors->ModelsPlayerLocal;
	}

	if ( IsSteamFriend( ) )
	{
		return Config->Colors->ModelsPlayerFriend;
	}

	if ( IsInvulnerable( ) )
	{
		return Config->Colors->ModelsPlayerUber;
	}

	if ( ColorScheme )
	{
		if ( TeamNum( ) == TEAM_RED )
		{
			return Config->Colors->ModelsPlayerRed;
		}
		else if ( TeamNum( ) == TEAM_BLU )
		{
			return Config->Colors->ModelsPlayerBlu;
		}
		else
		{
			return { 255, 255, 255 };
		}
	}
	else
	{
		if ( TeamNum( ) == gLocalPlayer->TeamNum( ) )
		{
			return Config->Colors->ModelsPlayerTeam;
		}
		else
		{
			return Config->Colors->ModelsPlayerEnemy;
		}
	}

	return { 255, 255, 255 };
}

Color CBasePlayer::PlayerModelsColor( int ColorScheme, bool Occluded )
{
	if ( IsLocalPlayer( ) )
	{
		return Occluded ? Config->Colors->ModelsPlayerLocalOccluded : Config->Colors->ModelsPlayerLocalUnoccluded;
	}

	if ( IsSteamFriend( ) )
	{
		return Occluded ? Config->Colors->ModelsPlayerFriendOccluded : Config->Colors->ModelsPlayerFriendUnoccluded;
	}

	if ( IsInvulnerable( ) )
	{
		return Occluded ? Config->Colors->ModelsPlayerUberOccluded : Config->Colors->ModelsPlayerUberUnoccluded;
	}

	if ( ColorScheme )
	{
		if ( TeamNum( ) == TEAM_RED )
		{
			return Occluded ? Config->Colors->ModelsPlayerRedOccluded : Config->Colors->ModelsPlayerRedUnoccluded;
		}
		else if ( TeamNum( ) == TEAM_BLU )
		{
			return Occluded ? Config->Colors->ModelsPlayerBluOccluded : Config->Colors->ModelsPlayerBluUnoccluded;
		}
		else
		{
			return { 255, 255, 255 };
		}
	}
	else
	{
		if ( TeamNum( ) == gLocalPlayer->TeamNum( ) )
		{
			return Occluded ? Config->Colors->ModelsPlayerTeamOccluded : Config->Colors->ModelsPlayerTeamUnoccluded;
		}
		else
		{
			return Occluded ? Config->Colors->ModelsPlayerEnemyOccluded : Config->Colors->ModelsPlayerEnemyUnoccluded;
		}
	}

	return { 255, 255, 255 };
}

Color CBasePlayer::PlayerOutlinesColor( int ColorScheme )
{
	if ( ColorScheme )
	{
		if ( TeamNum( ) == TEAM_RED )
		{
			return Config->Colors->OutlinesPlayerRed;
		}
		else if ( TeamNum( ) == TEAM_BLU )
		{
			return Config->Colors->OutlinesPlayerBlu;
		}
		else
		{
			return { 255, 255, 255 };
		}
	}
	else
	{
		if ( TeamNum( ) == gLocalPlayer->TeamNum( ) )
		{
			return Config->Colors->OutlinesPlayerTeam;
		}
		else
		{
			return Config->Colors->OutlinesPlayerEnemy;
		}
	}

	return { 255, 255, 255 };
}

Color CBaseObject::BuildingESPColor( int ColorScheme )
{
	if ( ColorScheme )
	{
		if ( TeamNum( ) == TEAM_RED )
		{
			return Config->Colors->ESPBuildingRed;
		}
		else if ( TeamNum( ) == TEAM_BLU )
		{
			return Config->Colors->ESPBuildingBlu;
		}
		else
		{
			return { 255, 255, 255 };
		}
	}
	else
	{
		if ( TeamNum( ) == gLocalPlayer->TeamNum( ) )
		{
			return Config->Colors->ESPBuildingTeam;
		}
		else
		{
			return Config->Colors->ESPBuildingEnemy;
		}
	}

	return { 255, 255, 255 };
}

Color CBaseObject::BuildingModelColor( int ColorScheme )
{
	if ( GetBuilder( )->IsLocalPlayer( ) )
	{
		return Config->Colors->ModelsBuildingLocal;
	}

	if ( GetBuilder( )->IsSteamFriend( ) )
	{
		return Config->Colors->ModelsBuildingFriend;
	}

	if ( ColorScheme )
	{
		if ( TeamNum( ) == TEAM_RED )
		{
			return Config->Colors->ModelsBuildingRed;
		}
		else if ( TeamNum( ) == TEAM_BLU )
		{
			return Config->Colors->ModelsBuildingBlu;
		}
		else
		{
			return { 255, 255, 255 };
		}
	}
	else
	{
		if ( TeamNum( ) == gLocalPlayer->TeamNum( ) )
		{
			return Config->Colors->ModelsBuildingTeam;
		}
		else
		{
			return Config->Colors->ModelsBuildingEnemy;
		}
	}

	return { 255, 255, 255 };
}

Color CBaseObject::BuildingModelsColor( int ColorScheme, bool Occluded )
{
	if ( GetBuilder( )->IsLocalPlayer( ) )
	{
		return Occluded ? Config->Colors->ModelsBuildingLocalOccluded : Config->Colors->ModelsBuildingLocalUnoccluded;
	}

	if ( GetBuilder( )->IsSteamFriend( ) )
	{
		return Occluded ? Config->Colors->ModelsBuildingFriendOccluded : Config->Colors->ModelsBuildingFriendUnoccluded;
	}

	if ( ColorScheme )
	{
		if ( TeamNum( ) == TEAM_RED )
		{
			return Occluded ? Config->Colors->ModelsBuildingRedOccluded : Config->Colors->ModelsBuildingRedUnoccluded;
		}
		else if ( TeamNum( ) == TEAM_BLU )
		{
			return Occluded ? Config->Colors->ModelsBuildingBluOccluded : Config->Colors->ModelsBuildingBluUnoccluded;
		}
		else
		{
			return { 255, 255, 255 };
		}
	}
	else
	{
		if ( TeamNum( ) == gLocalPlayer->TeamNum( ) )
		{
			return Occluded ? Config->Colors->ModelsBuildingTeamOccluded : Config->Colors->ModelsBuildingTeamUnoccluded;
		}
		else
		{
			return Occluded ? Config->Colors->ModelsBuildingEnemyOccluded : Config->Colors->ModelsBuildingEnemyUnoccluded;
		}
	}

	return { 255, 255, 255 };
}

Color CBaseObject::BuildingOutlinesColor( int ColorScheme )
{
	if ( ColorScheme )
	{
		if ( TeamNum( ) == TEAM_RED )
		{
			return Config->Colors->OutlinesBuildingRed;
		}
		else if ( TeamNum( ) == TEAM_BLU )
		{
			return Config->Colors->OutlinesBuildingBlu;
		}
		else
		{
			return { 255, 255, 255 };
		}
	}
	else
	{
		if ( TeamNum( ) == gLocalPlayer->TeamNum( ) )
		{
			return Config->Colors->OutlinesBuildingTeam;
		}
		else
		{
			return Config->Colors->OutlinesBuildingEnemy;
		}
	}

	return { 255, 255, 255 };
}

Color CBaseEntity::GetESPColor( int ColorScheme )
{
	if ( !this )
	{
		return { 255, 255, 255 };
	}

	if ( this->IsPlayer( ) )
	{
		return this->As<CBasePlayer>( )->PlayerESPColor( ColorScheme );
	}

	if ( this->IsBuilding( ) )
	{
		return this->As<CBaseObject>( )->BuildingESPColor( ColorScheme );
	}

	if ( this->IsHealth( FNV1A::Hash( gModelInfo->GetModelName( GetModel( ) ) ) ) )
	{
		return Config->Colors->ESPWorldHealth;
	}

	if ( this->IsAmmo( FNV1A::Hash( gModelInfo->GetModelName( GetModel( ) ) ) ) )
	{
		return Config->Colors->ESPWorldAmmo;
	}

	if ( this->IsBomb( ) )
	{
		return Config->Colors->ESPWorldBomb;
	}
	
	if ( this->IsProjectile( ) || this->IsIntel( ) || this->IsTank( ) || this->IsSentryBuster( ) )
	{
		if ( ColorScheme )
		{
			if ( this->TeamNum( ) == TEAM_RED )
			{
				return Config->Colors->ESPWorldRed;
			}
			else if ( this->TeamNum( ) == TEAM_BLU )
			{
				return Config->Colors->ESPWorldBlu;
			}
			else
			{
				return { 255, 255, 255 };
			}
		}
		else
		{
			if ( this->TeamNum( ) == gLocalPlayer->TeamNum( ) )
			{
				return Config->Colors->ESPWorldTeam;
			}
			else
			{
				return Config->Colors->ESPWorldEnemy;
			}
		}
	}

	return { 255, 255, 255 };
}

Color CBaseEntity::GetModelColor( int ColorScheme )
{
	if ( !this )
	{
		return { 255, 255, 255 };
	}

	if ( this->IsPlayer( ) )
	{
		return this->As<CBasePlayer>( )->PlayerModelColor( ColorScheme );
	}

	if ( this->IsBuilding( ) )
	{
		return this->As<CBaseObject>( )->BuildingModelColor( ColorScheme );
	}

	if ( this->IsHealth( FNV1A::Hash( gModelInfo->GetModelName( GetModel( ) ) ) ) )
	{
		return Config->Colors->ModelsWorldHealth;
	}

	if ( this->IsAmmo( FNV1A::Hash( gModelInfo->GetModelName( GetModel( ) ) ) ) )
	{
		return Config->Colors->ModelsWorldAmmo;
	}

	if ( this->IsBomb( ) )
	{
		return Config->Colors->ModelsWorldBomb;
	}

	if ( this->IsProjectile( ) || this->IsIntel( ) || this->IsTank( ) || this->IsSentryBuster( ) )
	{
		CBasePlayer* Thrower = GetThrower( );
		if ( !Thrower && this->IsProjectile( ) )
		{
			Thrower = GetOwnerEntity( )->As<CBasePlayer>( );
		}

		if ( !Thrower && this->IsProjectile( ) )
		{
			return { 255, 255, 255 };
		}

		if ( Thrower->IsLocalPlayer( ) )
		{
			return Config->Colors->ModelsWorldLocal;
		}

		if ( Thrower->IsSteamFriend( ) )
		{
			return Config->Colors->ModelsWorldFriend;
		}

		if ( ColorScheme )
		{
			if ( this->TeamNum( ) == TEAM_RED )
			{
				return Config->Colors->ModelsWorldRed;
			}
			else if ( this->TeamNum( ) == TEAM_BLU )
			{
				return Config->Colors->ModelsWorldBlu;
			}
			else
			{
				return { 255, 255, 255 };
			}
		}
		else
		{
			if ( this->TeamNum( ) == gLocalPlayer->TeamNum( ) )
			{
				return Config->Colors->ModelsWorldTeam;
			}
			else
			{
				return Config->Colors->ModelsWorldEnemy;
			}
		}
	}

	return { 255, 255, 255 };
}

Color CBaseEntity::GetModelsColor( int ColorScheme, bool Occluded )
{
	if ( !this )
	{
		return { 255, 255, 255 };
	}

	if ( this->IsPlayer( ) )
	{
		return this->As<CBasePlayer>( )->PlayerModelsColor( ColorScheme, Occluded );
	}

	if ( this->IsBuilding( ) )
	{
		return this->As<CBaseObject>( )->BuildingModelsColor( ColorScheme, Occluded );
	}

	if ( this->IsHealth( FNV1A::Hash( gModelInfo->GetModelName( GetModel( ) ) ) ) )
	{
		return Occluded ? Config->Colors->ModelsWorldHealthOccluded : Config->Colors->ModelsWorldHealthUnoccluded;
	}

	if ( this->IsAmmo( FNV1A::Hash( gModelInfo->GetModelName( GetModel( ) ) ) ) )
	{
		return Occluded ? Config->Colors->ModelsWorldAmmoOccluded : Config->Colors->ModelsWorldAmmoUnoccluded;
	}

	if ( this->IsBomb( ) )
	{
		return Occluded ? Config->Colors->ModelsWorldBombOccluded : Config->Colors->ModelsWorldBombUnoccluded;
	}

	if ( this->IsProjectile( ) || this->IsIntel( ) || this->IsTank( ) || this->IsSentryBuster( ) )
	{
		CBasePlayer* Thrower = GetThrower( );
		if ( !Thrower && this->IsProjectile( ) )
		{
			Thrower = GetOwnerEntity( )->As<CBasePlayer>( );
		}

		if ( !Thrower && this->IsProjectile( ) )
		{
			return { 255, 255, 255 };
		}

		if ( Thrower->IsLocalPlayer( ) )
		{
			return Occluded ? Config->Colors->ModelsWorldLocalOccluded : Config->Colors->ModelsWorldLocalUnoccluded;
		}

		if ( Thrower->IsSteamFriend( ) )
		{
			return Occluded ? Config->Colors->ModelsWorldFriendOccluded : Config->Colors->ModelsWorldLocalUnoccluded;
		}

		if ( ColorScheme )
		{
			if ( this->TeamNum( ) == TEAM_RED )
			{
				return Occluded ? Config->Colors->ModelsWorldRedOccluded : Config->Colors->ModelsWorldRedUnoccluded;
			}
			else if ( this->TeamNum( ) == TEAM_BLU )
			{
				return Occluded ? Config->Colors->ModelsWorldBluOccluded : Config->Colors->ModelsWorldBluUnoccluded;
			}
			else
			{
				return { 255, 255, 255 };
			}
		}
		else
		{
			if ( this->TeamNum( ) == gLocalPlayer->TeamNum( ) )
			{
				return Occluded ? Config->Colors->ModelsWorldTeamOccluded : Config->Colors->ModelsWorldTeamUnoccluded;
			}
			else
			{
				return Occluded ? Config->Colors->ModelsWorldEnemyOccluded : Config->Colors->ModelsWorldEnemyUnoccluded;
			}
		}
	}

	return { 255, 255, 255 };
}

Color CBaseEntity::GetOutlinesColor( int ColorScheme )
{
	if ( !this )
	{
		return { 255, 255, 255 };
	}

	if ( this->IsPlayer( ) )
	{
		return this->As<CBasePlayer>( )->PlayerOutlinesColor( ColorScheme );
	}

	if ( this->IsBuilding( ) )
	{
		return this->As<CBaseObject>( )->BuildingOutlinesColor( ColorScheme );
	}

	if ( this->IsHealth( FNV1A::Hash( gModelInfo->GetModelName( GetModel( ) ) ) ) )
	{
		return Config->Colors->OutlinesWorldHealth;
	}

	if ( this->IsAmmo( FNV1A::Hash( gModelInfo->GetModelName( GetModel( ) ) ) ) )
	{
		return Config->Colors->OutlinesWorldAmmo;
	}

	if ( this->IsBomb( ) )
	{
		return Config->Colors->OutlinesWorldBomb;
	}

	if ( this->IsProjectile( ) || this->IsIntel( ) || this->IsTank( ) || this->IsSentryBuster( ) )
	{
		if ( ColorScheme )
		{
			if ( this->TeamNum( ) == TEAM_RED )
			{
				return Config->Colors->OutlinesWorldRed;
			}
			else if ( this->TeamNum( ) == TEAM_BLU )
			{
				return Config->Colors->OutlinesWorldBlu;
			}
			else
			{
				return { 255, 255, 255 };
			}
		}
		else
		{
			if ( this->TeamNum( ) == gLocalPlayer->TeamNum( ) )
			{
				return Config->Colors->OutlinesWorldTeam;
			}
			else
			{
				return Config->Colors->OutlinesWorldEnemy;
			}
		}
	}

	return { 255, 255, 255 };
}

Color CBasePlayer::GetBeamsColor( int ColorScheme )
{
	if ( !this )
	{
		return { 255, 255, 255 };
	}

	if ( this->IsLocalPlayer( ) )
	{
		return Config->Colors->BeamsLocal;
	}

	if ( this->As<CBasePlayer>( )->IsSteamFriend( ) )
	{
		return Config->Colors->BeamsFriend;
	}

	if ( ColorScheme )
	{
		if ( this->TeamNum( ) == TEAM_RED )
		{
			return Config->Colors->BeamsRed;
		}
		else if ( this->TeamNum( ) == TEAM_BLU )
		{
			return Config->Colors->BeamsBlu;
		}
		else
		{
			return { 255, 255, 255 };
		}
	}
	else
	{
		if ( this->TeamNum( ) == gLocalPlayer->TeamNum( ) )
		{
			return Config->Colors->BeamsTeam;
		}
		else
		{
			return Config->Colors->BeamsEnemy;
		}
	}
}