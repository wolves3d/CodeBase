//==============================================================================
#ifndef __PacketManager_h_included__
#define __PacketManager_h_included__
//==============================================================================

#include "CodeBase/network/tcp_socket/tcp_socket.h"


struct ITransportPacket
{
	virtual uint GetHeaderSize() const = 0;
	virtual bool CheckHeader(const void * header, uint byteCount) = 0;

	virtual uint GetArgumentSize(const void * header) = 0;
	virtual bool CheckPacket(const void * packet, uint byteCount) = 0;

	virtual void FillHeader(void * dst, uint cmdID, uint cmdTag, uint byteCount) = 0;

	virtual uint GetCommandID(const void * header) = 0;
	virtual uint GetCommandTag(const void * header) = 0;
};


struct IPacketManagerDelegate
{
	/// обработчик входящих пакетов
	virtual void OnIncomingPacket(IAbstractSocket * socket, const byte *data, size_t dataSize) = 0;

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


	struct TSocketDesc
	{
		TSocketDesc(IAbstractSocket * s)
			: socket(s)
		{
			ResetBuffer();
		}

		void ResetBuffer()
		{
			writtenBytes = 0;
			targetByteCount = 0;
		}

		IAbstractSocket *socket;
		size_t writtenBytes;
		size_t targetByteCount;

		enum { READ_BUFFER_SIZE = 1024} ;
		byte buffer[READ_BUFFER_SIZE];
	};

	vector <TSocketDesc> m_socketList;

	byte m_readBuffer;
	byte m_bufferSize;

public:

	CPacketManager()
		: m_delegate(NULL)
	{}

	void SetDelegate(IPacketManagerDelegate * delegate) { m_delegate = delegate; };
	void SetTimeout(float delay);

	void AddClent(IAbstractSocket * socket)
	{
		m_socketList.push_back(
			TSocketDesc(socket));
	}

	void RemoveClent(CTcpSocket * clientSocket);

	/// обходим все сокеты, ищем полученные данные, callbacks
	void OnUpdate(ITransportPacket * packet)
	{
		//BufferObject buffer;
		//buffer.Init(1024);

		for (uint i = 0; i < m_socketList.size(); ++i)
		{
			TSocketDesc & desc = m_socketList[i];
			IAbstractSocket * socket = desc.socket;

			const uint headerSize = packet->GetHeaderSize();
			int targetSize = 0;

			if (0 == desc.targetByteCount)
			{
				// header wasn't filled yet
				targetSize = (headerSize - desc.writtenBytes);

				//printf("wait header %d bytes (%d - %d)\n", targetSize, headerSize, desc.writtenBytes);
			}
			else
			{
				// we've already got header, so read arguments
				targetSize = ((headerSize + desc.targetByteCount) - desc.writtenBytes);

				//printf("wait args %d bytes (%d + %d - %d)\n", targetSize, headerSize, desc.targetByteCount,  desc.writtenBytes);
			}

			// Read header
			byte * buf = (desc.buffer + desc.writtenBytes);
			int rcvd = 0;

			if (targetSize > 0)
			{
				const int rcvd = socket->Recv(buf, targetSize);

				if (rcvd > 0)
				{
					desc.writtenBytes += rcvd;
//					printf("written %d bytes\n", rcvd);
				}
			}

			if (rcvd == targetSize)
			{
				bool packetRcvd = true;

				if (0 == desc.targetByteCount)
				{
					// on header recvd
					desc.targetByteCount = packet->GetArgumentSize(desc.buffer);
/*
					printf("Got header %d, %d, %d arg size %d\n",
						(int)desc.buffer[0],
						(int)desc.buffer[1],
						(int)desc.buffer[2], desc.targetByteCount);
*/
					packetRcvd = (0 == desc.targetByteCount);
				}

				if (true == packetRcvd)
				{
					printf("Got packet %d, %d, %d arg size %d\n",
						(int)desc.buffer[0],
						(int)desc.buffer[1],
						(int)desc.buffer[2], desc.targetByteCount);

					m_delegate->OnIncomingPacket(socket, desc.buffer, desc.writtenBytes);
					desc.ResetBuffer();
				}
			}
			else
			{
				if ((-1 != rcvd) && (0 != rcvd))
				{
					FAIL("bad packet header");
				}
			}
		}
	}

	void SendPacket(CTcpSocket * clientSocket, BufferObject * data, void * callBack);
};

//==============================================================================
#endif // #ifndef __PacketManager_h_included__
//==============================================================================
