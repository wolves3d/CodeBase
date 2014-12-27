//==============================================================================
#ifndef __tcp_socket_h_included__
#define __tcp_socket_h_included__
//==============================================================================

#ifdef WIN32
#include <winsock.h>
#endif // WIN32


class CTcpSocket
{
public:
	CTcpSocket();

	// server
	int Init(int port);
	int Accept(CTcpSocket * outClient);
	  
	// client
	int Connect(const char *pIPaddr, unsigned int nPort);
	  
	// common
	int		Send(const void *pData, unsigned int nLen);
	int		Recv( char * pData, unsigned int nLen );
	void	Close();

//private:
	SOCKADDR_IN	m_addr;
	SOCKET		m_socket;
};

//==============================================================================
#endif // #ifndef __tcp_socket_h_included__
//==============================================================================