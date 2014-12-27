////////////////////////////////////////////////////////////////////////////////

#ifndef __vec3_h_included__
#define __vec3_h_included__

////////////////////////////////////////////////////////////////////////////////

class vec3
{
	public :

		inline vec3()
		{
		}

		inline vec3( scalar _x, scalar _y, scalar _z ) :
			x( _x ),
			y( _y ),
			z( _z )
		{
		}

		inline void Set( scalar _x, scalar _y, scalar _z )
		{
			x = _x;
			y = _y;
			z = _z;
		}

		inline void Add( const vec3 & vOffset )
		{
			x += vOffset.x;
			y += vOffset.y;
			z += vOffset.z;
		}

		inline void Sub( const vec3 & vOffset )
		{
			x -= vOffset.x;
			y -= vOffset.y;
			z -= vOffset.z;
		}

		inline void Scale( const float fScale )
		{
			x *= fScale;
			y *= fScale;
			z *= fScale;
		}

		inline float LengthSqr() const
		{
			return ( x * x + y * y + z * z );
		}

		inline float Length()
		{
			return sqrt( LengthSqr() );
		}

		inline void NormalizeTo( float fNorm )
		{
			const float fInvLen = fNorm / Length();

			x *= fInvLen;
			y *= fInvLen;
			z *= fInvLen;
		}

		inline void Normalize()
		{
			NormalizeTo( 1.0f );
		}

		inline void Rotate( const mat4 & mRot );
		inline void Multiply( const mat4 & mTrans );

		union
		{
			struct
			{
				scalar x, y, z;
			};

			struct
			{
				scalar s, t, r;
			};

			scalar pArray[ 3 ];
		};

		static vec3 vNull;
		static vec3 vOne;
};

inline vec3 operator * ( const vec3 & vA, float fScalar )
{
	return vec3(	vA.x * fScalar,
					vA.y * fScalar,
					vA.z * fScalar	);
}

inline vec3 operator / ( const vec3 & vA, float fScalar )
{
	return vec3(	vA.x / fScalar,
					vA.y / fScalar,
					vA.z / fScalar	);
}

inline vec3 operator + ( const vec3 & vA, const vec3 & vB )
{
	return vec3( vA.x + vB.x, vA.y + vB.y, vA.z + vB.z );
}

inline vec3 operator - ( const vec3 & vA, const vec3 & vB )
{
	return vec3( vA.x - vB.x, vA.y - vB.y, vA.z - vB.z );
}

inline float Dot( const vec3 & vA, const vec3 & vB )
{
	return ( vA.x * vB.x + vA.y * vB.y + vA.z * vB.z );
}

inline vec3 Cross( const vec3 & vA, const vec3 & vB )
{
	return vec3(	( vA.y * vB.z ) - ( vA.z * vB.y ),
					( vA.z * vB.x ) - ( vA.x * vB.z ),
					( vA.x * vB.y ) - ( vA.y * vB.x )	);
}

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __vec3_h_included__

////////////////////////////////////////////////////////////////////////////////