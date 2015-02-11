////////////////////////////////////////////////////////////////////////////////

#ifndef __ListView_h_included__
#define __ListView_h_included__

////////////////////////////////////////////////////////////////////////////////

class ENGINE_API CListView : public CBaseControl
{
	//==========================================================================
	// Tip: Public Methods & Fields
	//==========================================================================
	public :

	CListView( CBaseControl * pParent );
	~CListView();

	bool	ProcessInput	( const TInputMsg * pData );

	void	Render			();

	bool AddNode( const char * szName );
	
	//==========================================================================
	// Tip: Private Methods & Fields
	//==========================================================================
	private:

	struct TListData * m_pData;
};

////////////////////////////////////////////////////////////////////////////////

#endif //#ifndef __ListView_h_included__

////////////////////////////////////////////////////////////////////////////////