#include "pch.h"
#include "CommandManager.h"


CCommandManager::CCommandManager()
{
	m_packetMgr = NEW CPacketManager();
	m_packetMgr->SetDelegate(this);
}


bool CCommandManager::RegisterCommand(CNetworkCommand * command)
{
	if (NULL == command)
	{
		FAIL("Invalid command pointer");
		return false;
	}

	NetCommandID commandID = command->UniqueID();
	
	
	if (NULL == FindCommand(commandID, true))
	{
		m_commandMap[commandID] = command;
		return true;
	}
	else
	{
		FAIL("Command already registred");
		return false;
	}
}


CNetworkCommand * CCommandManager::FindCommand(NetCommandID commandID, bool allowFail)
{
	CommandMap::iterator it = m_commandMap.find(commandID);

	
	if (m_commandMap.end() != it)
	{
		return (it->second);
	}
	else if (false == allowFail)
	{
		FAIL("cannot find command");
	}

	return NULL;
}

	
void CCommandManager::SendCommand(CTcpSocket * dstClient, CNetworkCommand * command)
{
	BufferObject argBuffer(256);

	uint cmdSize = command->OnSend(&argBuffer, dstClient);
	dstClient->Send(argBuffer.GetConstPointer(), cmdSize);
}


void CCommandManager::OnUnknownCommand()
{
}


void CCommandManager::OnIncomingPacket(CTcpSocket * srcClient, BufferObject * data)
{

}

void CCommandManager::OnClientLost(CTcpSocket * srcClient)
{

}