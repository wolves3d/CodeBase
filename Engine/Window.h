////////////////////////////////////////////////////////////////////////////////

#ifndef __Window_h_included__
#define __Window_h_included__

////////////////////////////////////////////////////////////////////////////////

class ENGINE_API CWindow : public CBaseControl
{
	public :
	
		CWindow( CBaseControl * pParent = NULL );
		~CWindow();

		virtual bool ProcessInput	( const TInputMsg & tData );

		virtual void Render();

		virtual void OnClose()
		{
		}

		virtual void SetSize( float fWidth, float fHeight );

		virtual void	OnChildEvent( CBaseControl * pChild );

//		virtual void	OnMessage		( UIMessage eMsg, int nTag ) = 0;

	private :

		bool		m_bDragging;

		CButton *	m_pClose;
};

////////////////////////////////////////////////////////////////////////////////

#endif //#ifndef __Window_h_included__

////////////////////////////////////////////////////////////////////////////////