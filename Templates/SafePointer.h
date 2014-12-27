////////////////////////////////////////////////////////////////////////////////

#ifndef __SafePointer_h_included__
#define __SafePointer_h_included__

////////////////////////////////////////////////////////////////////////////////

template <class Type> class CSafePointer
{
	private:

		class CCounter
		{
			friend CSafePointer;

			public :

				inline CCounter():
					m_nValue( 0 )
				{
				}

				inline void Inc()
				{
					++m_nValue;
				}

				inline void Dec()
				{
					--m_nValue;
				}

				inline bool IsValid()
				{
					return ( 0 != m_nValue );
				}

			private :

				inline void __cdecl operator delete( void * pUserData )
				{
					g_pSystem->Delete( pUserData );
				}

				inline void __cdecl operator delete( void * pUserData,
					const char * filename, int line_num )
				{
					g_pSystem->Delete( pUserData );
				}

				int m_nValue;

		} * m_pCounter;

	public:

		inline CSafePointer( int nNull = NULL ) :
			m_pCounter( NULL ),
			m_pPointer( NULL )
		{			
		}

		inline CSafePointer( const CSafePointer<Type> & pPointer ) :
			m_pCounter( NULL ),
			m_pPointer( NULL )
		{
			if ( pPointer )
			{
				m_pPointer = pPointer.m_pPointer;
				m_pCounter = pPointer.m_pCounter;
				m_pCounter->Inc();
			}
		}

		inline ~CSafePointer()
		{
			CleanUp();
		}

		inline Type * operator ->() const
		{
			return GetPointer();
		}

		inline CSafePointer * operator = ( int nPointer )
		{
			DEBUG_ASSERT( NULL == nPointer );
			CleanUp();

			return this;
		}

		inline CSafePointer * operator =( Type * pPointer )
		{
			CleanUp();

			if ( pPointer )
			{
				if ( m_pCounter = NEW CCounter )
				{
					m_pCounter->Inc();
					m_pPointer = pPointer;
				}
			}

			return this;
		}

		inline CSafePointer & operator = ( const CSafePointer <Type> & pPointer )
		{
			CleanUp();

			if ( pPointer )
			{
				m_pCounter = pPointer.m_pCounter;
				m_pPointer = pPointer.m_pPointer;
				m_pCounter->Inc();
			}

			return ( *this );
		}

		inline operator const CCounter *() const
		{
			return m_pCounter;
		}

		/*inline operator const Type *() const
		{
			return GetPointer();
		}
		/*template <typename Y>
			operator Y*() const
		{
			return static_cast<Y*>( GetPointer() );  
		}
*
		template <typename CastType> operator const CastType () const
		{
			return static_cast <const CastType>( GetPointer() );
		}*/

	private:

		inline bool IsValid() const
		{
			if ( m_pCounter )
				if ( *m_pCounter > 0 )
					return true;

			return false;
		}

		inline void CleanUp()
		{
			if ( m_pCounter )
			{
				m_pCounter->Dec();

				if ( m_pCounter->IsValid() )
				{
					m_pCounter = NULL;
					m_pPointer = NULL;
				}
				else
				{
					DEL( m_pCounter );
					// DEL( m_pPointer );
				}
			}
		}

		inline Type * GetPointer() const
		{
			DEBUG_ASSERT( m_pCounter );

			if ( m_pCounter )
				if ( m_pCounter->IsValid() )
					return m_pPointer;

			return NULL;
		}

		Type	* m_pPointer;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __SafePointer_h_included__

////////////////////////////////////////////////////////////////////////////////