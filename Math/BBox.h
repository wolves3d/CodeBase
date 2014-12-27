////////////////////////////////////////////////////////////////////////////////

#ifndef __bbox_h_included__
#define __bbox_h_included__

////////////////////////////////////////////////////////////////////////////////

class bbox
{
	public:

		vec3 vMin, vMax;

		inline void Translate( const mat4 & matrix )
		{
			vMin.Multiply( matrix );
			vMax.Multiply( matrix );
		}

		inline bool RayHitTest( const vec3 * pInRay ) const
		{
			vec3 pRay[ 2 ];
			pRay[ 0 ] = pInRay[ 0 ];
			pRay[ 1 ] = pInRay[ 1 ];

			pRay[ 1 ].Set(	pRay[1].x - pRay[0].x,
							pRay[1].y - pRay[0].y,
							pRay[1].z - pRay[0].z );

			pRay[ 1 ].Normalize();

			TCollision tCollision;
			tCollision.fNearHit	= -1000000;
			tCollision.fFarHit	= +1000000;

			// Проекция бокса и луча на ось X
			tCollision.nAxis		= 0;
			tCollision.fRayStart	= pRay[ 0 ].x;
			tCollision.fRayDir		= pRay[ 1 ].x;

			if ( !TraceAxis( &tCollision ) )
				return false; // Нет пересечения с проекцией на ось X

			// Проекция бокса и луча на ось Y
			tCollision.nAxis		= 1;
			tCollision.fRayStart	= pRay[ 0 ].y;
			tCollision.fRayDir		= pRay[ 1 ].y;

			if ( !TraceAxis( &tCollision ) )
				return false; // Нет пересечения с проекцией на ось Y

			// Проекция бокса и луча на ось Z
			tCollision.nAxis		= 2;
			tCollision.fRayStart	= pRay[ 0 ].z;
			tCollision.fRayDir		= pRay[ 1 ].z;

			if ( !TraceAxis( &tCollision ) )
				return false; // Нет пересечения с проекцией на ось Z

			// Есть пересечения всех проекций бокса с лучем
			return true;
		}

	private:

		struct TCollision
		{
			int nAxis;
			float fRayStart, fRayDir;
			float fNearHit, fFarHit;
		};

		inline bool TraceAxis( TCollision * pCol ) const
		{
			// Параллелен ли луч проекции бокса на текущую ось?
			if ( 0.f == pCol->fRayDir )
			{
				if ( pCol->fRayStart < vMin.pArray[ pCol->nAxis ] )
					return false; // Луч ближе заданного промежутка

				if ( pCol->fRayStart > vMax.pArray[ pCol->nAxis ] )
					return false; // Луч дальше заданного промежутка

				// Луч параллелен текущей плоскости и лежит
				// в заданном промежутке
			}
			else
			{
				// Луч не параллелен проекции бокса на ось

				// Для замены деления на умножения
				const float fInvDir = 1.f / pCol->fRayDir;

				float t1 = ( vMin.pArray[ pCol->nAxis ] - pCol->fRayStart ) * fInvDir;
				float t2 = ( vMax.pArray[ pCol->nAxis ] - pCol->fRayStart ) * fInvDir;
				
				if ( t1 > t2 ) // Need swap?
				{
					float t = t2;
					t2 = t1;
					t1 = t;
				}

				if ( t1 > pCol->fNearHit )
					pCol->fNearHit = t1;

				if ( t2 < pCol->fFarHit )
					pCol->fFarHit = t2;
		
				if ( pCol->fNearHit > pCol->fFarHit )
					return false; // Луч не пересекает заданный интервал
		
				if ( pCol->fFarHit < 0 )
					return false; /* behind */
			}

			// Луч пересекает заданный интервал
			// Первая точка пересечения лежит на удалении t1 от начала луча
			// Вторая точка пересечения лежит на удалении t2 от начала луча
			return true;
		}
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __bbox_h_included__

////////////////////////////////////////////////////////////////////////////////