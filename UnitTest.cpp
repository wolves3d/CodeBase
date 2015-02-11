////////////////////////////////////////////////////////////////////////////////

#include "UnitTest.h"


/**
*
*/
int		CTestRunner::ms_nTestCount = 0;
CTest *	CTestRunner::ms_pTests[ MAX_TESTS ] = { 0 };


/**
*
*/
void CTestRunner::AddTest( CTest * pTest )
{
	if ( ( ms_nTestCount + 1 ) < MAX_TESTS )
	{
		ms_pTests[ ms_nTestCount ] = pTest;
		ms_nTestCount++;
	}
	else
	{
		// "Max tests reached!"
		__debugbreak();//__asm { int 3 }
	}
}


/**
*
*/
bool CTestRunner::Run( ITestRes * pRes )
{
	for ( int i = 0; i < ms_nTestCount; ++i )
	{
		CTest * pTest = ms_pTests[ i ];

		//__try
		{
			try
			{
				pTest->RunTest();
			}
			catch( TAssertInfo * pInfo )
			{
				pRes->Assert( pTest->GetName(), pInfo->szFileName, pInfo->nFileLine );
			}
		}
		//__except( EXCEPTION_EXECUTE_HANDLER  )
		//{
		//	pRes->Fail( ms_pTests[ i ]->GetName() );
		//}

		pRes->Next();
	}

	return pRes->Status();
}

////////////////////////////////////////////////////////////////////////////////