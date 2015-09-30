//==============================================================================
#ifndef __serialport_h_included__
#define __serialport_h_included__
//==============================================================================

#include "CodeBase/network/AbstractSocket.h"


class CSerialPort : public IAbstractSocket
{
public:

	// IAbstractSocket
	virtual int	Send(const void * data, uint byteCount);
	virtual int	Recv(void * buffer, uint maxByteCount);
	virtual void Close();

	CSerialPort();
	bool Open(const char * portName);
	bool IsValid();

private:

#ifdef WIN32
	HANDLE	m_port;
	DCB		m_config;
#else
	int m_file;
#endif

};

//==============================================================================
#endif // #ifndef __serialport_h_included__
//==============================================================================