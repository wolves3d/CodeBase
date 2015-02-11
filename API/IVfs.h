////////////////////////////////////////////////////////////////////////////////

#ifndef __IVfs_h_included__
#define __IVfs_h_included__

////////////////////////////////////////////////////////////////////////////////

enum EVfsEnum
{
	VFS_BINARY	= 1 << 0,
	VFS_TEXT	= 1 << 1,
	VFS_READ	= 1 << 2,
	VFS_WRITE	= 1 << 3,
	
	VFS_SEEK_SET,
	VFS_SEEK_CUR,
	VFS_SEEK_END
};

////////////////////////////////////////////////////////////////////////////////

struct IFile
{
	virtual size_t	GetSize	() const	= 0;
	virtual size_t	Tell	() const	= 0;
	virtual size_t	Remain	() const	= 0;
	// virtual void	Flush	()			= 0;;
};



struct IFileSearch
{
	/*
	virtual size_t	GetSize	() const	= 0;
	virtual size_t	Tell	() const	= 0;
	virtual size_t	Remain	() const	= 0;
	// virtual void	Flush	()			= 0;;
	*/
};


/**
*
*/
struct IVfs : public IBaseIntf
{
	virtual EResult OpenFile	( IFile ** ppOutFile, const char * szFileName, int nAccess )	= 0;
	virtual EResult	CloseFile	( IFile ** ppFile )								= 0;

	virtual int		Seek		( IFile * pFile, long nOffset, EVfsEnum eOrigin ) = 0;
	virtual size_t	Read	( IFile * pFile, void * pBuffer, size_t nSize )= 0;
	virtual	size_t	Write	( IFile * pFile, const void * pBuffer, size_t nSize )= 0;

	///

	virtual EResult OpenSearch	( IFileSearch ** ppOutSearch )	= 0;
	virtual bool	SearchNext	( IFileSearch * pSearch )		= 0;
	virtual EResult CloseSearch	( IFileSearch ** ppSearch )		= 0;

	// Utils ///////////////////////////////////////////////////////////////////

	virtual bool	IsFileExist	( const char * szFileName ) const = 0;
	virtual char *	LoadToMem	( const char * szFileName, int nAccess ) = 0;

	bool CheckRead( IFile * pFile, void * pBuffer, size_t nSize )
	{
		return ( nSize == Read( pFile, pBuffer, nSize ) );
	}
};

////////////////////////////////////////////////////////////////////////////////

extern IVfs * g_pVfs;

////////////////////////////////////////////////////////////////////////////////

#define VFS_GUID 8084

////////////////////////////////////////////////////////////////////////////////

#endif // #define __IVfs_h_included__

////////////////////////////////////////////////////////////////////////////////