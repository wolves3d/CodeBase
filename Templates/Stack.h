////////////////////////////////////////////////////////////////////////////////

#ifndef __Stack_h_included__
#define __Stack_h_included__

/*
================================================================================
//	Name:
//	Desc:
================================================================================
*/
template <class Item> class /*SYSTEM_API*/ CStack
{
	public:

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline CStack() :
			m_nMaxSize	( 0 ),
			m_nTop		( 0 ),
			m_pItems	( NULL ),
			m_bResizeable( true )
		{
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline ~CStack()
		{
			if ( m_pItems )
			{
				DEBUG_ASSERT( !"Unrealeased memory in CStack =(" );
				Delete();
			}
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------

		inline void Clear()
		{
			m_nTop = 0;
		}

		inline void Delete()
		{
			Clear();
			DEL_ARRAY( m_pItems );
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline bool Push( const Item & _item )
		{
			if ( m_nTop < m_nMaxSize )
			{
				m_pItems[ m_nTop ] = _item;
				++m_nTop;
				return true;
			}
			else
			{
				int nNewSize = ( !m_nMaxSize ) ? 8 : ( m_nMaxSize * 4 );

				if ( Resize( nNewSize ) )
				{
					m_pItems[ m_nTop ] = _item;
					++m_nTop;
					return true;
				}
			}

			return false;
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline bool Pop( Item & _item )
		{
			if ( m_nTop > 0 )
			{
				--m_nTop;
				_item = m_pItems[ m_nTop ];
				return true;
			}

			return false;
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline uint GetCount() const
		{
			return m_nTop;
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline Item * GetArray()
		{
			return m_pItems;
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline Item & GetItem( index_t idIndex )
		{
			DEBUG_ASSERT( idIndex >= 0 );
			DEBUG_ASSERT( idIndex < (index_t)m_nTop );

			return m_pItems[ idIndex ];
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline Item & operator []( index_t idIndex )
		{
			return GetItem( idIndex );
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline bool Alloc( uint nCount, bool bResizable = true )
		{
			if ( nCount <= m_nMaxSize )
				return true;

			DEBUG_ASSERT( 0 == m_nTop );
			DEL_ARRAY( m_pItems );

			m_bResizeable = bResizable;
			m_pItems = NEW Item [ nCount ];

			if ( m_pItems )
			{
				m_nMaxSize = nCount;
				return true;
			}

			m_nMaxSize = 0;
			return false;
		}

	private:

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline bool Resize( uint nNewSize )
		{
			DEBUG_ASSERT( true == m_bResizeable );

			if ( !m_bResizeable )
				return false;

			if ( nNewSize > m_nMaxSize )
			{
				Item * pItems = NEW Item [ nNewSize ];

				if ( pItems )
				{
					if ( m_pItems )
					{
						for ( uint n = 0; n < m_nMaxSize; ++n )
							pItems[ n ] = m_pItems[ n ];

						DEL_ARRAY( m_pItems );
					}

					m_nMaxSize	= nNewSize;
					m_pItems	= pItems;
					return true;
				}
			}

			return false;
		}

		//----------------------------------------------------------------------
		//	Tip: Members
		//----------------------------------------------------------------------
		bool	m_bResizeable;
		uint	m_nMaxSize;
		uint	m_nTop;
		Item *	m_pItems;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __Stack_h_included__

////////////////////////////////////////////////////////////////////////////////