#include "pch.h"
#include "CommandManager.h"


CCommandManager::CCommandManager(ITransportPacket * packet)
	: m_packet(packet)
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


ICommandHandler * CCommandManager::GetHandler(uint handlerID)
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


uint CCommandManager::RegisterHandler(ICommandHandler * handler)
{
	if (NULL == handler)
	{
		FAIL("Invalid handler pointer");
		return 0;
	}

	int handlerID = handler->GetCommandID();
	
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
}


void CCommandManager::OnUnknownCommand()
{
}


void CCommandManager::OnIncomingPacket(IAbstractSocket * socket, BufferObject * data)
{
// 	uint cmdID = buffer[0];
// 	uint argSize = buffer[1];

	const byte * packetBytes = (const byte *)data->GetConstPointer();
	uint handlerID = m_packet->GetCommandID(packetBytes);
	ICommandHandler * handler = GetHandler(handlerID);

	if (NULL == handler)
	{
		FAIL("command handler not found!");
		return;
	}

	const uint argByteCount = m_packet->GetArgumentSize(packetBytes);
	const byte * argData = (argByteCount > 0)
		? (packetBytes + m_packet->GetHeaderSize())
		: NULL;

	handler->OnResponse(argData, argByteCount, socket);
}

void CCommandManager::OnClientLost(CTcpSocket * srcClient)
{

}