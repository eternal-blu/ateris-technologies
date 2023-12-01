#pragma once
#include "baseclientdll.h"

struct CPlane_t {
	Vector Normal;
	float Dist;
	BYTE Type;
	BYTE SignBits;
	BYTE __pad[ 2 ];
};

class __declspec( align( 16 ) )VectorAligned : public Vector {
public:
	inline VectorAligned( void ) { };

	inline VectorAligned( float x, float y, float z ) {
		this->Set( x, y, z );
	}

	explicit VectorAligned( const Vector& other ) {
		this->Set( other.x, other.y, other.z );
	}

	VectorAligned& operator=( const Vector& other ) {
		this->Set( other.x, other.y, other.z );
		return *this;
	}

	float w;
};

struct Ray_t {
	VectorAligned Start;
	VectorAligned Delta;
	VectorAligned StartOffset;
	VectorAligned Extents;
	bool IsRay;
	bool IsSwept;

	void Init( Vector const& start, Vector const& end ) {
		Delta = end - start;
		IsSwept = ( Delta.LengthSqr( ) != 0 );
		Extents.Set( );
		IsRay = true;
		StartOffset.Set( );
		Start = start;
	}

	void Init( Vector const& start, Vector const& end, Vector const& mins, Vector const& maxs ) {
		Delta = end - start;
		IsSwept = ( Delta.LengthSqr( ) != 0 );
		Extents = maxs - mins;
		Extents *= 0.5f;
		IsRay = ( Extents.LengthSqr( ) < 1e-6 );
		StartOffset = mins + maxs;
		StartOffset *= 0.5f;
		Start = start + StartOffset;
		StartOffset *= -1.0f;
	}

	Vector InvDelta( ) const {
		Vector inv_delta = Vector( );

		for ( int axis = 0; axis < 3; ++axis ) {
			if ( Delta[ axis ] != 0.0f )
				inv_delta[ axis ] = 1.0f / Delta[ axis ];

			else
				inv_delta[ axis ] = FLT_MAX;
		}

		return inv_delta;
	}
};

#define DISPSURF_FLAG_SURFACE		( 1 << 0 )
#define DISPSURF_FLAG_WALKABLE		( 1 << 1 )
#define DISPSURF_FLAG_BUILDABLE		( 1 << 2 )
#define DISPSURF_FLAG_SURFPROP1		( 1 << 3 )
#define DISPSURF_FLAG_SURFPROP2		( 1 << 4 )

class CBaseTrace {
public:
	bool IsDispSurface( void ) { return ( ( DispFlags & DISPSURF_FLAG_SURFACE ) != 0 ); }
	bool IsDispSurfaceWalkable( void ) { return ( ( DispFlags & DISPSURF_FLAG_WALKABLE ) != 0 ); }
	bool IsDispSurfaceBuildable( void ) { return ( ( DispFlags & DISPSURF_FLAG_BUILDABLE ) != 0 ); }
	bool IsDispSurfaceProp1( void ) { return ( ( DispFlags & DISPSURF_FLAG_SURFPROP1 ) != 0 ); }
	bool IsDispSurfaceProp2( void ) { return ( ( DispFlags & DISPSURF_FLAG_SURFPROP2 ) != 0 ); }

public:
	Vector StartPosition = Vector( );
	Vector EndPosition = Vector( );
	CPlane_t Plane;
	float Fraction;
	int Contents;
	unsigned short DispFlags;
	bool AllSolid;
	bool StartSolid;
};

struct CSurface_t {
	const char* Name;
	short SurfaceProps;
	unsigned short Flags;
};

class CBaseEntity;
class IClientEntity;
class CGameTrace : public CBaseTrace {
public:
	float FractionLeftSolid;
	CSurface_t Surface;
	int HitGroup;
	short PhysicsBone;
	CBaseEntity* Ent;
	int Hitbox;

	bool DidHitWorld( ) const {
		return Ent == NULL;
	}

	bool DidHitNonWorldEntity( ) const {
		return Ent != NULL && !DidHitWorld( );
	}

	bool DidHit( ) const {
		return Fraction < 1 || AllSolid || StartSolid;
	}
};

enum TraceType_t {
	TRACE_EVERYTHING = 0,
	TRACE_WORLD_ONLY,
	TRACE_ENTITIES_ONLY,
	TRACE_EVERYTHING_FILTER_PROPS,
};

class CTraceFilter {
public:
	virtual bool ShouldHitEntity( void* ServerEntity, int ContentsMask ) = 0;
	virtual TraceType_t GetTraceType( ) const = 0;
};

class CTraceFilterEntitiesOnly : public CTraceFilter {
public:
	virtual bool ShouldHitEntity( void* ServerEntity, int ContentsMask ) override {
		return true;
	}

	virtual TraceType_t GetTraceType( ) const override {
		return TRACE_ENTITIES_ONLY;
	}
};

class CTraceFilterWorldOnly : public CTraceFilter {
public:
	virtual bool ShouldHitEntity( void* ServerEntity, int ContentsMask ) override {
		return false;
	}

	virtual TraceType_t GetTraceType( ) const override {
		return TRACE_WORLD_ONLY;
	}
};

class CTraceFilterWorldAndPropsOnly : public CTraceFilter {
public:
	virtual bool ShouldHitEntity( void* ServerEntity, int ContentsMask ) override {
		return false;
	}

	virtual TraceType_t GetTraceType( ) const override {
		return TRACE_EVERYTHING;
	}
};

class CTraceFilterHitAll : public CTraceFilter {
public:
	virtual bool ShouldHitEntity( void* ServerEntity, int ContentsMask ) override {
		return true;
	}

	virtual TraceType_t GetTraceType( ) const override {
		return TRACE_EVERYTHING;
	}
};

class CTraceFilterHitscan : public CTraceFilter {
public:
	virtual bool ShouldHitEntity( void* ServerEntity, int ContentsMask );

	virtual TraceType_t GetTraceType( ) const override {
		return TRACE_EVERYTHING;
	}

	IClientEntity* Skip = nullptr;
};

class IEngineTrace {
public:
	void TraceRay( const Ray_t& Ray, unsigned int Mask, CTraceFilter* Filter, CGameTrace* Trace )
	{
		return Memory->GetVFunc< void( __thiscall* )( IEngineTrace*, const Ray_t&, unsigned int, CTraceFilter*, CGameTrace* ) >( this, 4u )( this, Ray, Mask, Filter, Trace );
	}
};