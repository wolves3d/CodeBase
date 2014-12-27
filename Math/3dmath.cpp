#include "3dmath.h"

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <stdio.h>
#include "_Shared/Base.h"
#include "_Shared/API/ISystem.h"

ivec2	ivec2::vNull	= ivec2( 0, 0 );
vec2	vec2::vNull		= vec2( 0, 0 );
vec3	vec3::vNull		= vec3( 0, 0, 0 );
vec3	vec3::vOne		= vec3( 1, 1, 1 );
vec4	vec4::vNull		= vec4( 0, 0, 0, 0 );
quat	quat::qOne		= quat( 0, 0, 0, 1 );

mat4 mat4::mIdentity	=
{
	1, 0, 0, 0,
	0, 1, 0, 0,
	0, 0, 1, 0,
	0, 0, 0, 1
};

void mat4::Rotate( const quat & qRot )
{
	DEBUG_ASSERT( !"this shit doesn't work!" );

	mat4 mat;
	mat.Identity();

	float wx, wy, wz, xx, yy, yz, xy, xz, zz, x2, y2, z2;
	x2 = qRot.x + qRot.x;
	y2 = qRot.y + qRot.y;
	z2 = qRot.z + qRot.z;
	xx = qRot.x * x2;   xy = qRot.x * y2;   xz = qRot.x * z2;
	yy = qRot.y * y2;   yz = qRot.y * z2;   zz = qRot.z * z2;
	wx = qRot.w * x2;   wy = qRot.w * y2;   wz = qRot.w * z2;

	mat.m11=1.0f-(yy+zz); mat.m12=xy-wz;        mat.m13=xz+wy;
	mat.m21=xy+wz;        mat.m22=1.0f-(xx+zz); mat.m23=yz-wx;
	mat.m21=xz-wy;        mat.m32=yz+wx;        mat.m33=1.0f-(xx+yy);

	this->Mult( mat );
	// fixme: translate unmodified!!!
}