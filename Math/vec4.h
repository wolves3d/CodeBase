////////////////////////////////////////////////////////////////////////////////

#ifndef __vec4_h_included__
#define __vec4_h_included__

////////////////////////////////////////////////////////////////////////////////

class vec4
{
	public :

		inline vec4()
		{
		}

		inline vec4( float _x, float _y, float _z, float _w ) :
			x( _x ),
			y( _y ),
			z( _z ),
			w( _w )
		{
		}

		inline void Set( float _x, float _y, float _z, float _w )
		{
			x = _x;
			y = _y;
			z = _z;
			w = _w;
		}

		inline void vec4::Multiply( const mat4 & mTrans );
		union
		{
			struct { scalar x, y, z, w; };
			struct { scalar r, g, b, a; };
			struct { scalar pos_x, pos_y, width, height; };

			scalar pArray[ 4 ];
		};

		static vec4 vNull;
};

class ivec4
{
	public :

	inline ivec4()
	{
	}

	inline ivec4( int _x, int _y, int _z, int _w ) :
		x( _x ),
		y( _y ),
		z( _z ),
		w( _w )
	{
	}

	union
	{
		struct { int x, y, z, w; };
		struct { int pos_x, pos_y, width, height; };

		int pArray[ 4 ];
	};
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __vec4_h_included__

////////////////////////////////////////////////////////////////////////////////