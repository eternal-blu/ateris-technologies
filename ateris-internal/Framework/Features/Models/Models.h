#pragma once
#include "../../SDK/TF2/TF2.h"

class CModels
{
public:
	bool IsRendering_ = false;
	bool IsRenderingOverlay_ = false;
	std::unordered_map<CBaseEntity*, int> RenderedEntities {};
	std::unordered_map<CBaseEntity*, int> RenderedWeapons {};

	void DrawEntityAndAttachments( CBaseEntity* Entity, bool Overlay, int OverlayModel, bool OverlayWireframe, Color& Clr, float Alpha, float OverlayAlpha );
	void DrawOccludedAndUnoccludedModel( CBaseEntity* Entity, int Material, Color& Clr, float Alpha, bool IgnoreZ, bool Overlay, int OverlayModel, bool OverlayWireframe, float OverlayAlpha );
	void DrawOccludedAndUnoccludedModels( CBaseEntity* Entity, int MaterialOccluded, Color& ColorOccluded, float AlphaOccluded, bool OverlayOccluded, int OverlayOccludedModel, bool OverlayWireframeOccluded, float OverlayAlphaOccluded,
										  int MaterialUnoccluded, Color& ColorUnoccluded, float alphaUnoccluded, bool OverlayUnoccluded, int OverlayUnoccludedModel, bool OverlayWireframeUnoccluded, float OverlayAlphaUnoccluded );
	void RenderPlayers( );
	void RenderBuildings( );
	void RenderWorld( );
	void FixBacktrackOutlines( );

	bool HasStencil = false;
	bool HasSecondStencil = false;
	KeyValues* FlatMaterialVMT {};
	KeyValues* ShadedMaterialVMT {};
	KeyValues* FresnelMaterialVMT {};
	KeyValues* OverlayMaterialVMT {};
	KeyValues* KSOverlayMaterialVMT {};
	KeyValues* EsoOverlayMaterialVMT {};
	KeyValues* FlatOverlayMaterialVMT {};

	IMaterial* FlatMaterial {};
	IMaterial* ShadedMaterial {};
	IMaterial* FresnelMaterial {};
	IMaterial* OverlayMaterial {};
	IMaterial* KSOverlayMaterial {};
	IMaterial* EsoOverlayMaterial {};
	IMaterial* FlatOverlayMaterial {};
	void InitMaterials( );
	void ResetMaterials( );
	bool IsRendering( );
	bool IsRenderingOverlay( );
	bool IsUsedMaterial( IMaterial* material );
	bool IsEntityRendered( CBaseEntity* entity );
	void Render( );
};

inline CModels* Models = new CModels;