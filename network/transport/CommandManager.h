//==============================================================================
#ifndef __CommandManager_h_included__
#define __CommandManager_h_included__
//==============================================================================

// forward declaration
class CNetworkCommand;
class CCommandManager;

#include "PacketManager.h"


/*
typedef string NetCommandID;
#define HEADER "HE"


class CNetworkCommand
{
public:
	virtual NetCommandID UniqueID() const = 0;
	virtual void OnAnswer(CTcpSocket * srcClient, BufferObject * data) = 0;

	virtual uint OnSend(BufferObject * dstData, CTcpSocket * dstClient)
	{
		// Write command header
		return dstData->Write(0, HEADER, sizeof(HEADER));
	}
};
*/


struct IResponseHandler
{
	virtual const char * GetName() const = 0;
	virtual uint GetCommandID() const = 0;
	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr) = 0;
};

struct ICommandHandler
{
	virtual const char * GetName() const = 0;
	virtual uint GetCommandID() const = 0;
	virtual uint GetArgSize() const { return 0; }
	virtual uint FillData(void * buffer, uint maxByteCount) { return 0;  }
};


class CCommandManager
	: public IPacketManagerDelegate
{
public:

	// IPacketManagerDelegate

	virtual void OnIncomingPacket(IAbstractSocket * socket, BufferObject * data);
	virtual void OnClientLost(CTcpSocket * srcClient);
	
	virtual void OnSoftTimeout(float time, CTcpSocket * client, CNetworkCommand * command) {};
	virtual void OnHardTimeout(float time, CTcpSocket * client, CNetworkCommand * command) {};

	// CCommandManager
	CCommandManager(ITransportPacket * packet);

	/// Регистрация команды
	//bool RegisterCommand(CNetworkCommand * command);
	uint RegisterHandler(IResponseHandler * handler);
	IResponseHandler * GetHandler(uint handlerID);

	/// Кладет команду в другой поток и тут же возвращает управление
	void SendCommand(IAbstractSocket * socket, uint cmdID, void * data, uint byteCount);
	void SendCommand(IAbstractSocket * socket, ICommandHandler * handler);
	
	/// Получена входящая команда от клиента
	void OnUnknownCommand();

	void OnUpdate() { m_packetMgr->OnUpdate(m_packet); }

	CPacketManager * GetPacketManager() { return m_packetMgr; }

private:

	bool CheckHandlerID(uint handlerID);

	CPacketManager * m_packetMgr;
	ITransportPacket * m_packet;
	vector <IResponseHandler *> m_handlerList;
};

//==============================================================================
#endif // #ifndef __CommandManager_h_included__
//==============================================================================