//==============================================================================
#ifndef __ServerManager_h_included__
#define __ServerManager_h_included__
//==============================================================================

#include "CommandManager.h"
#include "CodeBase/thread.h"

class ListenTask : public CThreadTask
{
public:
	ListenTask(CTcpSocket * socket)
		: m_socket(socket)
	{
	}

	virtual void OnRun()
	{
		m_socket->Accept(&m_clientSocket);
	}

	const CTcpSocket & GetClientSocket()
	{
		return m_clientSocket;
	}

	CTcpSocket * m_socket;
	CTcpSocket m_clientSocket;
};


struct INetListenerDelegate
{
	virtual void OnClientConnected(CTcpSocket * socket) = 0;
};


class CNetListener
{
public:
	CNetListener(INetListenerDelegate * delegate)
		: m_delegate(delegate)
	{
	}

	bool Init(int portToListen)
	{
		m_port = portToListen;
		printf("Server listening on port %d\n", portToListen);
		m_serverSocket.Init(portToListen);

		m_listenThread.Init();
		m_listenThread.PushTask(new ListenTask(&m_serverSocket));
		Sleep(500); // FIXME for debug

		return true;
	}

	void OnUpdate()
	{
		MessageVector messages;
		m_listenThread.GetMessageQueue(&messages);

		for (uint i = 0; i < messages.size(); ++i)
		{
			const ThreadMsg & msg = messages[i];

			//switch (msg.code)

			if (ThreadMsg::TASK_COMPLETED == msg.code)
			{
				ListenTask * task = SAFE_CAST(ListenTask *, msg.task);
				CTcpSocket * clientSocket = new CTcpSocket(task->GetClientSocket());
				//m_clientList.push_back(clientSocket);

				m_delegate->OnClientConnected(clientSocket);

				// restart task
				printf("Server listening on port %d\n", m_port);
				//m_serverSocket.Init(m_port);
				m_listenThread.PushTask(task);
			}
		}
	}

	void Shutdown()
	{
		// close all clients first
		m_serverSocket.Close();
	}

private:

	CTcpSocket m_serverSocket;
	CThread m_listenThread;
	INetListenerDelegate * m_delegate;

	int m_port;

	//vector <CTcpSocket *> m_clientList;
};

//==============================================================================
#endif // #ifndef __ServerManager_h_included__
//==============================================================================
