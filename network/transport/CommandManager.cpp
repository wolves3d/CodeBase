#include "CodeBase.h"
#include "CommandManager.h"


CCommandManager::CCommandManager(ITransportPacket * packet)
	: m_packet(packet)
	, m_commandCounter(0)
{
	m_packetMgr = NEW CPacketManager();
	m_packetMgr->SetDelegate(this);

	const uint maxHandlerCount = 256;
	m_handlerList.reserve(maxHandlerCount);

	for (uint i = 0; i < maxHandlerCount; ++i)
	{
		m_handlerList.push_back(NULL);
	}
}


bool CCommandManager::CheckHandlerID(uint handlerID)
{
	return ((handlerID > 0) && (handlerID < m_handlerList.capacity()));
}


IResponseHandler * CCommandManager::GetHandler(uint handlerID)
{
	if (true == CheckHandlerID(handlerID))
	{
		if (handlerID < m_handlerList.size())
		{
			return m_handlerList[handlerID];
		}
	}

	return NULL;
}


uint CCommandManager::RegisterHandler(IResponseHandler * handler)
{
	if (NULL == handler)
	{
		FAIL("Invalid handler pointer");
		return 0;
	}

	int handlerID = handler->GetResponseID();
	
	if (false == CheckHandlerID(handlerID))
	{
		FAIL("Bad handler ID!");
		return 0;
	}

	if (NULL != GetHandler(handlerID))
	{
		FAIL("Command already registred");
		return 0;
	}
	
	m_handlerList[handlerID] = handler;
	printf("handler registred (%s) with id (%d)\n", handler->GetName(), handlerID);

	return handlerID;
}


void CCommandManager::UnregisterHandler(uint handlerID)
{
	if (NULL != GetHandler(handlerID))
	{
		m_handlerList[handlerID] = NULL;
		printf("handler UNregistred (%d)\n", handlerID);
	}
}

/*
void CCommandManager::SendCommand(IAbstractSocket * socket, uint cmdID, void * data, uint byteCount)
{
	const uint headerSize = m_packet->GetHeaderSize();
	const uint packetSize = (headerSize + byteCount);

	BufferObject argBuffer(256);
	if (argBuffer.GetSize() >= packetSize)
	{
		char * buf = (char *)argBuffer.GetPointer();
		m_packet->FillHeader(buf, cmdID, byteCount);

		// bound checked
		memcpy(buf + headerSize, data, byteCount);

		socket->Send(buf, packetSize);
	}
	else
	{
		FAIL("buffer too small");
	}
}*/


void CCommandManager::SendCommand(IAbstractSocket * socket, INetCommand * command)
{
	const uint cmdID = command->GetCommandID();
	const uint byteCount = 256;
	const uint headerSize = m_packet->GetHeaderSize();
	const uint packetSize = (headerSize + byteCount);
	uint commandUniqueID = 0;

	if (NULL != command->GetResponseID())
	{
		commandUniqueID = GetNextCmdNumber();
		AddUniqueHandler(commandUniqueID, socket, command); // FIXME add timeout to handler	
	}

	// FIXME: WARNING! Must link socket and all associated handlers
	// to kill all handlers on socket (abnormal) shutdown

	BufferObject argBuffer(packetSize);
	{
		char * buf = (char *)argBuffer.GetPointer();
		
		uint dataSize = command->OnFillData(buf + headerSize, byteCount);

		// insert commandUniqueID in header!!!
		m_packet->FillHeader(buf, cmdID, commandUniqueID, dataSize);
		
		socket->Send(buf, headerSize + dataSize);
	}
}


void CCommandManager::OnUnknownCommand()
{
}


void CCommandManager::OnIncomingPacket(IAbstractSocket * socket, BufferObject * data)
{
	const byte * packetBytes = (const byte *)data->GetConstPointer();
	uint handlerID = m_packet->GetCommandID(packetBytes);
	uint tag = m_packet->GetCommandTag(packetBytes);

	IResponseHandler * handler = (0 == tag)
		? GetHandler(handlerID)
		: FindHandler(tag, socket);

	if (NULL == handler)
	{
		FAIL("command handler not found!");
		return;
	}

	const uint argByteCount = m_packet->GetArgumentSize(packetBytes);
	const byte * argData = (argByteCount > 0)
		? (packetBytes + m_packet->GetHeaderSize())
		: NULL;

	handler->OnResponse(argData, argByteCount, socket, this);
	handler->OnCallback(argData, argByteCount, socket, this);
}

void CCommandManager::OnClientLost(CTcpSocket * srcClient)
{

}


CCommandManager::HandlerMap * CCommandManager::GetHandlerMap(IAbstractSocket * associatedSocket, uint * outHash)
{
	uint hash = 0;// hashFunc(associatedSocket);

	if (NULL != outHash)
	{
		(*outHash) = hash;
	}

	map <uint, HandlerMap *>::iterator it = m_socketHandlers.find(hash);
	if (m_socketHandlers.end() != it)
	{
		return (it->second);
	}

	return NULL;
}

void CCommandManager::AddUniqueHandler(uint uniqueCommandID, IAbstractSocket * associatedSocket, INetCommand * command)
{
	uint hash;
	HandlerMap * handlerMap = GetHandlerMap(associatedSocket, &hash);

	if (NULL == handlerMap)
	{
		handlerMap = new HandlerMap;
		m_socketHandlers[hash] = handlerMap;
	}

	handlerMap->m_uniqueHandlerList[uniqueCommandID] = command;
}


INetCommand * CCommandManager::FindHandler(uint uniqueCommandID, IAbstractSocket * associatedSocket)
{
	HandlerMap * handlerMap = GetHandlerMap(associatedSocket);

	if (NULL != handlerMap)
	{
		map <uint, INetCommand *>::const_iterator it = handlerMap->m_uniqueHandlerList.find(uniqueCommandID);

		if (handlerMap->m_uniqueHandlerList.end() != it)
		{
			return (it->second);
		}
	}

	DEBUG_MSG("response handler not found");
	return NULL;
}


void CCommandManager::RemoveUniqueHandler(IAbstractSocket * associatedSocket, uint uniqueCommandID)
{
	FAIL("implement me");
}

void CCommandManager::RemoveUniqueHandlers(IAbstractSocket * associatedSocket)
{
	FAIL("implement me");
}
