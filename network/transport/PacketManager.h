//==============================================================================
#ifndef __PacketManager_h_included__
#define __PacketManager_h_included__
//==============================================================================

#include "tcp_socket.h"


struct ITransportPacket
{
	virtual uint GetHeaderSize() const = 0;
	virtual bool CheckHeader(const void * header, uint byteCount) = 0;

	virtual uint GetArgumentSize(const void * header) = 0;
	virtual bool CheckPacket(const void * packet, uint byteCount) = 0;

	virtual void FillHeader(void * dst, uint cmdID, uint byteCount) = 0;

	virtual uint GetCommandID(const void * header) = 0;
};


struct IPacketManagerDelegate
{
	/// обработчик входящих пакетов
	virtual void OnIncomingPacket(IAbstractSocket * socket, BufferObject * data) = 0;

	virtual void OnClientLost(CTcpSocket * srcClient) = 0;

	/// Небольшая задержка, выводим "кружочек" и ждем...
	virtual void OnSoftTimeout(float time, CTcpSocket * client, CNetworkCommand * command) = 0;

	/// Дальше ждать смысла нет, ответ не получен в течении долгого времени
	virtual void OnHardTimeout(float time, CTcpSocket * client, CNetworkCommand * command) = 0;
};


class CPacketManager
{
private:
	IPacketManagerDelegate * m_delegate;
	vector <IAbstractSocket *> m_socketList;

public:

	CPacketManager()
		: m_delegate(NULL)
	{}

	void SetDelegate(IPacketManagerDelegate * delegate) { m_delegate = delegate; };
	void SetTimeout(float delay);

	void AddClent(IAbstractSocket * socket)
	{
		m_socketList.push_back(socket);
	}

	void RemoveClent(CTcpSocket * clientSocket);

	/// обходим все сокеты, ищем полученные данные, callbacks
	void OnUpdate(ITransportPacket * packet)
	{
		BufferObject buffer;
		buffer.Init(256);

		for (uint i = 0; i < m_socketList.size(); ++i)
		{
			IAbstractSocket * socket = m_socketList[i];

			const int headerSize = packet->GetHeaderSize();
			byte * buf = (byte *)buffer.GetPointer();

			int rcvd = socket->Recv(buf, headerSize);
			if (rcvd == headerSize)
			{
				const uint dataSize = packet->GetArgumentSize(buf);
				if (0 != dataSize)
				{
					buf += headerSize;

					if (dataSize != socket->Recv(buf, dataSize))
					{
						FAIL("invalid packet data size");
					}
				}

				m_delegate->OnIncomingPacket(socket, &buffer);
			}
			else
			{
				FAIL("bad packet header");
			}
		}
	}

	void SendPacket(CTcpSocket * clientSocket, BufferObject * data, void * callBack);
};

//==============================================================================
#endif // #ifndef __PacketManager_h_included__
//==============================================================================