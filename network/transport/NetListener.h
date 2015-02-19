//==============================================================================
#ifndef __ServerManager_h_included__
#define __ServerManager_h_included__
//==============================================================================

#include "CommandManager.h"
#include "thread.h"

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
		m_serverSocket.Init(portToListen);

		m_listenThread.Init();
		m_listenThread.PushTask(new ListenTask(&m_serverSocket));

		return true;
	}

	void OnUpdate()
	{
		MessageVector messages;
		m_listenThread.GetMessageQueue(&messages);

		for (uint i = 0; i < messages.size(); ++i)
		{
			const ThreadMsg & msg = messages[i];

			switch (msg.code)
			{
				case ThreadMsg::TASK_COMPLETED:
				{
					ListenTask * task = SAFE_CAST(ListenTask *, msg.task);
					CTcpSocket * clientSocket = new CTcpSocket(task->GetClientSocket());
					//m_clientList.push_back(clientSocket);

					m_delegate->OnClientConnected(clientSocket);

					// restart task
					m_listenThread.PushTask(task);
				}
				break;
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

	//vector <CTcpSocket *> m_clientList;
};

//==============================================================================
#endif // #ifndef __ServerManager_h_included__
//==============================================================================