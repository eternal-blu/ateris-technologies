#pragma once
#include "../../SDK/TF2/TF2.h"
#include "../Models/Models.h"

class COutlines
{
	bool IsRendering_ = false;

	struct OutlinedEntity
	{
		CBaseEntity* Entity;
		Color Clr;
		float Alpha;
	};

	std::vector<OutlinedEntity> GlowEnts;
	std::unordered_map<CBaseEntity*, int> RenderedEnts;

	void DrawEntity( CBaseEntity* entity, bool drawing_models );
	void DrawOutlineOnEntities( );
public:
	bool IsDrawingGlow = false;

	IMaterial* MaterialGlowColor {};
	ITexture* TextureRTFullFrame {};
	ITexture* RenderBuffer1 {};
	ITexture* RenderBuffer2 {};

	KeyValues* MaterialBlurXVMT {};
	IMaterial* MaterialBlurX {};
	KeyValues* MaterialBlurYVMT {};
	IMaterial* MaterialBlurY {};
	KeyValues* MaterialHaloVMT {};
	IMaterial* MaterialHalo {};

	void InitMaterials( );
	void ResetMaterials( );
	bool IsRendering( );
	bool IsUsedMaterial( IMaterial* material );
	bool IsEntityRendered( CBaseEntity* entity );
	void Render( );
};

inline COutlines* Outlines = new COutlines;