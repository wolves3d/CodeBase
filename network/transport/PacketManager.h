//==============================================================================
#ifndef __PacketManager_h_included__
#define __PacketManager_h_included__
//==============================================================================

#include "tcp_socket.h"


struct IPacketManagerDelegate
{
	/// ���������� �������� �������
	virtual void OnIncomingPacket(CTcpSocket * srcClient, BufferObject * data) = 0;

	virtual void OnClientLost(CTcpSocket * srcClient) = 0;

	/// ��������� ��������, ������� "��������" � ����...
	virtual void OnSoftTimeout(float time, CTcpSocket * client, CNetworkCommand * command) = 0;

	/// ������ ����� ������ ���, ����� �� ������� � ������� ������� �������
	virtual void OnHardTimeout(float time, CTcpSocket * client, CNetworkCommand * command) = 0;
};

class CPacketManager
{
public:

	void SetDelegate(IPacketManagerDelegate * delegate) { m_delegate = delegate; };
	void SetTimeout(float delay);

	void AddClent(CTcpSocket * clientSocket);
	void RemoveClent(CTcpSocket * clientSocket);

	/// ������� ��� ������, ���� ���������� ������, callbacks
	void OnUpdate()
	{

	}

	void SendPacket(CTcpSocket * clientSocket, BufferObject * data, void * callBack);

private:
	IPacketManagerDelegate * m_delegate;
};

//==============================================================================
#endif // #ifndef __PacketManager_h_included__
//==============================================================================