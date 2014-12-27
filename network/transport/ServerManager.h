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


struct IServerManagerDelegate
{
	virtual void OnClientConnected(CTcpSocket * socket) = 0;
};


class CServerManager
{
public:
	CServerManager(IServerManagerDelegate * delegate)
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

	bool OnUpdate()
	{
		// iterate accepted client list

		CheckListenThread();

		return true;

		// or false for program stop
	}

	void Shutdown()
	{
		// close all clients first

		m_serverSocket.Close();
	}

	CCommandManager * GetCommandManager() { return &m_commandMgr; }

private:

	void CheckListenThread()
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
					m_clientList.push_back(clientSocket);

					m_delegate->OnClientConnected(clientSocket);
				
					CNetworkCommand * command = m_commandMgr.FindCommand("ping");
					m_commandMgr.SendCommand(clientSocket, command);


					// restart task
					m_listenThread.PushTask(task);
				}
				break;
			}
		}
	}

	CCommandManager m_commandMgr;
	CTcpSocket m_serverSocket;
	CThread m_listenThread;
	IServerManagerDelegate * m_delegate;

	vector <CTcpSocket *> m_clientList;
};

//==============================================================================
#endif // #ifndef __ServerManager_h_included__
//==============================================================================