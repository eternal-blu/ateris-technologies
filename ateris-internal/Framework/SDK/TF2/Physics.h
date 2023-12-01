#pragma once
#include "Cvar.h"
#include "Vector.h"
#include "ModelInfo.h"

#define k_flMaxVelocity 2000.0f;
#define k_flMaxAngularVelocity 360.0f * 10.0f;
#define DEFAULT_MIN_FRICTION_MASS 10.0f;
#define DEFAULT_MAX_FRICTION_MASS 2500.0f;

enum
{
	COLLIDE_POLY = 0,
	COLLIDE_MOPP = 1,
	COLLIDE_BALL = 2,
	COLLIDE_VIRTUAL = 3,
};

class IPhysCollide
{
public:
	virtual ~IPhysCollide( ) {}

	virtual void* CreateSurfaceManager( short& ) const = 0;
	virtual void GetAllLedges( void* ledges ) const = 0;
	virtual unsigned int GetSerializationSize( ) const = 0;
	virtual unsigned int SerializeToBuffer( char* pDest, bool bSwap = false ) const = 0;
	virtual int GetVCollideIndex( ) const = 0;
	virtual Vector GetMassCenter( ) const = 0;
	virtual void SetMassCenter( const Vector& massCenter ) = 0;
	virtual Vector GetOrthographicAreas( ) const = 0;
	virtual void SetOrthographicAreas( const Vector& areas ) = 0;
	virtual float GetSphereRadius( ) const = 0;
	virtual void OutputDebugInfo( ) const = 0;
};

#define LEAFMAP_HAS_CUBEMAP					0x0001
#define LEAFMAP_HAS_SINGLE_VERTEX_SPAN		0x0002
#define LEAFMAP_HAS_MULTIPLE_VERTEX_SPANS	0x0004
struct leafmap_t
{
	void* pLeaf;
	unsigned short vertCount;
	byte	flags;
	byte	spanCount;
	unsigned short startVert[ 8 ];

	void SetHasCubemap( )
	{
		flags = LEAFMAP_HAS_CUBEMAP;
	}

	void SetSingleVertexSpan( int startVertIndex, int vertCountIn )
	{
		flags = 0;
		flags |= LEAFMAP_HAS_SINGLE_VERTEX_SPAN;
		startVert[ 0 ] = startVertIndex;
		vertCount = vertCountIn;
	}

	int MaxSpans( )
	{
		return sizeof( startVert ) - sizeof( startVert[ 0 ] );
	}
	const byte* GetSpans( ) const
	{
		return reinterpret_cast< const byte* >( &startVert[ 1 ] );
	}
	byte* GetSpans( )
	{
		return reinterpret_cast< byte* >( &startVert[ 1 ] );
	}

	void SetRLESpans( int startVertIndex, int spanCountIn, byte* pSpans )
	{
		flags = 0;
		if ( spanCountIn > MaxSpans( ) )
			return;
		if ( spanCountIn == 1 )
		{
			SetSingleVertexSpan( startVertIndex, pSpans[ 0 ] );
			return;
		}
		// write out a run length encoded list of verts to include in this model
		flags |= LEAFMAP_HAS_MULTIPLE_VERTEX_SPANS;
		startVert[ 0 ] = startVertIndex;
		vertCount = 0;
		spanCount = spanCountIn;
		byte* pSpanOut = GetSpans( );
		for ( int i = 0; i < spanCountIn; i++ )
		{
			pSpanOut[ i ] = pSpans[ i ];
			if ( !( i & 1 ) )
			{
				vertCount += pSpans[ i ];
			}
		}
	}

	inline bool HasSpans( ) const { return ( flags & ( LEAFMAP_HAS_SINGLE_VERTEX_SPAN | LEAFMAP_HAS_MULTIPLE_VERTEX_SPANS ) ) ? true : false; }
	inline bool HasCubemap( ) const { return ( flags & LEAFMAP_HAS_CUBEMAP ) ? true : false; }
	inline bool HasSingleVertexSpan( ) const { return ( flags & LEAFMAP_HAS_SINGLE_VERTEX_SPAN ) ? true : false; }
	inline bool HasRLESpans( ) const { return ( flags & LEAFMAP_HAS_MULTIPLE_VERTEX_SPANS ) ? true : false; }
};

struct collidemap_t
{
	int				leafCount;
	leafmap_t		leafmap[ 1 ];
};

class CPhysCollide : public IPhysCollide
{
public:
	static CPhysCollide* UnserializeFromBuffer( const char* pBuffer, unsigned int size, int index, bool swap = false );
	virtual const void* GetCompactSurface( ) const { return NULL; }
	virtual Vector GetOrthographicAreas( ) const { return Vector( 1, 1, 1 ); }
	virtual float GetSphereRadius( ) const { return 0; }
	virtual void ComputeOrthographicAreas( float epsilon ) {}
	virtual void SetOrthographicAreas( const Vector& areas ) {}
	virtual const collidemap_t* GetCollideMap( ) const { return NULL; }
};

#define METERS_PER_INCH					(0.0254f)
#define CUBIC_METERS_PER_CUBIC_INCH		(METERS_PER_INCH*METERS_PER_INCH*METERS_PER_INCH)
// 2.2 lbs / kg
#define POUNDS_PER_KG	(2.2f)
#define KG_PER_POUND	(1.0f/POUNDS_PER_KG)

// convert from pounds to kg
#define lbs2kg(x)		((x)*KG_PER_POUND)
#define kg2lbs(x)		((x)*POUNDS_PER_KG)

const float VPHYSICS_MIN_MASS = 0.1f;
const float VPHYSICS_MAX_MASS = 5e4f;

class IPhysicsShadowController;
class IPhysicsObject
{
public:
	virtual ~IPhysicsObject( void ) {}
	virtual bool IsStatic( ) const = 0;
	virtual bool IsAsleep( ) const = 0;
	virtual bool IsTrigger( ) const = 0;
	virtual bool IsFluid( ) const = 0;
	virtual bool IsHinged( ) const = 0;
	virtual bool IsCollisionEnabled( ) const = 0;
	virtual bool IsGravityEnabled( ) const = 0;
	virtual bool IsDragEnabled( ) const = 0;
	virtual bool IsMotionEnabled( ) const = 0;
	virtual bool IsMoveable( ) const = 0;
	virtual bool IsAttachedToConstraint( bool bExternalOnly ) const = 0;
	virtual void EnableCollisions( bool enable ) = 0;
	virtual void EnableGravity( bool enable ) = 0;
	virtual void EnableDrag( bool enable ) = 0;
	virtual void EnableMotion( bool enable ) = 0;
	virtual void SetGameData( void* pGameData ) = 0;
	virtual void* GetGameData( void ) const = 0;
	virtual void SetGameFlags( unsigned short userFlags ) = 0;
	virtual unsigned short GetGameFlags( void ) const = 0;
	virtual void SetGameIndex( unsigned short gameIndex ) = 0;
	virtual unsigned short GetGameIndex( void ) const = 0;
	virtual void SetCallbackFlags( unsigned short callbackflags ) = 0;
	virtual unsigned short GetCallbackFlags( void ) const = 0;
	virtual void Wake( void ) = 0;
	virtual void Sleep( void ) = 0;
	virtual void RecheckCollisionFilter( ) = 0;
	virtual void RecheckContactPoints( ) = 0;
	virtual void SetMass( float mass ) = 0;
	virtual float GetMass( void ) const = 0;
	virtual float GetInvMass( void ) const = 0;
	virtual Vector GetInertia( void ) const = 0;
	virtual Vector GetInvInertia( void ) const = 0;
	virtual void SetInertia( const Vector& inertia ) = 0;
	virtual void SetDamping( const float* speed, const float* rot ) = 0;
	virtual void GetDamping( float* speed, float* rot ) const = 0;
	virtual void SetDragCoefficient( float* pDrag, float* pAngularDrag ) = 0;
	virtual void SetBuoyancyRatio( float ratio ) = 0;
	virtual int GetMaterialIndex( ) const = 0;
	virtual void SetMaterialIndex( int materialIndex ) = 0;
	virtual unsigned int GetContents( ) const = 0;
	virtual void SetContents( unsigned int contents ) = 0;
	virtual float GetSphereRadius( ) const = 0;
	virtual float GetEnergy( ) const = 0;
	virtual Vector GetMassCenterLocalSpace( ) const = 0;
	virtual void SetPosition( const Vector& worldPosition, const QAngle& angles, bool isTeleport ) = 0;
	virtual void SetPositionMatrix( const Matrix3x4& matrix, bool isTeleport ) = 0;
	virtual void GetPosition( Vector* worldPosition, QAngle* angles ) const = 0;
	virtual void GetPositionMatrix( Matrix3x4* positionMatrix ) const = 0;
	virtual void SetVelocity( const Vector* velocity, const Vector* angularVelocity ) = 0;
	virtual void SetVelocityInstantaneous( const Vector* velocity, const Vector* angularVelocity ) = 0;
	virtual void GetVelocity( Vector* velocity, Vector* angularVelocity ) const = 0;
	virtual void AddVelocity( const Vector* velocity, const Vector* angularVelocity ) = 0;
	virtual void GetVelocityAtPoint( const Vector& worldPosition, Vector* pVelocity ) const = 0;
	virtual void GetImplicitVelocity( Vector* velocity, Vector* angularVelocity ) const = 0;
	virtual void LocalToWorld( Vector* worldPosition, const Vector& localPosition ) const = 0;
	virtual void WorldToLocal( Vector* localPosition, const Vector& worldPosition ) const = 0;
	virtual void LocalToWorldVector( Vector* worldVector, const Vector& localVector ) const = 0;
	virtual void WorldToLocalVector( Vector* localVector, const Vector& worldVector ) const = 0;
	virtual void ApplyForceCenter( const Vector& forceVector ) = 0;
	virtual void ApplyForceOffset( const Vector& forceVector, const Vector& worldPosition ) = 0;
	virtual void ApplyTorqueCenter( const Vector& torque ) = 0;
	virtual void CalculateForceOffset( const Vector& forceVector, const Vector& worldPosition, Vector* centerForce, Vector* centerTorque ) const = 0;
	virtual void CalculateVelocityOffset( const Vector& forceVector, const Vector& worldPosition, Vector* centerVelocity, Vector* centerAngularVelocity ) const = 0;
	virtual float CalculateLinearDrag( const Vector& unitDirection ) const = 0;
	virtual float CalculateAngularDrag( const Vector& objectSpaceRotationAxis ) const = 0;
	virtual bool GetContactPoint( Vector* contactPoint, IPhysicsObject** contactObject ) const = 0;
	virtual void SetShadow( float maxSpeed, float maxAngularSpeed, bool allowPhysicsMovement, bool allowPhysicsRotation ) = 0;
	virtual void UpdateShadow( const Vector& targetPosition, const QAngle& targetAngles, bool tempDisableGravity, float timeOffset ) = 0;
	virtual int GetShadowPosition( Vector* position, QAngle* angles ) const = 0;
	virtual /*IPhysicsShadowController*/void* GetShadowController( void ) const = 0;
	virtual void RemoveShadowController( ) = 0;
	virtual float ComputeShadowControl(/*const hlshadowcontrol_params_t &*/void* params, float secondsToArrival, float dt ) = 0;
	virtual const CPhysCollide* GetCollide( void ) const = 0;
	virtual const char* GetName( ) const = 0;
	virtual void BecomeTrigger( ) = 0;
	virtual void RemoveTrigger( ) = 0;
	virtual void BecomeHinged( int localAxis ) = 0;
	virtual void RemoveHinged( ) = 0;
	virtual /*IPhysicsFrictionSnapshot*/void* CreateFrictionSnapshot( ) = 0;
	virtual void DestroyFrictionSnapshot(/*IPhysicsFrictionSnapshot*/void* pSnapshot ) = 0;
	virtual void OutputDebugInfo( ) const = 0;
	void* m_pGameData;
	void* m_pObject;
	const CPhysCollide* m_pCollide;
	IPhysicsShadowController* m_pShadow;
	Vector			m_dragBasis;
	Vector			m_angDragBasis;
	bool			m_shadowTempGravityDisable : 5;
	bool			m_hasTouchedDynamic : 1;
	bool			m_asleepSinceCreation : 1;
	bool			m_forceSilentDelete : 1;
	unsigned char	m_sleepState : 2;
	unsigned char	m_hingedAxis : 3;
	unsigned char	m_collideType : 3;
	unsigned short	m_gameIndex;
private:
	unsigned short	m_materialIndex;
	unsigned short	m_activeIndex;
	unsigned short	m_callbacks;
	unsigned short	m_gameFlags;
	unsigned int	m_contentsMask;
	float			m_volume;
	float			m_buoyancyRatio;
	float			m_dragCoefficient;
	float			m_angDragCoefficient;
};

class IPhysicsEnvironment;
class IPhysicsSurfaceProps;
class IPhysicsConstraint;
class IPhysicsConstraintGroup;
class IPhysicsFluidController;
class IPhysicsSpring;
class IPhysicsVehicleController;
class IConvexInfo;
class IPhysicsObjectPairHash;
class IPhysicsCollisionSet;
class IPhysicsPlayerController;
class IPhysicsFrictionSnapshot;

struct Ray_t;
struct constraint_ragdollparams_t;
struct constraint_hingeparams_t;
struct constraint_fixedparams_t;
struct constraint_ballsocketparams_t;
struct constraint_slidingparams_t;
struct constraint_pulleyparams_t;
struct constraint_lengthparams_t;
struct constraint_groupparams_t;

struct vehicleparams_t;

struct fluidparams_t;
struct springparams_t;
struct objectparams_t
{
	Vector* massCenterOverride;
	float		mass;
	float		inertia;
	float		damping;
	float		rotdamping;
	float		rotInertiaLimit;
	const char* pName;
	void* pGameData;
	float		volume;
	float		dragCoefficient;
	bool		enableCollisions;
};
struct debugcollide_t;
class CGameTrace;
typedef CGameTrace trace_t;
struct physics_stats_t;
struct physics_performanceparams_t
{
	int		maxCollisionsPerObjectPerTimestep;
	int		maxCollisionChecksPerTimestep;
	float	maxVelocity;
	float	maxAngularVelocity;
	float	lookAheadTimeObjectsVsWorld;
	float	lookAheadTimeObjectsVsObject;
	float	minFrictionMass;
	float	maxFrictionMass;

	void Defaults( )
	{
		maxCollisionsPerObjectPerTimestep = 6;
		maxCollisionChecksPerTimestep = 250;
		maxVelocity = k_flMaxVelocity;
		maxAngularVelocity = k_flMaxAngularVelocity;
		lookAheadTimeObjectsVsWorld = 1.0f;
		lookAheadTimeObjectsVsObject = 0.5f;
		minFrictionMass = DEFAULT_MIN_FRICTION_MASS;
		maxFrictionMass = DEFAULT_MAX_FRICTION_MASS;
	}
};
struct virtualmeshparams_t;

//enum PhysInterfaceId_t;
struct physsaveparams_t;
struct physrestoreparams_t;
struct physprerestoreparams_t;

enum PhysInterfaceId_t
{
	PIID_UNKNOWN,
	PIID_IPHYSICSOBJECT,
	PIID_IPHYSICSFLUIDCONTROLLER,
	PIID_IPHYSICSSPRING,
	PIID_IPHYSICSCONSTRAINTGROUP,
	PIID_IPHYSICSCONSTRAINT,
	PIID_IPHYSICSSHADOWCONTROLLER,
	PIID_IPHYSICSPLAYERCONTROLLER,
	PIID_IPHYSICSMOTIONCONTROLLER,
	PIID_IPHYSICSVEHICLECONTROLLER,
	PIID_IPHYSICSGAMETRACE,

	PIID_NUM_TYPES
};

class ISave;
class IRestore;

class IVPhysicsDebugOverlay
{
public:
	virtual void AddEntityTextOverlay( int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char* format, ... ) = 0;
	virtual void AddBoxOverlay( const Vector& origin, const Vector& mins, const Vector& max, QAngle const& orientation, int r, int g, int b, int a, float duration ) = 0;
	virtual void AddTriangleOverlay( const Vector& p1, const Vector& p2, const Vector& p3, int r, int g, int b, int a, bool noDepthTest, float duration ) = 0;
	virtual void AddLineOverlay( const Vector& origin, const Vector& dest, int r, int g, int b, bool noDepthTest, float duration ) = 0;
	virtual void AddTextOverlay( const Vector& origin, float duration, const char* format, ... ) = 0;
	virtual void AddTextOverlay( const Vector& origin, int line_offset, float duration, const char* format, ... ) = 0;
	virtual void AddScreenTextOverlay( float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char* text ) = 0;
	virtual void AddSweptBoxOverlay( const Vector& start, const Vector& end, const Vector& mins, const Vector& max, const QAngle& angles, int r, int g, int b, int a, float flDuration ) = 0;
	virtual void AddTextOverlayRGB( const Vector& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char* format, ... ) = 0;
};

class IPhysics : public IAppSystem
{
public:
	virtual	IPhysicsEnvironment* CreateEnvironment( void ) = 0;
	virtual void DestroyEnvironment( IPhysicsEnvironment* ) = 0;
	virtual IPhysicsEnvironment* GetActiveEnvironmentByIndex( int index ) = 0;
	virtual IPhysicsObjectPairHash* CreateObjectPairHash( ) = 0;
	virtual void						DestroyObjectPairHash( IPhysicsObjectPairHash* pHash ) = 0;
	virtual IPhysicsCollisionSet* FindOrCreateCollisionSet( unsigned int id, int maxElementCount ) = 0;
	virtual IPhysicsCollisionSet* FindCollisionSet( unsigned int id ) = 0;
	virtual void						DestroyAllCollisionSets( ) = 0;
};

class CPhysConvex;
class CPhysPolysoup;
class ICollisionQuery;
class IVPhysicsKeyParser;
struct convertconvexparams_t;
class CPackedPhysicsDescription;

class CPolyhedron;

struct truncatedcone_t
{
	Vector	origin;
	Vector	normal;
	float	h;
	float	theta;
};

class IPhysicsCollision
{
public:
	virtual ~IPhysicsCollision( void ) {}

	virtual CPhysConvex* ConvexFromVerts( Vector** pVerts, int vertCount ) = 0;
	virtual CPhysConvex* ConvexFromPlanes( float* pPlanes, int planeCount, float mergeDistance ) = 0;
	virtual float			ConvexVolume( CPhysConvex* pConvex ) = 0;
	virtual float			ConvexSurfaceArea( CPhysConvex* pConvex ) = 0;
	virtual void			SetConvexGameData( CPhysConvex* pConvex, unsigned int gameData ) = 0;
	virtual void			ConvexFree( CPhysConvex* pConvex ) = 0;
	virtual CPhysConvex* BBoxToConvex( const Vector& mins, const Vector& maxs ) = 0;
	virtual CPhysConvex* ConvexFromConvexPolyhedron( const CPolyhedron& ConvexPolyhedron ) = 0;
	virtual void			ConvexesFromConvexPolygon( const Vector& vPolyNormal, const Vector* pPoints, int iPointCount, CPhysConvex** pOutput ) = 0;
	virtual CPhysPolysoup* PolysoupCreate( void ) = 0;
	virtual void			PolysoupDestroy( CPhysPolysoup* pSoup ) = 0;
	virtual void			PolysoupAddTriangle( CPhysPolysoup* pSoup, const Vector& a, const Vector& b, const Vector& c, int materialIndex7bits ) = 0;
	virtual CPhysCollide* ConvertPolysoupToCollide( CPhysPolysoup* pSoup, bool useMOPP ) = 0;
	virtual CPhysCollide* ConvertConvexToCollide( CPhysConvex** pConvex, int convexCount ) = 0;
	virtual CPhysCollide* ConvertConvexToCollideParams( CPhysConvex** pConvex, int convexCount, const convertconvexparams_t& convertParams ) = 0;
	virtual void			DestroyCollide( CPhysCollide* pCollide ) = 0;
	virtual int				CollideSize( CPhysCollide* pCollide ) = 0;
	virtual int				CollideWrite( char* pDest, CPhysCollide* pCollide, bool bSwap = false ) = 0;
	virtual CPhysCollide* UnserializeCollide( char* pBuffer, int size, int index ) = 0;
	virtual float			CollideVolume( CPhysCollide* pCollide ) = 0;
	virtual float			CollideSurfaceArea( CPhysCollide* pCollide ) = 0;
	virtual Vector			CollideGetExtent( const CPhysCollide* pCollide, const Vector& collideOrigin, const QAngle& collideAngles, const Vector& direction ) = 0;
	virtual void			CollideGetAABB( Vector* pMins, Vector* pMaxs, const CPhysCollide* pCollide, const Vector& collideOrigin, const QAngle& collideAngles ) = 0;
	virtual void			CollideGetMassCenter( CPhysCollide* pCollide, Vector* pOutMassCenter ) = 0;
	virtual void			CollideSetMassCenter( CPhysCollide* pCollide, const Vector& massCenter ) = 0;
	virtual Vector			CollideGetOrthographicAreas( const CPhysCollide* pCollide ) = 0;
	virtual void			CollideSetOrthographicAreas( CPhysCollide* pCollide, const Vector& areas ) = 0;
	virtual int				CollideIndex( const CPhysCollide* pCollide ) = 0;
	virtual CPhysCollide* BBoxToCollide( const Vector& mins, const Vector& maxs ) = 0;
	virtual int				GetConvexesUsedInCollideable( const CPhysCollide* pCollideable, CPhysConvex** pOutputArray, int iOutputArrayLimit ) = 0;
	virtual void TraceBox( const Vector& start, const Vector& end, const Vector& mins, const Vector& maxs, const CPhysCollide* pCollide, const Vector& collideOrigin, const QAngle& collideAngles, trace_t* ptr ) = 0;
	virtual void TraceBox( const Ray_t& ray, const CPhysCollide* pCollide, const Vector& collideOrigin, const QAngle& collideAngles, trace_t* ptr ) = 0;
	virtual void TraceBox( const Ray_t& ray, unsigned int contentsMask, IConvexInfo* pConvexInfo, const CPhysCollide* pCollide, const Vector& collideOrigin, const QAngle& collideAngles, trace_t* ptr ) = 0;
	virtual void TraceCollide( const Vector& start, const Vector& end, const CPhysCollide* pSweepCollide, const QAngle& sweepAngles, const CPhysCollide* pCollide, const Vector& collideOrigin, const QAngle& collideAngles, trace_t* ptr ) = 0;
	virtual bool			IsBoxIntersectingCone( const Vector& boxAbsMins, const Vector& boxAbsMaxs, const truncatedcone_t& cone ) = 0;
	virtual void			VCollideLoad( vcollide_t* pOutput, int solidCount, const char* pBuffer, int size, bool swap = false ) = 0;
	virtual void			VCollideUnload( vcollide_t* pVCollide ) = 0;
	virtual IVPhysicsKeyParser* VPhysicsKeyParserCreate( const char* pKeyData ) = 0;
	virtual void			VPhysicsKeyParserDestroy( IVPhysicsKeyParser* pParser ) = 0;
	virtual int				CreateDebugMesh( CPhysCollide const* pCollisionModel, Vector** outVerts ) = 0;
	virtual void			DestroyDebugMesh( int vertCount, Vector* outVerts ) = 0;
	virtual ICollisionQuery* CreateQueryModel( CPhysCollide* pCollide ) = 0;
	virtual void			DestroyQueryModel( ICollisionQuery* pQuery ) = 0;
	virtual IPhysicsCollision* ThreadContextCreate( void ) = 0;
	virtual void			ThreadContextDestroy( IPhysicsCollision* pThreadContex ) = 0;
	virtual CPhysCollide* CreateVirtualMesh( const virtualmeshparams_t& params ) = 0;
	virtual bool			SupportsVirtualMesh( ) = 0;
	virtual bool			GetBBoxCacheSize( int* pCachedSize, int* pCachedCount ) = 0;
	virtual CPolyhedron* PolyhedronFromConvex( CPhysConvex* const pConvex, bool bUseTempPolyhedron ) = 0;
	virtual void			OutputDebugInfo( const CPhysCollide* pCollide ) = 0;
	virtual unsigned int	ReadStat( int statID ) = 0;
};

class ICollisionQuery
{
public:
	virtual ~ICollisionQuery( ) {}
	virtual int		ConvexCount( void ) = 0;
	virtual int		TriangleCount( int convexIndex ) = 0;
	virtual unsigned int GetGameData( int convexIndex ) = 0;
	virtual void	GetTriangleVerts( int convexIndex, int triangleIndex, Vector* verts ) = 0;
	virtual void	SetTriangleVerts( int convexIndex, int triangleIndex, const Vector* verts ) = 0;
	virtual int		GetTriangleMaterialIndex( int convexIndex, int triangleIndex ) = 0;
	virtual void	SetTriangleMaterialIndex( int convexIndex, int triangleIndex, int index7bits ) = 0;
};

class IPhysicsGameTrace
{
public:
	virtual void VehicleTraceRay( const Ray_t& ray, void* pVehicle, trace_t* pTrace ) = 0;
	virtual	void VehicleTraceRayWithWater( const Ray_t& ray, void* pVehicle, trace_t* pTrace ) = 0;
	virtual bool VehiclePointInWater( const Vector& vecPoint ) = 0;
};

class IConvexInfo
{
public:
	virtual unsigned int GetContents( int convexGameData ) = 0;
};

class CPhysicsEventHandler;
class IPhysicsCollisionData
{
public:
	virtual void GetSurfaceNormal( Vector& out ) = 0;
	virtual void GetContactPoint( Vector& out ) = 0;
	virtual void GetContactSpeed( Vector& out ) = 0;
};


struct vcollisionevent_t
{
	IPhysicsObject* pObjects[ 2 ];
	int				surfaceProps[ 2 ];
	bool			isCollision;
	bool			isShadowCollision;
	float			deltaCollisionTime;
	float			collisionSpeed;
	IPhysicsCollisionData* pInternalData;
};

class IPhysicsCollisionEvent
{
public:
	virtual void PreCollision( vcollisionevent_t* pEvent ) = 0;
	virtual void PostCollision( vcollisionevent_t* pEvent ) = 0;
	virtual void Friction( IPhysicsObject* pObject, float energy, int surfaceProps, int surfacePropsHit, IPhysicsCollisionData* pData ) = 0;
	virtual void StartTouch( IPhysicsObject* pObject1, IPhysicsObject* pObject2, IPhysicsCollisionData* pTouchData ) = 0;
	virtual void EndTouch( IPhysicsObject* pObject1, IPhysicsObject* pObject2, IPhysicsCollisionData* pTouchData ) = 0;
	virtual void FluidStartTouch( IPhysicsObject* pObject, IPhysicsFluidController* pFluid ) = 0;
	virtual void FluidEndTouch( IPhysicsObject* pObject, IPhysicsFluidController* pFluid ) = 0;
	virtual void PostSimulationFrame( ) = 0;
	virtual void ObjectEnterTrigger( IPhysicsObject* pTrigger, IPhysicsObject* pObject ) {}
	virtual void ObjectLeaveTrigger( IPhysicsObject* pTrigger, IPhysicsObject* pObject ) {}
};


class IPhysicsObjectEvent
{
public:
	virtual void ObjectWake( IPhysicsObject* pObject ) = 0;
	virtual void ObjectSleep( IPhysicsObject* pObject ) = 0;
};

class IPhysicsConstraintEvent
{
public:
	virtual void ConstraintBroken( IPhysicsConstraint* ) = 0;
};

struct hlshadowcontrol_params_t
{
	Vector			targetPosition;
	QAngle			targetRotation;
	float			maxAngular;
	float			maxDampAngular;
	float			maxSpeed;
	float			maxDampSpeed;
	float			dampFactor;
	float			teleportDistance;
};

class IPhysicsShadowController
{
public:
	virtual ~IPhysicsShadowController( void ) {}

	virtual void Update( const Vector& position, const QAngle& angles, float timeOffset ) = 0;
	virtual void MaxSpeed( float maxSpeed, float maxAngularSpeed ) = 0;
	virtual void StepUp( float height ) = 0;
	virtual void SetTeleportDistance( float teleportDistance ) = 0;
	virtual bool AllowsTranslation( ) = 0;
	virtual bool AllowsRotation( ) = 0;
	virtual void SetPhysicallyControlled( bool isPhysicallyControlled ) = 0;
	virtual bool IsPhysicallyControlled( ) = 0;
	virtual void GetLastImpulse( Vector* pOut ) = 0;
	virtual void UseShadowMaterial( bool bUseShadowMaterial ) = 0;
	virtual void ObjectMaterialChanged( int materialIndex ) = 0;
	virtual float GetTargetPosition( Vector* pPositionOut, QAngle* pAnglesOut ) = 0;
	virtual float GetTeleportDistance( void ) = 0;
	virtual void GetMaxSpeed( float* pMaxSpeedOut, float* pMaxAngularSpeedOut ) = 0;
};

class CPhysicsObject : public IPhysicsObject {
public:
	void* GameData;
	void* Object;
	const CPhysCollide* Collide;
	IPhysicsShadowController* Shadow;
	Vector DragBasis;
	Vector AngularDragBasis;
	bool ShadowTempDisable : 5;
	bool HasTouchedDynamic : 1;
	bool AsleepSinceCreation : 1;
	bool ForceSilentDelete : 1;
	unsigned char SleepState : 2;
	unsigned char HingedAxis : 3;
	unsigned char CollideType : 3;
	unsigned short GameIndex;
	unsigned short MaterialIndex;
	unsigned short ActiveIndex;
	unsigned short Callback;
	unsigned short GameFlags;
	unsigned int ContentsMask;
	float Volume;
	float BuoyancyRatio;
	float DragCoEfficient;
	float AngularDragCoEfficient;
};

inline const float k_MaxVelocity = 2000.0f;
inline const float k_MaxAngularVelocity = 360.0f * 10.0f;

class IPhysicsMotionController;
class IMotionEvent
{
public:
	enum simresult_e { SIM_NOTHING = 0, SIM_LOCAL_ACCELERATION, SIM_LOCAL_FORCE, SIM_GLOBAL_ACCELERATION, SIM_GLOBAL_FORCE };
	virtual simresult_e	Simulate( IPhysicsMotionController* pController, IPhysicsObject* pObject, float deltaTime, Vector& linear, AngularImpulse& angular ) = 0;
};

class IPhysicsMotionController
{
public:
	virtual ~IPhysicsMotionController( void ) {}
	virtual void SetEventHandler( IMotionEvent* handler ) = 0;
	virtual void AttachObject( IPhysicsObject* pObject, bool checkIfAlreadyAttached ) = 0;
	virtual void DetachObject( IPhysicsObject* pObject ) = 0;
	virtual int CountObjects( void ) = 0;
	virtual void GetObjects( IPhysicsObject** pObjectList ) = 0;
	virtual void ClearObjects( void ) = 0;
	virtual void WakeObjects( void ) = 0;

	enum priority_t
	{
		LOW_PRIORITY = 0,
		MEDIUM_PRIORITY = 1,
		HIGH_PRIORITY = 2,
	};
	virtual void SetPriority( priority_t priority ) = 0;
};

class IPhysicsCollisionSolver
{
public:
	virtual int ShouldCollide( IPhysicsObject* pObj0, IPhysicsObject* pObj1, void* pGameData0, void* pGameData1 ) = 0;
	virtual int ShouldSolvePenetration( IPhysicsObject* pObj0, IPhysicsObject* pObj1, void* pGameData0, void* pGameData1, float dt ) = 0;
	virtual bool ShouldFreezeObject( IPhysicsObject* pObject ) = 0;
	virtual int AdditionalCollisionChecksThisTick( int currentChecksDone ) = 0;
	virtual bool ShouldFreezeContacts( IPhysicsObject** pObjectList, int objectCount ) = 0;
};

enum PhysicsTraceType_t
{
	VPHYSICS_TRACE_EVERYTHING = 0,
	VPHYSICS_TRACE_STATIC_ONLY,
	VPHYSICS_TRACE_MOVING_ONLY,
	VPHYSICS_TRACE_TRIGGERS_ONLY,
	VPHYSICS_TRACE_STATIC_AND_MOVING,
};

class IPhysicsTraceFilter
{
public:
	virtual bool ShouldHitObject( IPhysicsObject* pObject, int contentsMask ) = 0;
	virtual PhysicsTraceType_t	GetTraceType( ) const = 0;
};

class IPhysicsEnvironment
{
public:
	virtual ~IPhysicsEnvironment( void ) {}

	virtual void SetDebugOverlay( CreateInterfaceFn debugOverlayFactory ) = 0;
	virtual IVPhysicsDebugOverlay* GetDebugOverlay( void ) = 0;
	virtual void			SetGravity( const Vector& gravityVector ) = 0;
	virtual void			GetGravity( Vector* pGravityVector ) const = 0;
	virtual void			SetAirDensity( float density ) = 0;
	virtual float			GetAirDensity( void ) const = 0;
	virtual IPhysicsObject* CreatePolyObject( const CPhysCollide* pCollisionModel, int materialIndex, const Vector& position, const QAngle& angles, objectparams_t* pParams ) = 0;
	virtual IPhysicsObject* CreatePolyObjectStatic( const CPhysCollide* pCollisionModel, int materialIndex, const Vector& position, const QAngle& angles, objectparams_t* pParams ) = 0;
	virtual IPhysicsObject* CreateSphereObject( float radius, int materialIndex, const Vector& position, const QAngle& angles, objectparams_t* pParams, bool isStatic ) = 0;
	virtual void DestroyObject( IPhysicsObject* ) = 0;
	virtual IPhysicsFluidController* CreateFluidController( IPhysicsObject* pFluidObject, fluidparams_t* pParams ) = 0;
	virtual void DestroyFluidController( IPhysicsFluidController* ) = 0;
	virtual IPhysicsSpring* CreateSpring( IPhysicsObject* pObjectStart, IPhysicsObject* pObjectEnd, springparams_t* pParams ) = 0;
	virtual void DestroySpring( IPhysicsSpring* ) = 0;
	virtual IPhysicsConstraint* CreateRagdollConstraint( IPhysicsObject* pReferenceObject, IPhysicsObject* pAttachedObject, IPhysicsConstraintGroup* pGroup, const constraint_ragdollparams_t& ragdoll ) = 0;
	virtual IPhysicsConstraint* CreateHingeConstraint( IPhysicsObject* pReferenceObject, IPhysicsObject* pAttachedObject, IPhysicsConstraintGroup* pGroup, const constraint_hingeparams_t& hinge ) = 0;
	virtual IPhysicsConstraint* CreateFixedConstraint( IPhysicsObject* pReferenceObject, IPhysicsObject* pAttachedObject, IPhysicsConstraintGroup* pGroup, const constraint_fixedparams_t& fixed ) = 0;
	virtual IPhysicsConstraint* CreateSlidingConstraint( IPhysicsObject* pReferenceObject, IPhysicsObject* pAttachedObject, IPhysicsConstraintGroup* pGroup, const constraint_slidingparams_t& sliding ) = 0;
	virtual IPhysicsConstraint* CreateBallsocketConstraint( IPhysicsObject* pReferenceObject, IPhysicsObject* pAttachedObject, IPhysicsConstraintGroup* pGroup, const constraint_ballsocketparams_t& ballsocket ) = 0;
	virtual IPhysicsConstraint* CreatePulleyConstraint( IPhysicsObject* pReferenceObject, IPhysicsObject* pAttachedObject, IPhysicsConstraintGroup* pGroup, const constraint_pulleyparams_t& pulley ) = 0;
	virtual IPhysicsConstraint* CreateLengthConstraint( IPhysicsObject* pReferenceObject, IPhysicsObject* pAttachedObject, IPhysicsConstraintGroup* pGroup, const constraint_lengthparams_t& length ) = 0;
	virtual void DestroyConstraint( IPhysicsConstraint* ) = 0;
	virtual IPhysicsConstraintGroup* CreateConstraintGroup( const constraint_groupparams_t& groupParams ) = 0;
	virtual void DestroyConstraintGroup( IPhysicsConstraintGroup* pGroup ) = 0;
	virtual IPhysicsShadowController* CreateShadowController( IPhysicsObject* pObject, bool allowTranslation, bool allowRotation ) = 0;
	virtual void						DestroyShadowController( IPhysicsShadowController* ) = 0;
	virtual IPhysicsPlayerController* CreatePlayerController( IPhysicsObject* pObject ) = 0;
	virtual void						DestroyPlayerController( IPhysicsPlayerController* ) = 0;
	virtual IPhysicsMotionController* CreateMotionController( IMotionEvent* pHandler ) = 0;
	virtual void						DestroyMotionController( IPhysicsMotionController* pController ) = 0;
	virtual IPhysicsVehicleController* CreateVehicleController( IPhysicsObject* pVehicleBodyObject, const vehicleparams_t& params, unsigned int nVehicleType, IPhysicsGameTrace* pGameTrace ) = 0;
	virtual void						DestroyVehicleController( IPhysicsVehicleController* ) = 0;
	virtual void			SetCollisionSolver( IPhysicsCollisionSolver* pSolver ) = 0;
	virtual void			Simulate( float deltaTime ) = 0;
	virtual bool			IsInSimulation( ) const = 0;
	virtual float			GetSimulationTimestep( ) const = 0;
	virtual void			SetSimulationTimestep( float timestep ) = 0;
	virtual float			GetSimulationTime( ) const = 0;
	virtual void			ResetSimulationClock( ) = 0;
	virtual float			GetNextFrameTime( void ) const = 0;
	virtual void			SetCollisionEventHandler( IPhysicsCollisionEvent* pCollisionEvents ) = 0;
	virtual void			SetObjectEventHandler( IPhysicsObjectEvent* pObjectEvents ) = 0;
	virtual	void			SetConstraintEventHandler( IPhysicsConstraintEvent* pConstraintEvents ) = 0;
	virtual void			SetQuickDelete( bool bQuick ) = 0;
	virtual int				GetActiveObjectCount( ) const = 0;
	virtual void			GetActiveObjects( IPhysicsObject** pOutputObjectList ) const = 0;
	virtual const IPhysicsObject** GetObjectList( int* pOutputObjectCount ) const = 0;
	virtual bool			TransferObject( IPhysicsObject* pObject, IPhysicsEnvironment* pDestinationEnvironment ) = 0;
	virtual void			CleanupDeleteList( void ) = 0;
	virtual void			EnableDeleteQueue( bool enable ) = 0;
	virtual bool			Save( const physsaveparams_t& params ) = 0;
	virtual void			PreRestore( const physprerestoreparams_t& params ) = 0;
	virtual bool			Restore( const physrestoreparams_t& params ) = 0;
	virtual void			PostRestore( ) = 0;
	virtual bool IsCollisionModelUsed( CPhysCollide* pCollide ) const = 0;
	virtual void TraceRay( const Ray_t& ray, unsigned int fMask, IPhysicsTraceFilter* pTraceFilter, trace_t* pTrace ) = 0;
	virtual void SweepCollideable( const CPhysCollide* pCollide, const Vector& vecAbsStart, const Vector& vecAbsEnd,
								   const QAngle& vecAngles, unsigned int fMask, IPhysicsTraceFilter* pTraceFilter, trace_t* pTrace ) = 0;
	virtual void GetPerformanceSettings( physics_performanceparams_t* pOutput ) const = 0;
	virtual void SetPerformanceSettings( const physics_performanceparams_t* pSettings ) = 0;
	virtual void ReadStats( physics_stats_t* pOutput ) = 0;
	virtual void ClearStats( ) = 0;
	virtual unsigned int	GetObjectSerializeSize( IPhysicsObject* pObject ) const = 0;
	virtual void			SerializeObjectToBuffer( IPhysicsObject* pObject, unsigned char* pBuffer, unsigned int bufferSize ) = 0;
	virtual IPhysicsObject* UnserializeObjectFromBuffer( void* pGameData, unsigned char* pBuffer, unsigned int bufferSize, bool enableCollisions ) = 0;
	virtual void EnableConstraintNotify( bool bEnable ) = 0;
	virtual void DebugCheckContacts( void ) = 0;
};

const objectparams_t g_PhysDefaultObjectParams = {
	NULL,
	1.0,
	1.0,
	0.1f,
	0.1f,
	0.05f,
	"DEFAULT",
	NULL,
	0.f,
	1.0f,
	true,
};