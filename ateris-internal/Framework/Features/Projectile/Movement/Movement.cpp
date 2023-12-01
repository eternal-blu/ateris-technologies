#include "Movement.h"
#include "../../LagCompensation/LagComp.h"
#include "../../../../Globals.h"

static CUserCmd DummyCmd = {};

class CPlayerDataBackup
{
public:
	Vector m_vecOrigin = {};
	Vector m_vecVelocity = {};
	Vector m_vecViewOffset = {};
	unsigned int m_hGroundEntity = 0;
	int m_fFlags = 0;
	float m_flDucktime = 0.0f;
	float m_flDuckJumpTime = 0.0f;
	bool m_bDucked = false;
	bool m_bDucking = false;
	bool m_bInDuckJump = false;
	float m_flModelScale = 0.0f;

	int m_nButtons = 0;
	float m_flMaxspeed = 0.0f;
	float m_flFallVelocity = 0.0f;
	int m_surfaceProps = 0;
	void* m_pSurfaceData = nullptr;
	float m_surfaceFriction = 0.0f;
	char m_chTextureType = 0;
	int m_nPlayerCond = 0;
	int m_nPlayerCondEx = 0;
	int m_nPlayerCondEx2 = 0;
	int m_nPlayerCondEx3 = 0;
	int m_nPlayerCondEx4 = 0;
	int _condition_bits = 0;

	inline void Store( CBasePlayer* pPlayer )
	{
		m_vecOrigin = pPlayer->Origin( );
		m_vecVelocity = pPlayer->Velocity( );
		m_vecViewOffset = pPlayer->ViewOffset( );
		m_hGroundEntity = pPlayer->GroundEntity( );
		m_fFlags = pPlayer->Flags( );
		m_flDucktime = pPlayer->Ducktime( );
		m_flDuckJumpTime = pPlayer->DuckJumpTime( );
		m_bDucked = pPlayer->Ducked( );
		m_bDucking = pPlayer->Ducking( );
		m_bInDuckJump = pPlayer->InDuckJump( );
		m_flModelScale = pPlayer->ModelScale( );

		m_nButtons = pPlayer->Buttons( );
		m_flMaxspeed = pPlayer->MaxSpeed( );
		m_flFallVelocity = pPlayer->FallVelocity( );
		m_surfaceProps = pPlayer->SurfaceProps( );
		m_pSurfaceData = pPlayer->pSurfaceData( );
		m_surfaceFriction = pPlayer->SurfaceFriction( );
		m_chTextureType = pPlayer->CHTextureType( );
		m_nPlayerCond = pPlayer->PlayerCond( );
		m_nPlayerCondEx = pPlayer->PlayerCondEx( );
		m_nPlayerCondEx2 = pPlayer->PlayerCondEx2( );
		m_nPlayerCondEx3 = pPlayer->PlayerCondEx3( );
		m_nPlayerCondEx4 = pPlayer->PlayerCondEx4( );
		_condition_bits = pPlayer->condition_bits( );
	}

	inline void Restore( CBasePlayer* pPlayer )
	{
		pPlayer->Origin( ) = m_vecOrigin;
		pPlayer->Velocity( ) = m_vecVelocity;
		pPlayer->ViewOffset( ) = m_vecViewOffset;
		pPlayer->GroundEntity( ) = m_hGroundEntity;
		pPlayer->Flags( ) = m_fFlags;
		pPlayer->Ducktime( ) = m_flDucktime;
		pPlayer->DuckJumpTime( ) = m_flDuckJumpTime;
		pPlayer->Ducked( ) = m_bDucked;
		pPlayer->Ducking( ) = m_bDucking;
		pPlayer->InDuckJump( ) = m_bInDuckJump;
		pPlayer->ModelScale( ) = m_flModelScale;

		pPlayer->Buttons( ) = m_nButtons;
		pPlayer->m_flMaxspeed( ) = m_flMaxspeed;
		pPlayer->FallVelocity( ) = m_flFallVelocity;
		pPlayer->SurfaceProps( ) = m_surfaceProps;
		pPlayer->pSurfaceData( ) = m_pSurfaceData;
		pPlayer->SurfaceFriction( ) = m_surfaceFriction;
		pPlayer->CHTextureType( ) = m_chTextureType;
		pPlayer->PlayerCond( ) = m_nPlayerCond;
		pPlayer->PlayerCondEx( ) = m_nPlayerCondEx;
		pPlayer->PlayerCondEx2( ) = m_nPlayerCondEx2;
		pPlayer->PlayerCondEx3( ) = m_nPlayerCondEx3;
		pPlayer->PlayerCondEx4( ) = m_nPlayerCondEx4;
		pPlayer->condition_bits( ) = _condition_bits;
	}
};

static CPlayerDataBackup PlayerDataBackup;

void CMovementSim::SetupMoveData( CBasePlayer* pPlayer, CMoveData* pMoveData )
{
	if ( !pPlayer || !pMoveData )
		return;

	pMoveData->FirstRunOfFunctions = false;
	pMoveData->bGameCodeMovedPlayer = false;
	pMoveData->PlayerHandle = pPlayer->As<IHandleEntity>( )->GetRefEHandle( );
	pMoveData->Velocity = pPlayer->Velocity( );
	pMoveData->AbsOrigin = pPlayer->Origin( );
	pMoveData->MaxSpeed = pPlayer->MaxSpeed( );

	if ( PlayerDataBackup.m_fFlags & FL_DUCKING && PlayerDataBackup.m_fFlags & FL_ONGROUND )
		pMoveData->MaxSpeed *= 0.3333f;

	pMoveData->ViewAngles = { 0.0f, gMath->VelocityToAngles( pMoveData->Velocity ).y, 0.0f };

	Vector vForward = {}, vRight = {};

	gMath->AngleVectors( pMoveData->ViewAngles, &vForward, &vRight, nullptr );

	pMoveData->ForwardMove = ( pMoveData->Velocity.y - vRight.y / vRight.x * pMoveData->Velocity.x ) / ( vForward.y - vRight.y / vRight.x * vForward.x );
	pMoveData->SideMove = ( pMoveData->Velocity.x - vForward.x * pMoveData->ForwardMove ) / vRight.x;

	pMoveData->Angles = pMoveData->ViewAngles;
	pMoveData->OldAngles = pMoveData->Angles;

	if ( pPlayer->hConstraintEntity( ) )
		pMoveData->ConstraintCenter = pPlayer->hConstraintEntity( ).Get( )->GetAbsOrigin( );
	else pMoveData->ConstraintCenter = pPlayer->ConstraintCenter( );

	pMoveData->ConstraintRadius = pPlayer->ConstraintRadius( );
	pMoveData->ConstraintWidth = pPlayer->ConstraintWidth( );
	pMoveData->ConstraintSpeedFactor = pPlayer->ConstraintSpeedFactor( );
}

bool CMovementSim::Init( CBasePlayer* pPlayer )
{
	if ( !gGameMovement || !pPlayer || !pPlayer->IsAlive( ) || !pPlayer->IsPlayer( ) )
		return false;

	//set player
	Player = pPlayer;

	//set current command
	Player->CurrentCommand( ) = &DummyCmd; 

	//store player's data
	PlayerDataBackup.Store( Player );

	//store vars
	OldInPrediction = gPrediction->bInPrediction;
	OldFirstTimePrediction = gPrediction->FirstTimePredicted;
	OldFrameTime = gGlobals->frametime;

	//the hacks that make it work
	{
		if ( pPlayer->Flags( ) & FL_DUCKING )
		{
			pPlayer->Flags( ) &= ~FL_DUCKING; //breaks origin's z if FL_DUCKING is not removed
			pPlayer->Ducked( ) = true; //(mins/maxs will be fine when ducking as long as m_bDucked is true)
			pPlayer->Ducktime( ) = 0.0f;
			pPlayer->DuckJumpTime( ) = 0.0f;
			pPlayer->Ducking( ) = false;
			pPlayer->InDuckJump( ) = false;
		}

		if ( pPlayer != gLocalPlayer )
			pPlayer->GroundEntity( ) = -1; //without this nonlocal players get snapped to the floor

		pPlayer->ModelScale( ) -= 0.03125f; //fixes issues with corners

		if ( pPlayer->Flags( ) & FL_ONGROUND )
		{
			pPlayer->Origin( ).z += 0.03125f * 3.f; //to prevent getting stuck in the ground
			Directions[ Player->GetIndex( ) ].GroundState = true;
		}
		else
		{
			Directions[ Player->GetIndex( ) ].GroundState = false;
		}

		if ( pPlayer->FallVelocity( ) > 0.f )
		{
			Directions[ Player->GetIndex( ) ].WasFalling = true;
		}
		else
		{
			Directions[ Player->GetIndex( ) ].WasFalling = false;
		}

		//for some reason if xy vel is zero it doesn't predict
		if ( fabsf( pPlayer->EstimateAbsVelocity( ).x ) < 0.01f )
			pPlayer->Velocity( ).x = 0.015f;

		if ( fabsf( pPlayer->EstimateAbsVelocity( ).y ) < 0.01f )
			pPlayer->Velocity( ).y = 0.015f;

		if ( pPlayer->IsOnGround( ) || pPlayer->GroundEntity( ) )
			pPlayer->Velocity( ).z = 0.0f;

	}

	//setup move data
	
	SetupMoveData( pPlayer, &MoveData );
	ComputeAverageWalletTheft( pPlayer, &MoveData );

	return true;
}

void CMovementSim::Restore( )
{
	if ( !Player )
		return;

	Player->CurrentCommand( ) = nullptr;

	PlayerDataBackup.Restore( Player );

	gPrediction->bInPrediction = OldInPrediction;
	gPrediction->FirstTimePredicted = OldFirstTimePrediction;
	gGlobals->frametime = OldFrameTime;

	Player = nullptr;

	memset( &MoveData, 0, sizeof( CMoveData ) );
	memset( &PlayerDataBackup, 0, sizeof( CPlayerDataBackup ) );
}

void CMovementSim::JumpPrediction( CBasePlayer* Player, CMoveData* mData )
{
	auto& Records = Directions[ Player->GetIndex( ) ];
	if ( !Records.GroundState && Player->IsOnGround( ) )
	{
		mData->Buttons |= IN_JUMP;
		mData->OldButtons &= ~IN_JUMP;
		mData->UpMove = 320.f;
	}
}

// fuck jenga and his retarded ass fucking dogshit
// paster "Mad?" cant paste my spoonfed code
// hires me to implement it
// lol noobik

// This is considered a fraction per movement
// 1.f = sv_airaccelerate/sv_fricition
// 0.f = 0 friction
constexpr auto Marian = .64f;

void CMovementSim::ComputeAverageWalletTheft( CBasePlayer* Player, CMoveData* mData )
{
	Iacob = 0.0f;

	if ( !Config->Aimbot->Projectile->PredictStrafing )
		return;

	if ( !Player || !Player->IsAlive( ) || Player->IsDormant( ) ||
		Player->Velocity( ).Length2D( ) <= ( Player->MaxSpeed( ) * 0.2f ) )
		return;

	auto& AndrewTate = Directions[ Player->GetIndex( ) ];

	// We don't have enough records to form an average AAAAA
	if ( AndrewTate.Vec.size( ) < 2 )
		return;

	auto& PayPal = AndrewTate.Vec;
	std::vector< float > Pentagon = { };

	for ( auto n = 1u; n < PayPal.size( ); n++ )
		Pentagon.push_back( PayPal[ n - 1u ] - PayPal[ n ] );

	Iacob = Pentagon.front( );

	for ( const auto& Paste : Pentagon )
		Iacob = Marian * Paste + ( 1.0f - Marian ) * Iacob;

	const auto AUTISM_THRESHOLD = 360.f * gGlobals->interval_per_tick;
	Iacob = std::clamp( Iacob, -AUTISM_THRESHOLD, AUTISM_THRESHOLD );
}

Vector CMovementSim::RunTick( )
{
	if ( !gTFGameMovement || !Player || !Player->IsAlive( ) || !Player->IsPlayer( ) )
		return {};

	if ( Config->Aimbot->Projectile->PredictJumping )
		JumpPrediction( Player, &MoveData );

	//make sure frametime and prediction vars are right
	gPrediction->bInPrediction = true;
	gPrediction->FirstTimePredicted = false;
	gGlobals->frametime = gPrediction->EnginePaused ? 0.0f : gGlobals->interval_per_tick;

	MoveData.ViewAngles.y += Iacob;
	MoveData.ViewAngles.y = gMath->NormalizeAngle( MoveData.ViewAngles.y );

	if ( !( Player->Flags( ) & FL_ONGROUND ) )
	{
		MoveData.ForwardMove = 0.f;
		MoveData.SideMove = gMath->ReMapValClamped( 
			MoveData.ViewAngles.y, -35.f , 
			35.f, -450.f, 450.f );
	}

	bIsRunning = true;
	gTFGameMovement->ProcessMovement( Player, &MoveData );
	bIsRunning = false;
	
	return this->GetOrigin( );
}

void CMovementSim::FillDirections( int PlrIndex )
{
	if ( !gLocalPlayer )
		return;

	auto& Records = Directions[ PlrIndex ];
	CBasePlayer* Player = gEntityList->GetClientEntity( PlrIndex )->As<CBasePlayer>( );
	bool Falling = Player->FallVelocity( ) > 0.f;

	// micro optimization part 3
	VelocityRecords[ PlrIndex ] =
	{
		Player->SimulationTime( ),
		Player->Flags( ),
		Player->Origin( )
	};

	if ( Player->IsDormant( ) || !Player->IsAlive( ) )
	{
		if ( !Records.Vec.empty( ) )
			Records.Vec.clear( );

		return;
	}

	Vector Velo = Player->EstimateAbsVelocity( );
	Records.Vec.push_front( gMath->VelocityToAngles( Velo ).y );

	// todo: fuck with this more
	int RecordsMax = std::ceil( gMath->ReMapValClamped( abs(
		Player->Velocity( ).Length( ) ), 0.f, Player->MaxSpeed( ),
		28, 6 ) );

	while ( Records.Vec.size( ) > ( size_t ) RecordsMax )
		Records.Vec.pop_back( );
}