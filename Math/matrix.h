////////////////////////////////////////////////////////////////////////////////

class mat3
{
	public:

		float Determinant()
		{
			return (	  pArray[0] * ( pArray[4] * pArray[8] - pArray[7] * pArray[5] )
						- pArray[1] * ( pArray[3] * pArray[8] - pArray[6] * pArray[5] )
						+ pArray[2] * ( pArray[3] * pArray[7] - pArray[6] * pArray[4] )	);
		}

	scalar pArray[ 9 ];
};

class mat4
{
	public :

		void Rotate( const quat & qRot );

		inline void RotateX( const float fAngle )
		{
			mat4 mMult;

			const float fSin = sinf( fAngle * TO_RADIAN ),
						fCos = cosf( fAngle * TO_RADIAN );

			mMult.m11 = 1;		mMult.m12 = 0;		mMult.m13 = 0;		mMult.m14 = 0;
			mMult.m21 = 0;		mMult.m22 = fCos;	mMult.m23 = fSin;	mMult.m24 = 0;
			mMult.m31 = 0;		mMult.m32 = -fSin;	mMult.m33 = fCos;	mMult.m34 = 0;
			mMult.m41 = 0;		mMult.m42 = 0;		mMult.m43 = 0;		mMult.m44 = 1;

			Mult( mMult );
		}

		inline void RotateY( const float fAngle )
		{
			mat4 mMult;

			const float fSin = sinf( fAngle * TO_RADIAN ),
						fCos = cosf( fAngle * TO_RADIAN );

			mMult.m11 = fCos;	mMult.m12 = 0; mMult.m13 = -fSin;	mMult.m14 = 0;
			mMult.m21 = 0;		mMult.m22 = 1; mMult.m23 = 0;		mMult.m24 = 0;
			mMult.m31 = fSin;	mMult.m32 = 0; mMult.m33 = fCos;	mMult.m34 = 0;
			mMult.m41 = 0;		mMult.m42 = 0; mMult.m43 = 0;		mMult.m44 = 1;

			Mult( mMult );
		}

		inline void Mult( const mat4 & mMult )
		{
			mat4 mResult;

			mResult.m11 = m11 * mMult.m11 + m12 * mMult.m21 + m13 * mMult.m31 + m14 * mMult.m41;
			mResult.m12 = m11 * mMult.m12 + m12 * mMult.m22 + m13 * mMult.m32 + m14 * mMult.m42;
			mResult.m13 = m11 * mMult.m13 + m12 * mMult.m23 + m13 * mMult.m33 + m14 * mMult.m43;
			mResult.m14 = m11 * mMult.m14 + m12 * mMult.m24 + m13 * mMult.m34 + m14 * mMult.m44;

			mResult.m21 = m21 * mMult.m11 + m22 * mMult.m21 + m23 * mMult.m31 + m24 * mMult.m41;
			mResult.m22 = m21 * mMult.m12 + m22 * mMult.m22 + m23 * mMult.m32 + m24 * mMult.m42;
			mResult.m23 = m21 * mMult.m13 + m22 * mMult.m23 + m23 * mMult.m33 + m24 * mMult.m43;
			mResult.m24 = m21 * mMult.m14 + m22 * mMult.m24 + m23 * mMult.m34 + m24 * mMult.m44;

			mResult.m31 = m31 * mMult.m11 + m32 * mMult.m21 + m33 * mMult.m31 + m34 * mMult.m41;
			mResult.m32 = m31 * mMult.m12 + m32 * mMult.m22 + m33 * mMult.m32 + m34 * mMult.m42;
			mResult.m33 = m31 * mMult.m13 + m32 * mMult.m23 + m33 * mMult.m33 + m34 * mMult.m43;
			mResult.m34 = m31 * mMult.m14 + m32 * mMult.m24 + m33 * mMult.m34 + m34 * mMult.m44;

			mResult.m41 = m41 * mMult.m11 + m42 * mMult.m21 + m43 * mMult.m31 + m44 * mMult.m41;
			mResult.m42 = m41 * mMult.m12 + m42 * mMult.m22 + m43 * mMult.m32 + m44 * mMult.m42;
			mResult.m43 = m41 * mMult.m13 + m42 * mMult.m23 + m43 * mMult.m33 + m44 * mMult.m43;
			mResult.m44 = m41 * mMult.m14 + m42 * mMult.m24 + m43 * mMult.m34 + m44 * mMult.m44;

			(*this) = mResult;
		}

		inline void AbsTranslate( vec3 vOffset )
		{
			vOffset.Multiply( *this );
			m41 = vOffset.x;
			m42 = vOffset.y;
			m43 = vOffset.z;
		}

		inline void Scale( float fScale )
		{
			m11 *= fScale;
			m12 *= fScale;
			m13 *= fScale;

			m21 *= fScale;
			m22 *= fScale;
			m23 *= fScale;

			m31 *= fScale;
			m32 *= fScale;
			m33 *= fScale;
		}

		inline void Scale( const vec3 & vScale )
		{
			m11 *= vScale.x;
			m12 *= vScale.x;
			m13 *= vScale.x;

			m21 *= vScale.y;
			m22 *= vScale.y;
			m23 *= vScale.y;

			m31 *= vScale.z;
			m32 *= vScale.z;
			m33 *= vScale.z;
		}

		inline void Translate( vec3 vOffset )
		{
			mat4 mTrans;
			mTrans.Identity();

			mTrans.m41 = vOffset.x;
			mTrans.m42 = vOffset.y;
			mTrans.m43 = vOffset.z;

			Mult( mTrans );
		}

		inline void Identity()
		{
			m11 = m22 = m33 = m44 = 1.0f;
			
			m12 = m13 = m14 =
			m21 = m23 = m24 =
			m31 = m32 = m34 =
			m41 = m42 = m43 = 0;

		}


		void m4_submat( mat4 & mr, mat3 & mb, int i, int j )
		{
			int ti, tj, idst, jdst;

			for ( ti = 0; ti < 4; ti++ )
			{
				if ( ti < i )
				{
				  idst = ti;
				}
				else
				{
					if ( ti > i )
					{
						idst = ti-1;
					}
				}

				for ( tj = 0; tj < 4; tj++ )
				{
					if ( tj < j )
					{
						jdst = tj;
					}
					else
					{
						if ( tj > j )
						{
							jdst = tj-1;
						}
					}

					if ( ti != i && tj != j )
					{
						mb.pArray[idst*3 + jdst] = mr.pArray[ti*4 + tj ];
					}
				}
			}
		}

		// determinant
		float m4_det( mat4 & mr )
		{
			float  det, result = 0, i = 1;
			mat3 msub3;
			int     n;

			for ( n = 0; n < 4; n++, i *= -1 )
			{
				m4_submat( mr, msub3, 0, n );

				det     = msub3.Determinant();
				result += mr.pArray[n] * det * i;
			}

			return( result );
		}

		bool Inverse()
		{
			mat4	mr;
			mat4 & ma = (*this);

			float	mdet = m4_det( ma );
			mat3	mtemp;
			int     i, j, sign;

			if ( fabs( mdet ) < 0.0005 )
				return false;

			mdet = 1.f / mdet; // ( to replace dev by mul )

			for ( i = 0; i < 4; i++ )
			{
				for ( j = 0; j < 4; j++ )
				{
					sign = 1 - ( (i +j) % 2 ) * 2;

					m4_submat( ma, mtemp, i, j );

					mr.pArray[i+j*4] = ( mtemp.Determinant() * sign ) * mdet;
				}
			}

			for ( i = 0; i < 16; ++i )
				pArray[ i ] = mr.pArray[ i ];

			return true;
		}

		inline void Ortho( scalar fLeft, scalar fRight, scalar fTop, scalar fBottom, scalar fZNear, scalar fZFar )
		{
			m11 = 2 / ( fRight - fLeft );
			m12 = 0;
			m13 = 0;
			m14 = 0;//- ( fRight + fLeft ) / ( fRight - fLeft );

			m21 = 0;
			m22 = 2 / ( fTop - fBottom );
			m23 = 0;
			m24 = 0;//- ( fTop + fBottom ) / ( fTop - fBottom );

			m31 = 0;
			m32 = 0;
			m33 = - 2 / ( fZFar - fZNear );
			m34 = 0;//- ( fZFar + fZNear ) / ( fZFar - fZNear );

			m41 = -1;
			m42 = -1;
			m43 = 0;
			m44 = 1;
/*

			m11 = 2 / ( fRight - fLeft );
			m12 = 0;
			m13 = 0;
			m14 = - ( fRight + fLeft ) / ( fRight - fLeft );

			m21 = 0;
			m22 = 2 / ( fTop - fBottom );
			m23 = 0;
			m24 = - ( fTop + fBottom ) / ( fTop - fBottom );

			m31 = 0;
			m32 = 0;
			m33 = - 2 / ( fZFar - fZNear );
			m34 = - ( fZFar + fZNear ) / ( fZFar - fZNear );

			m41 = 0;//-1;
			m42 = 0;//-1;
			m43 = 0;
			m44 = 1;*/
		}

		inline void SetProjection( scalar fFOV, scalar fAspect, scalar fZnear, scalar fZfar )
		{
			fFOV /= 180;
			fFOV *= 3.14159265358979323846f;

			m11 = ( 1.f / tan( fFOV  / 2.f ) ) / fAspect;
			m12 = 0;
			m13 = 0;
			m14 = 0;

			m21 = 0;
			m22 = 1.f / tan( ( fFOV ) / 2.f );
			m23 = 0;
			m24 = 0;

			m31 = 0;
			m32 = 0;
			m33 = fZfar / ( fZnear - fZfar );
			m34 = -1;

			m41 = 0;
			m42 = 0;
			m43 = fZnear * fZfar / ( fZnear - fZfar );
			m44 = 0;
		}

		/*void Transpose()
		{
			for ( uint i = 0; i < 4; ++i )
			{
				for ( uint j = 0; j < 4; ++j )
				{
					if ( i != j )
					{
						scalar temp = pArray[ i * 4 + j ];
						pArray[ i * 4 + j ] = pArray[ j * 4 + i ];
						pArray[ j * 4 + i ] = temp;
					}
				}
			}
		}*/

		union
		{
			struct
			{
				scalar	m11, m12, m13, m14,
						m21, m22, m23, m24,
						m31, m32, m33, m34,
						m41, m42, m43, m44;
			};

			struct
			{
				vec4 vRow0, vRow1, vRow2, vRow3;
			};

			scalar	pArray[ 16 ];
		};

		static mat4 mIdentity;
};

////////////////////////////////////////////////////////////////////////////////