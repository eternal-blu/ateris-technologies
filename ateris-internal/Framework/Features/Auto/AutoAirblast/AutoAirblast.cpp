#include "AutoAirblast.h"
#include "..\..\..\..\Globals.h"

void GatherEntities( )
{
	AutoAirblast->ValidEntities.clear( );

	for ( auto& Ent : gEntities )
	{
		if ( !Ent ) 
		{
			continue;
		}

		if ( Ent->TeamNum( ) == gLocalPlayer->TeamNum( ) )
		{
			continue;
		}

		switch ( Ent->GetClientClass( )->ClassID )
		{
		case EClassID::CTFProjectile_Arrow:
			if ( Ent->EstimateAbsVelocity( ).Length( ) > 0.f && Config->OtherAuto->BlastArrows )
			{
				AutoAirblast->ValidEntities.push_back( Ent );
			}

			break;
			//case EClassID::CTFProjectile_Flare:
		case EClassID::CTFProjectile_Rocket:
		case EClassID::CTFProjectile_SentryRocket:
		case EClassID::CTFGrenadePipebombProjectile:
		case EClassID::CTFStickBomb:
		{
			if ( Ent->PipebombType( ) == TYPE_STICKY )
			{
				if ( Config->OtherAuto->BlastStickies )
				{
					AutoAirblast->ValidEntities.push_back( Ent );
				}
			}
			else
			{
				if ( Config->OtherAuto->BlastExplosive )
				{
					AutoAirblast->ValidEntities.push_back( Ent );
				}
			}
			break;
		}
		default: continue; break;
		}
	}
}

void CAutoAirblast::Run( CUserCmd* pCmd )
{
	G->Aimbot.ReflectVelocity = -1.f;

	if ( !gLocalPlayer || !gLocalPlayer->IsAlive( ) || !gLocalWeapon || gLocalPlayer->Class( ) != CLASS_PYRO || gLocalWeapon->Slot( ) != SLOT_PRIMARY
		|| gLocalWeapon->ItemDefinitionIndex( ) == Pyro_m_ThePhlogistinator )
	{
		return;
	}

	if ( !Config->OtherAuto->AutoAirblast || !Config->OtherAuto->Enabled )
	{
		return;
	}

	GatherEntities( );

	float Latency = gEngineClient->GetNetChannelInfo( )->GetLatency( FLOW_OUTGOING );

	for ( const auto& Entity : ValidEntities )
	{
		Vector Predicted = ( Entity->Origin( ) + ( Entity->EstimateAbsVelocity( ) * Latency ) );

		if ( gLocalPlayer->EyePosition( ).DistTo( Predicted ) <= 182.f
			&& TF2->IsVisible( gLocalPlayer, ( CBasePlayer* ) Entity, gLocalPlayer->EyePosition( ), Predicted ) )
		{
			G->Aimbot.ReflectVelocity = Entity->EstimateAbsVelocity( ).Length( );

			Vector Position = Predicted;
			if ( !G->Aimbot.PredictedPos.IsZero( ) && Config->OtherAuto->AutoAirblastPath )
				Position = G->Aimbot.PredictedPos;

			Vector Angle = gMath->CalcAngle( gLocalPlayer->EyePosition( ), Position );

			pCmd->buttons &= ~( IN_ATTACK );
			pCmd->viewangles = Angle;
			pCmd->buttons |= IN_ATTACK2;
		}
	}
}