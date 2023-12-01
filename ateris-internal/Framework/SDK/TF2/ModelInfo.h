#pragma once
#include "RenderView.h"
#include "EngineTrace.h"

class IMaterial;
class KeyValues;
struct vcollide_t;
struct model_t;
struct cplane_t;
struct studiohdr_t;
struct virtualmodel_t;
typedef unsigned char byte;
struct virtualterrainparams_t;
class CPhysCollide;
typedef unsigned short MDLHandle_t;
class CUtlBuffer;
typedef CGameTrace Trace_t;

enum modtype_t
{
	mod_bad = 0,
	mod_brush,
	mod_sprite,
	mod_studio
};

class IModelLoadCallback
{
public:
	virtual void OnModelLoadComplete( const model_t* pModel ) = 0;

protected:
	~IModelLoadCallback( );
};

class IVModelInfo
{
public:
	/*virtual ~IVModelInfo( void ) = 0;
	virtual const model_t* GetModel( int modelindex ) = 0;
	virtual int GetModelIndex( const char* name ) const = 0;
	virtual const char* GetModelName( const model_t* model ) const = 0;*/
	const char* GetModelName( const model_t* model )
	{
		return Memory->GetVFunc<const char*(__thiscall*)( IVModelInfo*, const model_t* )>( this, 3u )( this, model );
	}
	/*virtual vcollide_t* GetVCollide( const model_t* model ) = 0;
	virtual vcollide_t* GetVCollide( int modelindex ) = 0;
	virtual void GetModelBounds( const model_t* model, Vector& mins, Vector& maxs ) const = 0;
	virtual	void GetModelRenderBounds( const model_t* model, Vector& mins, Vector& maxs ) const = 0;
	virtual int GetModelFrameCount( const model_t* model ) const = 0;
	virtual int GetModelType( const model_t* model ) const = 0;
	virtual void* GetModelExtraData( const model_t* model ) = 0;
	virtual bool ModelHasMaterialProxy( const model_t* model ) const = 0;
	virtual bool IsTranslucent( model_t const* model ) const = 0;
	virtual bool IsTranslucentTwoPass( const model_t* model ) const = 0;
	virtual void RecomputeTranslucency( const model_t* model, int nSkin, int nBody, void* pClientRenderable, float fInstanceAlphaModulate = 1.0f ) = 0;
	virtual int GetModelMaterialCount( const model_t* model ) const = 0;
	virtual void GetModelMaterials( const model_t* model, int count, IMaterial** ppMaterial ) = 0;
	virtual bool IsModelVertexLit( const model_t* model ) const = 0;
	virtual const char* GetModelKeyValueText( const model_t* model ) = 0;
	virtual bool GetModelKeyValue( const model_t* model, CUtlBuffer& buf ) = 0;
	virtual float GetModelRadius( const model_t* model ) = 0;
	virtual const studiohdr_t* FindModel( const studiohdr_t* pStudioHdr, void** cache, const char* modelname ) const = 0;
	virtual const studiohdr_t* FindModel( void* cache ) const = 0;
	virtual	virtualmodel_t* GetVirtualModel( const studiohdr_t* pStudioHdr ) const = 0;
	virtual byte* GetAnimBlock( const studiohdr_t* pStudioHdr, int iBlock ) const = 0;
	virtual void GetModelMaterialColorAndLighting( const model_t* model, Vector const& origin, QAngle const& angles, Trace_t* pTrace, Vector& lighting, Vector& matColor ) = 0;
	virtual void GetIlluminationPoint( const model_t* model, IClientRenderable* pRenderable, Vector const& origin, QAngle const& angles, Vector* pLightingCenter ) = 0;
	virtual int GetModelContents( int modelIndex ) = 0;
	virtual studiohdr_t* GetStudioModel( const model_t* mod ) = 0;*/
	studiohdr_t* GetStudioModel( const model_t* model )
	{
		return Memory->GetVFunc<studiohdr_t * ( __thiscall* )( IVModelInfo*, const model_t* )>( this, 28u )( this, model );
	}
	/*virtual int GetModelSpriteWidth( const model_t* model ) const = 0;
	virtual int GetModelSpriteHeight( const model_t* model ) const = 0;
	virtual void SetLevelScreenFadeRange( float flMinSize, float flMaxSize ) = 0;
	virtual void GetLevelScreenFadeRange( float* pMinArea, float* pMaxArea ) const = 0;
	virtual void SetViewScreenFadeRange( float flMinSize, float flMaxSize ) = 0;
	virtual unsigned char ComputeLevelScreenFade( const Vector& vecAbsOrigin, float flRadius, float flFadeScale ) const = 0;
	virtual unsigned char ComputeViewScreenFade( const Vector& vecAbsOrigin, float flRadius, float flFadeScale ) const = 0;
	virtual int GetAutoplayList( const studiohdr_t* pStudioHdr, unsigned short** pAutoplayList ) const = 0;
	virtual CPhysCollide* GetCollideForVirtualTerrain( int index ) = 0;
	virtual bool IsUsingFBTexture( const model_t* model, int nSkin, int nBody, void* pClientRenderable ) const = 0;
	virtual const model_t* FindOrLoadModel( const char* name ) = 0;
	virtual void InitDynamicModels( ) = 0;
	virtual void ShutdownDynamicModels( ) = 0;
	virtual void AddDynamicModel( const char* name, int nModelIndex = -1 ) = 0;
	virtual void ReferenceModel( int modelindex ) = 0;
	virtual void UnreferenceModel( int modelindex ) = 0;
	virtual void CleanupDynamicModels( bool bForce = false ) = 0;
	virtual MDLHandle_t GetCacheHandle( const model_t* model ) const = 0;
	virtual int GetBrushModelPlaneCount( const model_t* model ) const = 0;
	virtual void GetBrushModelPlane( const model_t* model, int nIndex, cplane_t& plane, Vector* pOrigin ) const = 0;
	virtual int GetSurfacepropsForVirtualTerrain( int index ) = 0;
	virtual void OnLevelChange( ) = 0;
	virtual int GetModelClientSideIndex( const char* name ) const = 0;
	virtual int RegisterDynamicModel( const char* name, bool bClientSide ) = 0;
	virtual bool IsDynamicModelLoading( int modelIndex ) = 0;
	virtual void AddRefDynamicModel( int modelIndex ) = 0;
	virtual void ReleaseDynamicModel( int modelIndex ) = 0;
	virtual bool RegisterModelLoadCallback( int modelindex, IModelLoadCallback* pCallback, bool bCallImmediatelyIfLoaded = true ) = 0;
	virtual void UnregisterModelLoadCallback( int modelindex, IModelLoadCallback* pCallback ) = 0;*/
};

struct mstudiobbox_t
{
	int					bone;
	int					group;
	Vector				bbmin;
	Vector				bbmax;
	int					szhitboxnameindex;
	Vector angle;
	float m_radius;
	uint8_t _padding[ 0x10 ];
};


struct mstudiohitboxset_t
{
	int sznameindex;
	inline char* const name( void ) const { return ( ( char* )this ) + sznameindex; }
	int numhitboxes;
	int hitboxindex;
	mstudiobbox_t* hitbox( int i ) const { return ( mstudiobbox_t* )( ( ( BYTE* )this ) + hitboxindex ) + i; };
};

struct mstudiobone_t
{
	int					sznameindex;
	inline char* const pszName( void ) const { return ( ( char* )this ) + sznameindex; }
	int		 			parent;
	int					bonecontroller[ 6 ];
	Vector				pos;
	float				quat[ 4 ];
	Vector				rot;
	Vector				posscale;
	Vector				rotscale;
	float				poseToBone[ 3 ][ 4 ];
	float				qAlignment[ 4 ];
	int					flags;
	int					proctype;
	int					procindex;
	mutable int			physicsbone;
	inline void* pProcedure( ) const { if ( procindex == 0 ) return NULL; else return  ( void* )( ( ( byte* )this ) + procindex ); };
	int					surfacepropidx;
	inline char* const pszSurfaceProp( void ) const { return ( ( char* )this ) + surfacepropidx; }
	int					contents;
	int					unused[ 8 ];

	mstudiobone_t( ) {}
private:
	// No copy constructors allowed
	mstudiobone_t( const mstudiobone_t& vOther );
};

struct studiohdr_t
{
	int id;
	int version;
	int checksum;
	char name[ 64 ];
	int length;
	Vector eyeposition;
	Vector illumposition;
	Vector hull_min;
	Vector hull_max;
	Vector view_bbmin;
	Vector view_bbmax;
	int flags;
	int numbones;
	int boneindex;
	inline mstudiobone_t* GetBone( int i ) const { return ( mstudiobone_t* )( ( ( byte* )this ) + boneindex ) + i; };

	int numbonecontrollers;
	int bonecontrollerindex;
	int numhitboxsets;
	int hitboxsetindex;

	mstudiohitboxset_t* GetHitboxSet( int i ) const
	{
		return ( mstudiohitboxset_t* )( ( ( byte* )this ) + hitboxsetindex ) + i;
	}

	inline mstudiobbox_t* GetHitbox( int i, int set ) const
	{
		mstudiohitboxset_t const* s = GetHitboxSet( set );

		if ( !s )
			return NULL;

		return s->hitbox( i );
	}

	inline int GetHitboxCount( int set ) const
	{
		mstudiohitboxset_t const* s = GetHitboxSet( set );

		if ( !s )
			return 0;

		return s->numhitboxes;
	}

	int numlocalanim;
	int localanimindex;
	int numlocalseq;
	int localseqindex;
	mutable int activitylistversion;
	mutable int eventsindexed;
	int numtextures;
	int textureindex;
	int numcdtextures;
	int cdtextureindex;
	int numskinref;
	int numskinfamilies;
	int skinindex;
	int numbodyparts;
	int bodypartindex;
	int numlocalattachments;
	int localattachmentindex;
	int numlocalnodes;
	int localnodeindex;
	int localnodenameindex;
	int numflexdesc;
	int flexdescindex;
	int numflexcontrollers;
	int flexcontrollerindex;
	int numflexrules;
	int flexruleindex;
	int numikchains;
	int ikchainindex;
	int nummouths;
	int mouthindex;
	int numlocalposeparameters;
	int localposeparamindex;
	int surfacepropindex;
	int keyvalueindex;
	int keyvaluesize;
	int numlocalikautoplaylocks;
	int localikautoplaylockindex;
	float mass;
	int contents;
	int numincludemodels;
	int includemodelindex;
	mutable void* virtualModel;
	int szanimblocknameindex;
	int numanimblocks;
	int animblockindex;
	mutable void* animblockModel;
	int bonetablebynameindex;
	void* pVertexBase;
	void* pIndexBase;
	byte constdirectionallightdot;
	byte rootLOD;
	byte numAllowedRootLODs;
	byte unused[ 1 ];
	int unused4;
	int numflexcontrollerui;
	int	flexcontrolleruiindex;
	float flVertAnimFixedPointScale;
	int	unused3[ 1 ];
	int	studiohdr2index;
	int	unused2[ 1 ];
};