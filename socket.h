#ifndef __socket_h_included__
#define __socket_h_included__


class CSocket;


struct ISocketDelegates
{
	// Вызывается не из основоного потока
	virtual void ConnectionAcceptedUnsafe(int clientSocket) = 0;
};


class CSocket
{
	friend class CListenThread;
	enum ELocal
	{
		EMPTY_SOCKET = -1
	};

	static int			m_nGlobalInit;
	ISocketDelegates *	m_delegates;
	int					m_socket;
	bool				m_isNonblocking;
	class CThread	*	m_listenThread;

	bool InitSocket(bool isNonblocking = false);
	void LogError();

	void OnConnectionAccepted(int socket);
	void OnReceive(byte * buffer, uint size);

public:

	void SetNonblocking();
	CSocket(ISocketDelegates * delegates, int socket = EMPTY_SOCKET);

	bool Connect(const char * szHost, int port);

	void Close();
	void Release();

	bool StartListen(unsigned short port, bool isNonblocking);
	void StopListen();
	
	int Recv(void * buffer, unsigned int maxSize);
	int Send(void * buffer, unsigned int bufferSize);
	bool IsNonblocking();
};


#endif // #ifndef __socket_h_included__