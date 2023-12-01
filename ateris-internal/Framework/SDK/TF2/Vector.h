#pragma once

#include <math.h>
#include <cfloat>
#include <utility>
#define MAX_COORD_FLOAT				(16384.0f)
#define MIN_COORD_FLOAT				(-MAX_COORD_FLOAT)
#define M_RADPI		57.295779513082	
#define PI			3.14159265358979323846
#define DEG2RAD(x)	((float)(x) * (float)((float)(PI) / 180.0f))
#define RAD2DEG(x)	((float)(x) * (float)(180.0f / (float)(PI)))
#undef min
#undef max

typedef float vec_t;
inline unsigned long& FloatBits( vec_t& f )
{
	return *reinterpret_cast< unsigned long* >( &f );
}

inline unsigned long const& FloatBits( vec_t const& f )
{
	return *reinterpret_cast< unsigned long const* >( &f );
}

inline vec_t BitsToFloat( unsigned long i )
{
	return *reinterpret_cast< vec_t* >( &i );
}

inline bool IsFinite( vec_t f )
{
	return ( ( FloatBits( f ) & 0x7F800000 ) != 0x7F800000 );
}

inline unsigned long FloatAbsBits( vec_t f )
{
	return FloatBits( f ) & 0x7FFFFFFF;
}

class Vector2D
{
public:
	float x, y;

	Vector2D( void )
	{
		x = y = 0.0f;
	}

	Vector2D( float X, float Y )
	{
		x = X; y = Y;
	}

	Vector2D( float* v )
	{
		x = v[ 0 ]; y = v[ 1 ];
	}

	Vector2D( const float* v )
	{
		x = v[ 0 ]; y = v[ 1 ];
	}

	Vector2D( const Vector2D& v )
	{
		x = v.x; y = v.y;
	}

	Vector2D& operator=( const Vector2D& v )
	{
		x = v.x; y = v.y; return *this;
	}

	float& operator[]( int i )
	{
		return ( ( float* )this )[ i ];
	}

	float operator[]( int i ) const
	{
		return ( ( float* )this )[ i ];
	}

	Vector2D& operator+=( const Vector2D& v )
	{
		x += v.x; y += v.y; return *this;
	}

	Vector2D& operator-=( const Vector2D& v )
	{
		x -= v.x; y -= v.y; return *this;
	}

	Vector2D& operator*=( const Vector2D& v )
	{
		x *= v.x; y *= v.y; return *this;
	}

	Vector2D& operator/=( const Vector2D& v )
	{
		x /= v.x; y /= v.y; return *this;
	}

	Vector2D& operator+=( float v )
	{
		x += v; y += v; return *this;
	}

	Vector2D& operator-=( float v )
	{
		x -= v; y -= v; return *this;
	}

	Vector2D& operator*=( float v )
	{
		x *= v; y *= v; return *this;
	}

	Vector2D& operator/=( float v )
	{
		x /= v; y /= v; return *this;
	}

	Vector2D operator+( const Vector2D& v ) const
	{
		return Vector2D( x + v.x, y + v.y );
	}

	Vector2D operator-( const Vector2D& v ) const
	{
		return Vector2D( x - v.x, y - v.y );
	}

	Vector2D operator*( const Vector2D& v ) const
	{
		return Vector2D( x * v.x, y * v.y );
	}

	Vector2D operator/( const Vector2D& v ) const
	{
		return Vector2D( x / v.x, y / v.y );
	}

	Vector2D operator+( float v ) const
	{
		return Vector2D( x + v, y + v );
	}

	Vector2D operator-( float v ) const
	{
		return Vector2D( x - v, y - v );
	}

	Vector2D operator*( float v ) const
	{
		return Vector2D( x * v, y * v );
	}

	Vector2D operator/( float v ) const
	{
		return Vector2D( x / v, y / v );
	}

	bool IsValid( )
	{
		return IsFinite( x ) && IsFinite( y );
	}

	void Set( float X = 0.0f, float Y = 0.0f )
	{
		x = X; y = Y;
	}

	float Length( void ) const
	{
		return sqrtf( x * x + y * y );
	}

	float LengthSqr( void ) const
	{
		return ( x * x + y * y );
	}

	float DistTo( const Vector2D& v ) const
	{
		return ( *this - v ).Length( );
	}

	float DistToSqr( const Vector2D& v ) const
	{
		return ( *this - v ).LengthSqr( );
	}

	float Dot( const Vector2D& v ) const
	{
		return ( x * v.x + y * v.y );
	}

	bool IsZero( void ) const
	{
		return ( x > -0.01f && x < 0.01f &&
				 y > -0.01f && y < 0.01f );
	}
};

enum struct EWeaponType
{
	UNKNOWN, HITSCAN, PROJECTILE, MELEE
};

class Vector
{
public:
	float x, y, z;

public:
	void Zero()
	{
		x = y = z = 0.f;
	}

	__forceinline void Clear( )
	{
		x = y = z = 0.f;
	}

	Vector( float X = 0.0f, float Y = 0.0f, float Z = 0.0f )
	{
		x = X; y = Y; z = Z;
	}

	Vector( float* v )
	{
		x = v[ 0 ]; y = v[ 1 ]; z = v[ 2 ];
	}

	Vector( const float* v )
	{
		x = v[ 0 ]; y = v[ 1 ]; z = v[ 2 ];
	}

	Vector( const Vector& v )
	{
		x = v.x; y = v.y; z = v.z;
	}

	Vector( const Vector2D& v )
	{
		x = v.x; y = v.y; z = 0.0f;
	}

	Vector& operator=( const Vector& v )
	{
		x = v.x; y = v.y; z = v.z; return *this;
	}

	Vector& operator=( const Vector2D& v )
	{
		x = v.x; y = v.y; z = 0.0f; return *this;
	}

	float& operator[]( int i )
	{
		return ( ( float* )this )[ i ];
	}

	float operator[]( int i ) const
	{
		return ( ( float* )this )[ i ];
	}

	Vector& operator+=( const Vector& v )
	{
		x += v.x; y += v.y; z += v.z; return *this;
	}

	Vector& operator-=( const Vector& v )
	{
		x -= v.x; y -= v.y; z -= v.z; return *this;
	}

	Vector& operator*=( const Vector& v )
	{
		x *= v.x; y *= v.y; z *= v.z; return *this;
	}

	Vector& operator/=( const Vector& v )
	{
		x /= v.x; y /= v.y; z /= v.z; return *this;
	}

	Vector& operator+=( float v )
	{
		x += v; y += v; z += v; return *this;
	}

	Vector& operator-=( float v )
	{
		x -= v; y -= v; z -= v; return *this;
	}

	Vector& operator*=( float v )
	{
		x *= v; y *= v; z *= v; return *this;
	}

	Vector& operator/=( float v )
	{
		x /= v; y /= v; z /= v; return *this;
	}

	bool operator<=( const Vector other ) const
	{
		return fabsf( x ) <= other.x &&
			fabsf( y ) <= other.y &&
			fabsf( z ) <= other.z;
	}
	bool operator>=( const Vector other ) const
	{
		return fabsf( x ) >= other.x ||
			fabsf( y ) >= other.y ||
			fabsf( z ) >= other.z;
	}

	Vector operator+( const Vector& v ) const
	{
		return Vector( x + v.x, y + v.y, z + v.z );
	}

	Vector operator-( const Vector& v ) const
	{
		return Vector( x - v.x, y - v.y, z - v.z );
	}

	Vector operator*( const Vector& v ) const
	{
		return Vector( x * v.x, y * v.y, z * v.z );
	}

	Vector operator/( const Vector& v ) const
	{
		return Vector( x / v.x, y / v.y, z / v.z );
	}

	Vector operator+( float v ) const
	{
		return Vector( x + v, y + v, z + v );
	}

	Vector operator-( float v ) const
	{
		return Vector( x - v, y - v, z - v );
	}

	Vector operator*( float v ) const
	{
		return Vector( x * v, y * v, z * v );
	}

	Vector operator/( float v ) const
	{
		return Vector( x / v, y / v, z / v );
	}

	bool operator==( const Vector& v ) const
	{
		return ( this->x == v.x && this->y == v.y && this->z == v.z );
	}

	bool operator<=( const float other ) const
	{
		return fabsf( x ) <= other &&
			fabsf( y ) <= other &&
			fabsf( z ) <= other;
	}

	bool IsValid( )
	{
		return IsFinite( x ) && IsFinite( y ) && IsFinite( z );
	}

	void Set( float X = 0.0f, float Y = 0.0f, float Z = 0.0f )
	{
		x = X; y = Y; z = Z;
	}

	float Length( void ) const
	{
		return sqrtf( x * x + y * y + z * z );
	}

	float LengthSqr( void ) const
	{
		return ( x * x + y * y + z * z );
	}

	float Length2D( void ) const
	{
		return sqrtf( x * x + y * y );
	}

	float Length2DSqr( void ) const
	{
		return ( x * x + y * y );
	}

	float DistTo( const Vector& v ) const
	{
		return ( *this - v ).Length( );
	}

	float DistToSqr( const Vector& v ) const
	{
		return ( *this - v ).LengthSqr( );
	}

	float Dist2D( const Vector& v ) const
	{
		return ( *this - v ).Length2D( );
	}

	void CrossProduct( const Vector& b, Vector& result )
	{
		result.x = this->y * b.z - this->z * b.y;
		result.y = this->z * b.x - this->x * b.z;
		result.z = this->x * b.y - this->y * b.x;
	}

	Vector CrossProduct( const Vector& b ) const
	{
		return Vector( y * b.z - z * b.y, z * b.x - x * b.z, x * b.y - y * b.x );
	}

	float DotProduct( const Vector& v ) const
	{
		return ( x * v.x + y * v.y + z * v.z );
	}

	Vector Clamp( )
	{
		float pitch = x;
		float jaw = y;

		while ( jaw > 180.0f )
			jaw -= 360.0f;

		while ( jaw < -180.0f )
			jaw += 360.0f;

		return Vector( std::max( -89.f, std::min( 89.f, pitch ) ), jaw, 0.f );
	}

	Vector Look( Vector target ) const
	{
		target -= *this;
		Vector angles;
		if ( target.y == 0.0f && target.x == 0.0f )
		{
			angles.x = ( target.z > 0.0f ) ? 270.0f : 90.0f;
			angles.y = 0.0f;
		}
		else
		{
			angles.x = static_cast< float >( atan2( -target.z, target.Length2D( ) ) * 180.f / PI );
			angles.y = static_cast< float >( atan2( target.y, target.x ) * 180.f / PI );
		}

		angles.z = 0.0f;
		return angles;
	}

	Vector AngleTo( ) const
	{
		Vector angles;
		if ( y == 0.0f && x == 0.0f )
		{
			angles.x = ( z > 0.0f ) ? 270.0f : 90.0f;
			angles.y = 0.0f;
		}
		else
		{
			angles.x = static_cast< float >( atan2( -z, Length2D( ) ) * 180.f / PI );
			angles.y = static_cast< float >( atan2( y, x ) * 180.f / PI );
		}

		angles.z = 0.0f;
		return angles;
	}

	Vector Normalized( ) const
	{
		const float ln = Length( );
		if ( isnan( ln ) || ln == 0.f )
			return Vector( );
		Vector ret = *this;
		ret.x /= ln;
		ret.y /= ln;
		ret.z /= ln;
		return ret;
	};

	float Normalize( )
	{
		float fl_Length = Length( );
		float fl_Length_normal = 1.f / ( FLT_EPSILON + fl_Length );

		x = x * fl_Length_normal;
		y = y * fl_Length_normal;
		z = z * fl_Length_normal;

		return fl_Length;
	}

	float NormalizeInPlace( )
	{
		return Normalize( );
	}

	Vector VectorMA( float Scale, const Vector& direction )
	{
		Vector dest;
		dest.x = this->x + Scale * direction.x;
		dest.y = this->y + Scale * direction.y;
		dest.z = this->z + Scale * direction.z;
		return dest;
	}

	Vector ToAngle( ) const noexcept
	{
		return Vector{ RAD2DEG( atan2( -z, hypot( x, y ) ) ),
					 RAD2DEG( atan2( y, x ) ),
					 0.0f };
	}

	Vector FromAngle( ) const noexcept
	{
		return Vector{ ( float )( cos( DEG2RAD( x ) ) * cos( DEG2RAD( y ) ) ),
					 ( float )( cos( DEG2RAD( x ) ) * sin( DEG2RAD( y ) ) ),
					 ( float )( -sin( DEG2RAD( x ) ) ) };
	}

	void Rotate2D( const float& flYaw )
	{
		float s, c, r = DEG2RAD( flYaw );
		s = ( float )sin( r ); c = ( float )cos( r );

		float _x = x, _y = y;

		x = ( _x * c ) - ( _y * s );
		y = ( _x * s ) + ( _y * c );
	}

	bool IsZero( void ) const
	{
		return ( x > -0.01f && x < 0.01f &&
				 y > -0.01f && y < 0.01f &&
				 z > -0.01f && z < 0.01f );
	}

	Vector Scale( float fl )
	{
		return Vector( x * fl, y * fl, z * fl );
	}

	void Init( float ix = 0, float iy = 0, float iz = 0 )
	{
		x = ix; y = iy; z = iz;
	}

	void MultAdd( Vector& vOther, float fl )
	{
		x += fl * vOther.x;
		y += fl * vOther.y;
		z += fl * vOther.z;
	}
};

class Vector4D
{
public:
	float x, y, z, w;
};

struct Matrix3x4
{
public:
	Matrix3x4( ) {}
	Matrix3x4(
		float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23 )
	{
		matrix3x4[ 0 ][ 0 ] = m00;	matrix3x4[ 0 ][ 1 ] = m01; matrix3x4[ 0 ][ 2 ] = m02; matrix3x4[ 0 ][ 3 ] = m03;
		matrix3x4[ 1 ][ 0 ] = m10;	matrix3x4[ 1 ][ 1 ] = m11; matrix3x4[ 1 ][ 2 ] = m12; matrix3x4[ 1 ][ 3 ] = m13;
		matrix3x4[ 2 ][ 0 ] = m20;	matrix3x4[ 2 ][ 1 ] = m21; matrix3x4[ 2 ][ 2 ] = m22; matrix3x4[ 2 ][ 3 ] = m23;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	void Init( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin )
	{
		matrix3x4[ 0 ][ 0 ] = xAxis.x; matrix3x4[ 0 ][ 1 ] = yAxis.x; matrix3x4[ 0 ][ 2 ] = zAxis.x; matrix3x4[ 0 ][ 3 ] = vecOrigin.x;
		matrix3x4[ 1 ][ 0 ] = xAxis.y; matrix3x4[ 1 ][ 1 ] = yAxis.y; matrix3x4[ 1 ][ 2 ] = zAxis.y; matrix3x4[ 1 ][ 3 ] = vecOrigin.y;
		matrix3x4[ 2 ][ 0 ] = xAxis.z; matrix3x4[ 2 ][ 1 ] = yAxis.z; matrix3x4[ 2 ][ 2 ] = zAxis.z; matrix3x4[ 2 ][ 3 ] = vecOrigin.z;
	}

	//-----------------------------------------------------------------------------
	// Creates a matrix where the X axis = forward
	// the Y axis = left, and the Z axis = up
	//-----------------------------------------------------------------------------
	Matrix3x4( const Vector& xAxis, const Vector& yAxis, const Vector& zAxis, const Vector& vecOrigin )
	{
		Init( xAxis, yAxis, zAxis, vecOrigin );
	}

	inline void Invalidate( void )
	{
		for ( int i = 0; i < 3; i++ )
		{
			for ( int j = 0; j < 4; j++ )
			{
				matrix3x4[ i ][ j ] = 0x7FC00000;
			}
		}
	}

	float* operator[]( int i ) { return matrix3x4[ i ]; }
	const float* operator[]( int i ) const { return matrix3x4[ i ]; }
	float* Base( ) { return &matrix3x4[ 0 ][ 0 ]; }
	const float* Base( ) const { return &matrix3x4[ 0 ][ 0 ]; }

	float matrix3x4[ 3 ][ 4 ];
};

using QAngle = Vector;
using RadianEuler = Vector;
using VelocityImpulse = Vector;
using AngularImpulse = Vector;

inline float k_flMaxEntityPosCoord = MAX_COORD_FLOAT;
inline float k_flMaxEntityEulerAngle = 360.0 * 1000.0f;

inline bool IsEntityCoordinateReasonable( const vec_t c )
{
	float r = k_flMaxEntityPosCoord;
	return c > -r && c < r;
}

inline bool IsEntityQAngleReasonable( const QAngle& q )
{
	float r = k_flMaxEntityEulerAngle;
	return
		q.x > -r && q.x < r &&
		q.y > -r && q.y < r &&
		q.z > -r && q.z < r;
}