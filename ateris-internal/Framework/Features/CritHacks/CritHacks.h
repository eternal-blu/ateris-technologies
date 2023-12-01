#pragma once
#include "../../SDK/TF2/TF2.h"

class CCrithack
{
public:

	struct CCritCommand
	{
		int Seed = 0;
		int CmdNum = 0;
	};

	struct CDamageData
	{
		float RangedDamage = 0.f;
		float CritDamage = 0.f;
		float BoostedDamage = 0.f;
		float CorrectDamage = 0.f;
		float MeleeDamage = 0.f;

		CDamageData( float RangedDamage, 
			float CritDamage, 
			float BoostedDamage, 
			float CorrectDamage, 
			float MeleeDamage )
			: 
			RangedDamage( RangedDamage ), 
			CritDamage( CritDamage ), 
			BoostedDamage( BoostedDamage ), 
			CorrectDamage( CorrectDamage ),
			MeleeDamage( MeleeDamage )
		{
		}

		CDamageData( ) = default;
	};

	struct CriticalData
	{
		CDamageData* Damage = new CDamageData( );
		float UnFixedChance = 0.f;
		float CorrectCritChance = 0.f;

		bool Forcing = false;
		bool CritBan = false;
		bool CanCrit = false;

		float CritCost = 0.f;

		std::unordered_map<int, std::vector<CCritCommand>> Safe = {};
		std::unordered_map<int, std::vector<CCritCommand>> Crit = {};

		float Multiplier = 0.f;
	};

	struct PlayerStatus
	{
		bool Updated = false;
		int Health = 0;
		int Class = 0;
	};

	CriticalData* Data = new CriticalData;

	void Reset( );
	float BucketCap( );
	void OnEvent( IGameEvent* Event );
	void CanCrit( int cmd );
	void HandleCanFireRandomCriticalShot( float Chance );
	bool IsAttackCriticalHandler( );
	void Fill( CUserCmd* pCmd );
	float DamageTillCrit( );
	bool CanForce( );
	void Resync( );
	void UpdatePlayers( );
	bool ShouldForceCrit( );
	void Force( CUserCmd* pCmd );
	float CalculateCost( CBaseCombatWeapon* Weapon );
	//float GetWeaponMaxMult( CBaseCombatWeapon* Weapon );
	int PotentialCrits( );
	int DecryptEncryptSeed( CBaseCombatWeapon* Weapon, const int seed );
	bool IsPureCritCommand( const int command_number, const int range, const bool lower_than );

	int WishRandomSeed = -1;
	bool ShouldRecalculate = true;
	bool HasFiredMelee = false;

	std::array<PlayerStatus, 64> PlayersStatus;

private:
	void ClearCommandsIfNeeded( CUserCmd* pCmd );
};

inline CCrithack* CritHack = new CCrithack;