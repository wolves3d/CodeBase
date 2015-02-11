////////////////////////////////////////////////////////////////////////////////


/**
*
*/
struct TAssertInfo
{
	TAssertInfo( const char * szFile, int nLine ):
		szFileName	( szFile ),
		nFileLine	( nLine )
	{
	}

	const char *	szFileName;
	int				nFileLine;
};


/**
*
*/
struct ITestRes
{
	virtual void Init	( const char * szLogFile ) = 0;
	virtual void Release() = 0;

	virtual void Assert	( const char * szTest, const char * szFile, int nLine ) = 0;
	virtual void Fail	( const char * szTest ) = 0;
	virtual bool Next	() = 0;

	virtual bool Status	() const = 0;
};


/**
*
*/
struct CTest
{
	public:
		virtual void			RunTest()			= 0;
		virtual const char *	GetName() const		= 0;
};


/**
*
*/
class CTestRunner
{
	public:

	static void AddTest( CTest * );
	static bool Run( ITestRes * pRes );

	#define MAX_TESTS 256

	static int		ms_nTestCount;
	static CTest *	ms_pTests[ MAX_TESTS ];
};


/**
*
*/
#define UNIT_TEST( X )							\
												\
class X : public CTest							\
{												\
												\
public:											\
												\
												\
	X()											\
	{											\
		CTestRunner::AddTest( this );			\
	}											\
												\
	const char * GetName() const				\
	{											\
		return #X;								\
	}											\
												\
	void RunTest();								\
												\
} X##_instance;									\
												\
void X::RunTest()

////////////////////////////////////////////////////////////////////////////////

#ifdef DEBUG_ASSERT
#undef DEBUG_ASSERT
#endif //#ifdef DEBUG_ASSERT

/**
*
*/
//#define UNIT_ASSERT( X ) if ( !( X ) ) { m_bUnitTestResult = false; pRes->Assert( GetName(), __FILE__, __LINE__ ); }
#define UNIT_ASSERT( X ) if ( !( X ) ) throw( &TAssertInfo( __FILE__, __LINE__ ) );
#define DEBUG_ASSERT( X ) if ( !( X ) ) throw( &TAssertInfo( __FILE__, __LINE__ ) );

////////////////////////////////////////////////////////////////////////////////