#include "Models.h"
#include "../../../Globals.h"

bool CModels::IsRendering( )
{
	return IsRendering_;
}

bool CModels::IsRenderingOverlay( )
{
	return IsRenderingOverlay_;
}

bool CModels::IsUsedMaterial( IMaterial* material )
{
	return ( material == FlatMaterial
			 || material == ShadedMaterial
			 || material == FresnelMaterial
			 || material == OverlayMaterial
			 || material == KSOverlayMaterial
			 || material == EsoOverlayMaterial
			 || material == FlatOverlayMaterial );
}

bool CModels::IsEntityRendered( CBaseEntity* entity )
{
	if ( !entity || entity->IsDormant( ) )
		return false;

	if ( entity->IsPlayer( ) )
	{
		if ( Config->Models->Players->TwoModels )
		{
			return ( RenderedEntities.find( entity ) != RenderedEntities.end( ) && RenderedEntities[ entity ] == 2 );
		}
		else
		{
			return RenderedEntities.find( entity ) != RenderedEntities.end( );
		}
	}

	else if ( entity->IsBuilding( ) )
	{
		if ( Config->Models->Buildings->TwoModels )
		{
			return ( RenderedEntities.find( entity ) != RenderedEntities.end( ) && RenderedEntities[ entity ] == 2 );
		}
		else
		{
			return RenderedEntities.find( entity ) != RenderedEntities.end( );
		}
	}

	else if ( entity->IsProjectile( ) )
	{
		if ( Config->Models->World->TwoModels )
		{
			return ( RenderedEntities.find( entity ) != RenderedEntities.end( ) && RenderedEntities[ entity ] == 2 );
		}
		else
		{
			return RenderedEntities.find( entity ) != RenderedEntities.end( );
		}
	}

	else if ( entity->IsHealth( FNV1A::Hash( gModelInfo->GetModelName( entity->GetModel( ) ) ) ) )
	{
		if ( Config->Models->World->TwoModels )
		{
			return ( RenderedEntities.find( entity ) != RenderedEntities.end( ) && RenderedEntities[ entity ] == 2 );
		}
		else
		{
			return RenderedEntities.find( entity ) != RenderedEntities.end( );
		}
	}

	else if ( entity->IsAmmo( FNV1A::Hash( gModelInfo->GetModelName( entity->GetModel( ) ) ) ) )
	{
		if ( Config->Models->World->TwoModels )
		{
			return ( RenderedEntities.find( entity ) != RenderedEntities.end( ) && RenderedEntities[ entity ] == 2 );
		}
		else
		{
			return RenderedEntities.find( entity ) != RenderedEntities.end( );
		}
	}

	else
	{
		if ( Config->Models->Players->TwoModels )
		{
			return ( RenderedEntities.find( entity ) != RenderedEntities.end( ) && RenderedEntities[ entity ] == 2 );
		}
		else
		{
			return RenderedEntities.find( entity ) != RenderedEntities.end( );
		}
	}
}

void CModels::InitMaterials( )
{
	if ( !FlatMaterial )
	{
		if ( !FlatMaterialVMT )
		{
			FlatMaterialVMT = new KeyValues( "UnlitGeneric" );
			FlatMaterialVMT->SetString( "$basetexture", "vgui/white_additive" );
		}

		FlatMaterial = gMaterialSystem->CreateMaterial( "FlatMaterial", FlatMaterialVMT );
		//FlatMaterial->IncrementReferenceCount( );
	}

	if ( !ShadedMaterial )
	{
		if ( !ShadedMaterialVMT )
		{
			ShadedMaterialVMT = new KeyValues( "VertexLitGeneric" );
			ShadedMaterialVMT->SetString( "$basetexture", "vgui/white_additive" );
			ShadedMaterialVMT->SetString( "$bumpmap", "vgui/white_additive" );
			ShadedMaterialVMT->SetString( "$halflambert", "1" );
			ShadedMaterialVMT->SetString( "$selfillum", "1" );
			ShadedMaterialVMT->SetString( "$selfillumfresnel", "1" );
			ShadedMaterialVMT->SetString( "$selfillumfresnelminmaxexp", "[0 0.25 1]" );
		}

		ShadedMaterial = gMaterialSystem->CreateMaterial( "ShadedMaterial", ShadedMaterialVMT );
		//ShadedMaterial->IncrementReferenceCount( );
	}

	if ( !FresnelMaterial )
	{
		if ( !FresnelMaterialVMT )
		{
			FresnelMaterialVMT = new KeyValues( "VertexLitGeneric" );
			FresnelMaterialVMT->SetString( "$basetexture", "vgui/white_additive" );
			FresnelMaterialVMT->SetString( "$bumpmap", "models/player/shared/shared_normal" );
			//FresnelMaterialVMT->SetString( "$color2", "[35 35 35]" );
			FresnelMaterialVMT->SetString( "$selfillum", "1" );
			FresnelMaterialVMT->SetString( "$selfillumfresnel", "1" );
			FresnelMaterialVMT->SetString( "$selfillumfresnelminmaxexp", "[0.5 0.5 0]" );

			FresnelMaterialVMT->SetString( "$envmap", "skybox/sky_dustbowl_01" );
			FresnelMaterialVMT->SetString( "$envmapfresnel", "1" );
			FresnelMaterialVMT->SetString( "$envmaptint", "[1 1 1]" );

			FresnelMaterialVMT->SetString( "$phong", "1" );
			FresnelMaterialVMT->SetString( "$phongfresnelranges", "[0 0.5 1]" );
			//FresnelMaterialVMT->SetString( "$selfillumtint", "[0.04 0.03 0.06]" );
		}

		FresnelMaterial = gMaterialSystem->CreateMaterial( "FresnelMaterial", FresnelMaterialVMT );
		//FresnelMaterial->IncrementReferenceCount( );
	}

	if ( !OverlayMaterial )
	{
		if ( !OverlayMaterialVMT )
		{
			OverlayMaterialVMT = new KeyValues( "VertexLitGeneric" );

			//original
			/*OverlayMaterialVMT->SetString( "$basetexture", "models/player/shared/ice_player" );
			OverlayMaterialVMT->SetString( "$bumpmap", "models/player/shared/shared_normal" );
			OverlayMaterialVMT->SetString( "$translucent", "1" );
			OverlayMaterialVMT->SetString( "$additive", "1" );
			OverlayMaterialVMT->SetString( "$phong", "1" );
			OverlayMaterialVMT->SetString( "$phongfresnelranges", "[0 0.5 1]" );
			OverlayMaterialVMT->SetString( "$phongtint", "[0 0 0]" );
			OverlayMaterialVMT->SetString( "$envmap", "skybox/sky_dustbowl_01" );
			OverlayMaterialVMT->SetString( "$envmapfresnel", "1" );
			OverlayMaterialVMT->SetString( "$envmaptint", "[1 1 1]" );
			OverlayMaterialVMT->SetString( "$selfillum", "1" );
			OverlayMaterialVMT->SetString( "$selfillumtint", "[0 0 0]" );
			OverlayMaterialVMT->SetString( "$wireframe", "0" );
			OverlayMaterialVMT->SetString( "$cloakPassEnabled", "1" );*/

			OverlayMaterialVMT->SetString( "$basetexture", "vgui/white_additive" );
			OverlayMaterialVMT->SetString( "$bumpmap", "models/player/shared/shared_normal" );
			OverlayMaterialVMT->SetString( "$additive", "1" );

			OverlayMaterialVMT->SetString( "$selfillum", "1" );
			OverlayMaterialVMT->SetString( "$selfillumFresnel", "1" );

			OverlayMaterialVMT->SetString( "$envmap", "skybox/sky_dustbowl_01" );
			OverlayMaterialVMT->SetString( "$envmapfresnel", "1" );
			OverlayMaterialVMT->SetString( "$envmaptint", "[1 1 1]" );

			OverlayMaterialVMT->SetString( "$phong", "1" );
			OverlayMaterialVMT->SetString( "$phongfresnelranges", "[0 0 1.5]" );
		}

		OverlayMaterial = gMaterialSystem->CreateMaterial( "OverlayMaterial", OverlayMaterialVMT );
		//OverlayMaterial->IncrementReferenceCount( );
	}

	if ( !KSOverlayMaterial )
	{
		if ( !KSOverlayMaterialVMT )
		{
			KSOverlayMaterialVMT = new KeyValues( "VertexLitGeneric" );

			KSOverlayMaterialVMT->SetString( "$basetexture", "vgui/white_additive" );
			KSOverlayMaterialVMT->SetString( "$additive", "1" );

			KSOverlayMaterialVMT->SetString( "$selfillum", "1" );
			KSOverlayMaterialVMT->SetString( "$selfillumFresnel", "1" );
			KSOverlayMaterialVMT->SetString( "$selfillumtint", "[0 0 0]" );

			KSOverlayMaterialVMT->SetString( "$sheenPassEnabled", "1" );
			KSOverlayMaterialVMT->SetString( "$sheenmap", "cubemaps/cubemap_sheen001" );
			KSOverlayMaterialVMT->SetString( "$sheenmapmask", "effects/AnimatedSheen/animatedsheen0" );
			KSOverlayMaterialVMT->SetString( "$sheenmaptint", "[5 5 5]" );
			KSOverlayMaterialVMT->SetString( "$sheenmapmaskframe", "0" );
			KSOverlayMaterialVMT->SetString( "$sheenindex", "0" );
			KSOverlayMaterialVMT->SetString( "$sheenmapmaskscalex", "110" );
			KSOverlayMaterialVMT->SetString( "$sheenmapmaskscaley", "110" );
			KSOverlayMaterialVMT->SetString( "$sheenmapmaskdirection", "2" );

			static auto Proxies = new KeyValues( "Proxies" );
			if ( Proxies )
			{
				static auto AnimatedTexture = new KeyValues( "AnimatedTexture" );
				if ( AnimatedTexture )
				{
					KSOverlayMaterialVMT->SetString( "$sheenmapmaskscalex", "110" );
					KSOverlayMaterialVMT->SetString( "$sheenmapmaskscaley", "110" );
					KSOverlayMaterialVMT->SetString( "$sheenmapmask", "effects/AnimatedSheen/animatedsheen0" );
					AnimatedTexture->SetString( "animatedTextureVar", "$sheenmapmask" );
					AnimatedTexture->SetString( "animatedTextureFrameNumVar", "$sheenmapmaskframe" );
					AnimatedTexture->SetString( "animatedTextureFrameRate", "15" );
					Proxies->AddSubkey( AnimatedTexture );
				}
				KSOverlayMaterialVMT->AddSubkey( Proxies );
			}
		}

		KSOverlayMaterial = gMaterialSystem->CreateMaterial( "KSOverlayMaterial", KSOverlayMaterialVMT );
		//KSOverlayMaterial->IncrementReferenceCount( );
	}

	if ( !EsoOverlayMaterial )
	{
		if ( !EsoOverlayMaterialVMT )
		{
			EsoOverlayMaterialVMT = new KeyValues( "VertexLitGeneric" );

			EsoOverlayMaterialVMT->SetString( "$basetexture", "vgui/white_additive" );
			EsoOverlayMaterialVMT->SetString( "$bumpmap", "models/player/shared/shared_normal" );
			EsoOverlayMaterialVMT->SetString( "$additive", "1" );

			EsoOverlayMaterialVMT->SetString( "$envmap", "skybox/sky_dustbowl_01" );
			EsoOverlayMaterialVMT->SetString( "$envmapfresnel", "1" );
			//EsoOverlayMaterialVMT->SetString( "$envmaptint", "[1 1 1]" );

			EsoOverlayMaterialVMT->SetString( "$phong", "1" );
			EsoOverlayMaterialVMT->SetString( "$phongfresnelranges", "[0 1.499 1.5]" );

			EsoOverlayMaterialVMT->SetString( "$selfillum", "1" );
			EsoOverlayMaterialVMT->SetString( "$selfillumFresnel", "1" );
		}
		EsoOverlayMaterial = gMaterialSystem->CreateMaterial( "EsoOverlayMaterial", EsoOverlayMaterialVMT );
		//EsoOverlayMaterial->IncrementReferenceCount( );
	}

	if ( !FlatOverlayMaterial )
	{
		if ( !FlatOverlayMaterialVMT )
		{
			FlatOverlayMaterialVMT = new KeyValues( "UnlitGeneric" );
			FlatOverlayMaterialVMT->SetString( "$basetexture", "vgui/white_additive" );
			FlatOverlayMaterialVMT->SetString( "$additive", "1" );
		}
		FlatOverlayMaterial = gMaterialSystem->CreateMaterial( "FlatOverlayMaterial", FlatOverlayMaterialVMT );
	}
}

void CModels::ResetMaterials( )
{
	if ( FlatMaterial )
	{
		FlatMaterial->DecrementReferenceCount( );
		FlatMaterial = nullptr;
		FlatMaterialVMT = nullptr;
	}

	if ( ShadedMaterial )
	{
		ShadedMaterial->DecrementReferenceCount( );
		ShadedMaterial = nullptr;
		ShadedMaterialVMT = nullptr;
	}

	if ( FresnelMaterial )
	{
		FresnelMaterial->DecrementReferenceCount( );
		FresnelMaterial = nullptr;
		FresnelMaterialVMT = nullptr;
	}

	if ( OverlayMaterial )
	{
		OverlayMaterial->DecrementReferenceCount( );
		OverlayMaterial = nullptr;
		OverlayMaterialVMT = nullptr;
	}

	if ( KSOverlayMaterial )
	{
		KSOverlayMaterial->DecrementReferenceCount( );
		KSOverlayMaterial = nullptr;
		KSOverlayMaterialVMT = nullptr;
	}

	if ( EsoOverlayMaterial )
	{
		EsoOverlayMaterial->DecrementReferenceCount( );
		EsoOverlayMaterial = nullptr;
		EsoOverlayMaterialVMT = nullptr;
	}

	if ( FlatOverlayMaterial )
	{
		FlatOverlayMaterial->DecrementReferenceCount( );
		FlatOverlayMaterial = nullptr;
		FlatOverlayMaterialVMT = nullptr;
	}
}

void CModels::DrawEntityAndAttachments( CBaseEntity* Entity, bool Overlay, int OverlayModel, bool OverlayWireframe, Color& Clr, float Alpha, float OverlayAlpha )
{
	if ( !Entity || Entity->IsDormant( ) )
		return;

	IsRendering_ = true;
	Entity->DrawModel( STUDIO_RENDER );

	if ( Entity->IsPlayer( ) && Entity->As<CBasePlayer>( )->IsAlive( ) )
	{
		const auto& Weapon = Entity->As<CBasePlayer>( )->ActiveWeapon( );

		if ( Weapon && !Weapon->IsDormant( ) )
		{
			if ( Weapon->ShouldDraw( ) )
			{
				Weapon->DrawModel( STUDIO_RENDER );
			}
		}
	}

	//if ( !OverlayMaterial || !KSOverlayMaterial || !EsoOverlayMaterial ) // crash on uninject
	//{
	//	return;
	//}

	if ( Overlay && OverlayMaterial && KSOverlayMaterial && EsoOverlayMaterial )
	{
		IsRenderingOverlay_ = true;
		switch ( OverlayModel )
		{
			case 0:
			{
				if ( IMaterialVar* $envmaptint = OverlayMaterial->FindVar( "$envmaptint", nullptr, false ) )
					$envmaptint->SetVecValue( ( float )Clr.r / 255.f, ( float )Clr.g / 255.f, ( float )Clr.b / 255.f );

				OverlayMaterial->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, OverlayWireframe );
				OverlayMaterial->SetMaterialVarFlag( MATERIAL_VAR_ADDITIVE, true );
				gModelRender->ForcedMaterialOverride( OverlayMaterial );
				break;
			}

			case 1:
			{
				if ( IMaterialVar* $sheenmaptint = KSOverlayMaterial->FindVar( "$sheenmaptint", nullptr, false ) )
					$sheenmaptint->SetVecValue( ( float )Clr.r / 255.f, ( float )Clr.g / 255.f, ( float )Clr.b / 255.f );

				KSOverlayMaterial->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, OverlayWireframe );
				KSOverlayMaterial->SetMaterialVarFlag( MATERIAL_VAR_ADDITIVE, true );
				gModelRender->ForcedMaterialOverride( KSOverlayMaterial );
				break;
			}

			case 2:
			{
				if ( IMaterialVar* $envmaptint = EsoOverlayMaterial->FindVar( "$envmaptint", nullptr, false ) )
					$envmaptint->SetVecValue( ( float )Clr.r / 255.f, ( float )Clr.g / 255.f, ( float )Clr.b / 255.f );

				EsoOverlayMaterial->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, OverlayWireframe );
				EsoOverlayMaterial->SetMaterialVarFlag( MATERIAL_VAR_ADDITIVE, true );
				gModelRender->ForcedMaterialOverride( EsoOverlayMaterial );
				break;
			}

			case 3:
			{
				gRenderView->SetColorModulation( Clr );
				FlatOverlayMaterial->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, OverlayWireframe );
				gModelRender->ForcedMaterialOverride( FlatOverlayMaterial );
			}

			default: break;
		}

		//keep this for use later on fixing phong on mat_hdr_level > 0
		/*if ( IMaterialVar* $phongfresnelranges = OverlayMaterial->FindVar( "$phongfresnelranges", nullptr, false ) )
			$phongfresnelranges->SetVecValue( 0.f, 0.f, 0.1f );*/

		gRenderView->SetBlend( OverlayAlpha );
		Entity->DrawModel( STUDIO_RENDER );

		if ( Entity->IsPlayer( ) && Entity->As<CBasePlayer>( )->IsAlive( ) )
		{
			const auto& Weapon = Entity->As<CBasePlayer>( )->ActiveWeapon( );

			if ( Weapon && !Weapon->IsDormant( ) )
			{
				if ( Weapon->ShouldDraw( ) )
				{
					Weapon->DrawModel( STUDIO_RENDER );
					RenderedWeapons[ Weapon ]++;
				}
			}
		}
		IsRenderingOverlay_ = false;
	}

	RenderedEntities[ Entity ]++;
	IsRendering_ = false;
}

void CModels::DrawOccludedAndUnoccludedModel( CBaseEntity* Entity, int Material, Color& Clr, float Alpha, bool IgnoreZ, bool Overlay, int OverlayModel, bool OverlayWireframe, float OverlayAlpha )
{
	if ( !Entity || !Entity->ShouldDraw( ) )
	{
		return;
	}

	auto Context = gMaterialSystem->GetRenderContext( );
	if ( !Context )
		return;

	Context->ClearBuffers( false, false, false );

	Context->SetStencilEnable( true );

	switch ( Material )
	{
		case 1:
		{
			gModelRender->ForcedMaterialOverride( FlatMaterial );
			break;
		}
		case 2:
		{
			gModelRender->ForcedMaterialOverride( ShadedMaterial );
			break;
		}
		case 3:
		{
			if ( auto $envmaptint = FresnelMaterial->FindVar( "$envmaptint", nullptr, false ) )
				$envmaptint->SetVecValue( Clr.r / 255.f, Clr.g / 255.f, Clr.b / 255.f );

			if ( auto $selfillumtint = FresnelMaterial->FindVar( "$selfillumtint", nullptr, false ) )
				$selfillumtint->SetVecValue( 0.f, 0.f, 0.f );
			gModelRender->ForcedMaterialOverride( FresnelMaterial );

			break;
		}
		default: break;
	}
	//if ( material ) material->IncrementReferenceCount( );

	Context->SetStencilCompareFunction( STENCILCOMPARISONFUNCTION_ALWAYS );
	Context->SetStencilPassOperation( Config->Models->Global->RenderingOrder ? STENCILOPERATION_KEEP : STENCILOPERATION_REPLACE );
	Context->SetStencilFailOperation( STENCILOPERATION_KEEP );
	Context->SetStencilZFailOperation( Config->Models->Global->RenderingOrder ? STENCILOPERATION_KEEP : STENCILOPERATION_REPLACE );
	Context->SetStencilReferenceValue( 1 );
	Context->SetStencilWriteMask( 0xFF );
	Context->SetStencilTestMask( 0x0 );

	if ( Material )
	{
		gRenderView->SetColorModulation( Clr );
		gRenderView->SetBlend( Alpha );
	}
	else
	{
		gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
		gRenderView->SetBlend( 1.0f );
	}

	if ( IgnoreZ )
		Context->DepthRange( 0.0f, 0.2f );

	DrawEntityAndAttachments( Entity, Overlay, OverlayModel, OverlayWireframe, Clr, Alpha, OverlayAlpha );
	Context->DepthRange( 0.0f, 1.0f );

	Context->SetStencilEnable( false );
	gModelRender->ForcedMaterialOverride( nullptr );
	gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
	gRenderView->SetBlend( 1.0f );
}

void CModels::DrawOccludedAndUnoccludedModels( CBaseEntity* Entity, int MaterialOccluded, Color& ColorOccluded, float AlphaOccluded, bool OverlayOccluded, int OverlayOccludedModel, bool OverlayWireframeOccluded, float OverlayAlphaOccluded,
											   int MaterialUnoccluded, Color& ColorUnoccluded, float AlphaUnoccluded, bool OverlayUnoccluded, int OverlayUnoccludedModel, bool OverlayWireframeUnoccluded, float OverlayAlphaUnoccluded )
{
	if ( !Entity || !Entity->ShouldDraw( ) )
		return;

	auto Context = gMaterialSystem->GetRenderContext( );
	if ( !Context )
		return;

	Context->ClearBuffers( false, false, false );

	Context->SetStencilEnable( true );

	switch ( MaterialUnoccluded )
	{
		case 1:
		{
			gModelRender->ForcedMaterialOverride( FlatMaterial );
			break;
		}
		case 2:
		{
			gModelRender->ForcedMaterialOverride( ShadedMaterial );
			break;
		}
		case 3:
		{
			gModelRender->ForcedMaterialOverride( FresnelMaterial );
			break;
		}
		default: break;
	}

	Context->SetStencilCompareFunction( STENCILCOMPARISONFUNCTION_ALWAYS );
	Context->SetStencilPassOperation( STENCILOPERATION_REPLACE );
	Context->SetStencilFailOperation( STENCILOPERATION_KEEP );
	Context->SetStencilZFailOperation( STENCILOPERATION_KEEP );
	Context->SetStencilReferenceValue( 1 );
	Context->SetStencilWriteMask( 0xFF );
	Context->SetStencilTestMask( 0x0 );

	if ( MaterialUnoccluded )
	{
		gRenderView->SetColorModulation( ColorUnoccluded );
		gRenderView->SetBlend( AlphaUnoccluded );
	}
	else
	{
		gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
		gRenderView->SetBlend( 1.0f );
	}

	DrawEntityAndAttachments( Entity, OverlayUnoccluded, OverlayUnoccludedModel, OverlayWireframeUnoccluded, ColorUnoccluded, AlphaUnoccluded, OverlayAlphaUnoccluded );

	switch ( MaterialOccluded )
	{
		case 1:
		{
			gModelRender->ForcedMaterialOverride( FlatMaterial );
			break;
		}
		case 2:
		{
			gModelRender->ForcedMaterialOverride( ShadedMaterial );
			break;
		}
		case 3:
		{
			gModelRender->ForcedMaterialOverride( FresnelMaterial );
			break;
		}
		default: break;
	}

	Context->ClearBuffers( false, false, false );
	Context->SetStencilCompareFunction( STENCILCOMPARISONFUNCTION_EQUAL );
	Context->SetStencilPassOperation( STENCILOPERATION_KEEP );
	Context->SetStencilFailOperation( STENCILOPERATION_KEEP );
	Context->SetStencilZFailOperation( STENCILOPERATION_KEEP );
	Context->SetStencilReferenceValue( 0 );
	Context->SetStencilWriteMask( 0x0 );
	Context->SetStencilTestMask( 0xFF );

	if ( MaterialOccluded )
	{
		gRenderView->SetColorModulation( ColorOccluded );
		gRenderView->SetBlend( AlphaOccluded );
	}
	else
	{
		gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
		gRenderView->SetBlend( 1.0f );
	}

	Context->DepthRange( 0.0f, 0.2f );
	DrawEntityAndAttachments( Entity, OverlayOccluded, OverlayOccludedModel, OverlayWireframeOccluded, ColorOccluded, AlphaOccluded, OverlayAlphaOccluded );
	Context->DepthRange( 0.0f, 1.0f );

	Context->SetStencilEnable( false );
	gModelRender->ForcedMaterialOverride( nullptr );
	gRenderView->SetColorModulation( 1.0f, 1.0f, 1.0f );
	gRenderView->SetBlend( 1.0f );
}

void CModels::FixBacktrackOutlines( )
{
	if ( !Config->Visuals->DrawBackTrack || !Config->Visuals->BackTrackOutline )
	{
		return;
	}

	for ( const auto& Player : gPlayers )
	{
		if ( !Player || Player->IsDormant( ) )
		{
			continue;
		}

		Color Col = { 255, 255, 255, 255 };
		DrawOccludedAndUnoccludedModel( Player, 0, Col, 1.0f, false, false, 0, false, 1.0f );
	}
}

void CModels::RenderPlayers( )
{
	if ( !Config->Models->Players->Enabled )
	{
		return;
	}

	for ( const auto& Player : gPlayers )
	{
		if ( !Player || Player->IsDormant( ) )
		{
			continue;
		}

		if ( Player != gLocalPlayer && !Player->IsSteamFriend( ) )
		{
			if ( !Config->Models->Players->Team && !Player->IsEnemy( ) )
			{
				continue;
			}

			if ( !Config->Models->Players->Enemy && Player->IsEnemy( ) )
			{
				continue;
			}
		}
		else
		{
			if ( Player->IsSteamFriend( ) && !Config->Models->Players->Friends )
			{
				continue;
			}

			if ( Player == gLocalPlayer && !Config->Models->Players->Local )
			{
				continue;
			}
		}

		//color Occluded = player->TeamNum( ) == TF_TEAM_RED ? Color( 165, 57, 43, 128 ) : Color( 48, 54, 160, 128 );
		//color Unoccluded = player->TeamNum( ) == TF_TEAM_RED ? Color( 255, 120, 100, 128 ) : Color( 105, 115, 255, 128 );
		Color Occluded = Player->GetModelsColor( Config->Models->Global->ColorScheme, true );
		Color Unoccluded = Player->GetModelsColor( Config->Models->Global->ColorScheme, false );
		Color Model = Player->GetModelColor( Config->Models->Global->ColorScheme );

		if ( Config->Models->Players->TwoModels )
		{
			DrawOccludedAndUnoccludedModels( Player, Config->Models->Players->Occluded->Model,
											 Occluded, Config->Models->Players->Occluded->Alpha,
											 Config->Models->Players->Occluded->OverlayType ? Player->HasHighKD( ) : Config->Models->Players->Occluded->Overlay,
											 Config->Models->Players->Occluded->OverlayModel, Config->Models->Players->Occluded->OverlayWireframe, Config->Models->Players->Occluded->OverlayAlpha,
											 Config->Models->Players->Unoccluded->Model, Unoccluded, Config->Models->Players->Unoccluded->Alpha,
											 Config->Models->Players->Unoccluded->OverlayType ? Player->HasHighKD( ) : Config->Models->Players->Unoccluded->Overlay,
											 Config->Models->Players->Unoccluded->OverlayModel, Config->Models->Players->Unoccluded->OverlayWireframe, Config->Models->Players->Unoccluded->OverlayAlpha );
		}
		else
		{
			DrawOccludedAndUnoccludedModel( Player, Config->Models->Players->Model,
											Model, Config->Models->Players->Alpha,
											Config->Models->Players->IgnoreZ,
											Config->Models->Players->OverlayType ? Player->HasHighKD( ) : Config->Models->Players->Overlay,
											Config->Models->Players->OverlayModel, Config->Models->Players->OverlayWireframe, Config->Models->Players->OverlayAlpha );
		}

		if ( Player->IsAlive( ) )
		{
			auto Attachment = Player->MoveChild( );

			for ( int i = 0; i < 32; i++ )
			{
				if ( !Attachment || Attachment->IsDormant( ) )
					continue;

				if ( Config->Models->Players->TwoModels )
				{
					DrawOccludedAndUnoccludedModels( Attachment, Config->Models->Players->Occluded->Model,
													 Occluded, Config->Models->Players->Occluded->Alpha,
													 Config->Models->Players->Occluded->OverlayType ? Player->HasHighKD( ) : Config->Models->Players->Occluded->Overlay,
													 Config->Models->Players->Occluded->OverlayModel, Config->Models->Players->Occluded->OverlayWireframe, Config->Models->Players->Occluded->OverlayAlpha,
													 Config->Models->Players->Unoccluded->Model, Unoccluded, Config->Models->Players->Unoccluded->Alpha,
													 Config->Models->Players->Unoccluded->OverlayType ? Player->HasHighKD( ) : Config->Models->Players->Unoccluded->Overlay,
													 Config->Models->Players->Unoccluded->OverlayModel, Config->Models->Players->Unoccluded->OverlayWireframe, Config->Models->Players->Unoccluded->OverlayAlpha );
				}
				else
				{
					DrawOccludedAndUnoccludedModel( Attachment, Config->Models->Players->Model,
													Model, Config->Models->Players->Alpha,
													Config->Models->Players->IgnoreZ,
													Config->Models->Players->OverlayType ? Player->HasHighKD( ) : Config->Models->Players->Overlay,
													Config->Models->Players->OverlayModel, Config->Models->Players->OverlayWireframe, Config->Models->Players->OverlayAlpha );
				}

				Attachment = Attachment->NextMovePeer( );
			}
		}
	}
}

void CModels::RenderBuildings( )
{
	if ( !Config->Models->Buildings->Enabled )
	{
		return;
	}

	for ( const auto& Building : gBuildings )
	{
		if ( !Building || Building->Placing( ) || Building->IsDormant( ) || !Building->Health( ) )
		{
			continue;
		}

		if ( !Building->IsEntityOnScreen( ) )
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
			if ( !Config->Models->Buildings->Team && !Building->IsEnemy( ) )
			{
				continue;
			}

			if ( !Config->Models->Buildings->Enemy && Building->IsEnemy( ) )
			{
				continue;
			}
		}
		else
		{
			if ( Builder->IsSteamFriend( ) && !Config->Models->Buildings->Friends )
			{
				continue;
			}

			if ( Builder == gLocalPlayer && !Config->Models->Buildings->Local )
			{
				continue;
			}
		}

		Color Occluded = Building->GetModelsColor( Config->Models->Global->ColorScheme, true );
		Color Unoccluded = Building->GetModelsColor( Config->Models->Global->ColorScheme, false );
		Color Model = Building->GetModelColor( Config->Models->Global->ColorScheme );

		if ( Config->Models->Buildings->TwoModels )
		{
			DrawOccludedAndUnoccludedModels( Building, Config->Models->Buildings->Occluded->Model,
											 Occluded, Config->Models->Buildings->Occluded->Alpha,
											 Config->Models->Buildings->Occluded->Overlay, Config->Models->Buildings->Occluded->OverlayModel, Config->Models->Buildings->Occluded->OverlayWireframe, Config->Models->Buildings->Occluded->OverlayAlpha,
											 Config->Models->Buildings->Unoccluded->Model, Unoccluded, Config->Models->Buildings->Unoccluded->Alpha, 
											 Config->Models->Buildings->Unoccluded->Overlay, Config->Models->Buildings->Unoccluded->OverlayModel, Config->Models->Buildings->Unoccluded->OverlayWireframe, Config->Models->Buildings->Unoccluded->OverlayAlpha );
		}
		else
		{
			DrawOccludedAndUnoccludedModel( Building, Config->Models->Buildings->Model, 
											Model, Config->Models->Buildings->Alpha, 
											Config->Models->Buildings->IgnoreZ, 
											Config->Models->Buildings->Overlay, Config->Models->Buildings->OverlayModel, Config->Models->Buildings->OverlayWireframe, Config->Models->Buildings->OverlayAlpha );
		}
	}
}

void CModels::RenderWorld( )
{
	if ( !Config->Models->World->Enabled )
	{
		return;
	}

	if ( Config->Models->World->Healthpacks )
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

			Color Occluded = Health->GetModelsColor( Config->Models->Global->ColorScheme, true );
			Color Unoccluded = Health->GetModelsColor( Config->Models->Global->ColorScheme, false );
			Color Model = Health->GetModelColor( Config->Models->Global->ColorScheme );

			if ( Config->Models->World->TwoModels )
			{
				DrawOccludedAndUnoccludedModels( Health, Config->Models->World->Occluded->Model,
												 Occluded, Config->Models->World->Occluded->Alpha,
												 Config->Models->World->Occluded->Overlay, Config->Models->World->Occluded->OverlayModel, Config->Models->World->Occluded->OverlayWireframe, Config->Models->World->Occluded->OverlayAlpha,
												 Config->Models->World->Unoccluded->Model, Unoccluded, Config->Models->World->Unoccluded->Alpha,
												 Config->Models->World->Unoccluded->Overlay, Config->Models->World->Unoccluded->OverlayModel, Config->Models->World->Unoccluded->OverlayWireframe, Config->Models->World->Unoccluded->OverlayAlpha );
			}
			else
			{
				DrawOccludedAndUnoccludedModel( Health, Config->Models->World->Model,
												Model, Config->Models->World->Alpha,
												Config->Models->World->IgnoreZ, Config->Models->World->Overlay,
												Config->Models->World->OverlayModel, Config->Models->World->OverlayWireframe, Config->Models->World->OverlayAlpha );
			}
		}
	}

	if ( Config->Models->World->Ammopacks )
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

			Color Occluded = Ammo->GetModelsColor( Config->Models->Global->ColorScheme, true );
			Color Unoccluded = Ammo->GetModelsColor( Config->Models->Global->ColorScheme, false );
			Color Model = Ammo->GetModelColor( Config->Models->Global->ColorScheme );

			if ( Config->Models->World->TwoModels )
			{
				DrawOccludedAndUnoccludedModels( Ammo, Config->Models->World->Occluded->Model,
												 Occluded, Config->Models->World->Occluded->Alpha,
												 Config->Models->World->Occluded->Overlay, Config->Models->World->Occluded->OverlayModel, Config->Models->World->Occluded->OverlayWireframe, Config->Models->World->Occluded->OverlayAlpha,
												 Config->Models->World->Unoccluded->Model, Unoccluded, Config->Models->World->Unoccluded->Alpha,
												 Config->Models->World->Unoccluded->Overlay, Config->Models->World->Unoccluded->OverlayModel, Config->Models->World->Unoccluded->OverlayWireframe, Config->Models->World->Unoccluded->OverlayAlpha );
			}
			else
			{
				DrawOccludedAndUnoccludedModel( Ammo, Config->Models->World->Model,
												Model, Config->Models->World->Alpha,
												Config->Models->World->IgnoreZ, Config->Models->World->Overlay,
												Config->Models->World->OverlayModel, Config->Models->World->OverlayWireframe, Config->Models->World->OverlayAlpha );
			}
		}
	}

	if ( Config->Models->World->Intels )
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

			Color Occluded = Intel->GetModelsColor( Config->Models->Global->ColorScheme, true );
			Color Unoccluded = Intel->GetModelsColor( Config->Models->Global->ColorScheme, false );
			Color Model = Intel->GetModelColor( Config->Models->Global->ColorScheme );

			if ( Config->Models->World->TwoModels )
			{
				DrawOccludedAndUnoccludedModels( Intel, Config->Models->World->Occluded->Model,
												 Occluded, Config->Models->World->Occluded->Alpha,
												 Config->Models->World->Occluded->Overlay, Config->Models->World->Occluded->OverlayModel, Config->Models->World->Occluded->OverlayWireframe, Config->Models->World->Occluded->OverlayAlpha,
												 Config->Models->World->Unoccluded->Model, Unoccluded, Config->Models->World->Unoccluded->Alpha,
												 Config->Models->World->Unoccluded->Overlay, Config->Models->World->Unoccluded->OverlayModel, Config->Models->World->Unoccluded->OverlayWireframe, Config->Models->World->Unoccluded->OverlayAlpha );
			}
			else
			{
				DrawOccludedAndUnoccludedModel( Intel, Config->Models->World->Model,
												Model, Config->Models->World->Alpha,
												Config->Models->World->IgnoreZ, Config->Models->World->Overlay,
												Config->Models->World->OverlayModel, Config->Models->World->OverlayWireframe, Config->Models->World->OverlayAlpha );
			}
		}
	}

	if ( Config->Models->World->Bombs )
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

			Color Occluded = Bomb->GetModelsColor( Config->Models->Global->ColorScheme, true );
			Color Unoccluded = Bomb->GetModelsColor( Config->Models->Global->ColorScheme, false );
			Color Model = Bomb->GetModelColor( Config->Models->Global->ColorScheme );

			if ( Config->Models->World->TwoModels )
			{
				DrawOccludedAndUnoccludedModels( Bomb, Config->Models->World->Occluded->Model,
												 Occluded, Config->Models->World->Occluded->Alpha,
												 Config->Models->World->Occluded->Overlay, Config->Models->World->Occluded->OverlayModel, Config->Models->World->Occluded->OverlayWireframe, Config->Models->World->Occluded->OverlayAlpha,
												 Config->Models->World->Unoccluded->Model, Unoccluded, Config->Models->World->Unoccluded->Alpha,
												 Config->Models->World->Unoccluded->Overlay, Config->Models->World->Unoccluded->OverlayModel, Config->Models->World->Unoccluded->OverlayWireframe, Config->Models->World->Unoccluded->OverlayAlpha );
			}
			else
			{
				DrawOccludedAndUnoccludedModel( Bomb, Config->Models->World->Model,
												Model, Config->Models->World->Alpha,
												Config->Models->World->IgnoreZ, Config->Models->World->Overlay,
												Config->Models->World->OverlayModel, Config->Models->World->OverlayWireframe, Config->Models->World->OverlayAlpha );
			}
		}
	}

	if ( Config->Models->World->MvMCash )
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

			Color Occluded = Config->Colors->ModelsWorldMoneyOccluded;
			Color Unoccluded = Config->Colors->ModelsWorldMoneyUnoccluded;
			Color Model = Config->Colors->ModelsWorldMoney;

			if ( Config->Models->World->TwoModels )
			{
				DrawOccludedAndUnoccludedModels( Cash, Config->Models->World->Occluded->Model,
												 Occluded, Config->Models->World->Occluded->Alpha,
												 Config->Models->World->Occluded->Overlay, Config->Models->World->Occluded->OverlayModel, Config->Models->World->Occluded->OverlayWireframe, Config->Models->World->Occluded->OverlayAlpha,
												 Config->Models->World->Unoccluded->Model, Unoccluded, Config->Models->World->Unoccluded->Alpha,
												 Config->Models->World->Unoccluded->Overlay, Config->Models->World->Unoccluded->OverlayModel, Config->Models->World->Unoccluded->OverlayWireframe, Config->Models->World->Unoccluded->OverlayAlpha );
			}
			else
			{
				DrawOccludedAndUnoccludedModel( Cash, Config->Models->World->Model,
												Model, Config->Models->World->Alpha,
												Config->Models->World->IgnoreZ, Config->Models->World->Overlay,
												Config->Models->World->OverlayModel, Config->Models->World->OverlayWireframe, Config->Models->World->OverlayAlpha );
			}
		}
	}

	if ( Config->Models->World->MPPowerups )
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

			Color Occluded = Config->Colors->ModelsWorldPowerupOccluded;
			Color Unoccluded = Config->Colors->ModelsWorldPowerupUnoccluded;
			Color Model = Config->Colors->ModelsWorldPowerup;

			if ( Config->Models->World->TwoModels )
			{
				DrawOccludedAndUnoccludedModels( Powerup, Config->Models->World->Occluded->Model,
												 Occluded, Config->Models->World->Occluded->Alpha,
												 Config->Models->World->Occluded->Overlay, Config->Models->World->Occluded->OverlayModel, Config->Models->World->Occluded->OverlayWireframe, Config->Models->World->Occluded->OverlayAlpha,
												 Config->Models->World->Unoccluded->Model, Unoccluded, Config->Models->World->Unoccluded->Alpha,
												 Config->Models->World->Unoccluded->Overlay, Config->Models->World->Unoccluded->OverlayModel, Config->Models->World->Unoccluded->OverlayWireframe, Config->Models->World->Unoccluded->OverlayAlpha );
			}
			else
			{
				DrawOccludedAndUnoccludedModel( Powerup, Config->Models->World->Model,
												Model, Config->Models->World->Alpha,
												Config->Models->World->IgnoreZ, Config->Models->World->Overlay,
												Config->Models->World->OverlayModel, Config->Models->World->OverlayWireframe, Config->Models->World->OverlayAlpha );
			}
		}
	}

	if ( Config->Models->World->MvMTanks || Config->Models->World->MvMSentryBusters )
	{
		for ( const auto& Entity : gMvMEntities )
		{
			if ( !Entity || Entity->IsDormant( ) )
			{
				continue;
			}

			if ( !Config->Models->World->MvMSentryBusters && Entity->IsSentryBuster( ) )
			{
				continue;
			}

			if ( !Config->Models->World->MvMTanks && Entity->IsTank( ) )
			{
				continue;
			}

			if ( !Entity->IsEntityOnScreen( ) )
			{
				continue;
			}

			if ( !Config->Models->World->Team && !Entity->IsEnemy( ) )
			{
				continue;
			}

			if ( !Config->Models->World->Enemy && Entity->IsEnemy( ) )
			{
				continue;
			}

			Color Occluded = Entity->GetModelsColor( Config->Models->Global->ColorScheme, true );
			Color Unoccluded = Entity->GetModelsColor( Config->Models->Global->ColorScheme, false );
			Color Model = Entity->GetModelColor( Config->Models->Global->ColorScheme );

			if ( Config->Models->World->TwoModels )
			{
				DrawOccludedAndUnoccludedModels( Entity, Config->Models->World->Occluded->Model,
												 Occluded, Config->Models->World->Occluded->Alpha,
												 Config->Models->World->Occluded->Overlay, Config->Models->World->Occluded->OverlayModel, Config->Models->World->Occluded->OverlayWireframe, Config->Models->World->Occluded->OverlayAlpha,
												 Config->Models->World->Unoccluded->Model, Unoccluded, Config->Models->World->Unoccluded->Alpha,
												 Config->Models->World->Unoccluded->Overlay, Config->Models->World->Unoccluded->OverlayModel, Config->Models->World->Unoccluded->OverlayWireframe, Config->Models->World->Unoccluded->OverlayAlpha );
			}
			else
			{
				DrawOccludedAndUnoccludedModel( Entity, Config->Models->World->Model,
												Model, Config->Models->World->Alpha,
												Config->Models->World->IgnoreZ, Config->Models->World->Overlay,
												Config->Models->World->OverlayModel, Config->Models->World->OverlayWireframe, Config->Models->World->OverlayAlpha );
			}
		}
	}

	if ( Config->Models->World->Projectiles )
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

			if ( !Thrower )
			{
				continue;
			}

			if ( Thrower != gLocalPlayer && !Thrower->IsSteamFriend( ) )
			{
				if ( !Config->Models->World->Team && !Projectile->IsEnemy( ) )
				{
					continue;
				}

				if ( !Config->Models->World->Enemy && Projectile->IsEnemy( ) )
				{
					continue;
				}
			}
			else
			{
				if ( Thrower->IsSteamFriend( ) && !Config->Models->World->Friends )
				{
					continue;
				}

				if ( Thrower == gLocalPlayer && !Config->Models->World->Local )
				{
					continue;
				}
			}

			Color Occluded = Projectile->GetModelsColor( Config->Models->Global->ColorScheme, true );
			Color Unoccluded = Projectile->GetModelsColor( Config->Models->Global->ColorScheme, false );
			Color Model = Projectile->GetModelColor( Config->Models->Global->ColorScheme );

			if ( Config->Models->World->TwoModels )
			{
				DrawOccludedAndUnoccludedModels( Projectile, Config->Models->World->Occluded->Model,
												 Occluded, Config->Models->World->Occluded->Alpha,
												 Config->Models->World->Occluded->Overlay, Config->Models->World->Occluded->OverlayModel, Config->Models->World->Occluded->OverlayWireframe, Config->Models->World->Occluded->OverlayAlpha,
												 Config->Models->World->Unoccluded->Model, Unoccluded, Config->Models->World->Unoccluded->Alpha,
												 Config->Models->World->Unoccluded->Overlay, Config->Models->World->Unoccluded->OverlayModel, Config->Models->World->Unoccluded->OverlayWireframe, Config->Models->World->Unoccluded->OverlayAlpha );
			}
			else
			{
				DrawOccludedAndUnoccludedModel( Projectile, Config->Models->World->Model,
												Model, Config->Models->World->Alpha,
												Config->Models->World->IgnoreZ, Config->Models->World->Overlay,
												Config->Models->World->OverlayModel, Config->Models->World->OverlayWireframe, Config->Models->World->OverlayAlpha );
			}
		}
	}
}

void CModels::Render( )
{
	if ( !RenderedEntities.empty( ) )
		RenderedEntities.clear( );

	if ( !RenderedWeapons.empty( ) )
		RenderedWeapons.clear( );

	if ( !gLocalPlayer )
		return;

	if ( !Config->Models->Global->Enabled )
	{
		FixBacktrackOutlines( );
		return;
	}

	if ( !Config->Models->Players->Enabled )
	{
		FixBacktrackOutlines( );
	}

	if ( G->ProjectileCamera->IsDrawingProjectileCamera )
		return;

	HasStencil = false;
	HasSecondStencil = false;

	RenderWorld( );
	RenderBuildings( );
	RenderPlayers( );

	gRenderView->SetColorModulation( { 255, 255, 255, 255 } );
	gRenderView->SetBlend( 1.0f );
	gModelRender->ForcedMaterialOverride( nullptr );
}