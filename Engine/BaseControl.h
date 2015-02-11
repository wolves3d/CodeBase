////////////////////////////////////////////////////////////////////////////////

#ifndef __BaseControl_h_included__
#define __BaseControl_h_included__

////////////////////////////////////////////////////////////////////////////////

enum UICtrlFlag
{
	UCF_MODAL_FRAME = 1 << 0,
	UCF_STATIC_EDGE = 1 << 1,
	UCF_CLIENT_EDGE	= 1 << 2,
	UCF_SYSMENU		= 1 << 3,
	UCF_IMAGE		= 1 << 4,
};

enum UIMessage
{
	EUI_MOUSE_MOVE,
	EUI_MOUSE_OVER,
	EUI_LEFT_CLICK,
	EUI_RIGHT_CLICK
};

////////////////////////////////////////////////////////////////////////////////

class ENGINE_API CBaseControl
{
	friend class CWindow;
	friend class CButton;

	public :

					CBaseControl	( CBaseControl * pParent = NULL );
					~CBaseControl	();
	void			Release			();

	CBaseControl *	GetParent		();

	int				GetTag			() const;

	dword			GetFlag			() const;
	void			SetFlag			( dword dwFlag );

	bool			IsFocused		() const;
	void			SetFocus		( bool bFocus = true );

	virtual const char *	GetText			() const;
	virtual uint			GetTextLength	() const;
	virtual void			SetText			( const char * szText );

	const vec2 &	GetPos			() const;
	void			SetPos			( float x, float y );

	void			SetSize			( float fWidth, float fHeight );
	const vec2 &	GetSize			() const;

	void			SetClip			( float fWidth, float fHeight );
	const vec2 &	GetClip			() const;

	vec2			CalcPos			() const;

	const vec4 &	CalcClientRect	();
	const vec4 &	GetClientRect	() const;

	bool			IsEnabled		() const;
	void			Enable			( bool bEnable = true );

	bool			IsVisible		() const;
	void			Show			( bool bShow = true );

	virtual bool	IsMouseOver		( const ivec2 & vMouse );

	dword			GetColor		() const;
	void			SetColor		( dword dwColor );

	uint			GetChildCount	() const;
	CBaseControl *	GetChildControl	( uint nCtrlID );
	int				AddControl		( CBaseControl * pControl, int nTag );
	void			RemoveControl	( int nTag );

	virtual void	OnChildEvent( CBaseControl * pChild ) {};

	// Если сообщение обработано, то возвращаемое значение - истина
	virtual bool	ProcessInput( const TInputMsg & tData )
	{
		//----------------------------------------------------------------------
		// Tip: Process input for children
		//----------------------------------------------------------------------
		index_t nIndex = INVALID_INDEX;

		for ( uint n = 0; n < GetChildCount(); ++n )
		{
			CBaseControl * pChild = GetChildControl( n );

			if ( pChild->ProcessInput( tData ) )
			{
				return true; // msg was processed by last child call
			}
		}

		return false;
	}

	virtual bool	Init		() { return true; }
	virtual void	Render		();

	//protected :

		struct TCtrlData *		m_pData;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __BaseControl_h_included__

////////////////////////////////////////////////////////////////////////////////