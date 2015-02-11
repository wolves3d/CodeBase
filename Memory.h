////////////////////////////////////////////////////////////////////////////////

#ifndef __Memory_h_included__
#define __Memory_h_included__


//==============================================================================
// Name: operator new
//==============================================================================
inline void * __cdecl operator new(	size_t size )
{
	return g_pSystem->Alloc( size, 0, 0 );
}


//==============================================================================
// Name: operator new
//==============================================================================
inline void * __cdecl operator new(	size_t size,
									const char * filename, int line_num )
{
	return g_pSystem->Alloc( size, filename, line_num );
}


//==============================================================================
// Name: operator new[]
//==============================================================================
inline void * __cdecl operator new[](	size_t size,
										const char * filename, int line_num )
{
	return g_pSystem->Alloc( size, filename, line_num );
}


//==============================================================================
// Name: operator delete
//==============================================================================
inline void __cdecl operator delete( void * pUserData )
{
	g_pSystem->Delete( pUserData );
}


//==============================================================================
// Name: operator delete[]
//==============================================================================
inline void __cdecl operator delete[]( void * pUserData )
{
	g_pSystem->Delete( pUserData );
}


//==============================================================================
// Name: operator delete
//==============================================================================
inline void __cdecl operator delete(	void * pUserData,
										const char * filename, int line_num )
{
	g_pSystem->Delete( pUserData );
}


//==============================================================================
// Name: operator delete[]
//==============================================================================
inline void __cdecl operator delete[](	void * pUserData,
										const char * filename, int line_num )
{
	g_pSystem->Delete( pUserData );
}

////////////////////////////////////////////////////////////////////////////////

#define NEW new( __FILE__, __LINE__ )
#define DEL( X ) { if ( X ) delete X; X = NULL; }
#define DEL_ARRAY( X ) if ( X ) { delete [] X; X = NULL; }
#define PURE_DEL delete

////////////////////////////////////////////////////////////////////////////////

#endif // #ifndef __Memory_h_included__

////////////////////////////////////////////////////////////////////////////////