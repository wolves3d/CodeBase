/**
*
*/
struct ISample
{
	virtual uint GetLength	()												= 0;
	virtual uint GetData	( void * pDest, uint nOffset, uint nSize )		= 0;
};


/**
*
*/
struct IVoice
{
	virtual void			QueueSample	( ISample * pSample )				= 0;
	virtual void			ClearQueue	()									= 0;

	virtual void			Play		( bool bRewind = false )			= 0;
	virtual bool			IsPlaying	()									= 0;
	virtual uint			GetPlayPos	()									= 0;
	//virtual void			Pause		()									= 0;

	virtual void			SetVolume	( float fVol )						= 0;
	virtual float			GetVolume	()									= 0;

	virtual void			SetPos		( const vec3 & vPos )				= 0;
	virtual const vec3 &	GetPos		()									= 0;

	virtual void			Update		()									= 0;
};


/**
*
*/
struct ISoundMgr : public IBaseIntf
{
	virtual bool		Init		( HWND hWindow )						= 0;
	virtual void		Update		()										= 0;

	virtual IVoice *	CreateVoice	( bool bStereo )						= 0;
	virtual ISample *	CreateSample( const char * szFileName )				= 0;
	virtual ISample *	CreateSample( const void * pData, uint nDataSize )	= 0;

	virtual void		SetListener	( const vec3 * pPos, const vec3 *pTgt, const vec3 *pUp ) = 0;
};

////////////////////////////////////////////////////////////////////////////////