#pragma once
#include "../TF2/Vector.h"
#include "../../Framework.h"
#include "FNV1A.h"
#undef min
#undef max

class CMath
{
public:
	float NormalizeAngle(float ang)
	{
		if ( !std::isfinite( ang ) )
			return 0.0f;

		while ( ang > 180.0f )
			ang -= 360.0f;

		while ( ang < -180.0f )
			ang += 360.0f;

		return ang;
	}

	float NormalizeVector(Vector& v)
	{
		float len = v.Length();
		if (!len) v.Set(); else v /= len;
		return len;
	}

	void ClampAngles(Vector& angles)
	{
		angles.x = std::max( -89.f, std::min( 89.f, NormalizeAngle( angles.x ) ) );
		angles.y = NormalizeAngle(angles.y);
		angles.z = 0.0f;
	}

	Vector CalcAngle(const Vector& source, const Vector& destination)
	{
		Vector angles = Vector(0.0f, 0.0f, 0.0f);
		Vector delta = (source - destination);
		float fHyp = float(sqrt((delta.x * delta.x) + (delta.y * delta.y)));

		angles.x = (atanf(delta.z / fHyp) * M_RADPI);
		angles.y = (atanf(delta.y / delta.x) * M_RADPI);
		angles.z = 0.0f;

		if (delta.x >= 0.0f)
			angles.y += 180.0f;

		return angles;
	}

	float CalcFov(const Vector& src, const Vector& dst)
	{
		Vector v_src = Vector();
		AngleVectors(src, &v_src);

		Vector v_dst = Vector();
		AngleVectors(dst, &v_dst);

		float result = RAD2DEG(acos(v_dst.DotProduct(v_src) / v_dst.Length2D()));

		if (!isfinite(result) || isinf(result) || isnan(result))
			result = 0.0f;

		return result;
	}

	float DotProduct(const Vector& a, const Vector& b) {
		return (a.x * b.x
			+ a.y * b.y
			+ a.z * b.z);
	}

	void VectorTransform(const Vector& input, const Matrix3x4& matrix, Vector& output)
	{
		output[0] = DotProduct(input, Vector(matrix[0][0], matrix[0][1], matrix[0][2])) + matrix[0][3];
		output[1] = DotProduct(input, Vector(matrix[1][0], matrix[1][1], matrix[1][2])) + matrix[1][3];
		output[2] = DotProduct(input, Vector(matrix[2][0], matrix[2][1], matrix[2][2])) + matrix[2][3];
	}

	void SinCos(float radians, float* sine, float* cosine)
	{
		_asm
		{
			fld		DWORD PTR[radians]
			fsincos

			mov edx, DWORD PTR[cosine]
			mov eax, DWORD PTR[sine]

			fstp DWORD PTR[edx]
			fstp DWORD PTR[eax]
		}
	}

	void AngleVectors(const Vector& angles, Vector* forward)
	{
		float sp, sy, cp, cy;

		SinCos( DEG2RAD( angles.x ), &sp, &cp );
		SinCos( DEG2RAD( angles.y ), &sy, &cy );

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}
	}

	void AngleVectors(const Vector& angles, Vector* forward, Vector* right, Vector* up)
	{
		float sr, sp, sy, cr, cp, cy;
		SinCos( DEG2RAD( angles.x ), &sp, &cp );
		SinCos( DEG2RAD( angles.y ), &sy, &cy );
		SinCos( DEG2RAD( angles.z ), &sr, &cr );

		if (forward)
		{
			forward->x = cp * cy;
			forward->y = cp * sy;
			forward->z = -sp;
		}

		if (right)
		{
			right->x = (-1 * sr * sp * cy + -1 * cr * -sy);
			right->y = (-1 * sr * sp * sy + -1 * cr * cy);
			right->z = -1 * sr * cp;
		}

		if (up)
		{
			up->x = (cr * sp * cy + -sr * -sy);
			up->y = (cr * sp * sy + -sr * cy);
			up->z = cr * cp;
		}
	}

	void VectorAngles( const Vector& forward, Vector& angles )
	{
		float tmp = 0.0f, yaw = 0.0f, pitch = 0.0f;

		if ( !forward.y && !forward.x )
		{
			yaw = 0.0f;

			if ( forward.z > 0.0f )
				pitch = 270.0f;
			else
				pitch = 90.0f;
		}

		else
		{
			yaw = RAD2DEG( atan2f( forward.y, forward.x ) );

			if ( yaw < 0.0f )
				yaw += 360.0f;

			tmp = forward.Length2D( );
			pitch = RAD2DEG( atan2f( -forward.z, tmp ) );

			if ( pitch < 0.0f )
				pitch += 360.0f;
		}

		angles[ 0 ] = pitch;
		angles[ 1 ] = yaw;
		angles[ 2 ] = 0.0f;
	}

	void AngleMatrix( const Vector& angles, Matrix3x4& matrix ) {
		float sr, sp, sy, cr, cp, cy;

		SinCos( DEG2RAD( angles.y ), &sy, &cy );
		SinCos( DEG2RAD( angles.x ), &sp, &cp );
		SinCos( DEG2RAD( angles.z ), &sr, &cr );

		matrix[ 0 ][ 0 ] = cp * cy;
		matrix[ 1 ][ 0 ] = cp * sy;
		matrix[ 2 ][ 0 ] = -sp;

		float crcy = cr * cy;
		float crsy = cr * sy;
		float srcy = sr * cy;
		float srsy = sr * sy;

		matrix[ 0 ][ 1 ] = sp * srcy - crsy;
		matrix[ 1 ][ 1 ] = sp * srsy + crcy;
		matrix[ 2 ][ 1 ] = sr * cp;

		matrix[ 0 ][ 2 ] = ( sp * crcy + srsy );
		matrix[ 1 ][ 2 ] = ( sp * crsy - srcy );
		matrix[ 2 ][ 2 ] = cr * cp;

		matrix[ 0 ][ 3 ] = 0.0f;
		matrix[ 1 ][ 3 ] = 0.0f;
		matrix[ 2 ][ 3 ] = 0.0f;
	}

	void MatrixSetColumn( const Vector& in, int column, Matrix3x4& out ) {
		out[ 0 ][ column ] = in.x;
		out[ 1 ][ column ] = in.y;
		out[ 2 ][ column ] = in.z;
	}

	inline float NormalizeRadians(float a) noexcept {
		return std::isfinite(a) ? std::remainder(a, PI * 2) : 0.0f;
	}

	float ComputeRadDiff(float a1, float a2)
	{
		double delta = NormalizeRadians(a1 - a2);

		if (a1 > a2) {
			if (delta >= PI)
				delta -= PI * 2;
		}

		else if (delta <= -PI)
			delta += PI * 2;

		return static_cast<float>(delta);
	}

	Vector VelocityToAngles(Vector& direction)
	{
		auto Magnitude = [&](const Vector& v) -> float {
			return sqrtf(v.DotProduct(v));
		};

		float yaw, pitch;

		if (direction.y == 0.0f && direction.x == 0.0f)
		{
			yaw = 0.0f;

			if (direction.z > 0.0f)
				pitch = 270.0f;

			else pitch = 90.0f;
		}

		else
		{
			yaw = RAD2DEG(atan2f(direction.y, direction.x));
			pitch = RAD2DEG(atan2f(-direction.z, Magnitude(Vector(direction))));

			if (yaw < 0.0f)
				yaw += 360.0f;

			if (pitch < 0.0f)
				pitch += 360.0f;
		}

		return { pitch, yaw, 0.0f };
	}

	float ReMapValClamped(float val, float a, float b, float c, float d) {
		if (a == b)
			return val >= b ? d : c;

		float val_ = (val - a) / (b - a);
		val_ = std::clamp(val_, 0.0f, 1.0f);

		return c + (d - c) * val_;
	}

	float GetFraction( const float val, const float max, const float min ) {
		return std::clamp( ( val - min ) / ( max - min ), 0.f, 1.f );
	}

	inline void MatrixCopy( const Matrix3x4& src, Matrix3x4& dst ) {
		if ( &src != &dst )
		{
			memcpy( dst.matrix3x4, src.matrix3x4, 12 * sizeof( float ) );
		}
	}

	inline void ConcatTransforms( const Matrix3x4& in1, const Matrix3x4& in2, Matrix3x4& out )
	{
		if ( &in1 == &out ) {
			Matrix3x4 in1b;
			MatrixCopy( in1, in1b );
			ConcatTransforms( in1b, in2, out );
			return;
		}

		if ( &in2 == &out ) {
			Matrix3x4 in2b;
			MatrixCopy( in2, in2b );
			ConcatTransforms( in1, in2b, out );
			return;
		}

		out[ 0 ][ 0 ] = in1[ 0 ][ 0 ] * in2[ 0 ][ 0 ] + in1[ 0 ][ 1 ] * in2[ 1 ][ 0 ] +
			in1[ 0 ][ 2 ] * in2[ 2 ][ 0 ];
		out[ 0 ][ 1 ] = in1[ 0 ][ 0 ] * in2[ 0 ][ 1 ] + in1[ 0 ][ 1 ] * in2[ 1 ][ 1 ] +
			in1[ 0 ][ 2 ] * in2[ 2 ][ 1 ];
		out[ 0 ][ 2 ] = in1[ 0 ][ 0 ] * in2[ 0 ][ 2 ] + in1[ 0 ][ 1 ] * in2[ 1 ][ 2 ] +
			in1[ 0 ][ 2 ] * in2[ 2 ][ 2 ];
		out[ 0 ][ 3 ] = in1[ 0 ][ 0 ] * in2[ 0 ][ 3 ] + in1[ 0 ][ 1 ] * in2[ 1 ][ 3 ] +
			in1[ 0 ][ 2 ] * in2[ 2 ][ 3 ] + in1[ 0 ][ 3 ];
		out[ 1 ][ 0 ] = in1[ 1 ][ 0 ] * in2[ 0 ][ 0 ] + in1[ 1 ][ 1 ] * in2[ 1 ][ 0 ] +
			in1[ 1 ][ 2 ] * in2[ 2 ][ 0 ];
		out[ 1 ][ 1 ] = in1[ 1 ][ 0 ] * in2[ 0 ][ 1 ] + in1[ 1 ][ 1 ] * in2[ 1 ][ 1 ] +
			in1[ 1 ][ 2 ] * in2[ 2 ][ 1 ];
		out[ 1 ][ 2 ] = in1[ 1 ][ 0 ] * in2[ 0 ][ 2 ] + in1[ 1 ][ 1 ] * in2[ 1 ][ 2 ] +
			in1[ 1 ][ 2 ] * in2[ 2 ][ 2 ];
		out[ 1 ][ 3 ] = in1[ 1 ][ 0 ] * in2[ 0 ][ 3 ] + in1[ 1 ][ 1 ] * in2[ 1 ][ 3 ] +
			in1[ 1 ][ 2 ] * in2[ 2 ][ 3 ] + in1[ 1 ][ 3 ];
		out[ 2 ][ 0 ] = in1[ 2 ][ 0 ] * in2[ 0 ][ 0 ] + in1[ 2 ][ 1 ] * in2[ 1 ][ 0 ] +
			in1[ 2 ][ 2 ] * in2[ 2 ][ 0 ];
		out[ 2 ][ 1 ] = in1[ 2 ][ 0 ] * in2[ 0 ][ 1 ] + in1[ 2 ][ 1 ] * in2[ 1 ][ 1 ] +
			in1[ 2 ][ 2 ] * in2[ 2 ][ 1 ];
		out[ 2 ][ 2 ] = in1[ 2 ][ 0 ] * in2[ 0 ][ 2 ] + in1[ 2 ][ 1 ] * in2[ 1 ][ 2 ] +
			in1[ 2 ][ 2 ] * in2[ 2 ][ 2 ];
		out[ 2 ][ 3 ] = in1[ 2 ][ 0 ] * in2[ 0 ][ 3 ] + in1[ 2 ][ 1 ] * in2[ 1 ][ 3 ] +
			in1[ 2 ][ 2 ] * in2[ 2 ][ 3 ] + in1[ 2 ][ 3 ];
	}

	inline void MatrixAngles( const Matrix3x4& matrix, Vector& angles ) {
		//Vec3 forward, left, up;

		// extract the basis vectors from the matrix. since we only need the z
		// component of the up vector, we don't get x and y.
		const Vector forward = { matrix[ 0 ][ 0 ], matrix[ 1 ][ 0 ], matrix[ 2 ][ 0 ] };
		const Vector left = { matrix[ 0 ][ 1 ], matrix[ 1 ][ 1 ], matrix[ 2 ][ 1 ] };
		const Vector up = { 0.f, 0.f, matrix[ 2 ][ 2 ] };

		float len = forward.Length2D( );

		// enough here to get angles?
		if ( len > 0.001f ) {
			angles.x = RAD2DEG( std::atan2( -forward.z, len ) );
			angles.y = RAD2DEG( std::atan2( forward.y, forward.x ) );
			angles.z = RAD2DEG( std::atan2( left.z, up.z ) );
		}

		else {
			angles.x = RAD2DEG( std::atan2( -forward.z, len ) );
			angles.y = RAD2DEG( std::atan2( -left.x, left.y ) );
			angles.z = 0.f;
		}
	}

	inline void GetMatrixOrigin( const Matrix3x4& source, Vector& target ) {
		target.x = source[ 0 ][ 3 ];
		target.y = source[ 1 ][ 3 ];
		target.z = source[ 2 ][ 3 ];
	}

	inline Vector GetRotatedPosition( Vector start, const float rotation, const float distance ) {
		const auto rad = RAD2DEG( rotation );
		start.x += cosf( rad ) * distance;
		start.y += sinf( rad ) * distance;
		return start;
	}

	typedef float VMatrixRaw_t[ 4 ];
	inline void MatrixMultiply( const Matrix3x4& src1, const Matrix3x4& src2, Matrix3x4& dst )
	{
		// Make sure it works if src1 == dst or src2 == dst
		Matrix3x4 tmp1, tmp2;
		const VMatrixRaw_t* s1 = ( &src1 == &dst ) ? tmp1.matrix3x4 : src1.matrix3x4;
		const VMatrixRaw_t* s2 = ( &src2 == &dst ) ? tmp2.matrix3x4 : src2.matrix3x4;

		if ( &src1 == &dst )
		{
			MatrixCopy( src1, tmp1 );
		}
		if ( &src2 == &dst )
		{
			MatrixCopy( src2, tmp2 );
		}

		dst[ 0 ][ 0 ] = s1[ 0 ][ 0 ] * s2[ 0 ][ 0 ] + s1[ 0 ][ 1 ] * s2[ 1 ][ 0 ] + s1[ 0 ][ 2 ] * s2[ 2 ][ 0 ];
		dst[ 0 ][ 1 ] = s1[ 0 ][ 0 ] * s2[ 0 ][ 1 ] + s1[ 0 ][ 1 ] * s2[ 1 ][ 1 ] + s1[ 0 ][ 2 ] * s2[ 2 ][ 1 ];
		dst[ 0 ][ 2 ] = s1[ 0 ][ 0 ] * s2[ 0 ][ 2 ] + s1[ 0 ][ 1 ] * s2[ 1 ][ 2 ] + s1[ 0 ][ 2 ] * s2[ 2 ][ 2 ];
		dst[ 0 ][ 3 ] = s1[ 0 ][ 0 ] * s2[ 0 ][ 3 ] + s1[ 0 ][ 1 ] * s2[ 1 ][ 3 ] + s1[ 0 ][ 2 ] * s2[ 2 ][ 3 ];

		dst[ 1 ][ 0 ] = s1[ 1 ][ 0 ] * s2[ 0 ][ 0 ] + s1[ 1 ][ 1 ] * s2[ 1 ][ 0 ] + s1[ 1 ][ 2 ] * s2[ 2 ][ 0 ];
		dst[ 1 ][ 1 ] = s1[ 1 ][ 0 ] * s2[ 0 ][ 1 ] + s1[ 1 ][ 1 ] * s2[ 1 ][ 1 ] + s1[ 1 ][ 2 ] * s2[ 2 ][ 1 ];
		dst[ 1 ][ 2 ] = s1[ 1 ][ 0 ] * s2[ 0 ][ 2 ] + s1[ 1 ][ 1 ] * s2[ 1 ][ 2 ] + s1[ 1 ][ 2 ] * s2[ 2 ][ 2 ];
		dst[ 1 ][ 3 ] = s1[ 1 ][ 0 ] * s2[ 0 ][ 3 ] + s1[ 1 ][ 1 ] * s2[ 1 ][ 3 ] + s1[ 1 ][ 2 ] * s2[ 2 ][ 3 ];

		dst[ 2 ][ 0 ] = s1[ 2 ][ 0 ] * s2[ 0 ][ 0 ] + s1[ 2 ][ 1 ] * s2[ 1 ][ 0 ] + s1[ 2 ][ 2 ] * s2[ 2 ][ 0 ];
		dst[ 2 ][ 1 ] = s1[ 2 ][ 0 ] * s2[ 0 ][ 1 ] + s1[ 2 ][ 1 ] * s2[ 1 ][ 1 ] + s1[ 2 ][ 2 ] * s2[ 2 ][ 1 ];
		dst[ 2 ][ 2 ] = s1[ 2 ][ 0 ] * s2[ 0 ][ 2 ] + s1[ 2 ][ 1 ] * s2[ 1 ][ 2 ] + s1[ 2 ][ 2 ] * s2[ 2 ][ 2 ];
		dst[ 2 ][ 3 ] = s1[ 2 ][ 0 ] * s2[ 0 ][ 3 ] + s1[ 2 ][ 1 ] * s2[ 1 ][ 3 ] + s1[ 2 ][ 2 ] * s2[ 2 ][ 3 ];
	}

	inline void VectorRotate( const Vector& in1, const Matrix3x4& in2, Vector& out )
	{
		for ( int i = 0; i < 3; i++ )
		{
			out[ i ] = in1.DotProduct( ( Vector& )in2.matrix3x4[ i ] );
		}
	}

	inline void VectorRotate( const Vector& in1, const Vector& in2, Vector& out )
	{
		Matrix3x4 matRotate;
		AngleMatrix( in2, matRotate );
		VectorRotate( in1, matRotate, out );
	}

	inline float AngleDiffRadians( float a1, float a2 ) noexcept
	{
		float delta;

		delta = NormalizeRadians( a1 - a2 );

		if ( a1 > a2 )
		{
			if ( delta >= PI )
			{
				delta -= static_cast< float >( PI * 2 );
			}
		}
		else
		{
			if ( delta <= -PI )
			{
				delta += static_cast< float >( PI * 2 );
			}
		}

		return delta;
	}
};

inline CMath* gMath = new CMath;