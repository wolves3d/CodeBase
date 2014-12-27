////////////////////////////////////////////////////////////////////////////////

#include "Stack.h"

template <class Item> class CArray
{
	public:

		inline CArray() :
			m_pItems( NULL	),
			m_nSize	( 0		)
		{
		}

		~CArray()
		{
			DEBUG_ASSERT( !GetCount() );
			Delete();
		}

		inline size_t GetCount() const
		{
			return m_nSize;
		}

		inline bool Alloc( size_t nNumItems )
		{
			Delete();

			if ( m_pItems = NEW Item[ nNumItems ] )
			{
				m_nSize = nNumItems;
				return true;
			}
		
			return false;
		}

		inline void Delete()
		{
			DEL_ARRAY( m_pItems );
			m_nSize = 0;
		}

		inline Item & operator []( size_t nIndex )
		{
			DEBUG_ASSERT( nIndex >= 0 );
			DEBUG_ASSERT( nIndex < m_nSize );

			return m_pItems[ nIndex ];
		}

	private:
		Item * m_pItems;
		size_t m_nSize;
};

////////////////////////////////////////////////////////////////////////////////

/**	@addtogroup base_types
*	@{
*/


//! Контейнер (массив с "дырками")

//! Надо тут написать про избыточность и про то как вообще работает этот контейнер
//!
template <class Item> class CContainer
{
	public:

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline CContainer() :
			m_nMaxSize	( 0 ),
			m_nNumItems	( 0 ),
			m_pItems	( NULL ),

			m_nNumVFlags( 0 ),
			m_pVFlags	( NULL )
		{
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline uint GetNumItems() const
		{
			return m_nNumItems;
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline ~CContainer()
		{
			DEBUG_ASSERT( !m_pItems );
			m_Vacancy.Delete();
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline void Clear()
		{
			m_nNumItems = 0;
			m_Vacancy.Clear();

			for( size_t i = 0; i < m_nNumVFlags; ++i )
				m_pVFlags[ i ] = 0;

			m_nNumVFlags = 0;
		}


		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline void Delete()
		{
			Clear();
			DEL_ARRAY( m_pItems );
			DEL_ARRAY( m_pVFlags );
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline uint Add( const Item & _item )
		{
			if ( m_Vacancy.GetCount() )
			{
				//--------------------------------------------------------------
				//	Tip: There are some "holes"
				//--------------------------------------------------------------
				int nIndex;
				m_Vacancy.Pop( nIndex );
				m_pItems[ nIndex ] = _item;

				SetValid( nIndex, true );
				++m_nNumItems;
				return nIndex;
			}

			//--------------------------------------------------------------
			//	Tip: There are no "holes"
			//--------------------------------------------------------------
			if ( m_nNumItems < m_nMaxSize )
			{
				m_pItems[ m_nNumItems ] = _item;
				++m_nNumItems;
			}
			else
			{
				//--------------------------------------------------------------
				//	Tip: Resize
				//--------------------------------------------------------------
				uint nNewSize = ( !m_nMaxSize ) ? 8 : ( m_nMaxSize * 4 );

				if ( !Resize( nNewSize ) )
					return INVALID_INDEX;

				m_pItems[ m_nNumItems ] = _item;
				++m_nNumItems;
			}

			SetValid( ( m_nNumItems - 1 ), true );
			return ( m_nNumItems - 1 );
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline void Remove( index_t idIndex )
		{
			DEBUG_ASSERT( IsValid( idIndex ) );

			if ( IsValid( idIndex ) )
			{
				SetValid( idIndex, false );
				m_Vacancy.Push( (int)idIndex );
				--m_nNumItems;
			}
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline bool Iterate( index_t & pInOutIndex )
		{
			if ( !m_pItems )
				return false;

			while ( true )
			{
				++pInOutIndex;

				if ( IsValid( pInOutIndex ) )
				{
					return true;
				}

				if ( pInOutIndex >= m_nMaxSize )
					return false;
			}




			size_t	nBlock;
			int		nBit;

			if ( INVALID_INDEX == pInOutIndex )
			{
				nBlock = 0;
				nBit = -1; // it's ok for next code
			}
			else if ( pInOutIndex >= m_nMaxSize )
			{
				return false;
			}
			else
			{
				nBlock	= pInOutIndex / 32;
				nBit	= (int)pInOutIndex;

				if ( nBlock > 0 )
					nBit -= (int)( nBlock * 32 );
			}

			if ( 31 == nBit )
			{
				nBit = -1; // it's ok for next code
				++nBlock;

				if ( nBlock >= m_nNumVFlags )
					return false;
			}

			do
			{
				do
				{
					++nBit;

					if ( ( ( nBlock * 32 ) + nBit ) > m_nMaxSize )
						return false;

					if ( m_pVFlags[ nBlock ] & ( 1 << nBit ) )
					{
						pInOutIndex = ( nBlock * 32 ) + nBit;
						return true;
					}
				} while ( nBit < 31 );

				++nBlock;
				nBit = 0;
			}
			while ( nBlock < m_nNumVFlags );

			return false;
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline bool IsValid( index_t idIndex ) const
		{
			if ( INVALID_INDEX == idIndex )
				return false;

			size_t nBlock	= idIndex / 32;
			size_t nBit		= idIndex;

			if ( nBlock > 0 )
				nBit -= ( nBlock * 32 );

			return (	( idIndex >= 0 )						&&
						( idIndex < m_nMaxSize )				&&
						( m_pVFlags[ nBlock ] & ( 1 << nBit ) )		);
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline Item & operator []( index_t idIndex )
		{
			DEBUG_ASSERT( IsValid( idIndex ) );
			return m_pItems[ idIndex ];
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline const Item & operator []( index_t idIndex ) const
		{
			DEBUG_ASSERT( IsValid( idIndex ) );
			return m_pItems[ idIndex ];
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline bool Resize( uint nNewSize )
		{
			if ( nNewSize > m_nMaxSize )
			{
				uint nNumVFlags = ( nNewSize / 32 );

				if ( !nNumVFlags )
					nNumVFlags = 1;

				Item *	pItems = NEW Item [ nNewSize ];
				dword *	pVFlags= NEW dword[ nNumVFlags ];

				if ( pItems && pVFlags )
				{
					if ( m_pItems )
					{
						uint i;

						for ( i = 0; i < m_nNumItems; ++i )
							pItems[ i ] = m_pItems[ i ];

						for ( i = 0; i < m_nNumVFlags; ++i )
							pVFlags[ i ] = m_pVFlags[ i ];

						for ( i = m_nNumVFlags; i < nNumVFlags; ++i )
							pVFlags[ i ] = 0;

						DEL_ARRAY( m_pItems );
						DEL_ARRAY( m_pVFlags );
					}
					else
					{
						for ( uint i = 0; i < nNumVFlags; ++i )
							pVFlags[ i ] = 0;
					}

					m_nMaxSize	= nNewSize;
					m_pItems	= pItems;

					m_nNumVFlags= nNumVFlags;
					m_pVFlags	= pVFlags;

					return true;
				}

				DEL_ARRAY( pItems );
				DEL_ARRAY( pVFlags );
			}

			return false;
		}

	private:

	//----------------------------------------------------------------------
	//	Tip:
	//----------------------------------------------------------------------
	inline void SetValid( index_t idIndex, bool bValid )
	{
		size_t nBlock	= idIndex / 32;
		size_t nBit	= idIndex;

		if ( nBlock > 0 )
			nBit -= ( nBlock * 32 );

		if ( bValid )
		{
			// set bit?
			m_pVFlags[ nBlock ] |= ( 1 << nBit );
		}
		else
		{
			// remove bit?
//				__debugbreak(); // check this block!
			m_pVFlags[ nBlock ] = m_pVFlags[ nBlock ] &~ ( 1 << nBit );
		}
	}

	////////////////////////////////////////////////////////////////////////

	uint			m_nMaxSize;		///< Максимальное количество элементов (размер массива)
	uint			m_nNumItems;	///< Количество элементов
	Item	*		m_pItems;		///< Указатель на массив элементов

	uint			m_nNumVFlags;
	dword *			m_pVFlags;
	CStack <int>	m_Vacancy;		///< Массив(стек) пустых мест
};

/** @} */

////////////////////////////////////////////////////////////////////////////////