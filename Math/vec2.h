////////////////////////////////////////////////////////////////////////////////

#ifndef __vec2_h_included__
#define __vec2_h_included__

////////////////////////////////////////////////////////////////////////////////

class vec2
{
	public :

		inline vec2()
		{
		}

		inline vec2( scalar _x, scalar _y ) :
			x( _x ),
			y( _y )
		{
		}

		inline void Add( const vec2 & vOffset )
		{
			x += vOffset.x;
			y += vOffset.y;
		}

		inline void Set( scalar _x, scalar _y )
		{
			x = _x;
			y = _y;
		}

		union
		{
			struct { scalar x, y; };
			struct { scalar u, v; };
			struct { scalar s, t; };

			scalar pArray[ 2 ];
		};

		static vec2 vNull;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __vec2_h_included__

////////////////////////////////////////////////////////////////////////////////