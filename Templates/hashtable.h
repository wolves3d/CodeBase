////////////////////////////////////////////////////////////////////////////////

#ifndef __HashTable_h_included__
#define __HashTable_h_included__

////////////////////////////////////////////////////////////////////////////////

#include "Vector.h"

////////////////////////////////////////////////////////////////////////////////

struct HashIter
{
	inline HashIter() :
		nLevel	( -1 ),
		nItem	( 0 )
	{
	}

	int nLevel;
	int nItem;
};

////////////////////////////////////////////////////////////////////////////////

template <class Type> class CHashTable
{
	public:

		//======================================================================
		//
		//======================================================================
		inline CHashTable() :
			m_pLevels	( 0 ),
			m_nNumLevels( 0 ),
			m_nNumItems	( 0 )
		{ }


        /*
        ================================================================================
        //	Name: Копирующий по значению конструктор.
        //	Desc:
        ================================================================================
        */
        inline CHashTable( const CHashTable<Type> & src ) :
            m_pLevels	( src.m_pLevels ),
            m_nNumLevels( src.m_nNumLevels ),
            m_nNumItems	( src.m_nNumItems )
        { }

		//======================================================================
		//
		//======================================================================
		inline ~CHashTable()
		{
			if ( m_nNumLevels )
				Cleanup();
		}

		//======================================================================
		//
		//======================================================================
		inline bool Init( int nNumLevels = 128 )
		{
			if ( m_nNumLevels )
				Cleanup();

			m_nNumLevels = nNumLevels;

			if ( m_nNumLevels > 0 )
			{
				m_pLevels = NEW CVector <THashItem> [ m_nNumLevels ];
				return ( NULL != m_pLevels );
			}

			return false;
		}

		//======================================================================
		//
		//======================================================================
		inline void Cleanup()
		{
			for ( int n = 0; n < m_nNumLevels; ++n )
				m_pLevels[ n ].Delete();

			DEL_ARRAY( m_pLevels );

			m_nNumLevels= 0;
			m_nNumItems	= 0;
		}

		//======================================================================
		//
		//======================================================================
		/*inline void Delete()
		{
			DEL_ARRAY( m_pLevels );
			Cleanup();
		}*/

		//======================================================================
		//
		//======================================================================
		inline int GetNumItems() const
		{
			return m_nNumItems;
		}

		/*
		================================================================================
		//	Name: Проверить, существует ли элемент.
		//	Desc:
		================================================================================
		*/
		inline bool Contains( const char * szName ) const
		{
			Type temp;
			return GetItem( szName, temp );
		}

		//======================================================================
		//
		//======================================================================
		inline bool AddItem( const char * szName, Type item )
		{
			if ( szName )
			{
				if ( strlen( szName ) < 256 )
				{
					if ( !m_pLevels )
					{
						if ( !Init() )
							return false;
					}

					THashItem tHashItem( szName, item );

					int n = Hash( szName ) % m_nNumLevels;

					m_pLevels[ n ].AddToTail( tHashItem );
					++m_nNumItems;

					return true;
				}
			}

			return false;
		}

		//======================================================================
		//
		//======================================================================
		inline void RemoveItem( const char * szName )
		{
			if ( szName && m_pLevels )
			{
				if ( strlen( szName ) < 256 )
				{
					int n = Hash( szName ) % m_nNumLevels;

					for ( int i = 0; i < m_pLevels[ n ].GetCount(); ++i )
					{
						if ( !strcmp( szName, m_pLevels[ n ][ i ].szName ) )
						{
							m_pLevels[ n ].Remove( i );
							--m_nNumItems;
							return;
						}
					}
				}
			}
		}

		//======================================================================
		//
		//======================================================================
		inline void RemoveAndIterNext( HashIter & iter )
		{
			HashIter del = iter;
			GetNext( iter ); // out value

			if ( del.nLevel < m_nNumLevels )
			{
				if ( del.nItem < m_pLevels[ del.nLevel ].GetCount() )
				{
					m_pLevels[ del.nLevel ].Remove( del.nItem );
					--m_nNumItems;
				}
			}
		}

		//======================================================================
		//
		//======================================================================
		inline bool GetItem( const char * szName, Type & item ) const
		{
			if ( szName && m_pLevels )
			{
				if ( strlen( szName ) < 256 )
				{
					int n = Hash( szName ) % m_nNumLevels;

					for ( size_t i = 0; i < m_pLevels[ n ].GetCount(); ++i )
					{
						const THashItem & tHashItem = m_pLevels[ n ][ i ];

						if ( tHashItem.sName.Compare( szName ) )
						{
							item = tHashItem.tItem;
							return true;
						}
					}
				}
			}

			return false;
		}

		//======================================================================
		//
		//======================================================================
		inline bool GetItem( const HashIter & iter, Type & item  ) const
		{
			if ( iter.nLevel < m_nNumLevels )
			{
				if ( iter.nItem < m_pLevels[ iter.nLevel ].GetNumItems() )
				{
					CVector <THashItem> & level = m_pLevels[ iter.nLevel ];
					item = level.GetItem( iter.nItem ).tItem;
					return true;
				}
			}

			return false;
		}


		/*
		================================================================================
		//	Name: Получить ключ элемента.
		//	Desc:
		================================================================================
		*/
		inline const char * GetName( const HashIter & iter ) const
		{
			if ( iter.nLevel < m_nNumLevels )
			{
				if ( iter.nItem < m_pLevels[ iter.nLevel ].GetNumItems() )
				{
					return m_pLevels[ iter.nLevel ].GetItem( iter.nItem ).sName.GetString();
				}
			}

			return NULL;
		}


		/*
		================================================================================
		//	Name: Получить хеш ключа элемента.
		//	Desc:
		================================================================================
		*
		inline int GetHashNumber( const char * szString  ) const
		{
			D_ASSERT ( strlen( szString ) < 256 );
			return ( Hash( szString ) % m_nNumLevels );
		}*/


		//======================================================================
		//
		//======================================================================
		inline void ResetIterator( HashIter & iter )
		{
			iter.nLevel = -1;
		}

		//======================================================================
		//
		//======================================================================
		inline bool Iterate( HashIter & iter ) const
		{
			if ( iter.nLevel == (-1) )
			{
				for ( iter.nLevel = 0; iter.nLevel < m_nNumLevels; ++iter.nLevel )
				{
					if ( m_pLevels[ iter.nLevel ].GetNumItems() )
					{
						iter.nItem = 0;
						return true;
					}
				}

				return false;
			}

			if ( iter.nLevel < m_nNumLevels )
			{
				if ( iter.nItem < m_pLevels[ iter.nLevel ].GetNumItems() )
				{
					if ( ( iter.nItem + 1 ) < m_pLevels[ iter.nLevel ].GetNumItems() )
					{
						++iter.nItem;
						return true;
					}
					else
					{
						++iter.nLevel;

						while ( iter.nLevel < m_nNumLevels )
						{
							if ( m_pLevels[ iter.nLevel ].GetNumItems() )
							{
								iter.nItem = 0;
								return true;
							}

							++iter.nLevel;
						}
					}
				}
			}

			return false;
		}

		//======================================================================
		//
		//======================================================================
		inline dword Hash( const char * szString ) const
		{
			#define CHAR_BIT 8
			#define BITS_IN_int     ( sizeof(int) * CHAR_BIT )
			#define THREE_QUARTERS  ((int) ((BITS_IN_int * 3) / 4))
			#define ONE_EIGHTH      ((int) (BITS_IN_int / 8))
			#define HIGH_BITS       ( ~((unsigned int)(~0) >> ONE_EIGHTH ))

			unsigned int hash_value, i;

			for ( hash_value = 0; (*szString); ++szString )
			{
				hash_value = ( hash_value << ONE_EIGHTH ) + *szString;
				if (( i = hash_value & HIGH_BITS ) != 0 )
					hash_value = (hash_value ^ (i >> THREE_QUARTERS)) & ~HIGH_BITS;
			}

			return ( hash_value );
		}


        /*
        ================================================================================
        //	Name: Копировать содержимое хеш-таблицы.
        //	Desc: Будет вызван метод CopyFrom(...).
        ================================================================================
        */
        CHashTable<Type> & operator=( const CHashTable<Type> &other )
        {
            CopyFrom( other );
            return *this;
        };


        /*
        ================================================================================
        //	Name: Копировать содержимое хеш-таблицы.
        //	Desc:
        ================================================================================
        */
        void CopyFrom( const CHashTable<Type> &other )
        {
            // Возможна оптимизация....

            Cleanup();

            if ( other.GetNumItems() == 0 )
                return;

            Init( other.m_nNumLevels );

            HashIter iter;
            while ( other.Iterate( iter ) )
            {
                Type element;
                other.GetItem(iter, element);
                AddItem( other.GetName(iter), element );
            }
        }

	private:

		//======================================================================
		struct THashItem
		{
			inline THashItem()
			{
			}

			inline THashItem( const char * szName, Type item ) :
				sName( szName ),
				tItem( item )
			{
			}

			CStr sName;
			Type tItem;
		};

		//======================================================================
		CVector <THashItem> *		m_pLevels;
		int							m_nNumLevels;
		int							m_nNumItems;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __HashTable_h_included__

////////////////////////////////////////////////////////////////////////////////