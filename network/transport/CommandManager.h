//==============================================================================
#ifndef __CommandManager_h_included__
#define __CommandManager_h_included__
//==============================================================================

// forward declaration
class CNetworkCommand;

#include "PacketManager.h"


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


class CCommandManager
	: public IPacketManagerDelegate
{
public:

	// IPacketManagerDelegate

	virtual void OnIncomingPacket(CTcpSocket * srcClient, BufferObject * data);
	virtual void OnClientLost(CTcpSocket * srcClient);
	
	virtual void OnSoftTimeout(float time, CTcpSocket * client, CNetworkCommand * command) {};
	virtual void OnHardTimeout(float time, CTcpSocket * client, CNetworkCommand * command) {};

	// CCommandManager
	CCommandManager();

	/// Регистрация команды
	bool RegisterCommand(CNetworkCommand * command);

	CNetworkCommand * FindCommand(NetCommandID commandID, bool allowFail = false);

	/// Кладет команду в другой поток и тут же возвращает управление
	void SendCommand(CTcpSocket * dstClient, CNetworkCommand * command);

	/// Получена входящая команда от клиента
	void OnUnknownCommand();



private:
	CPacketManager * m_packetMgr;

	typedef map <NetCommandID, CNetworkCommand *> CommandMap;
	CommandMap m_commandMap;
};

//==============================================================================
#endif // #ifndef __CommandManager_h_included__
//==============================================================================