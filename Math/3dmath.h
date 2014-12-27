////////////////////////////////////////////////////////////////////////////////

#ifndef __3Dmath_h_included__
#define __3Dmath_h_included__

////////////////////////////////////////////////////////////////////////////////

//#define _USE_MATH_DEFINES
#include <math.h>

#define M_PI		3.14159265358979323846f
#define TO_RADIAN	0.017453292519943295769236907684886f

////////////////////////////////////////////////////////////////////////////////

inline int f1_sgn(const float& k) // - функция, вычисляющая знак числа.
{
	if( k > 0 ) return 1;
	if( k < 0 ) return -1;
	return 0;
}

////////////////////////////////////////////////////////////////////////////////

typedef float scalar;

////////////////////////////////////////////////////////////////////////////////

class mat4;
class quat;
class vec3;

////////////////////////////////////////////////////////////////////////////////

class ivec2
{
	public :

		inline ivec2()
		{
		}

		inline ivec2( int _x, int _y ) :
			x( _x ),
			y( _y )
		{
		}

		inline void Set( int _x, int _y )
		{
			x = _x;
			y = _y;
		}

		inline void Add( const ivec2 & vOffset )
		{
			x += vOffset.x;
			y += vOffset.y;
		}

		union
		{
			struct { int x, y; };

			int pArray[ 2 ];
		};

		static ivec2 vNull;
};

inline ivec2 operator + ( const ivec2 & vA, const ivec2 & vB )
{
	return ivec2( vA.x + vB.x, vA.y + vB.y );
}

////////////////////////////////////////////////////////////////////////////////

#include "vec2.h"
#include "vec3.h"
#include "vec4.h"
#include "plane.h"
#include "matrix.h"
#include "quat.h"
#include "bbox.h"

////////////////////////////////////////////////////////////////////////////////

inline void vec3::Multiply( const mat4 & mTrans )
{
	const float _x = ( mTrans.m11 * x ) + ( mTrans.m21 * y ) + ( mTrans.m31 * z ) + mTrans.m41;
	const float _y = ( mTrans.m12 * x ) + ( mTrans.m22 * y ) + ( mTrans.m32 * z ) + mTrans.m42;
	const float _z = ( mTrans.m13 * x ) + ( mTrans.m23 * y ) + ( mTrans.m33 * z ) + mTrans.m43;

	x = _x;
	y = _y;
	z = _z;
}

inline void vec3::Rotate( const mat4 & mRot )
{
	const float _x = ( mRot.m11 * x ) + ( mRot.m21 * y ) + ( mRot.m31 * z );
	const float _y = ( mRot.m12 * x ) + ( mRot.m22 * y ) + ( mRot.m32 * z );
	const float _z = ( mRot.m13 * x ) + ( mRot.m23 * y ) + ( mRot.m33 * z );

	x = _x;
	y = _y;
	z = _z;
}

inline void vec4::Multiply( const mat4 & mTrans )
{
	const float _x = ( mTrans.m11 * x ) + ( mTrans.m21 * y ) + ( mTrans.m31 * z ) + ( mTrans.m41 * w );
	const float _y = ( mTrans.m12 * x ) + ( mTrans.m22 * y ) + ( mTrans.m32 * z ) + ( mTrans.m42 * w );
	const float _z = ( mTrans.m13 * x ) + ( mTrans.m23 * y ) + ( mTrans.m33 * z ) + ( mTrans.m43 * w );
	const float _w = ( mTrans.m14 * x ) + ( mTrans.m24 * y ) + ( mTrans.m34 * z ) + ( mTrans.m44 * w );

	x = _x;
	y = _y;
	z = _z;
	w = _w;
}


/**
*
*/
inline bool RayTriangle( const vec3 * pRay, const vec3 * pVertices )
{
	const vec3 vDir = pRay[ 1 ] - pRay[ 0 ];

	CPlane plane( Cross(	pVertices[ 1 ] - pVertices[ 0 ],
		pVertices[ 2 ] - pVertices[ 0 ] ), 0 );

	plane.fOffset = Dot( plane.vNormal, pVertices[ 0 ] );


	// вычисляем расстояния между концами отрезка и плоскостью треугольника.
	float r1 = Dot( plane.vNormal, pRay[ 0 ] - pVertices[ 0 ] );
	float r2 = Dot( plane.vNormal, pRay[ 1 ] - pVertices[ 0 ] );

	// если оба конца отрезка лежат по одну сторону от плоскости, то отрезок
	// не пересекает треугольник.
	if ( f1_sgn( r1 ) == f1_sgn( r2 ) )
		return false;

	const vec3 &	v1 = pVertices[ 0 ],
		v2 = pVertices[ 1 ],
		v3 = pVertices[ 2 ];

	// вычисляем точку пересечения отрезка с плоскостью треугольника.
	vec3 ip = ( pRay[ 0 ] + ( ( pRay[ 1 ] - pRay[ 0 ] ) * ( -r1 / ( r2 - r1 ) ) ) );

	// проверяем, находится ли точка пересечения внутри треугольника.
	if( Dot( Cross( (v2 - v1), (ip - v1) ), plane.vNormal ) <= 0 )
		return false;

	if( Dot( Cross( (v3 - v2), (ip - v2) ), plane.vNormal ) <= 0 )
		return false;

	if( Dot( Cross( (v1 - v3), (ip - v3) ), plane.vNormal ) <= 0 )
		return false;

	//pc = ip;
	return true;
};



/*
int
intersect_triangle_barycentric(
double orig[3], double dir[3],
double vert0[3], double vert1[3], double vert2[3],
double planeq[4], int i1, int i2,
double *t, double *alpha, double *beta)
{
double	dot, dot2;
double	point[2];
double	u0, v0, u1, v1, u2, v2;

// is ray parallel to plane?
dot = dir[0] * planeq[0] + dir[1] * planeq[1] + dir[2] * planeq[2];
if (dot < EPSILON && dot > -EPSILON)		// or use culling check
return 0;

// find distance to plane and intersection point
dot2 = orig[0] * planeq[0] +
orig[1] * planeq[1] + orig[2] * planeq[2];
*t = -(planeq[3] + dot2 ) / dot;
point[0] = orig[i1] + dir[i1] * *t;
point[1] = orig[i2] + dir[i2] * *t;

// begin barycentric intersection algorithm
u0 = point[0] - vert0[i1];
v0 = point[1] - vert0[i2];
u1 = vert1[i1] - vert0[i1];
u2 = vert2[i1] - vert0[i1];
v1 = vert1[i2] - vert0[i2];
v2 = vert2[i2] - vert0[i2];

// calculate and compare barycentric coordinates
if (u1 == 0) {		// uncommon case
*beta = u0 / u2;
if (*beta < 0 || *beta > 1)
return 0;
*alpha = (v0 - *beta * v2) / v1;
}
else {			// common case, used for this analysis
*beta = (v0 * u1 - u0 * v1) / (v2 * u1 - u2 * v1);
if (*beta < 0 || *beta > 1)
return 0;
*alpha = (u0 - *beta * u2) / u1;
}

if (*alpha < 0 || (*alpha + *beta) > 1.0)
return 0;

return 1;
}
*/

////////////////////////////////////////////////////////////////////////////////

#endif // ifndef __3Dmath_h_included__

////////////////////////////////////////////////////////////////////////////////