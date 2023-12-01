#include "Outlines.h"
#include "../../../Globals.h"

bool COutlines::IsRendering( )
{
	return IsRendering_;
}

bool COutlines::IsUsedMaterial( IMaterial* material )
{
	return material == MaterialGlowColor;
}

bool COutlines::IsEntityRendered( CBaseEntity* entity )
{
	if ( !entity || entity->IsDormant( ) )
		return false;

	return RenderedEnts.find( entity ) != RenderedEnts.end( );
}

void COutlines::InitMaterials( )
{
	if ( !MaterialGlowColor )
	{
		MaterialGlowColor = gMaterialSystem->FindMaterial( ( "dev/glow_color" ), "Other textures" );
		MaterialGlowColor->IncrementReferenceCount( );
	}

	if ( !TextureRTFullFrame )
	{
		TextureRTFullFrame = gMaterialSystem->FindTexture( ( "_rt_FullFrameFB" ), "RenderTargets" );
		TextureRTFullFrame->IncrementReferenceCount( );
	}

	if ( !RenderBuffer1 )
	{
		RenderBuffer1 = gMaterialSystem->CreateNamedRenderTargetTextureEx(
			( "glow_buffer_1" ), TextureRTFullFrame->GetActualWidth( ), TextureRTFullFrame->GetActualHeight( ),
			RT_SIZE_LITERAL, IMAGE_FORMAT_RGB888, MATERIAL_RT_DEPTH_SHARED,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA, CREATERENDERTARGETFLAGS_HDR );
		RenderBuffer1->IncrementReferenceCount( );
	}

	if ( !RenderBuffer2 )
	{
		RenderBuffer2 = gMaterialSystem->CreateNamedRenderTargetTextureEx(
			( "glow_buffer_2" ), TextureRTFullFrame->GetActualWidth( ), TextureRTFullFrame->GetActualHeight( ),
			RT_SIZE_LITERAL, IMAGE_FORMAT_RGB888, MATERIAL_RT_DEPTH_SHARED,
			TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA, CREATERENDERTARGETFLAGS_HDR );
		RenderBuffer2->IncrementReferenceCount( );
	}

	if ( !MaterialBlurX )
	{
		if ( !MaterialBlurXVMT )
		{
			MaterialBlurXVMT = new KeyValues( "BlurFilterX" );
			MaterialBlurXVMT->SetString( "$basetexture", "glow_buffer_1" );
			MaterialBlurXVMT->SetString( "$wireframe", "0" );
		}
		MaterialBlurX = gMaterialSystem->CreateMaterial( "MaterialBlurX", MaterialBlurXVMT );
	}

	if ( !MaterialBlurY )
	{
		if ( !MaterialBlurYVMT )
		{
			MaterialBlurYVMT = new KeyValues( "BlurFilterY" );
			MaterialBlurYVMT->SetString( "$basetexture", "glow_buffer_2" );
			MaterialBlurYVMT->SetString( "$wireframe", "0" );
		}
		MaterialBlurY = gMaterialSystem->CreateMaterial( "MaterialBlurY", MaterialBlurYVMT );
	}

	if ( !MaterialHalo )
	{
		if ( !MaterialHaloVMT )
		{
			MaterialHaloVMT = new KeyValues( "UnlitGeneric" );
			MaterialHaloVMT->SetString( "$basetexture", "glow_buffer_1" );
			MaterialHaloVMT->SetString( "$wireframe", "0" );
			MaterialHaloVMT->SetString( "$additive", "1" );
		}
		MaterialHalo = gMaterialSystem->CreateMaterial( "MaterialHalo", MaterialHaloVMT );
	}
}

void COutlines::ResetMaterials( )
{
	if ( MaterialGlowColor )
	{
		MaterialGlowColor->DecrementReferenceCount( );
		//m_material_GlowColor->delete_if_unreferenced( );
		MaterialGlowColor = nullptr;
	}

	if ( TextureRTFullFrame )
	{
		TextureRTFullFrame->DecrementReferenceCount( );
		//m_rt_full_frame->delete_if_unreferenced( );
		TextureRTFullFrame = nullptr;
	}

	if ( RenderBuffer1 )
	{
		RenderBuffer1->DecrementReferenceCount( );
		//m_render_buffer1->delete_if_unreferenced( );
		RenderBuffer1 = nullptr;
	}

	if ( RenderBuffer2 )
	{
		RenderBuffer2->DecrementReferenceCount( );
		//m_render_buffer2->delete_if_unreferenced( );
		RenderBuffer2 = nullptr;
	}

	if ( MaterialBlurX )
	{
		MaterialBlurX->DecrementReferenceCount( );
		//m_material_blur_x->delete_if_unreferenced( );
		MaterialBlurX = nullptr;
		MaterialBlurXVMT = nullptr;
	}

	if ( MaterialBlurY )
	{
		MaterialBlurY->DecrementReferenceCount( );
		//m_material_blur_y->delete_if_unreferenced( );
		MaterialBlurY = nullptr;
		MaterialBlurYVMT = nullptr;
	}

	if ( MaterialHalo )
	{
		MaterialHalo->DecrementReferenceCount( );
		//m_material_halo->delete_if_unreferenced( );
		MaterialHalo = nullptr;
		MaterialHaloVMT = nullptr;
	}
}

void COutlines::DrawEntity( CBaseEntity* entity, bool drawing_models )
{
	if ( !entity || entity->IsDormant( ) || !entity->ShouldDraw( ) )
		return;

	IsRendering_ = true;

	if ( !drawing_models )
		IsDrawingGlow = true;

	entity->DrawModel( STUDIO_RENDER | STUDIO_NOSHADOWS );

	if ( drawing_models )
		RenderedEnts[ entity ] = true;

	if ( !drawing_models )
		IsDrawingGlow = false;

	IsRendering_ = false;
}

void COutlines::DrawOutlineOnEntities( )
{
	int w = TF2->ScreenWidth;
	int h = TF2->ScreenHeight;

	if ( w < 1 || h < 1 || w > 4096 || h > 2160 )
		return;

	const auto& RenderContext = gMaterialSystem->GetRenderContext( );
	if ( !RenderContext )
		return;

	if ( IMaterialVar* Bloom = MaterialBlurY->FindVar( "$bloomamount", NULL, false ) )
	{
		Bloom->SetIntValue( Config->Outlines->Global->BlurScale );
	}

	//if ( !models->m_has_stencil )
	//{
	RenderContext->ClearBuffers( false, false, false );
	RenderContext->SetStencilEnable( true );
	RenderContext->SetStencilCompareFunction( STENCILCOMPARISONFUNCTION_ALWAYS );
	RenderContext->SetStencilPassOperation( Config->Models->Global->RenderingOrder ? STENCILOPERATION_KEEP : STENCILOPERATION_REPLACE );
	RenderContext->SetStencilFailOperation( STENCILOPERATION_KEEP );
	RenderContext->SetStencilZFailOperation( Config->Models->Global->RenderingOrder ? STENCILOPERATION_KEEP : STENCILOPERATION_REPLACE );
	RenderContext->SetStencilReferenceValue( 1 );
	RenderContext->SetStencilWriteMask( 0xFF );
	RenderContext->SetStencilTestMask( 0x0 );
	//}

	gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
	gRenderView->SetBlend( 1.0f );

	if ( Config->Outlines->Players->Enabled )
	{
		for ( const auto& Player : gPlayers )
		{
			if ( !Player || Player->IsDormant( ) )
			{
				continue;
			}

			if ( !Player->IsEntityOnScreen( ) )
			{
				continue;
			}

			if ( Player != gLocalPlayer && !Player->IsSteamFriend( ) )
			{
				if ( !Config->Outlines->Players->Team && !Player->IsEnemy( ) )
				{
					continue;
				}

				if ( !Config->Outlines->Players->Enemy && Player->IsEnemy( ) )
				{
					continue;
				}
			}
			else
			{
				if ( Player->IsSteamFriend( ) && !Config->Outlines->Players->Friends )
				{
					continue;
				}

				if ( Player == gLocalPlayer && !Config->Outlines->Players->Local )
				{
					continue;
				}
			}

			Color GlowColor = Player->GetOutlinesColor( Config->Outlines->Global->ColorScheme );
			if ( Player->IsLocalPlayer( ) )
			{
				GlowColor = Config->Colors->OutlinesPlayerLocal;
			}

			if ( Player->IsSteamFriend( ) )
			{
				GlowColor = Config->Colors->OutlinesPlayerFriend;
			}

			if ( Player->IsInvulnerable( ) )
			{
				GlowColor = Config->Colors->OutlinesPlayerUber;
			}

			OutlinedEntity OutlinedPlayer = {};
			OutlinedPlayer.Entity = Player;
			OutlinedPlayer.Clr = GlowColor;
			OutlinedPlayer.Alpha = Config->Outlines->Players->Alpha;

			GlowEnts.push_back( OutlinedPlayer );

			// either free the allocated memory here
			// by doing https://www.geeksforgeeks.org/delete-and-free-in-cpp/
			// or fucking hell just make a non pointer one
			// and let the compiler handle stuff

			if ( !Models->IsEntityRendered( Player ) )
				DrawEntity( Player, true );

			auto Attachment = Player->MoveChild( );
			for ( int i = 0; i < 32; i++ )
			{
				if ( !Attachment || Attachment->IsDormant( ) )
					continue;

				if ( Attachment->GetClientClass( )->ClassID == EClassID::CTFWearable )
				{
					OutlinedEntity OutlinedAttachment = {};
					OutlinedAttachment.Entity = Attachment;
					OutlinedAttachment.Clr = GlowColor;
					OutlinedAttachment.Alpha = Config->Outlines->Players->Alpha;

					GlowEnts.push_back( OutlinedAttachment );
					if ( !Models->IsEntityRendered( Attachment ) )
						DrawEntity( Attachment, true );
				}

				Attachment = Attachment->NextMovePeer( );
			}

			const auto& Weapon = Player->ActiveWeapon( );

			if ( Weapon && !Weapon->IsDormant( ) )
			{
				OutlinedEntity OutlinedWeapon = {};
				OutlinedWeapon.Entity = Weapon;
				OutlinedWeapon.Clr = GlowColor;
				OutlinedWeapon.Alpha = Config->Outlines->Players->Alpha;

				GlowEnts.push_back( OutlinedWeapon );
				if ( !Models->IsEntityRendered( Weapon ) )
					DrawEntity( Weapon, true );
			}
		}
	}

	if ( Config->Outlines->Buildings->Enabled )
	{
		for ( const auto& Building : gBuildings )
		{
			const auto& Builder = gEntityList->GetClientEntityFromHandle( Building->Builder( ) )->As<CBasePlayer>( );
			if ( !Building || Building->IsDormant( ) )
				continue;

			if ( !Building->IsEntityOnScreen( ) )
				continue;

			if ( Builder != gLocalPlayer && !Builder->IsSteamFriend( ) )
			{
				if ( !Config->Outlines->Buildings->Team && !Building->IsEnemy( ) )
				{
					continue;
				}

				if ( !Config->Outlines->Buildings->Enemy && Building->IsEnemy( ) )
				{
					continue;
				}
			}
			else
			{
				if ( Builder->IsSteamFriend( ) && !Config->Outlines->Buildings->Friends )
				{
					continue;
				}

				if ( Builder == gLocalPlayer && !Config->Outlines->Buildings->Local )
				{
					continue;
				}
			}

			Color GlowColor = Building->GetOutlinesColor( Config->Outlines->Global->ColorScheme );
			if ( Builder && Builder->IsLocalPlayer( ) )
			{
				GlowColor = Config->Colors->OutlinesBuildingLocal;
			}

			if ( Builder && Builder->IsSteamFriend( ) )
			{
				GlowColor = Config->Colors->OutlinesBuildingFriend;
			}

			OutlinedEntity OutlinedBuilding = {};
			OutlinedBuilding.Entity = Building;
			OutlinedBuilding.Clr = GlowColor;
			OutlinedBuilding.Alpha = Config->Outlines->Buildings->Alpha;

			GlowEnts.push_back( OutlinedBuilding );
			if ( !Models->IsEntityRendered( Building ) )
				DrawEntity( Building, true );
		}
	}

	if ( Config->Outlines->World->Enabled )
	{
		if ( Config->Outlines->World->Healthpacks )
		{
			for ( const auto& Health : gHealthpacks )
			{
				if ( !Health || Health->IsDormant( ) )
				{
					continue;
				}

				if ( !Health->IsEntityOnScreen( ) )
				{
					continue;
				}

				Color GlowColor = Health->GetOutlinesColor( Config->Outlines->Global->ColorScheme );

				OutlinedEntity OutlinedHealth = {};
				OutlinedHealth.Entity = Health;
				OutlinedHealth.Clr = GlowColor;
				OutlinedHealth.Alpha = Config->Outlines->World->Alpha;

				GlowEnts.push_back( OutlinedHealth );
				if ( !Models->IsEntityRendered( Health ) )
					DrawEntity( Health, true );
			}
		}

		if ( Config->Outlines->World->Ammopacks )
		{
			for ( const auto& Ammo : gAmmopacks )
			{
				if ( !Ammo || Ammo->IsDormant( ) )
				{
					continue;
				}

				if ( !Ammo->IsEntityOnScreen( ) )
				{
					continue;
				}

				Color GlowColor = Ammo->GetOutlinesColor( Config->Outlines->Global->ColorScheme );

				OutlinedEntity OutlinedAmmo = {};
				OutlinedAmmo.Entity = Ammo;
				OutlinedAmmo.Clr = GlowColor;
				OutlinedAmmo.Alpha = Config->Outlines->World->Alpha;

				GlowEnts.push_back( OutlinedAmmo );
				if ( !Models->IsEntityRendered( Ammo ) )
					DrawEntity( Ammo, true );
			}
		}

		if ( Config->Outlines->World->Intels )
		{
			for ( const auto& Intel : gIntels )
			{
				if ( !Intel || Intel->IsDormant( ) )
				{
					continue;
				}

				if ( !Intel->IsEntityOnScreen( ) )
				{
					continue;
				}

				Color GlowColor = Intel->GetOutlinesColor( Config->Outlines->Global->ColorScheme );

				OutlinedEntity OutlinedIntel = {};
				OutlinedIntel.Entity = Intel;
				OutlinedIntel.Clr = GlowColor;
				OutlinedIntel.Alpha = Config->Outlines->World->Alpha;

				GlowEnts.push_back( OutlinedIntel );
				if ( !Models->IsEntityRendered( Intel ) )
					DrawEntity( Intel, true );
			}
		}

		if ( Config->Outlines->World->Bombs )
		{
			for ( const auto& Bomb : gBombs )
			{
				if ( !Bomb || Bomb->IsDormant( ) )
				{
					continue;
				}

				if ( !Bomb->IsEntityOnScreen( ) )
				{
					continue;
				}

				Color GlowColor = Bomb->GetOutlinesColor( Config->Outlines->Global->ColorScheme );

				OutlinedEntity OutlinedBomb = {};
				OutlinedBomb.Entity = Bomb;
				OutlinedBomb.Clr = GlowColor;
				OutlinedBomb.Alpha = Config->Outlines->World->Alpha;

				GlowEnts.push_back( OutlinedBomb );
				if ( !Models->IsEntityRendered( Bomb ) )
					DrawEntity( Bomb, true );
			}
		}

		if ( Config->Outlines->World->MvMCash )
		{
			for ( const auto& Cash : gMvMCash )
			{
				if ( !Cash || Cash->IsDormant( ) )
				{
					continue;
				}

				if ( !Cash->IsEntityOnScreen( ) )
				{
					continue;
				}

				Color GlowColor = Config->Colors->OutlinesWorldMoney;

				OutlinedEntity OutlinedBomb = {};
				OutlinedBomb.Entity = Cash;
				OutlinedBomb.Clr = GlowColor;
				OutlinedBomb.Alpha = Config->Outlines->World->Alpha;

				GlowEnts.push_back( OutlinedBomb );
				if ( !Models->IsEntityRendered( Cash ) )
					DrawEntity( Cash, true );
			}
		}

		if ( Config->Outlines->World->MPPowerups )
		{
			for ( const auto& Powerup : gPowerups )
			{
				if ( !Powerup || Powerup->IsDormant( ) )
				{
					continue;
				}

				if ( !Powerup->IsEntityOnScreen( ) )
				{
					continue;
				}

				Color GlowColor = Config->Colors->OutlinesWorldPowerup;

				OutlinedEntity OutlinedBomb = {};
				OutlinedBomb.Entity = Powerup;
				OutlinedBomb.Clr = GlowColor;
				OutlinedBomb.Alpha = Config->Outlines->World->Alpha;

				GlowEnts.push_back( OutlinedBomb );
				if ( !Models->IsEntityRendered( Powerup ) )
					DrawEntity( Powerup, true );
			}
		}

		if ( Config->Outlines->World->MvMTanks || Config->Models->World->MvMSentryBusters )
		{
			for ( const auto& Entity : gMvMEntities )
			{
				if ( !Entity || Entity->IsDormant( ) )
				{
					continue;
				}

				if ( !Config->Outlines->World->MvMSentryBusters && Entity->IsSentryBuster( ) )
				{
					continue;
				}

				if ( !Config->Outlines->World->MvMTanks && Entity->IsTank( ) )
				{
					continue;
				}

				if ( !Entity->IsEntityOnScreen( ) )
				{
					continue;
				}

				if ( !Config->Outlines->World->Team && !Entity->IsEnemy( ) )
				{
					continue;
				}

				if ( !Config->Outlines->World->Enemy && Entity->IsEnemy( ) )
				{
					continue;
				}

				Color GlowColor = Entity->GetOutlinesColor( Config->Outlines->Global->ColorScheme );

				OutlinedEntity OutlinedProjectile = {};
				OutlinedProjectile.Entity = Entity;
				OutlinedProjectile.Clr = GlowColor;
				OutlinedProjectile.Alpha = Config->Outlines->World->Alpha;

				GlowEnts.push_back( OutlinedProjectile );
				if ( !Models->IsEntityRendered( Entity ) )
					DrawEntity( Entity, true );
			}
		}

		if ( Config->Outlines->World->Projectiles )
		{
			for ( const auto& Projectile : gProjectiles )
			{
				if ( !Projectile || Projectile->IsDormant( ) )
				{
					continue;
				}

				if ( !Projectile->IsEntityOnScreen( ) )
				{
					continue;
				}

				CBasePlayer* Thrower = Projectile->GetThrower( );
				if ( !Thrower )
				{
					Thrower = Projectile->GetOwnerEntity( )->As<CBasePlayer>( );
				}

				if ( Thrower != gLocalPlayer && !Thrower->IsSteamFriend( ) )
				{
					if ( !Config->Outlines->World->Team && !Projectile->IsEnemy( ) )
					{
						continue;
					}

					if ( !Config->Outlines->World->Enemy && Projectile->IsEnemy( ) )
					{
						continue;
					}
				}
				else
				{
					if ( Thrower->IsSteamFriend( ) && !Config->Outlines->World->Friends )
					{
						continue;
					}

					if ( Thrower == gLocalPlayer && !Config->Outlines->World->Local )
					{
						continue;
					}
				}

				Color GlowColor = Projectile->GetOutlinesColor( Config->Outlines->Global->ColorScheme );
				if ( Thrower && Thrower->IsLocalPlayer( ) )
				{
					GlowColor = Config->Colors->OutlinesWorldLocal;
				}

				if ( Thrower && Thrower->IsSteamFriend( ) )
				{
					GlowColor = Config->Colors->OutlinesWorldFriend;
				}

				OutlinedEntity OutlinedProjectile = {};
				OutlinedProjectile.Entity = Projectile;
				OutlinedProjectile.Clr = GlowColor;
				OutlinedProjectile.Alpha = Config->Outlines->World->Alpha;

				GlowEnts.push_back( OutlinedProjectile );
				if ( !Models->IsEntityRendered( Projectile ) )
					DrawEntity( Projectile, true );
			}
		}
	}

	RenderContext->SetStencilEnable( false );
	if ( GlowEnts.empty( ) )
		return;

	gModelRender->ForcedMaterialOverride( MaterialGlowColor );

	RenderContext->PushRenderTargetAndViewport( ); {
		if ( RenderBuffer1 ) RenderContext->SetRenderTarget( RenderBuffer1 );
		RenderContext->Viewport( 0, 0, w, h );
		RenderContext->ClearColor4ub( 0, 0, 0, 0 );
		RenderContext->ClearBuffers( true, false, false );

		for ( const auto& GlowEntity : GlowEnts )
		{
			if ( !GlowEntity.Entity )
				continue;

			gRenderView->SetColorModulation( GlowEntity.Clr );
			gRenderView->SetBlend( GlowEntity.Alpha );

			DrawEntity( GlowEntity.Entity, false );
		}

	} RenderContext->PopRenderTargetAndViewport( );

	if ( Config->Outlines->Global->BlurOutline )
	{
		RenderContext->PushRenderTargetAndViewport( ); {
			RenderContext->Viewport( 0, 0, w, h );

			RenderContext->SetRenderTarget( RenderBuffer2 );
			RenderContext->DrawScreenSpaceRectangle( MaterialBlurX, 0, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );

			RenderContext->SetRenderTarget( RenderBuffer1 );
			RenderContext->DrawScreenSpaceRectangle( MaterialBlurY, 0, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		} RenderContext->PopRenderTargetAndViewport( );
	}

	RenderContext->SetStencilEnable( true );
	if ( !Models->HasStencil )
	{
		RenderContext->SetStencilCompareFunction( STENCILCOMPARISONFUNCTION_EQUAL );
		RenderContext->SetStencilPassOperation( STENCILOPERATION_KEEP );
		RenderContext->SetStencilFailOperation( STENCILOPERATION_KEEP );
		RenderContext->SetStencilZFailOperation( STENCILOPERATION_KEEP );
		RenderContext->SetStencilReferenceValue( 0 );
		RenderContext->SetStencilWriteMask( 0x0 );
		RenderContext->SetStencilTestMask( 0xFF );
	}

	if ( Config->Outlines->Global->StencilOutline )
	{
		if ( !Config->Outlines->Global->CheapStencil )
		{
			RenderContext->DrawScreenSpaceRectangle( MaterialHalo, -Config->Outlines->Global->StencilScale, -Config->Outlines->Global->StencilScale, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
			RenderContext->DrawScreenSpaceRectangle( MaterialHalo, Config->Outlines->Global->StencilScale, Config->Outlines->Global->StencilScale, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
			RenderContext->DrawScreenSpaceRectangle( MaterialHalo, Config->Outlines->Global->StencilScale, -Config->Outlines->Global->StencilScale, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
			RenderContext->DrawScreenSpaceRectangle( MaterialHalo, -Config->Outlines->Global->StencilScale, Config->Outlines->Global->StencilScale, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		}
		RenderContext->DrawScreenSpaceRectangle( MaterialHalo, -Config->Outlines->Global->StencilScale, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		RenderContext->DrawScreenSpaceRectangle( MaterialHalo, 0, -Config->Outlines->Global->StencilScale, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		RenderContext->DrawScreenSpaceRectangle( MaterialHalo, 0, Config->Outlines->Global->StencilScale, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		RenderContext->DrawScreenSpaceRectangle( MaterialHalo, Config->Outlines->Global->StencilScale, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
	}

	if ( Config->Outlines->Global->BlurOutline )
	{
		RenderContext->DrawScreenSpaceRectangle( MaterialHalo, 0, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
	}

	RenderContext->SetStencilEnable( false );
	gModelRender->ForcedMaterialOverride( nullptr );
	gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
	gRenderView->SetBlend( 1.0f );
}

void COutlines::Render( )
{
	if ( !RenderedEnts.empty( ) )
		RenderedEnts.clear( );

	if ( !GlowEnts.empty( ) )
		GlowEnts.clear( );

	if ( !gLocalPlayer || !gEngineClient->IsInGame( ) )
		return;

	if ( !Config->Outlines->Global->Enabled )
		return;

	if ( G->ProjectileCamera->IsDrawingProjectileCamera )
		return;

	DrawOutlineOnEntities( );

	gRenderView->SetColorModulation( { 255, 255, 255, 255 } );
	gRenderView->SetBlend( 1.0f );
	gModelRender->ForcedMaterialOverride( nullptr );
}