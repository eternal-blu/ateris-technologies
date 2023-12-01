#pragma once
//jod bless mfed

class CGameRulesProxy
{
public:
};

class CTeamplayRoundBasedRulesProxy : public CGameRulesProxy
{
public:
	NETVAR( RoundState, int, "CTeamplayRoundBasedRulesProxy", "m_iRoundState" ); //68
	NETVAR( InWaitingForPlayers, bool, "CTeamplayRoundBasedRulesProxy", "m_bInWaitingForPlayers" ); //84
	NETVAR( WinningTeam, int, "CTeamplayRoundBasedRulesProxy", "m_iWinningTeam" ); //76
	NETVAR( InOvertime, bool, "CTeamplayRoundBasedRulesProxy", "m_bInOvertime" ); //72
	NETVAR( InSetup, bool, "CTeamplayRoundBasedRulesProxy", "m_bInSetup" ); //73
	NETVAR( SwitchedTeamsThisRound, bool, "CTeamplayRoundBasedRulesProxy", "m_bSwitchedTeamsThisRound" ); //74
	NETVAR( AwaitingReadyRestart, bool, "CTeamplayRoundBasedRulesProxy", "m_bAwaitingReadyRestart" ); //85
	NETVAR( RestartRoundTime, float, "CTeamplayRoundBasedRulesProxy", "m_flRestartRoundTime" ); //88
	NETVAR( MapResetTime, float, "CTeamplayRoundBasedRulesProxy", "m_flMapResetTime" ); //92
	NETVAR( RoundsPlayed, int, "CTeamplayRoundBasedRulesProxy", "m_nRoundsPlayed" ); //292
	NETVAR( NextRespawnWave, void*, "CTeamplayRoundBasedRulesProxy", "m_flNextRespawnWave" ); //96
	NETVAR( TeamRespawnWaveTimes, void*, "CTeamplayRoundBasedRulesProxy", "m_TeamRespawnWaveTimes" ); //304
	NETVAR( TeamReady, void*, "CTeamplayRoundBasedRulesProxy", "m_bTeamReady" ); //224
	NETVAR( StopWatch, bool, "CTeamplayRoundBasedRulesProxy", "m_bStopWatch" ); //256
	NETVAR( MultipleTrains, bool, "CTeamplayRoundBasedRulesProxy", "m_bMultipleTrains" ); //257
	NETVAR( PlayerReady, void*, "CTeamplayRoundBasedRulesProxy", "m_bPlayerReady" ); //258
	NETVAR( CheatsEnabledDuringLevel, bool, "CTeamplayRoundBasedRulesProxy", "m_bCheatsEnabledDuringLevel" ); //291
	NETVAR( CountdownTime, float, "CTeamplayRoundBasedRulesProxy", "m_flCountdownTime" ); //296
	NETVAR( StateTransitionTime, float, "CTeamplayRoundBasedRulesProxy", "m_flStateTransitionTime" ); //300
};

class CTFGameRulesProxy : public CTeamplayRoundBasedRulesProxy
{
public:
	NETVAR( GameType, int, "CTFGameRulesProxy", "m_nGameType" ); //492
	NETVAR( StopWatchState, int, "CTFGameRulesProxy", "m_nStopWatchState" ); //496
	NETVAR( TeamGoalStringRed, const char*, "CTFGameRulesProxy", "m_pszTeamGoalStringRed" ); //500
	NETVAR( TeamGoalStringBlue, const char*, "CTFGameRulesProxy", "m_pszTeamGoalStringBlue" ); //756
	NETVAR( CapturePointEnableTime, float, "CTFGameRulesProxy", "m_flCapturePointEnableTime" ); //1012
	NETVAR( HudType, int, "CTFGameRulesProxy", "m_nHudType" ); //1020
	NETVAR( IsInTraining, bool, "CTFGameRulesProxy", "m_bIsInTraining" ); //1024
	NETVAR( AllowTrainingAchievements, bool, "CTFGameRulesProxy", "m_bAllowTrainingAchievements" ); //1025
	NETVAR( IsWaitingForTrainingContinue, bool, "CTFGameRulesProxy", "m_bIsWaitingForTrainingContinue" ); //1026
	NETVAR( IsTrainingHUDVisible, bool, "CTFGameRulesProxy", "m_bIsTrainingHUDVisible" ); //1027
	NETVAR( IsInItemTestingMode, bool, "CTFGameRulesProxy", "m_bIsInItemTestingMode" ); //1028
	NETVAR( BonusLogic, int, "CTFGameRulesProxy", "m_hBonusLogic" ); //1044
	NETVAR( PlayingKoth, bool, "CTFGameRulesProxy", "m_bPlayingKoth" ); //1048
	NETVAR( PlayingMedieval, bool, "CTFGameRulesProxy", "m_bPlayingMedieval" ); //1051
	NETVAR( PlayingHybrid_CTF_CP, bool, "CTFGameRulesProxy", "m_bPlayingHybrid_CTF_CP" ); //1052
	NETVAR( PlayingSpecialDeliveryMode, bool, "CTFGameRulesProxy", "m_bPlayingSpecialDeliveryMode" ); //1053
	NETVAR( PlayingRobotDestructionMode, bool, "CTFGameRulesProxy", "m_bPlayingRobotDestructionMode" ); //1050
	NETVAR( RedKothTimer, int, "CTFGameRulesProxy", "m_hRedKothTimer" ); //1076
	NETVAR( BlueKothTimer, int, "CTFGameRulesProxy", "m_hBlueKothTimer" ); //1080
	NETVAR( MapHolidayType, int, "CTFGameRulesProxy", "m_nMapHolidayType" ); //1084
	NETVAR( Handle, int, "CTFGameRulesProxy", "m_itHandle" ); //1844
	NETVAR( PlayingMannVsMachine, bool, "CTFGameRulesProxy", "m_bPlayingMannVsMachine" ); //1054
	NETVAR( BirthdayPlayer, int, "CTFGameRulesProxy", "m_hBirthdayPlayer" ); //1848
	NETVAR( BossHealth, int, "CTFGameRulesProxy", "m_nBossHealth" ); //1832
	NETVAR( MaxBossHealth, int, "CTFGameRulesProxy", "m_nMaxBossHealth" ); //1836
	NETVAR( BossNormalizedTravelDistance, int, "CTFGameRulesProxy", "m_fBossNormalizedTravelDistance" ); //1840
	NETVAR( MannVsMachineAlarmStatus, bool, "CTFGameRulesProxy", "m_bMannVsMachineAlarmStatus" ); //1055
	NETVAR( HaveMinPlayersToEnableReady, bool, "CTFGameRulesProxy", "m_bHaveMinPlayersToEnableReady" ); //1056
	NETVAR( BountyModeEnabled, bool, "CTFGameRulesProxy", "m_bBountyModeEnabled" ); //1057
	NETVAR( HalloweenEffect, int, "CTFGameRulesProxy", "m_nHalloweenEffect" ); //1852
	NETVAR( HalloweenEffectStartTime, float, "CTFGameRulesProxy", "m_fHalloweenEffectStartTime" ); //1856
	NETVAR( HalloweenEffectDuration, float, "CTFGameRulesProxy", "m_fHalloweenEffectDuration" ); //1860
	NETVAR( HalloweenScenario, int, "CTFGameRulesProxy", "m_halloweenScenario" ); //1864
	NETVAR( HelltowerPlayersInHell, bool, "CTFGameRulesProxy", "m_bHelltowerPlayersInHell" ); //1069
	NETVAR( IsUsingSpells, bool, "CTFGameRulesProxy", "m_bIsUsingSpells" ); //1070
	NETVAR( CompetitiveMode, bool, "CTFGameRulesProxy", "m_bCompetitiveMode" ); //1058
	NETVAR( MatchGroupType, int, "CTFGameRulesProxy", "m_nMatchGroupType" ); //1064
	NETVAR( MatchEnded, bool, "CTFGameRulesProxy", "m_bMatchEnded" ); //1068
	NETVAR( PowerupMode, bool, "CTFGameRulesProxy", "m_bPowerupMode" ); //1049
	NETVAR( CustomUpgradesFile, const char*, "CTFGameRulesProxy", "m_pszCustomUpgradesFile" ); //1088
	NETVAR( TruceActive, bool, "CTFGameRulesProxy", "m_bTruceActive" ); //1071
	NETVAR( ShowMatchSummary, bool, "CTFGameRulesProxy", "m_bShowMatchSummary" ); //1348
	NETVAR( MapHasMatchSummaryStage, bool, "CTFGameRulesProxy", "m_bMapHasMatchSummaryStage" ); //1349
	NETVAR( PlayersAreOnMatchSummaryStage, bool, "CTFGameRulesProxy", "m_bPlayersAreOnMatchSummaryStage" ); //1350
	NETVAR( StopWatchWinner, bool, "CTFGameRulesProxy", "m_bStopWatchWinner" ); //1351
	NETVAR( PlayerWantsRematch, void*, "CTFGameRulesProxy", "m_ePlayerWantsRematch" ); //1352
	NETVAR( RematchState, int, "CTFGameRulesProxy", "m_eRematchState" ); //1488
	NETVAR( NextMapVoteOptions, void*, "CTFGameRulesProxy", "m_nNextMapVoteOptions" ); //1492
	NETVAR( ForceUpgrades, int, "CTFGameRulesProxy", "m_nForceUpgrades" ); //1868
	NETVAR( ForceEscortPushLogic, int, "CTFGameRulesProxy", "m_nForceEscortPushLogic" ); //1872
	NETVAR( RopesHolidayLightsAllowed, bool, "CTFGameRulesProxy", "m_bRopesHolidayLightsAllowed" ); //1073

	inline bool IsPlayerReady( int playerIndex )
	{
		if ( playerIndex > 101 )
			return false;

		constexpr auto offset = 258;
		bool* ReadyStatus = reinterpret_cast< bool* >( reinterpret_cast< uintptr_t >( this ) + offset );
		if ( !ReadyStatus )
			return false;

		return ReadyStatus[ playerIndex ];
	}

	inline gamerules_roundstate_t GetState( )
	{
		return *reinterpret_cast< gamerules_roundstate_t* >( RoundState( ) );
	}
};

class CTFGameRules
{
public:
	CTFGameRulesProxy* GetProxy( )
	{
		return reinterpret_cast< CTFGameRulesProxy* >( this );
	}

	CTFGameRules* Get( )
	{
		return **Memory->FindSignature<CTFGameRules***, 2u>( "client.dll", "8B 0D ? ? ? ? 56 57 8B 01 8B 40 ? FF D0 84 C0" );
	}
};