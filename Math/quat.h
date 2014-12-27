////////////////////////////////////////////////////////////////////////////////

class quat
{
	public :

		inline quat()
		{
		}

		inline quat( float x, float y, float z, float w )
		{
			Set( x, y, z, w );
		}

		inline quat( const quat & qCopyFrom )
		{
			(*this) = qCopyFrom;
		}

		inline void Set( float x, float y, float z, float w )
		{
			vDir.Set( x, y, z );
			fAngle = w;
		}

		inline float Norm() const
		{
			return sqrt( vDir.LengthSqr() + fAngle * fAngle );
		}

		inline void Normalize()
		{
			float f = 1 / ( vDir.LengthSqr() + fAngle * fAngle );
			
			vDir.x  *= f;
			vDir.y  *= f;
			vDir.z  *= f;
			fAngle	*= f;
		}

		inline void Mult( const quat & qMult )
		{
			float	A = (fAngle + vDir.x) * (qMult.fAngle + qMult.vDir.x),
					B = (vDir.z - vDir.y) * (qMult.vDir.y - qMult.vDir.z),
					C = (vDir.x - fAngle) * (qMult.vDir.y + qMult.vDir.z),
					D = (vDir.y + vDir.z) * (qMult.vDir.x - qMult.fAngle),
					E = (vDir.x + vDir.z) * (qMult.vDir.x + qMult.vDir.y),
					F = (vDir.x - vDir.z) * (qMult.vDir.x - qMult.vDir.y),
					G = (fAngle + vDir.y) * (qMult.fAngle - qMult.vDir.z),
					H = (fAngle - vDir.y) * (qMult.fAngle + qMult.vDir.z);

			fAngle = B + (-E - F + G + H) * 0.5f;
			vDir.x = A - ( E + F + G + H) * 0.5f; 
			vDir.y =-C + ( E - F + G - H) * 0.5f;
			vDir.z =-D + ( E - F - G + H) * 0.5f;
		}

		inline void RotateY( const float _fAngle )
		{
			quat qRot( 0, 1, 0, cosf( 0.5f * _fAngle * TO_RADIAN ) );

			qRot.vDir.Scale( sinf( 0.5f * _fAngle * TO_RADIAN ) );

			Mult( qRot );

			//vDir.x = sinf( _fAngle * TO_RADIAN ),
			//vDir.z = cosf( _fAngle * TO_RADIAN );
		}

		inline void ToMatrix( mat4 & mOut ) const
		{
			mOut.Identity();

			float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
			x2 = vDir.x + vDir.x;    y2 = vDir.y + vDir.y;    z2 = vDir.z + vDir.z;
			xx = vDir.x * x2;   xy = vDir.x * y2;   xz = vDir.x * z2;
			yy = vDir.y * y2;   yz = vDir.y * z2;   zz = vDir.z * z2;
			wx = fAngle * x2;   wy = fAngle * y2;   wz = fAngle * z2;

			mOut.m11 = 1.0f - (yy + zz);
			mOut.m12 = xy - wz;
			mOut.m13 = xz + wy;

			mOut.m21 = xy + wz;
			mOut.m22 = 1.0f - (xx + zz);
			mOut.m23 = yz - wx;

			mOut.m31 = xz - wy;
			mOut.m32 = yz + wx;
			mOut.m33 = 1.0f - (xx + yy);
		}

		union
		{
			struct
			{
				vec3	vDir;
				float	fAngle;
			};

			struct 
			{
				float x, y, z, w;
			};
		};

		static quat qOne;
};


/**
*
*/
inline void Slerp( quat * res, quat * q, quat * p, float t)
{
	float DELTA = 0.0001f;
	float p1[4];
	float omega, cosom, sinom, scale0, scale1;

	// косинус угла
	cosom = q->x*p->x + q->y*p->y + q->z*p->z + q->w*p->w;

	if ( cosom < 0.f )
	{ 
		cosom = -cosom;
		p1[0] = - p->x;  p1[1] = - p->y;
		p1[2] = - p->z;  p1[3] = - p->w;
	}
	else
	{
		p1[0] = p->x;    p1[1] = p->y;
		p1[2] = p->z;    p1[3] = p->w;
	}

	if ( ( 1.f - cosom ) > DELTA )
	{
		// стандартный случай (slerp)
		omega = acos(cosom);
		sinom = sin(omega);
		scale0 = sinf( ( 1.f - t ) * omega ) / sinom;
		scale1 = sinf( t * omega) / sinom;
	}
	else
	{        
		// если маленький угол - линейная интерполяция
		scale0 = 1.f - t;
		scale1 = t;
	}

	res->x = scale0 * q->x + scale1 * p1[0];
	res->y = scale0 * q->y + scale1 * p1[1];
	res->z = scale0 * q->z + scale1 * p1[2];
	res->w = scale0 * q->w + scale1 * p1[3];
}

////////////////////////////////////////////////////////////////////////////////