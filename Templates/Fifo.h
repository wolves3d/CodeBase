////////////////////////////////////////////////////////////////////////////////

#ifndef __Vector_h_included__
#define __Vector_h_included__

////////////////////////////////////////////////////////////////////////////////

template <class Item> class CFifo
{
	public:

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline CFifo() :
			m_nMaxSize		( 0 ),
			m_nNumItems		( 0 ),
			m_pItems		( NULL ),
			m_bResizeable	( true )
		{
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline uint GetCount() const
		{
			return m_nNumItems;
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline ~CVector()
		{
			if ( 0 != m_nNumItems )
			{
				// Предупреждаем пользователя о возможных утечках памяти.
				// Массив перед удалением не был высвобожден
				DEBUG_ASSERT( !m_pItems );
			}

			DEL_ARRAY( m_pItems );
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline void Clear()
		{
			m_nNumItems = 0;
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline void Delete()
		{
			Clear();
			DEL_ARRAY( m_pItems );
			m_nMaxSize = 0;
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline index_t AddToTail( const Item & _item )
		{
			//--------------------------------------------------------------
			//	Tip: Check space
			//--------------------------------------------------------------
			if ( m_nNumItems < m_nMaxSize )
			{
				m_pItems[ m_nNumItems ] = _item;
				++m_nNumItems;
			}
			else
			{
				//--------------------------------------------------------------
				//	Tip: Resize needed
				//--------------------------------------------------------------
				uint nNewSize = ( !m_nMaxSize ) ? 8 : ( m_nMaxSize * 4 );

				if ( !Resize( nNewSize ) )
					return INVALID_INDEX;

				m_pItems[ m_nNumItems ] = _item;
				++m_nNumItems;
			}

			return ( m_nNumItems - 1 );
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
		inline void Remove( index_t idIndex )
		{
			DEBUG_ASSERT( IsValid( idIndex ) );

			if ( IsValid( idIndex ) )
			{
				--m_nNumItems;

				if ( m_nNumItems )
				{
					for ( index_t i = idIndex; i < m_nNumItems; ++i )
					{
						m_pItems[ i ] =  m_pItems[ i + 1 ];
					}
				}
			}
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline Item & GetItem( index_t idIndex )
		{
			DEBUG_ASSERT( IsValid( idIndex ) );
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
		inline const Item & operator []( index_t idIndex ) const
		{
			DEBUG_ASSERT( IsValid( idIndex ) );
			return m_pItems[ idIndex ];
		}

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline bool Resize( uint nNewSize, bool bResizeable = true )
		{
			if ( nNewSize <= m_nMaxSize )
			{
				// Новый размер, меньше текущего. Выходим
				return true;
			}

			if ( m_nMaxSize )
			// Если уже есть выделенная память
			{
				if ( !m_bResizeable )
				// Если запрещено менять размер вектора
				{
					DEBUG_MSG( "Resizing fixed size vector!" )
					return false;
				}
			}

			Item * pItems = NEW Item [ nNewSize ];

			if ( NULL == pItems )
			{
				// Нету свободной памяти =(
				return false;
			}

			//--------------------------------------------------------------
			//	Tip: Двигаем прежние данные
			//--------------------------------------------------------------
			for ( size_t i = 0; i < m_nNumItems; ++i )
				pItems[ i ] = m_pItems[ i ];

			DEL_ARRAY( m_pItems );

			m_pItems		= pItems;
			m_nMaxSize		= nNewSize;
			m_bResizeable	= bResizeable;

			return true;
		}

	private:

		//----------------------------------------------------------------------
		//	Tip:
		//----------------------------------------------------------------------
		inline bool IsValid( index_t idIndex ) const
		{
			return ( ( idIndex >= 0 ) && ( m_nNumItems > idIndex ) );
		}

		uint			m_nMaxSize;
		uint			m_nNumItems;
		Item	*		m_pItems;
		bool			m_bResizeable;
};

////////////////////////////////////////////////////////////////////////////////

typedef CVector <uint> CUintVector;

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __Vector_h_included__

////////////////////////////////////////////////////////////////////////////////