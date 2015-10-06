//==============================================================================
#ifndef __tcp_socket_h_included__
#define __tcp_socket_h_included__
//==============================================================================

#ifdef WIN32

#include <winsock2.h>
#include <ws2tcpip.h>

#else

#include <fcntl.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef int SOCKET;
#define InetPton inet_pton

#endif // WIN32

#include "CodeBase/network/AbstractSocket.h"


class CTcpSocket : public IAbstractSocket
{
public:

	// IAbstractSocket
	virtual int	Send(const void * data, uint byteCount);
	virtual int	Recv(void * buffer, uint maxByteCount);
	virtual void Close();

	CTcpSocket();

	// server
	int Init(int port);
	int Accept(CTcpSocket * outClient);
	  
	// client
	int Connect(const char *pIPaddr, unsigned int nPort);

//private:
	//sockaddr_in
	sockaddr_in	m_addr;
	SOCKET		m_socket;
};

//==============================================================================
#endif // #ifndef __tcp_socket_h_included__
//==============================================================================