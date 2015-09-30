////////////////////////////////////////////////////////////////////////////////

#ifndef __ISystem_h_included__
#define __ISystem_h_included__

////////////////////////////////////////////////////////////////////////////////
#define SYSTEM_API
/*
#ifdef SYSTEM_IMPLEMENTATION

	#define SYSTEM_API __declspec( dllexport )

#else

	#define SYSTEM_API __declspec( dllimport )

#endif // #ifdef SYSTEM_IMPLEMENTATION
	*/
////////////////////////////////////////////////////////////////////////////////

struct ITestRes;

////////////////////////////////////////////////////////////////////////////////

enum EResult
{
	R_OK = 0,			// no error
	R_GENERIC_ERROR,	// unknown
	R_OUT_OF_MEMORY,	//
	R_INVALID_ARG,
	R_INVALID_OP,
	R_FILE_ERROR
};

/*
================================================================================
//	Name:
//	Desc:
================================================================================
*/
struct TFileChunk
{
	size_t	nID;
	long	nSize;
};

struct ILexer
{
	virtual bool Alloc		( uint nSize, const char * pData = 0 )	= 0;
	virtual bool SetData	( const char * pData, uint nSize )		= 0;
	virtual void Free		()										= 0;
	virtual void Release	()										= 0;
	virtual bool ExpectFor	( char * szToken )						= 0;
	virtual bool GetToken	( char * szToken, uint nMaxLen )		= 0;
	virtual bool GetString	( char * szBuffer, uint nMaxLen )		= 0;
	virtual bool GetInteger	( int & nInteger )						= 0;
	virtual bool GetFloat	( float & fFloat )						= 0;
};

/*
================================================================================
//	Name:
//	Desc:
================================================================================
*/
struct IEasyWin
{
	virtual bool		Create		( HINSTANCE hInst ) = 0;
	virtual bool		Idle		() = 0;
	virtual HWND		GetHandle	() const = 0;
	virtual HINSTANCE	GetInstance	() const = 0;
	virtual void		SetSize		( int nWidth, int nHeight )				= 0;
	virtual void		GetSize		( int * pOutWidth, int * pOutHeight ) const = 0;
	virtual void		SetTitle	( const char *szTitle ) = 0;
};


//==============================================================================
//
// Базовый интерфейс.
//
//==============================================================================
enum EInterfaceType
{
	IT_VFS = 0,
	IT_ENGINE,
	IT_RENDERER,
	IT_RES_MGR,
	IT_INPUT,
	IT_UTIL,
	IT_SOUND_MGR,

	IT_MAX_TYPES
};

struct TIntfDesc
{
	int				nGUID;//GUID
	EInterfaceType	eType;
	int				nVersion;
	char *			szDescString;
};


/**
*
*/
struct IBaseIntf
{
	virtual const TIntfDesc *	GetInterfaceDesc() const	= 0;
	virtual void				Release			()			= 0;
};


/**
*
*/
struct IClassDesc
{
	virtual void DeclField( const char * szType, const char * szName, uint nOffset ) = 0;
	virtual const char * GetName() = 0;
};


/**
*
*/
struct ICounter
{
	virtual const char *	GetName			() const						= 0;

	virtual float			GetTime			() const						= 0;
	virtual void			ResetTime		()								= 0;

	virtual uint			GetValue		() const						= 0;
	virtual void			IncValue		( size_t nInc = 1 )				= 0;
	virtual void			ResetValue		()								= 0;
};

//!

//!
//!
#define REG_VAR_STRING( name, val, var ) IVar * name##_loc = g_pVarStorage->RegVar( name ); name##_loc->SetString( val );


//!

//!
//!
struct IVar
{
	virtual bool			SetString	( const char * szString )			= 0;
	virtual const char *	GetString	() const							= 0;
};


//!

//!
//!
struct IVarStorage
{
	virtual IVar *			RegVar		( const char * szVarName )			= 0;
	virtual IVar *			GetVar		( const char * szVarName )			= 0;

	virtual uint			GetVarID	( const char * szVarName ) const	= 0;

	virtual const char *	GetVarName	( uint nVar ) const					= 0;
	virtual ETypeID			GetVarType	( uint nVar ) const					= 0;

	virtual bool			SetFloat	( uint nVar, float	fVal )			= 0;
	virtual bool			SetInt		( uint nVar, int	nVal )			= 0;
	virtual bool			SetString	( uint nVar, const char *szString )	= 0;

	virtual float			GetFloat	( uint nVar ) const					= 0;
	virtual int				GetInt		( uint nVar ) const					= 0;
	virtual const char *	GetString	( uint nVar ) const					= 0;
};


/*
================================================================================
//	Name:
//	Desc:
================================================================================
*/
struct SYSTEM_API ISystem
{
	virtual bool	Init		( const char * szPath = NULL )				= 0;
	virtual void	Release		()											= 0;

	//--------------------------------------------------------------------------
	//	Tip: Memory management
	//--------------------------------------------------------------------------
	virtual void *	Alloc	(	size_t nSize,
								const char * szFileName, int nLine )		= 0;
	virtual void	Delete	(	void * pData )								= 0;

	// Debug ------------------------------------------------------------------

	virtual void	WriteLog	(	const char * szLog )					= 0;
	virtual void	AssertMsg	(	const char * szFile, int nLine,
									const char * szString )					= 0;
	//--------------------------------------------------------------------------
	//	Tip: Types information
	//--------------------------------------------------------------------------
	virtual ETypeID GetType			( const char * szType )					= 0;
	virtual uint	GetSizeOf		( ETypeID eType )						= 0;
	
	//--------------------------------------------------------------------------
	//	Tip: Classes desc
	//--------------------------------------------------------------------------
	/*
	virtual IClassDesc *	CreateClassDesc	( const char * szName, void * pFunc ) = 0;
	virtual void *			CreateClassInst	( const char * szName ) = 0;
	
	//--------------------------------------------------------------------------
	//	Tip: Libraries
	//--------------------------------------------------------------------------
	//virtual size_t		GetNumInterfaces( EInterfaceType eType = IT_ANY )= 0;
	virtual IBaseIntf *		CreateInterface	( int nIntfGUID )				= 0;
	virtual IBaseIntf *		GetInterface	( EInterfaceType eType )		= 0;
	*/
	//--------------------------------------------------------------------------
	//	Tip: Benchmarks, counters
	//--------------------------------------------------------------------------
	//virtual ITestRes *		CreateTestRes	()								= 0;

	virtual ICounter *		AddCounter		( const char * szName )			= 0;
	virtual void			RemoveCounter	( ICounter * pCounter )			= 0;
	virtual void			StartCount		( ICounter * pCounter )			= 0;
	virtual void			StopCount		( ICounter * pCounter )			= 0;

	virtual ICounter *		GetCounter		( uint nCounter )				= 0;
	virtual uint			GetCounterCount	()								= 0;

	virtual IVarStorage *	GetVarStorage	()								= 0;
};

////////////////////////////////////////////////////////////////////////////////

extern ISystem * g_pSystem;

////////////////////////////////////////////////////////////////////////////////

typedef ISystem * (*PROC_CREATESYSTEM)();

extern "C"
{
	__declspec( dllexport ) ISystem * CreateSystem();
}


/**
*
*/
struct ILibrary
{
	virtual size_t				GetNumInterfaces() const						= 0;
	virtual const TIntfDesc *	GetInterfaceDesc( size_t nInterfaceID ) const	= 0;
	virtual IBaseIntf *			CreateInterface	( int nInterfaceID )			= 0;
};


/**
*
*/
#define DECL_CLASS( Class, Func ) IClassDesc * p##Class##_Desc = g_pSystem->CreateClassDesc( #Class, Func ); Class * p##Class = NULL;
#define DECL_FIELD( Class, Type, Name, Field ) p##Class##_Desc->DeclField( #Type, Name, (uint)&p##Class->Field );

////////////////////////////////////////////////////////////////////////////////

#define SysWrite( X ) if ( g_pSystem ) g_pSystem->WriteLog( X )

//#define ADD_COUNTERSysWrite( X ) if ( g_pSystem ) g_pSystem->WriteLog( X )

////////////////////////////////////////////////////////////////////////////////

#define RELEASE( X ) if ( X ) { X->Release(); X = NULL; }

////////////////////////////////////////////////////////////////////////////////

#define	DBG_STR_NULL_ARG	"Required argument is a null pointer"
#define	DBG_STR_OUT_OF_MEM	"Out of memory"
#define	DBG_STR_INVALID_ID	"Invalid index"

#define DEBUG_LOG(debugMode, debugStr)

// #ifdef _DEBUG
// 
// #define DEBUG_ASSERT( X ) if ( !( X ) ) g_pSystem->AssertMsg( __FILE__, __LINE__, #X );
// #define DEBUG_MSG( X ) g_pSystem->AssertMsg( __FILE__, __LINE__, X );
// 
// #else

#define DEBUG_ASSERT( X ) if ( !( X ) ) AssertMsg( __FILE__, __LINE__, #X );
#define DEBUG_MSG( X ) AssertMsg( __FILE__, __LINE__, #X );

//#endif // #ifdef _DEBUG

////////////////////////////////////////////////////////////////////////////////

class CBenchmark
{
	public:

		CBenchmark( ICounter * pCounter )
		{
			m_pCounter = pCounter;
			g_pSystem->StartCount( m_pCounter );
		}

		~CBenchmark()
		{
			g_pSystem->StopCount( m_pCounter );
		}

	private:

		ICounter * m_pCounter;
};

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __ISystem_h_included__

////////////////////////////////////////////////////////////////////////////////