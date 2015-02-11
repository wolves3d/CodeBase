////////////////////////////////////////////////////////////////////////////////

#ifndef __IXMLWrapper_h_included__
#define __IXMLWrapper_h_included__

////////////////////////////////////////////////////////////////////////////////

class CXMLAttr
{
	public:

		inline CXMLAttr( const char * szName, const char * szValue ) :
			m_Name	( szName ),
			m_Value	( szValue )
		{
		}

		//----------------------------------------------------------------------
		// Tip: Name
		//----------------------------------------------------------------------
		inline const char * GetName() const
		{
			return m_Name.GetString();
		}

		inline bool SetName( const char * szName )
		{
			return m_Name.Set( szName );
		}

		inline const char * GetValue() const
		{
			return m_Value.GetString();
		}

		inline bool SetValue( const char * szValue )
		{
			return m_Value.Set( szValue );
		}

	private:

		CStr	m_Name;
		CStr	m_Value;
};

////////////////////////////////////////////////////////////////////////////////

class CXMLElement
{
	public:

		inline ~CXMLElement()
		{
			for ( index_t i = INVALID_INDEX; m_pChilds.Iterate( i ); )
				DEL( m_pChilds[ i ] );

			m_pChilds.Delete();

			for ( index_t i = INVALID_INDEX; m_pAttrs.Iterate( i ); )
				DEL( m_pAttrs[ i ] );

			m_pAttrs.Delete();
		}

		//----------------------------------------------------------------------
		// Tip: Name
		//----------------------------------------------------------------------
		inline const char * GetName() const
		{
			return m_Name.GetString();
		}

		inline bool SetName( const char * szName )
		{
			return m_Name.Set( szName );
		}

		//----------------------------------------------------------------------
		// Tip: Child elements
		//----------------------------------------------------------------------
		inline size_t GetNumChilds() const
		{
			return m_pChilds.GetNumItems();
		}
		
		inline bool AddChild( CXMLElement * pChild )
		{
			if ( pChild )
			{
				m_pChilds.Add( pChild );
				return true;
			}

			return false;
		}
		
		inline CXMLElement * GetChild( size_t nElem )
		{
			return m_pChilds[ nElem ];
		}

		inline CXMLElement * GetChild( const char * szElemName )
		{
			index_t i = INVALID_INDEX;

			while ( m_pChilds.Iterate( i ) )
			{
				CXMLElement * pElem = m_pChilds[ i ];

				if ( CStr::Compare( szElemName, pElem->GetName() ) )
				{
					return pElem;
				}
			}

			return NULL;
		}

		//----------------------------------------------------------------------
		// Tip: Attributes
		//----------------------------------------------------------------------
		inline size_t GetNumAttrs() const
		{
			return m_pAttrs.GetNumItems();
		}
		
		inline bool AddAttr( const char * szName, const char * szValue )
		{
			CXMLAttr * pAttr = NEW CXMLAttr( szName, szValue );

			if ( pAttr )
			{
				m_pAttrs.Add( pAttr );
				return true;
			}

			return false;
		}
		
/*		inline const CXMLAttr * GetAttr( const size_t nAttr ) const
		{
			return m_pAttrs[ nAttr ];
		}*/

		inline CXMLAttr * GetAttr( const size_t nAttr )
		{
			return m_pAttrs[ nAttr ];
		}

		inline CXMLAttr * GetAttr( const char * szAttrName )
		{
			index_t i = INVALID_INDEX;

			while ( m_pAttrs.Iterate( i ) )
			{
				CXMLAttr * pAttr = m_pAttrs[ i ];

				if ( CStr::Compare( szAttrName, pAttr->GetName() ) )
				{
					return pAttr;
				}
			}

			return NULL;
		}

	private:

		CStr	m_Name;

		CContainer <CXMLElement *>	m_pChilds;
		CContainer <CXMLAttr *>		m_pAttrs;
};

////////////////////////////////////////////////////////////////////////////////

struct IXMLWrapper : public IBaseIntf
{
	virtual EResult LoadXML( const char * szFileName, CXMLElement ** ppOutElement ) = 0;
	virtual EResult LoadXMLFromMem( char * pBuffer, uint nSize, CXMLElement ** ppOutElement ) = 0;
	virtual EResult SaveXML( const char * szFileName, const CXMLElement * pRootElement ) = 0;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __IXMLWrapper_h_included__

////////////////////////////////////////////////////////////////////////////////