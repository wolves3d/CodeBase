/**
*
*/
class CPlane
{
	public:

	inline CPlane()
	{
	};

	inline CPlane( const vec3 & vNrml, const float fOff ) :
		vNormal( vNrml	),
		fOffset( fOff	)
	{
	}

	inline void Normalize()
	{
		const float fInvMag = 1.f / vNormal.Length();

		vNormal.x	*= fInvMag;
		vNormal.y	*= fInvMag;
		vNormal.z	*= fInvMag;
		fOffset		*= fInvMag;
	}

	vec3	vNormal;
	float	fOffset;
};