////////////////////////////////////////////////////////////////////////////////

#ifndef __String_h_included__
#define __String_h_included__
#include "stdio.h"

////////////////////////////////////////////////////////////////////////////////

class CStr
{
	#define MAX_STATIC_SIZE 248

	public:

		inline CStr():
			m_pDynamic	( NULL	),
			m_nSize		( 0		),
			m_nMaxSize	( MAX_STATIC_SIZE )
		{
			m_pArray[ 0 ] = 0;
		}

		inline CStr( const char * szString )
		{
			CStr();
			Set( szString );
		}

		inline char operator []( int n )
		{
			return ( m_pDynamic ) ? m_pDynamic[ n ] : m_pArray[ n ];
		}

		inline void Clear()
		{
			m_nSize			= 0;
			m_pArray[ 0 ]	= 0;
		}

		inline uint GetSize() const
		{
			return m_nSize;
		}

		inline const char * GetCString() const
		{
			return ( m_pDynamic ) ? m_pDynamic : m_pArray;
		}

		inline bool Set( const char * szString )
		{
			if ( !szString )
				return false;

			if ( !szString[ 0 ] )
			{
				Clear();
				return true;
			}

			//------------------------------------------------------------------
			// Tip: Get string size
			//------------------------------------------------------------------
			uint nLength = GetSize( szString );

			//------------------------------------------------------------------
			// Tip: Try to copy string to static data
			//------------------------------------------------------------------
			if ( nLength <= MAX_STATIC_SIZE )
			{
				m_nSize = nLength;
				++nLength;

				while ( nLength-- )
				{
					m_pArray[ nLength ] = szString[ nLength ];
				}

				return true;
			}
			else
			{
				
			}

			return false;
		}

		inline void Cut( uint nOffset )
		{
			if ( nOffset < 1 )
				return;

			if ( m_nSize > nOffset )
			{
				m_nSize = nOffset;
				m_pArray[ nOffset - 1 ] = 0;
			}
		}

		inline bool Add( const char *szString )
		{
			if ( szString )
			{
				size_t nLength = GetSize( szString );

				if ( !nLength )
					return true;

				//--------------------------------------------------------------
				// Tip: Try to copy add_string to static data
				//--------------------------------------------------------------
				if ( ( m_nSize + nLength ) < MAX_STATIC_SIZE )
				{
					if ( m_nSize )
						--m_nSize; // del zero term

					for ( size_t i = 0; i < nLength; ++i )
					{
						m_pArray[ m_nSize ] = szString[ i ];
						++m_nSize;
					}

					return true;
				}
				else
				{
					DEBUG_MSG( "string dosen't fit to static data" );
				}
			}

			return false;
		}

		inline bool AddNumber( int nNumber )
		{
			int nDigits = 1;

			for ( float fNumber = (float)nNumber; fNumber >= 10.f; ++nDigits )
				fNumber *= 0.1f;
			
			//--------------------------------------------------------------
			// Tip: Try to copy add_string to static data
			//--------------------------------------------------------------
			if ( ( m_nSize + nDigits ) < MAX_STATIC_SIZE )
			{
				sprintf( m_pArray + m_nSize - 1, "%d", nNumber );
				m_nSize += nDigits;
				m_pArray[ m_nSize ] = 0;
				return true;
			}
			else
			{
				DEBUG_ASSERT( !"string dosen't fit to static data" );
			}

			return false;
		}

		static bool Compare( const char * szStringA, const char * szStringB )
		{
			if ( szStringA == szStringB )
				return true;

			if ( !szStringA || !szStringB )
				return false;

			size_t nSizeA = GetSize( szStringA );
			size_t nSizeB = GetSize( szStringB );

			if ( nSizeA != nSizeB )
				return false;

			while ( nSizeA-- )
			{
				if ( szStringA[ nSizeA ] != szStringB[ nSizeA ] )
					return false;
			}

			return true;
		}

		inline bool Compare( const char * szString ) const
		{
			if ( !szString )
				return false;

			size_t nSize = GetSize( szString );

			if ( nSize != m_nSize )
				return false;

			const char * pArray = ( m_pDynamic ) ? m_pDynamic : m_pArray;

			while ( nSize-- )
			{
				if ( pArray[ nSize ] != szString[ nSize ] )
					return false;
			}

			return true;
		}

		bool Alloc(uint nNewSize)
		{
			return (nNewSize < MAX_STATIC_SIZE);
		}

		void * Map(uint * pOutMaxSize = NULL)
		{
			if (pOutMaxSize)
				(*pOutMaxSize) = m_nMaxSize;

			return m_pArray;
		}

		void Unmap()
		{

		}

		inline bool Compare( const CStr & tString ) const
		{
			if ( tString.GetSize() != m_nSize )
				return false;

			const char * pArrayA = ( m_pDynamic ) ? m_pDynamic : m_pArray;
			const char * pArrayB = ( tString.m_pDynamic ) ?
				tString.m_pDynamic : tString.m_pArray;

			for ( size_t i = 0; i < m_nSize; ++i )
				if ( pArrayA[ i ] != pArrayB[ i ] )
					return false;

			return true;
		}

		inline CStr & operator = ( const char * szString )
		{
			Set( szString );
			return (* this);
		}

		inline operator const char * () const
		{
			return GetCString();
		}


		static uint Copy( char * pDest, const char * pSrc, uint nDestMaxSize )
		{
			if ( pDest && pSrc && nDestMaxSize )
			{
				uint nSrcLength = GetSize( pSrc );

				if ( nSrcLength >= nDestMaxSize )
				{
					memcpy( pDest, pSrc, ( nDestMaxSize - 1 ) );
					pDest[ nDestMaxSize - 1 ] = 0;
					return nDestMaxSize;
				}
				
				memcpy( pDest, pSrc, nSrcLength );
				pDest[ nSrcLength ] = 0;
				return nSrcLength;
			}

			return 0;
		}

		static bool FinishWith( const char * szWhere, const char * szFinish )
		{
			if ( ( NULL == szWhere ) || ( NULL == szFinish ) )
				return false;

			uint nWhereSize = GetSize( szWhere );

			if ( 0 == nWhereSize )
				return false;

			uint nFinishSize = GetSize( szFinish );
				
			if ( nFinishSize > nWhereSize )
				return false; // Окончание длиннее чем строка где ищем

			const char * pEnd = szWhere + ( nWhereSize - nFinishSize );

			while ( nFinishSize-- )
			{
				if ( pEnd[ nFinishSize ] != szFinish[ nFinishSize ] )
					return false; // Какой-то из символов не совпал
			}

			return true;
		}

		// Считает длинну строки + 1 для null terminator
		// Это не аналог strlen!!!
		static uint GetSize( const char *szString )
		{
			uint nLength = 0;

			if ( szString )
				while ( szString[ nLength++ ] );

			return nLength;
		}

		private:

		char	m_pArray[ MAX_STATIC_SIZE ];
		char *	m_pDynamic;
		uint	m_nSize;
		uint	m_nMaxSize;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __String_h_included__

////////////////////////////////////////////////////////////////////////////////