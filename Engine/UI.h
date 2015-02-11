////////////////////////////////////////////////////////////////////////////////

#ifndef __UI_h_included__
#define __UI_h_included__

////////////////////////////////////////////////////////////////////////////////

#include "BaseControl.h"
//#include "StaticText.h"
#include "Window.h"
#include "Button.h"
#include "Edit.h"
#include "ListView.h"

////////////////////////////////////////////////////////////////////////////////

struct ISprite2D
{
	//virtual ~ISprite2D() { DEBUG_MSG( "Not allowed to delete!" ) };
	//virtual void			Release()								= 0;

	virtual const vec2 &	GetPos() const							= 0;
	virtual void			SetPos( float x, float y )				= 0;


	// ¬нимание! SetSize не действительна, если не задана позици€!
	virtual void			SetSize( float fWidth, float fHeight )	= 0;
	virtual const vec2		GetSize() const							= 0;

	virtual void			SetUVs( const vec4 & vUV ) = 0;

	virtual bool			IsVisible() const						= 0;

	virtual void			SetMaterial( IMaterial * pMaterial )	= 0;
	virtual IMaterial *		GetMaterial()							= 0;

	virtual uint			GetFirstVertex	() const				= 0;
	virtual uint			GetID			() const				= 0;
};

////////////////////////////////////////////////////////////////////////////////

struct IText2D
{
	virtual void			Release()								= 0;

	virtual const vec2 &	GetPos() const							= 0;
	virtual void			SetPos( float x, float y )				= 0;

	virtual void			SetText( const char * szText )			= 0;
	virtual void			SetColor( const vec4 & vColor )			= 0;

	virtual void			Render()			= 0;
};


/*
================================================================================
//	Name:
//	Desc:
================================================================================
*/
struct IMyFont
{
	virtual const PTexture	GetTexture	() const				= 0;
	virtual const vec4 &	GetCharUV	( char cChar ) const	= 0;
	virtual float			GetCharWidth( char cChar ) const	= 0;
};


/*
================================================================================
//	Name:
//	Desc:
================================================================================
*/
struct IUIManager
{
	virtual ~IUIManager() {};
	virtual	CBaseControl *	GetDesktop	()									= 0;
	virtual void			SetDesktop	( CBaseControl * pDesktop )			= 0;
	virtual bool			PushDesktop	()									= 0;
	virtual bool			PopDesktop	()									= 0;

	virtual bool			LoadFont	( const char * szFntFile, const char * szTexture  )	= 0;
	virtual const IMyFont *	GetFont		( const char * szName ) = 0;

	virtual void			ShowCursor( bool bShow = true )					= 0;
	virtual const ivec2 &	GetMousePos() const								= 0;

	virtual ISprite2D *		CreateSprite() = 0;
	virtual void			DeleteSprite( ISprite2D * pSprite ) = 0;
	virtual void			PushSprite	( ISprite2D * pSprite, bool bDrawBeforeUI = false ) = 0;
//	virtual void			RenderSprite( ISprite2D * pSprite ) = 0;
	virtual IText2D *		CreateText	() = 0;

	virtual void			Render		() = 0;

	virtual void SetActiveControl( CBaseControl * pActiveControl ) = 0;
	virtual const CBaseControl * GetActiveControl() const = 0;
	virtual void InitControl( CBaseControl * pCtrl ) = 0;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __UI_h_included__

////////////////////////////////////////////////////////////////////////////////