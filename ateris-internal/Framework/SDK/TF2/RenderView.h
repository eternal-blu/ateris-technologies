#pragma once

#include "BaseClientDLL.h"
#include "Entities.h"
#include "Material.h"

#define MAX_AREA_STATE_BYTES 32
#define MAX_AREA_PORTAL_STATE_BYTES 24

struct VisibleFogVolumeInfo_t;
class IWorldRenderList;
struct VisOverrideData_t;
class IBrushRenderer;
class Frustum;
struct DrawBrushModelMode_t;
class WorldListInfo_t;

enum ERenderDepthMode
{
	DEPTH_MODE_NORMAL = 0,
	DEPTH_MODE_SHADOW = 1,
	DEPTH_MODE_SSA0 = 2,
	DEPTH_MODE_OVERRIDE = 3,
	DEPTH_MODE_MAX
};

enum StereoEye_t
{
	STEREO_EYE_MONO = 0,
	STEREO_EYE_LEFT = 1,
	STEREO_EYE_RIGHT = 2,
	STEREO_EYE_MAX = 3,
};

class CViewSetup
{
public:
	int x;
	int UnscaledX;
	int y;
	int UnscaledY;
	int width;
	int UnscaledWidth;
	int height;
	StereoEye_t StereoEye;
	int UnscaledHeight;
	bool Ortho;
	float OrthoLeft;
	float OrthoTop;
	float OrthoRight;
	float OrthoBottom;
	float fov;
	float fovViewmodel;
	Vector origin;
	Vector angles;
	float zNear;
	float zFar;
	float zNearViewmodel;
	float zFarViewmodel;
	bool RenderToSubrectOfLargerScreen;
	float AspectRatio;
	bool OffCenter;
	float OffCenterTop;
	float OffCenterBottom;
	float OffCenterLeft;
	float OffCenterRight;
	bool DoBloomAndToneMapping;
	bool CacheFullSceneState;
	bool ViewToProjectionOverride;
	ViewMatrix ViewToProjection;
};

struct model_t;
class IVRenderView
{
public:
	/*virtual void DrawBrushModel( IClientEntity* baseentity, model_t* model, const Vector& origin, const QAngle& angles, bool bUnused ) = 0;
	virtual void DrawIdentityBrushModel( IWorldRenderList* pList, model_t* model ) = 0;
	virtual void TouchLight( struct dlight_t* light ) = 0;
	virtual void Draw3DDebugOverlays( void ) = 0;
	virtual void SetBlend( float blend ) = 0;*/
	void SetBlend( float blend )
	{
		return Memory->GetVFunc<void( __thiscall* )( IVRenderView*, float )>( this, 4u )( this, blend );
	}
	/*virtual float GetBlend( void ) = 0;
	virtual void SetColorModulation( float const* blend ) = 0;*/
	void SetColorModulation( float const* blend )
	{
		return Memory->GetVFunc<void( __thiscall* )( IVRenderView*, float const* )>( this, 6u )( this, blend );
	}
	/*virtual void GetColorModulation( float* blend ) = 0;
	virtual void SceneBegin( void ) = 0;
	virtual void SceneEnd( void ) = 0;
	virtual void GetVisibleFogVolume( const Vector& eyePoint, VisibleFogVolumeInfo_t* pInfo ) = 0;
	virtual IWorldRenderList* CreateWorldList( ) = 0;
	virtual void BuildWorldLists( IWorldRenderList* pList, WorldListInfo_t* pInfo, int iForceFViewLeaf, const VisOverrideData_t* pVisData = NULL, bool bShadowDepth = false, float* pReflectionWaterHeight = NULL ) = 0;
	virtual void DrawWorldLists( IWorldRenderList* pList, unsigned long flags, float waterZAdjust ) = 0;
	virtual void DrawTopView( bool enable ) = 0;
	virtual void TopViewBounds( Vector2D const& mins, Vector2D const& maxs ) = 0;
	virtual void DrawLights( void ) = 0;
	virtual void DrawMaskEntities( void ) = 0;
	virtual void DrawTranslucentSurfaces( IWorldRenderList* pList, int sortIndex, unsigned long flags, bool bShadowDepth ) = 0;
	virtual void DrawLineFile( void ) = 0;
	virtual void DrawLightmaps( IWorldRenderList* pList, int pageId ) = 0;
	virtual void ViewSetupVis( bool novis, int numorigins, const Vector origin[ ] ) = 0;
	virtual bool AreAnyLeavesVisible( int* leafList, int nLeaves ) = 0;
	virtual	void VguiPaint( void ) = 0;
	virtual void ViewDrawFade( BYTE* color, IMaterial* pMaterial ) = 0;
	virtual void OLD_SetProjectionMatrix( float fov, float zNear, float zFar ) = 0;
	virtual Color GetLightAtPoint( Vector& pos ) = 0;
	virtual int GetViewEntity( void ) = 0;
	virtual float GetFieldOfView( void ) = 0;
	virtual unsigned char** GetAreaBits( void ) = 0;
	virtual void SetFogVolumeState( int nVisibleFogVolume, bool bUseHeightFog ) = 0;
	virtual void InstallBrushSurfaceRenderer( IBrushRenderer* pBrushRenderer ) = 0;
	virtual void DrawBrushModelShadow( IClientRenderable* pRenderable ) = 0;
	virtual	bool LeafContainsTranslucentSurfaces( IWorldRenderList* pList, int sortIndex, unsigned long flags ) = 0;
	virtual bool DoesBoxIntersectWaterVolume( const Vector& mins, const Vector& maxs, int leafWaterDataID ) = 0;
	virtual void SetAreaState( unsigned char chAreaBits[ MAX_AREA_STATE_BYTES ], unsigned char chAreaPortalBits[ MAX_AREA_PORTAL_STATE_BYTES ] ) = 0;
	virtual void VGui_Paint( int mode ) = 0;
	virtual void Push3DView( const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes ) = 0;
	virtual void Push2DView( const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes ) = 0;
	virtual void PopView( Frustum frustumPlanes ) = 0;
	virtual void SetMainView( const Vector& vecOrigin, const QAngle& angles ) = 0;
	virtual void ViewSetupVisEx( bool novis, int numorigins, const Vector origin[ ], unsigned int& returnFlags ) = 0;
	virtual void OverrideViewFrustum( Frustum custom ) = 0;
	virtual void DrawBrushModelShadowDepth( IClientEntity* baseentity, model_t* model, const Vector& origin, const QAngle& angles, ERenderDepthMode DepthMode ) = 0;
	virtual void UpdateBrushModelLightmap(model_t* model, IClientRenderable* pRenderable ) = 0;
	virtual void BeginUpdateLightmaps( void ) = 0;
	virtual void EndUpdateLightmaps( void ) = 0;
	virtual void OLD_SetOffCenterProjectionMatrix( float fov, float zNear, float zFar, float flAspectRatio, float flBottom, float flTop, float flLeft, float flRight ) = 0;
	virtual void OLD_SetProjectionMatrixOrtho( float left, float top, float right, float bottom, float zNear, float zFar ) = 0;
	virtual void Push3DView( const CViewSetup& view, int nFlags, ITexture* pRenderTarget, Frustum frustumPlanes, ITexture* pDepthTexture ) = 0;
	virtual void GetMatricesForView( const CViewSetup& view, ViewMatrix* pWorldToView, ViewMatrix* pViewToProjection, ViewMatrix* pWorldToProjection, ViewMatrix* pWorldToPixels ) = 0;*/
	void GetMatricesForView( const CViewSetup& view, ViewMatrix* pWorldToView, ViewMatrix* pViewToProjection, ViewMatrix* pWorldToProjection, ViewMatrix* pWorldToPixels )
	{
		return Memory->GetVFunc<void( __thiscall* )( IVRenderView*, const CViewSetup &, ViewMatrix *, ViewMatrix *, ViewMatrix *, ViewMatrix * )>( this, 50u )( this, view, pWorldToView, pViewToProjection, pWorldToProjection, pWorldToPixels );
	}
	//virtual void DrawBrushModelEx( IClientEntity* baseentity, model_t* model, const Vector& origin, const QAngle& angles, DrawBrushModelMode_t mode ) = 0;
	inline void SetColorModulation( float r, float g, float b )
	{
		float clr[ 3 ] = { r, g, b };
		SetColorModulation( clr );
	}
	inline void SetColorModulation( Color clr )
	{
		Vector col = { ( float )clr.r / 255, ( float )clr.g / 255, ( float )clr.b / 255 };
		SetColorModulation( &col.x );
	}
};