#include "CodeBase/CodeBase.h"
#include "tcp_socket.h"


#ifdef WIN32
bool g_wsaStartupFlag = false;
#endif


CTcpSocket::CTcpSocket()
	: m_socket( NULL )
{

#ifdef WIN32
	if (false == g_wsaStartupFlag)
	{
		WSADATA WsaData;
		int res = WSAStartup(0x0101, &WsaData);
		//printf("WSAStartup: %d\n", res);
		//WSACleanup();
		g_wsaStartupFlag = true;
	}
#endif

}


int CTcpSocket::Init(int port)
{
	m_socket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	printf("init_socket: %d\n", m_socket);

	m_addr.sin_port = htons(port);
	m_addr.sin_family = AF_INET;

	return 0;
}

int CTcpSocket::Accept(CTcpSocket * outClient)
{
	if (NULL == outClient)
		return 0;
	
	m_addr.sin_addr.s_addr = INADDR_ANY;
	bind(m_socket, (sockaddr *)&m_addr, sizeof(m_addr));
	listen(m_socket, SOMAXCONN);

	socklen_t addrLen = sizeof(outClient->m_addr);
	outClient->m_socket = accept(m_socket, (sockaddr *) &(outClient->m_addr), &addrLen);
	
#ifdef WIN32
	u_long value = 1;
	ioctlsocket(outClient->m_socket, FIONBIO, &value);
#else	
	fcntl(outClient->m_socket, F_SETFL, O_NONBLOCK);
#endif // #ifdef WIN32

	return 1;
}

int CTcpSocket::Connect(const char *pIPaddr, unsigned int nPort)
{
	if ( m_socket )
	{
		return -1;
	}

	m_socket = socket( AF_INET, SOCK_STREAM, IPPROTO_TCP );

#ifdef WIN32
	u_long value = 1;
	ioctlsocket(m_socket, FIONBIO, &value);
#else	
	fcntl(m_socket, F_SETFL, O_NONBLOCK);
#endif // #ifdef WIN32

	m_addr.sin_family		= AF_INET;
	m_addr.sin_addr.s_addr	= inet_addr(pIPaddr);
	m_addr.sin_port			= htons( nPort );

	/*
	DWORD dwValue = 1;

	if (SOCKET_ERROR == setsockopt(m_socket, SOL_SOCKET, TCP_NODELAY, (char *)&dwValue, sizeof(dwValue)))
	{
		printf("Set TCP_NODELAY: failed\n");
		//wprintf(L"setsockopt for SO_KEEPALIVE failed with error: %u\n", WSAGetLastError());
	}
	else
	{
		printf("Set TCP_NODELAY: ON\n");
	}
		*/



	printf( "Connecting to %s port %d... ", pIPaddr, nPort );

	int nRes = connect( m_socket, (struct sockaddr *)&m_addr, sizeof(struct sockaddr) );

	if ( 0 == nRes )
	{
		printf( "OK\n" );
	}
	else
	{
		printf ( "Error\n" );
	}

	return nRes;
}

int CTcpSocket::Send(const void *pData, unsigned int nLen)
{
	//if ( NULL == m_socket )
	//	return -1;

	if ( pData )
	{
		return send( m_socket, (const char *)pData, nLen, 0 );
	}

	return -1;
}

int CTcpSocket::Recv(void * buffer, uint maxByteCount)
{
	if (0 == maxByteCount)
		return -1;

	if (NULL == buffer)
		return -1;

	int res = recv(m_socket, (char *)buffer, maxByteCount, 0);
	return res;
}

void CTcpSocket::Close()
{
	if ( NULL != m_socket )
	{
		shutdown( m_socket, 2 );
		m_socket = NULL;
	}
}