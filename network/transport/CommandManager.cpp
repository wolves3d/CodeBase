#include "CodeBase/CodeBase.h"
#include "CommandManager.h"


// for debug
string u_string_format(const char *fmt, ...);


CCommandManager::CCommandManager(ITransportPacket * packet, bool singleCommandMode)
	: m_packet(packet)
	, m_commandCounter(0)
	, m_sentCommandID(0)
	, m_singleCommandMode(singleCommandMode)
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


uint CCommandManager::GetNextCmdNumber()
{
	++m_commandCounter;

	if (m_commandCounter >= 0xFF)
	{
		m_commandCounter = 1;
	}

	return m_commandCounter;
}


void CCommandManager::SendCommand(IAbstractSocket * socket, INetCommand * command)
{
	const uint byteCount = 256;
	static byte staticBuffer[byteCount];

	const uint cmdID = command->GetCommandID();
	const uint headerSize = m_packet->GetHeaderSize();

	byte * buf = staticBuffer;
	const uint dataSize = command->OnFillData(buf + headerSize, byteCount - headerSize);
	const uint packetSize = (headerSize + dataSize);
	
	// fill packet header ------------------------------------------------------

	uint z = command->GetResponseID();

	uint commandUniqueID = (NULL != command->GetResponseID())
		? GetNextCmdNumber()
		: 0;

	m_packet->FillHeader(buf, cmdID, commandUniqueID, dataSize);

	// send to socket or save in queue -----------------------------------------

	if ((0 == m_sentCommandID) && (0 == m_packetQueue.size()))
	{
		SendPacket(
			&TDelayedPacket(socket, command, commandUniqueID, buf, packetSize));
	}
	else
	{
		// FIXME: socket may be lost, clean on socket lost

		TDelayedPacket * delayedPacket = NEW TDelayedPacket(
			socket, command, commandUniqueID, buf, packetSize);

		m_packetQueue.push_back(delayedPacket);

		printf("command queue inc: %d\n", m_packetQueue.size());
	}
}


void CCommandManager::SendPacket(TDelayedPacket *packet)
{
	const uint requestID = packet->command->GetCommandID();

	packet->socket->Send(
		packet->packetData,
		packet->packetSize);

	if (true == m_singleCommandMode)
	{
		// FIXME: socket with command may be lost
		DEBUG_ASSERT(0 != requestID);
		m_sentCommandID = requestID;
	}


	// FIXME: WARNING! Must link socket and all associated handlers
	// to kill all handlers on socket (abnormal) shutdown
	if (0 != packet->uniqueHandlerTag)
	{
		AddUniqueHandler(
			packet->uniqueHandlerTag,
			packet->socket,
			packet->command); // FIXME add (timeout & socketLost) to handler	
	}
	else
	{
		DEL(packet->command);
	}
}


void CCommandManager::OnUpdate()
{
	m_packetMgr->OnUpdate(m_packet);

	if (m_packetQueue.size() > 0)
	{
		if (0 == m_sentCommandID)
		{
			TDelayedPacket * packetRecord = m_packetQueue[0];
			SendPacket(packetRecord);
			
			m_packetQueue.erase(m_packetQueue.begin());
			DEL(packetRecord);

			printf("command queue dec: %d\n", m_packetQueue.size());
		}
	}
}


void CCommandManager::OnUnknownCommand()
{
}


void CCommandManager::OnIncomingPacket(IAbstractSocket * socket, const byte *data, size_t dataSize)
{
	const byte * packetBytes = data;// (const byte *)data->GetConstPointer();
	uint handlerID = m_packet->GetCommandID(packetBytes);
	uint tag = m_packet->GetCommandTag(packetBytes);

	IResponseHandler * uniqueHandler = (0 != tag)
		? FindHandler(tag, socket)
		: NULL;

	IResponseHandler * handler = (NULL == uniqueHandler)
		? GetHandler(handlerID)
		: uniqueHandler;

	if (NULL == handler)
	{
		string errorStr = u_string_format("command handler (%d tag:%d) not found!", handlerID, tag);
		FAIL(errorStr.c_str());
		return;
	}

	const uint argByteCount = m_packet->GetArgumentSize(packetBytes);
	const byte * argData = (argByteCount > 0)
		? (packetBytes + m_packet->GetHeaderSize())
		: NULL;

	handler->OnResponse(argData, argByteCount, socket, this);
	handler->OnCallback(argData, argByteCount, socket, this);

	if (NULL != uniqueHandler)
	{
		RemoveUniqueHandler(socket, tag);
	}

	if (handlerID == m_sentCommandID)
	{
		m_sentCommandID = 0;
	}
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


	return NULL;
}


void CCommandManager::RemoveUniqueHandler(IAbstractSocket * associatedSocket, uint uniqueCommandID)
{
	HandlerMap * handlerMap = GetHandlerMap(associatedSocket);

	if (NULL != handlerMap)
	{
		map <uint, INetCommand *>::const_iterator it = handlerMap->m_uniqueHandlerList.find(uniqueCommandID);

		if (handlerMap->m_uniqueHandlerList.end() != it)
		{
			handlerMap->m_uniqueHandlerList.erase(it);
			return;
		}
	}

	DEBUG_MSG("unique handler not found");
}

void CCommandManager::RemoveUniqueHandlers(IAbstractSocket * associatedSocket)
{
	FAIL("implement me");
}
