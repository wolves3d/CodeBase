////////////////////////////////////////////////////////////////////////////////

#ifndef __Vector_h_included__
#define __Vector_h_included__

////////////////////////////////////////////////////////////////////////////////

template <class Item> class CVector
{
	long volatile	m_itemCount; // must be 32-bit aligned
	long			m_nMaxSize;
	Item	*		m_pItems;
	bool			m_bResizeable;

	inline bool IsValid( index_t idIndex ) const
	{
		return ( ( idIndex >= 0 ) && ( m_nNumItems > idIndex ) );
	}

public:
	//----------------------------------------------------------------------
	//	Tip:
	//----------------------------------------------------------------------
	inline CVector() :
		m_nMaxSize		(0),
		m_itemCount		(0),
		m_pItems		(NULL),
		m_bResizeable	(false)
	{
	}

	//----------------------------------------------------------------------
	//	Tip:
	//----------------------------------------------------------------------
	inline uint GetCount() const
	{
		return m_itemCount;
	}

	//----------------------------------------------------------------------
	//	Tip:
	//----------------------------------------------------------------------
	inline ~CVector()
	{
		if (0 != m_itemCount)
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
		m_itemCount = 0;
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
	inline index_t AddToTail(const Item & _item)
	{
		long itemID = InterlockedExchangeAdd(&m_itemCount, 1);

		//--------------------------------------------------------------
		//	Tip: Check space
		//--------------------------------------------------------------
		if (itemID < m_nMaxSize)
		{
			m_pItems[itemID] = _item;
		}
		else
		{
			InterlockedExchangeAdd(&m_itemCount, -1);
			
			if (m_bResizeable)
			{
				uint nNewSize = ( !m_nMaxSize ) ? 8 : ( m_nMaxSize * 4 );

				if (true == Resize(nNewSize))
				{
					return AddToTail(_item);
				}
			}

			assert(false);
			return INVALID_INDEX;
		}

		return itemID;
	}

	//----------------------------------------------------------------------
	//	Tip:
	//----------------------------------------------------------------------
	inline Item * GetArray()
	{
		return (0 == GetCount())
			? NULL
			: m_pItems;
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
	inline bool Resize(long nNewSize)
	{
		if ( nNewSize <= m_nMaxSize )
		{
			// Новый размер, меньше текущего. Выходим
			return true;
		}

// 		if ( m_nMaxSize )
// 		// Если уже есть выделенная память
// 		{
// 			if ( !m_bResizeable )
// 			// Если запрещено менять размер вектора
// 			{
// 				DEBUG_MSG("Resizing fixed size vector!");
// 				return false;
// 			}
// 		}

		Item * pItems = NEW Item [ nNewSize ];

		if ( NULL == pItems )
		{
			// Нету свободной памяти =(
			return false;
		}

		//--------------------------------------------------------------
		//	Tip: Двигаем прежние данные
		//--------------------------------------------------------------
		for (long i = 0; i < m_itemCount; ++i)
			pItems[i] = m_pItems[i];

		DEL_ARRAY( m_pItems );

		m_pItems		= pItems;
		m_nMaxSize		= nNewSize;

		return true;
	}
};

////////////////////////////////////////////////////////////////////////////////

typedef CVector <int>	CVector_Int;
typedef CVector <uint>	CUintVector;

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __Vector_h_included__

////////////////////////////////////////////////////////////////////////////////