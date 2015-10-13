#include "CodeBase/CodeBase.h"
#include "CommandManager.h"


// for debug
string u_string_format(const char *fmt, ...);


CCommandManager::CCommandManager(ITransportPacket * packet, bool singleCommandMode)
	: m_packet(packet)
	, m_commandCounter(0)
	, m_singleCommandMode(singleCommandMode)
	, m_sentCommandID(0)
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
		LOG_ERROR("Invalid handler pointer");
		return 0;
	}

	int handlerID = handler->GetResponseID();
	
	if (false == CheckHandlerID(handlerID))
	{
		LOG_ERROR("Bad handler ID!");
		return 0;
	}

	if (NULL != GetHandler(handlerID))
	{
		LOG_ERROR("Command already registred");
		return 0;
	}
	
	m_handlerList[handlerID] = handler;
	LOG_INFO("handler registred (%s) with id (%d)", handler->GetName(), handlerID);

	return handlerID;
}


void CCommandManager::UnregisterHandler(uint handlerID)
{
	if (NULL != GetHandler(handlerID))
	{
		m_handlerList[handlerID] = NULL;
		LOG_INFO("handler UNregistred (%d)", handlerID);
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

	//uint z = command->GetResponseID();

	uint commandUniqueID = (0 != command->GetResponseID())
		? GetNextCmdNumber()
		: 0;

	m_packet->FillHeader(buf, cmdID, commandUniqueID, dataSize);

	// send to socket or save in queue -----------------------------------------

	if ((0 == m_sentCommandID) && (0 == m_packetQueue.size()))
	{
		TDelayedPacket sendNowPacket(socket, command, commandUniqueID, buf, packetSize);
		SendPacket(&sendNowPacket);
	}
	else
	{
		// FIXME: socket may be lost, clean on socket lost

		TDelayedPacket * delayedPacket = NEW TDelayedPacket(
			socket, command, commandUniqueID, buf, packetSize);

		m_packetQueue.push_back(delayedPacket);

		LOG_INFO("command queue inc:%llu (cmdid: %u)", (unsigned long long)m_packetQueue.size(), command->GetCommandID());
	}
}


void CCommandManager::SendPacket(TDelayedPacket *packet)
{
	const uint requestID = packet->command->GetCommandID();

//	printf("Send CMD id:%d size:%d\n", packet->packetData[0], packet->packetSize);

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

			LOG_INFO("command queue dec: %llu", (unsigned long long)m_packetQueue.size());
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

//	printf("OnIncomingPacket handler:%d tag:%d size:%d\n", handlerID, tag, dataSize);

	IResponseHandler * uniqueHandler = (0 != tag)
		? FindHandler(tag, socket)
		: NULL;

	IResponseHandler * handler = (NULL == uniqueHandler)
		? GetHandler(handlerID)
		: uniqueHandler;

	if (NULL == handler)
	{
		LOG_ERROR("command handler (%d tag:%d) not found!", handlerID, tag);
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
		map <uint, INetCommand *>::iterator it = handlerMap->m_uniqueHandlerList.find(uniqueCommandID);

		if (handlerMap->m_uniqueHandlerList.end() != it)
		{
			handlerMap->m_uniqueHandlerList.erase(it);
			return;
		}
	}

	LOG_ERROR("unique handler not found");
}

void CCommandManager::RemoveUniqueHandlers(IAbstractSocket * associatedSocket)
{
	LOG_ERROR("implement me");
}
