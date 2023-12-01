#include "AutoDetonate.h"

void CAutoDetonation::Run(CUserCmd* pCmd)
{
	if (!gLocalPlayer || !gLocalPlayer->IsAlive() || !gLocalWeapon || !Config->OtherAuto->Enabled || !Config->OtherAuto->AutoDetonation)
		return;

	if (!TF2->IsProjectileWeapon(gLocalWeapon))
		return;

	if (gLocalPlayer->Class() != CLASS_DEMOMAN && gLocalPlayer->Class() != CLASS_PYRO)
		return;

	bool Detonate = false;
	bool ShouldAim = false;
	Vector StickyPos = {};

	for (auto& Sticky : gProjectiles)
	{
		if ( Sticky->GetClientClass()->ClassID != EClassID::CTFProjectile_Flare &&
			( Sticky->GetClientClass( )->ClassID != EClassID::CTFGrenadePipebombProjectile 
			|| Sticky->PipebombType( ) != TYPE_STICKY ))
			continue;

		if ( Sticky->GetThrower() != gLocalPlayer )
			continue;

		if ( Sticky->StickyType( ) == TF_GL_MODE_REMOTE_DETONATE_PRACTICE )
			continue;

		if ( gGlobals->curtime < Sticky->CreationTime( ) + TF2->AttribHookFloat( 0.8f, "sticky_arm_time", gLocalPlayer, 0x0, true ) )
			continue;

		CBaseEntity* Entity = nullptr;
		for (CEntitySphereQuery Sphere(Sticky->WorldSpaceCenter(), Config->OtherAuto->AutoDetonationMode == 0 ? 80.f : ( Sticky->Touched( ) ? 120.f : 100.f ) ); (Entity = Sphere.CurrentEntity()) != nullptr; Sphere.NextEntity())
		{
			if (!Entity || Entity == gLocalPlayer || Entity->IsDormant() || Entity->TeamNum() == gLocalPlayer->TeamNum())
				continue;

			if (!Entity->IsBuilding() &&
				!Entity->IsPlayer() &&
				!(Entity->GetClientClass()->ClassID == EClassID::CTFGrenadePipebombProjectile && Entity->PipebombType() == TYPE_STICKY))
				continue;

			if (Entity->IsBuilding() && !Config->OtherAuto->TargetBuildings)
				continue;

			if (Entity->IsPlayer() && !Config->OtherAuto->TargetPlayers)
				continue;

			if ( ( Entity->GetClientClass( )->ClassID == EClassID::CTFGrenadePipebombProjectile
				&& Entity->PipebombType( ) == TYPE_STICKY ) && 
				( !Config->OtherAuto->TargetStickies || gLocalWeapon->ItemDefinitionIndex() != Demoman_s_TheQuickiebombLauncher) )
				continue;

			if (Entity->IsPlayer())
			{
				if (!Entity->As<CBasePlayer>()->IsAlive())
					continue;

				if (Entity->As<CBasePlayer>()->IsSteamFriend() && !Config->OtherAuto->TargetFriends)
					continue;

				if (Entity->As<CBasePlayer>()->IsBonked() && !Config->OtherAuto->TargetBonk)
					continue;

				if (Entity->As<CBasePlayer>()->IsInvulnerable( ) && !Config->OtherAuto->TargetUber )
					continue;

				if (gLocalPlayer->Class() == CLASS_DEMOMAN ?
					Entity->As<CBasePlayer>()->IsExplosiveCharged() : Entity->As<CBasePlayer>()->IsFireCharged() && !Config->OtherAuto->TargetVacc)
					continue;
			}

			if (Entity->IsBuilding())
			{
				if (!Entity->As<CBasePlayer>()->IsAlive())
					continue;

				if (Entity->As<CBaseObject>()->GetBuilder() && Entity->As<CBaseObject>()->GetBuilder()->IsSteamFriend() && !Config->OtherAuto->TargetFriends)
					continue;
			}

			if (!TF2->IsVisible(Sticky->As<CBasePlayer>(),
				Entity->As<CBasePlayer>(), Sticky->WorldSpaceCenter(), Entity->WorldSpaceCenter(), MASK_SOLID & ~CONTENTS_GRATE))
				continue;

			if ( Sticky->DefensiveBomb( ) )
			{
				ShouldAim = true;
				StickyPos = Sticky->Origin( );
			}

			Detonate = true;
			break;
		}

		if (Detonate)
			break;
	}

	if (Detonate)
	{
		pCmd->buttons |= IN_ATTACK2;
		if ( ShouldAim )
		{
			pCmd->viewangles = gMath->CalcAngle( gLocalPlayer->EyePosition( ), StickyPos );
		}
	}
}