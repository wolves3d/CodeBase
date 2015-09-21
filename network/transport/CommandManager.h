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

// Forward declaration
struct IResponseHandler;

typedef void (CBaseObject::*SEL_Response)(const byte * data, uint size, IResponseHandler * handler, IAbstractSocket * socket, CCommandManager * mgr, void * userArg);


struct ICallback
{
	/// true - to unregister, false - to keep
	virtual bool CallBack(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr, void * userArg) = 0;
};


#define HANDLER_HEADER(HANDLER_NAME, COMMAND_ID) \
	public: \
	virtual const char * GetName() const { return #HANDLER_NAME; } \
	virtual uint GetResponseID() const { return COMMAND_ID; } \
	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)

struct IResponseHandler
{
	virtual const char * GetName() const = 0;
	virtual uint GetResponseID() const = 0;;
	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr) {};

	IResponseHandler()
		: m_callbackTarget(NULL)
		, m_callbackFunc(NULL)
		, m_callbackArg(NULL)
	{}

	//virtual const char * GetName() const = 0;

	void SetCallback(ICallback * callback, void * callbackArg)
	{
// 		m_callback = callback;
// 		m_callbackArg = callbackArg;
	}

	void SetCallback(SEL_Response func, CBaseObject * target, void * userData)
	{
		m_callbackFunc = func;
		m_callbackTarget = target;
		m_callbackArg = userData;
	}

	void OnCallback(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr)
	{
		if (NULL != m_callbackTarget)
		{
			if (NULL != m_callbackFunc)
			{
				(m_callbackTarget->*m_callbackFunc)(data, size, this, socket, mgr, m_callbackArg);
			}
		}
	}

	//ICallback * GetCallback() { return m_callback; }
	//void * GetCallbackArg() { return m_callbackArg; }
	//ICallback * m_callback;
	
	CBaseObject * m_callbackTarget;
	SEL_Response m_callbackFunc;
	void * m_callbackArg;
};


#define RESPONSE_HANDLER_DECL(HANDLER_NAME, COMMAND_ID) \
class HANDLER_NAME : public IResponseHandler \
{ \
public: \
	virtual const char * GetName() const { return #HANDLER_NAME; } \
	virtual uint GetResponseID() const { return COMMAND_ID; } \
	virtual void OnResponse(const byte * data, uint size, IAbstractSocket * socket, CCommandManager * mgr); \
};


struct INetCommand : IResponseHandler
{
	virtual uint GetCommandID() const = 0;
	virtual uint OnFillData(void * buffer, uint maxByteCount) { return 0; }
};


#define REQUEST_HANDLER_BODY(REQ_NAME, CMD_ID, RSP_ID) \
	public: \
	virtual const char * GetName() const { return #REQ_NAME; } \
	virtual uint GetCommandID() const { return CMD_ID; } \
	virtual uint GetResponseID() const { return RSP_ID; } \
	virtual uint OnFillData(void * buffer, uint maxByteCount); \


#define REQUEST_HANDLER_DECL(REQUEST_NAME, COMMAND_ID, RESPONSE_ID) \
class REQUEST_NAME : public INetCommand \
{ \
REQUEST_HANDLER_BODY(REQUEST_NAME, COMMAND_ID, RESPONSE_ID) \
};


/*
#define REQUEST_HANDLER_DECL(REQUEST_NAME, COMMAND_ID, RESPONSE_ID) \
class REQUEST_NAME : public INetCommand \
{ \
	public: \
	virtual const char * GetName() const { return #REQUEST_NAME; } \
	virtual uint GetCommandID() const { return COMMAND_ID; } \
	virtual uint GetResponseID() const { return RESPONSE_ID; } \
	virtual uint OnFillData(void * buffer, uint maxByteCount); \
};
*/



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

	uint RegisterHandler(IResponseHandler * handler);
	void UnregisterHandler(uint handlerID);
	IResponseHandler * GetHandler(uint handlerID);

	/// Кладет команду в другой поток и тут же возвращает управление
//	void SendCommand(IAbstractSocket * socket, uint cmdID, void * data, uint byteCount);
	void SendCommand(IAbstractSocket * socket, INetCommand * command);
	
	/// Получена входящая команда от клиента
	void OnUnknownCommand();

	void OnUpdate() { m_packetMgr->OnUpdate(m_packet); }

	CPacketManager * GetPacketManager() { return m_packetMgr; }

private:
	void AddUniqueHandler(uint uniqueCommandID, IAbstractSocket * associatedSocket, INetCommand * command);
	void RemoveUniqueHandler(IAbstractSocket * associatedSocket, uint uniqueCommandID);
	void RemoveUniqueHandlers(IAbstractSocket * associatedSocket);
	INetCommand * FindHandler(uint uniqueCommandID, IAbstractSocket * associatedSocket);

	bool CheckHandlerID(uint handlerID);

	CPacketManager * m_packetMgr;
	ITransportPacket * m_packet;

	/// Universal handlers (not associated with particular socket)
	vector <IResponseHandler *> m_handlerList;


	//map <uint, INetCommand *> m_uniqueHandlerList;
	struct HandlerMap
	{
		map <uint, INetCommand *> m_uniqueHandlerList;
	};
	map <uint, HandlerMap *> m_socketHandlers;
	HandlerMap * GetHandlerMap(IAbstractSocket * associatedSocket, uint * outHash = NULL);


	uint m_commandCounter;
	uint GetNextCmdNumber() { return ++m_commandCounter; }
};

//==============================================================================
#endif // #ifndef __CommandManager_h_included__
//==============================================================================