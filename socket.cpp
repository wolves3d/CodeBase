#include "common.h"
#include "socket.h"
#include <winsock2.h>
#include "thread.h"


int CSocket::m_nGlobalInit = 0;


CSocket::CSocket(ISocketDelegates * delegates, int socket)
	: m_socket(socket)
	, m_delegates(delegates)
	, m_isNonblocking(false)
	, m_listenThread(NULL)
{
}


bool CSocket::InitSocket(bool isNonblocking)
{
	assert(EMPTY_SOCKET == m_socket);

	if (EMPTY_SOCKET != m_socket)
		return false;

	//--------------------------------------------------------------------------

	if (0 == m_nGlobalInit)
	{
		WSADATA wsaData;
		int startupResult = WSAStartup(MAKEWORD(2, 2), &wsaData);

		if (0 != startupResult)
		{
			Log("WSAStartup failed with error: %d\n", startupResult);
			return false;
		}

		m_nGlobalInit++;
	}

	m_socket = socket(AF_INET, SOCK_STREAM, /*IPPROTO_TCP*/ 0);

	if (0 > m_socket)
	{
		LogError();
		return false;
	}

	if (isNonblocking)
	{
		SetNonblocking();
		m_isNonblocking = true;
	}

	return true;
}


void CSocket::SetNonblocking()
{
	/*
	int flags;

// If they have O_NONBLOCK, use the Posix way to do it
#if defined(O_NONBLOCK)
	// Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5.
	if (-1 == (flags = fcntl(fd, F_GETFL, 0)))
		flags = 0;
	return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
	//Otherwise, use the old way of doing it
	flags = 1;
	return ioctl(m_socket, FIOBIO, &flags);
#endif
	*/

	// If arg is non zero, non-blocking mode is enabled.
	u_long arg = 1;
	if (0 == ioctlsocket(m_socket, FIONBIO, &arg))
		return;

	Log("Make non-blocking socket failed");
}


void CSocket::LogError()
{
	Log("FAILED - ");

	int errorCode = WSAGetLastError();

	switch (errorCode)
	{
		case ECONNREFUSED:
		case WSAECONNREFUSED:
			Log("No one listening on the remote address\n");
		break;

		default:
			Log("Unknown error\n");
	}
}


bool CSocket::Connect(const char * szHost, int port)
{
	/*unsigned long*/int addrIP = inet_addr(szHost);
	Log("Connecting to %s:%d... ", szHost, port);

	if (false == InitSocket())
		return false;

	addrIP = ((127 << 24 ) | 1);

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(addrIP);

	if (0 > connect(m_socket, (sockaddr *)(&addr), sizeof(sockaddr_in)))
	{
		LogError();
		return false;
	}

	return true;
}


void CSocket::Close()
{
	assert(EMPTY_SOCKET != m_socket);

	if (EMPTY_SOCKET != m_socket)
	{
		Log("Closing socket... ");
		closesocket(m_socket);
		//shutdown(m_socket, SD_BOTH);
		m_socket = EMPTY_SOCKET;
		Log("OK\n");
	}
}


void CSocket::Release()
{
	if (EMPTY_SOCKET != m_socket)
		Close();

	delete this;

	//--------------------------------------------------------------------------
	/*
	m_nGlobalInit--;

	if (0 == m_nGlobalInit)
	{
		WSACleanup();
	}
	*/
}


int CSocket::Recv(void * buffer, unsigned int maxSize)
{
	assert(EMPTY_SOCKET != m_socket);

	if (EMPTY_SOCKET != m_socket)
	{
		int byteCount = recv(m_socket, (char *)buffer, maxSize, 0);

		if (byteCount >= 0)
		{
			return byteCount;
		}

		// FIXME ADD fail log here
	}

	return -1;
}


int CSocket::Send(void * buffer, unsigned int bufferSize)
{
	assert(EMPTY_SOCKET != m_socket);

	if (EMPTY_SOCKET != m_socket)
	{
		int byteCount = send(m_socket, (char *)buffer, bufferSize, 0);

		if (byteCount >= 0)
		{
			return byteCount;
		}

		// FIXME ADD fail log here
	}

	return -1;
}


bool CSocket::IsNonblocking()
{
	return m_isNonblocking;
}


void CSocket::OnConnectionAccepted(int socket)
{
	m_delegates->ConnectionAcceptedUnsafe(socket);
}


class CListenThread : public CThread
{
	CSocket * m_socket;

	virtual void * Main()
	{
		SetActive(true);

		while (IsActive()) // FIXME need to break
		{
			int clientSocket = accept(m_socket->m_socket, NULL, NULL);

			if (clientSocket < 0)
			{
				if (false == m_socket->IsNonblocking())
				{
					Log("Accept failed\n");
				}

				Idle(10);
				continue;
			}

			//----------------------------------------------------------------------

			sockaddr_in clientAddr;
			int addrSize = sizeof(clientAddr);
			getsockname(clientSocket, (sockaddr *)(&clientAddr), &addrSize);

			int ip4addr = clientAddr.sin_addr.s_addr;
			Log("client accepted from %d.%d.%d.%d\n",
				0x000000FF & ip4addr,
				0x0000FF00 & ip4addr,
				0x00FF0000 & ip4addr,
				0xFF000000 & ip4addr);

			m_socket->OnConnectionAccepted(clientSocket);
		}

		return NULL;
	}

public:
	CListenThread(CSocket * socket)
		: m_socket(socket)
	{
	}
};


bool CSocket::StartListen(unsigned short port, bool isNonblocking)
{
	if (NULL == m_delegates)
	{
		Log("Error: No delegates - no listen\n");
		assert(false);
		return false;
	}

	Log("Listening on %u\n", port);

	if (false == InitSocket(isNonblocking))
		return false;

	sockaddr_in addr;
	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);
	addr.sin_addr.s_addr = htonl(INADDR_ANY);

	if (0 > bind(m_socket, (sockaddr *)(&addr), sizeof(addr)))
	{
		// FIXME ADD log here
		return false;
	}

	listen(m_socket, 1);

	//--------------------------------------------------------------------------

	m_listenThread = NEW CListenThread(this);

	if (NULL != m_listenThread)
	{
		m_listenThread->Start(CThread::EWM_NORMAL);
		m_listenThread->WaitForStart();
		return true;
	}

	// FIXME ADD log here
	return false;
};