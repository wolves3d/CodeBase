//==============================================================================
#ifndef __AbstractSocket_h_included__
#define __AbstractSocket_h_included__
//==============================================================================

struct IAbstractSocket
{
	virtual int	Send(const void * data, uint byteCount) = 0;
	virtual int	Recv(void * buffer, uint maxByteCount) = 0;
	virtual void Close() = 0;
};

//==============================================================================
#endif // #ifndef __AbstractSocket_h_included__
//==============================================================================