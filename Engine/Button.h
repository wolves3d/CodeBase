////////////////////////////////////////////////////////////////////////////////

#ifndef __Button_h_included__
#define __Button_h_included__

////////////////////////////////////////////////////////////////////////////////

class CButton : public CBaseControl
{
	public :

		inline CButton( CBaseControl * pParent = NULL ) :
			CBaseControl( pParent ),
			m_bPressed( false )
		{
		}
		
		bool Create()
		{
			return false;
			//m_pButtonShape = g_pEngine
		}

		bool ProcessInput( const TInputMsg & tData );
		void Render();

	private :

		inline void Press( bool bPress = true )
		{
			m_bPressed = bPress;
		}

		bool m_bPressed;

		IMesh	* m_pButtonShape;
};

////////////////////////////////////////////////////////////////////////////////

#endif //#ifndef __Button_h_included__

////////////////////////////////////////////////////////////////////////////////